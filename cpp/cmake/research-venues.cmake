set(LIMITLESS_CLIENT_BUILD_EXAMPLES OFF)
set(LIMITLESS_CLIENT_BUILD_TESTS OFF)
set(OPINION_CLIENT_BUILD_EXAMPLES OFF)
set(OPINION_CLIENT_BUILD_TESTS OFF)

FetchContent_Declare(
    limitless_client
    GIT_REPOSITORY https://github.com/SebastianBoehler/limitless-cpp-client.git
    GIT_TAG bd244df5fd619ec0445c95d4f1cf419fab29ef50
)
FetchContent_Declare(
    opinion_client
    GIT_REPOSITORY https://github.com/SebastianBoehler/opinion-cpp-client.git
    GIT_TAG da9c2ccc4c5e76c3cb3626e102f8fb1326832174
)
FetchContent_MakeAvailable(limitless_client opinion_client)

add_library(research_venue_quotes STATIC src/venue-research/quotes.cpp)
target_include_directories(research_venue_quotes PUBLIC src/venue-research)
target_link_libraries(research_venue_quotes PUBLIC nlohmann_json::nlohmann_json)

add_executable(research_venues
    src/venue-research/main.cpp
    src/venue-research/limitless.cpp
    src/venue-research/opinion.cpp
)
target_link_libraries(research_venues PRIVATE research_venue_quotes limitless::client opinion::client)
