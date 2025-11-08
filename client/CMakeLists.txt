add_executable(clientmain
    main.cpp
    udpclient.cpp
)

target_include_directories(clientmain PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(clientmain PRIVATE 
    spdlog::spdlog
    common
)