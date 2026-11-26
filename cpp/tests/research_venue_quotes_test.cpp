#include "quotes.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

int main()
{
    using namespace venue_research;
    Book yes{"yes", 100, std::nullopt, {{0.40, 2}, {0.47, 2}}, {{0.54, 3}, {0.48, 3}}};
    Book no{"no", 101, 90, {{0.49, 1}}, {{0.10, 0}, {0.50, 1}}};

    const auto metrics = quote_metrics(yes, no);
    if (std::abs(metrics.at("yes_spread").get<double>() - 0.01) > 1e-12 ||
        std::abs(metrics.at("bundle_ask").get<double>() - 0.98) > 1e-12)
    {
        std::cerr << "Expected the best positive-size prices in unsorted books\n";
        return 1;
    }

    no.bids.push_back({0.51, 1});
    if (!quote_metrics(yes, no).is_null())
    {
        std::cerr << "Crossed books must not produce quote metrics\n";
        return 1;
    }
    no.asks.clear();
    if (!quote_metrics(yes, no).is_null())
    {
        std::cerr << "Missing asks must not produce quote metrics\n";
        return 1;
    }
    if (book_json(no).at("source_timestamp_ms") != 90 ||
        !book_json(yes).at("source_timestamp_ms").is_null())
    {
        std::cerr << "A source timestamp must stay distinct from receipt time\n";
        return 1;
    }
}
