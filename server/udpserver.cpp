#include "udpserver.h"

namespace nUdpServer {

UdpServer::UdpServer(const int& port,
                     const std::string& ip,
                     std::shared_ptr<spdlog::logger> logger,
                     std::shared_ptr<nSessionManager::SessionManager> sessionManager,
                     std::shared_ptr<std::atomic<bool>> running) 
                     : port(port), ip(ip), logger(logger), sessionManager(sessionManager),
                       running(running), localRunning(true) {
    logger->debug("UDP server start initialization");
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        logger->critical("Failed to create socket: {}", strerror(errno));
        throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        close(udpSocket);
        logger->critical("Invalid ip address: {}. Error: {}", ip, strerror(errno));
        throw std::runtime_error("Invalid ip address: " + ip 
                               + ". Error: " + strerror(errno));
    }

    logger->info("UDP socket created on {} : {}", ip, port);
}

void UdpServer::start() {
    if (bind(udpSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        logger->critical("Failed to bind socket: {}", strerror(errno));
        close(udpSocket);
        throw std::runtime_error("Failed to bind socket: " + std::string(strerror(errno)));
    }

    logger->debug("Success to bind socket");

    listenerThread = std::thread(&UdpServer::listenLoop, this);
}

bool UdpServer::checkImsi(const std::string& imsi) {
    if (imsi.size() != 15) return false;
    for (size_t i = 0; i < imsi.size(); ++i) {
        if (!isdigit(imsi[i])) return false;
    }
    return true;
}

void UdpServer::listenLoop() {
    char buffer[1024];
    sockaddr_in clientAddr;
    socklen_t sizeClientAddr = sizeof(clientAddr);

    logger->info("UDP server started and waiting for data");

    while (localRunning) {
        ssize_t sizeRecvFrom = recvfrom(udpSocket, buffer, sizeof(buffer), 
                                    0, (sockaddr*)&clientAddr, &sizeClientAddr);
        if (sizeRecvFrom < 0) {
            logger->error("Recvfrom failed: {}", strerror(errno));
            continue;
        }

        buffer[sizeRecvFrom] = '\0';
        std::string imsi(buffer);

        if (!imsi.empty() && std::isspace(static_cast<unsigned char>(imsi.back()))) {
            imsi.pop_back();
        }

        if (!(*running)) {
            if (!imsi.empty()) {
                std::string response = "rejected\n";
                logger->warn("Trying to create a new session after shut down. IMSI = {}. "
                             "The response sent was rejected", imsi);
                sessionManager->addRecordForRejectSession(imsi);
                sendto(udpSocket, response.c_str(), response.size(), 0,
                  (sockaddr*)&clientAddr, sizeClientAddr);
            }
            continue;
        }

        if (!checkImsi(imsi)) {
            std::string response = "rejected\n";
            logger->warn("The Imsi number doesn't meet the stated requirements. "
                         "It doesn't have 15 digits. IMSI = {}. ", imsi);
            sessionManager->addRecordForRejectSession(imsi);
            sendto(udpSocket, response.c_str(), response.size(), 0,
                (sockaddr*)&clientAddr, sizeClientAddr);
            continue;
        }

        logger->info("Received IMSI={} from {}:{}",
                     imsi,
                     inet_ntoa(clientAddr.sin_addr),
                     ntohs(clientAddr.sin_port));
        std::string response = "";
        try {
            if (sessionManager->createSession(imsi)) {
                response = "created\n";
            } else {
                response = "rejected\n";
            }
        } catch (const std::exception& e) {
            logger->error("Problem creating session. Error: {}", std::string(e.what()));
        }
        if (response == "") response = "rejected\n";
        sendto(udpSocket, response.c_str(), response.size(), 0,
              (sockaddr*)&clientAddr, sizeClientAddr);
        logger->info("Send to {}:{} that session created", inet_ntoa(clientAddr.sin_addr),
                     ntohs(clientAddr.sin_port));
    }
    logger->info("UDP server stopped");
}

void UdpServer::stop() {
    if ((*running)) return;

    localRunning.store(false);

    sockaddr_in selfAddr{};
    selfAddr.sin_family = AF_INET;
    selfAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &selfAddr.sin_addr) < 0) {
        close(udpSocket);
        logger->critical("Trying to stop UDP server. Invalid ip address: {}. Error: {}",
                          ip, strerror(errno));
        throw std::runtime_error("Trying to stop server. Invalid ip address: "
                                + ip
                                + ". Error: "
                                + strerror(errno));
    }
    logger->debug("Sending an empty UDP packet to close recvfrom");
    sendto(udpSocket, "", 0, 0, (sockaddr*)&selfAddr, sizeof(selfAddr));

    if (listenerThread.joinable()) {
        listenerThread.join();        
    }

    close(udpSocket);

    logger->info("The UDP server shut down correctly");
}

} // nUdpServer