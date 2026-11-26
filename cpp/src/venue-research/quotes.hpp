#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace venue_research
{
    using Json = nlohmann::json;

    struct Level
    {
        double price;
        double shares;
    };

    struct Book
    {
        std::string token;
        std::int64_t received_ms;
        std::optional<std::int64_t> source_ms;
        std::vector<Level> bids;
        std::vector<Level> asks;
    };

    std::int64_t now_ms();
    double decimal(const std::string &value);
    Json book_json(const Book &book);
    Json quote_metrics(const Book &yes, const Book &no);

    struct Scan
    {
        int examined = 0;
        int recorded = 0;
        int quoted = 0;
        int errors = 0;
        double spread_sum = 0;

        void emit(Json market, const Book &yes, const Book &no);
        void error(const std::string &market, const std::string &message);
    };

    void scan_limitless(int limit, Scan &scan);
    void scan_opinion(int limit, Scan &scan);
}
