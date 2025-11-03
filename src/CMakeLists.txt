add_library(common STATIC
    logger.cpp
    configmanager.cpp
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(common PUBLIC
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)