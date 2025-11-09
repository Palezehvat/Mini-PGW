add_library(serverlib
    udp_server.cpp
    http_server.cpp
    session.cpp
    cdr.cpp
)

target_include_directories(serverlib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(serverlib PUBLIC 
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    httplib
    common
)

add_executable(servermain
    main.cpp
)

target_link_libraries(servermain PRIVATE serverlib)