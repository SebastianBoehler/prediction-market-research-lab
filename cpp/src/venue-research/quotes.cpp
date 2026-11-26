#include "quotes.hpp"

#include <chrono>
#include <cmath>
#include <iostream>

namespace venue_research
{
    namespace
    {
        std::optional<double> best(const std::vector<Level> &levels, bool bid)
        {
            std::optional<double> price;
            for (const auto &level : levels)
            {
                if (!std::isfinite(level.shares) || level.shares <= 0 ||
                    !std::isfinite(level.price) || level.price <= 0 || level.price >= 1)
                    continue;
                if (!price || (bid ? level.price > *price : level.price < *price))
                    price = level.price;
            }
            return price;
        }

        Json levels_json(const std::vector<Level> &levels)
        {
            Json rows = Json::array();
            for (const auto &level : levels)
                rows.push_back({{"price", level.price}, {"shares", level.shares}});
            return rows;
        }
    }

    std::int64_t now_ms()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    double decimal(const std::string &value)
    {
        std::size_t consumed = 0;
        const double result = std::stod(value, &consumed);
        if (consumed != value.size() || !std::isfinite(result))
            throw std::runtime_error("Invalid order book decimal");
        return result;
    }

    Json book_json(const Book &book)
    {
        return {{"token_id", book.token}, {"received_at_ms", book.received_ms},
                {"source_timestamp_ms", book.source_ms ? Json(*book.source_ms) : Json()},
                {"bids", levels_json(book.bids)}, {"asks", levels_json(book.asks)}};
    }

    Json quote_metrics(const Book &yes, const Book &no)
    {
        const auto yes_bid = best(yes.bids, true);
        const auto yes_ask = best(yes.asks, false);
        const auto no_bid = best(no.bids, true);
        const auto no_ask = best(no.asks, false);
        if (!yes_bid || !yes_ask || !no_ask || *yes_bid > *yes_ask ||
            (no_bid && *no_bid > *no_ask))
            return nullptr;

        return {{"yes_bid", *yes_bid}, {"yes_ask", *yes_ask}, {"no_ask", *no_ask},
                {"yes_spread", *yes_ask - *yes_bid}, {"bundle_ask", *yes_ask + *no_ask}};
    }

    void Scan::emit(Json market, const Book &yes, const Book &no)
    {
        auto metrics = quote_metrics(yes, no);
        if (!metrics.is_null())
        {
            ++quoted;
            spread_sum += metrics.at("yes_spread").get<double>();
        }

        market["schema_version"] = 1;
        market["books"] = {{"yes", book_json(yes)}, {"no", book_json(no)}};
        market["quote_metrics"] = std::move(metrics);
        std::cout << market.dump() << '\n';
        ++recorded;
    }

    void Scan::error(const std::string &market, const std::string &message)
    {
        ++errors;
        std::cerr << market << ": " << message << '\n';
    }
}
