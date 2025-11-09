add_executable(tests client_server_tests.cpp server_tests.cpp common_tests.cpp)

target_link_libraries(tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    serverlib
    clientlib
    common
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

add_definitions(-DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/server
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_SOURCE_DIR}/tests/mocks
)

include(GoogleTest)
gtest_discover_tests(tests)