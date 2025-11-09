add_library(clientlib STATIC
    udp_client.cpp
)

target_include_directories(clientlib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(clientlib PUBLIC 
    spdlog::spdlog
    common
)

add_executable(clientmain
    main.cpp
)

target_link_libraries(clientmain PRIVATE clientlib)