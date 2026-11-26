#include "quotes.hpp"

#include <charconv>
#include <iostream>
#include <string_view>

int main(int argc, char **argv)
{
    using namespace venue_research;
    const std::string_view venue = argc > 1 ? argv[1] : "";
    int limit = 0;
    const std::string_view count = argc == 3 ? argv[2] : "";
    const auto parsed = std::from_chars(count.data(), count.data() + count.size(), limit);
    if ((venue != "limitless" && venue != "opinion") || parsed.ec != std::errc{} ||
        parsed.ptr != count.data() + count.size() || limit < 1 || limit > 20)
    {
        std::cerr << "Usage: research_venues <limitless|opinion> <market-limit 1-20>\n";
        return 2;
    }

    Scan scan;
    try
    {
        if (venue == "limitless")
            scan_limitless(limit, scan);
        else
            scan_opinion(limit, scan);
    }
    catch (const std::exception &error)
    {
        scan.error(std::string(venue), error.what());
    }

    std::cerr << venue << ": examined " << scan.examined << ", recorded " << scan.recorded
              << ", complete quotes " << scan.quoted
              << ", errors " << scan.errors;
    if (scan.quoted > 0)
        std::cerr << ", mean YES spread " << scan.spread_sum / scan.quoted << " per share";
    std::cerr << '\n';
    return scan.errors > 0 || !std::cout ? 1 : 0;
}
