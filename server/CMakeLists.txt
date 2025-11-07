add_executable(servermain
    main.cpp
    udpserver.cpp
    httpserver.cpp
    session.cpp
    cdr.cpp
)

target_include_directories(servermain PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(servermain PRIVATE 
    spdlog::spdlog
    httplib
    common
)