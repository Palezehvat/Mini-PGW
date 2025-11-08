#include "udpclient.h"

namespace nUdpClient {
    UdpClient::UdpClient(const int& serverPort,
                         const std::string& serverIp,
                         std::shared_ptr<spdlog::logger> logger) :
                         serverPort(serverPort), serverIp(serverIp), logger(logger) {
        logger->debug("UDP client start initialization in {}:{}", serverIp, serverPort);
        udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udpSocket < 0) {
            logger->critical("Failed to create socket: {}", strerror(errno));
            throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
        }

        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);

        if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
            close(udpSocket);
            logger->critical("Invalid ip address: {}. Error: {}", serverIp, strerror(errno));
            throw std::runtime_error("Invalid ip address: " + serverIp 
                                + ". Error: " + strerror(errno));
        }

        logger->info("UDP client created successfully on {}:{}", serverIp, serverPort);
    }

    UdpClient::~UdpClient() {
        if (udpSocket >= 0) {
            close(udpSocket);
            logger->debug("Socket in udp client closed");
        }
    }

    std::string UdpClient::sendMessage(const std::string& imsi) {
        logger->debug("Send message to server");

        ssize_t resultSend = sendto(udpSocket, imsi.c_str(), imsi.size(), 0,
                        (sockaddr*)&serverAddr, sizeof(serverAddr));

        if (resultSend < 0) {
            logger->error("Failed to send IMSI = {}. Error: ", imsi, strerror(errno));
            throw std::runtime_error("Error then trying to send IMSI = " + imsi + ". Error: " +
                                     strerror(errno));
        }

        logger->info("The message to the server has been sent successfully");

        char buffer[1024];
        sockaddr_in fromAddr;
        socklen_t sizeFromAddr = sizeof(fromAddr);

        ssize_t sizeRecvFrom = recvfrom(udpSocket, buffer, sizeof(buffer), 
                                    0, (sockaddr*)&fromAddr, &sizeFromAddr);

        if (sizeRecvFrom < 0) {
            logger->error("Failed to get message after send IMSI = {}. Error: ",
                           imsi, strerror(errno));
            throw std::runtime_error("Error then trying to get result after send IMSI = " + imsi 
                                   + ". Error: " + strerror(errno));
        }

        buffer[sizeRecvFrom] = '\0';
        std::string result(buffer);
        logger->info("The message from the server was successfully received");
        return result;
    }
}