#include "quotes.hpp"

#include <opinion/clob_client.hpp>

#include <chrono>
#include <thread>

namespace venue_research
{
    namespace
    {
        Book read_book(const opinion::ClobClient &client, const std::string &token)
        {
            // The public API allows five requests per second per IP.
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            const auto result = client.get_orderbook(token);
            const auto received_ms = now_ms();
            if (!result)
                throw std::runtime_error(result.error().message);

            const auto &source = result.value();
            if (!source.token_id.empty() && source.token_id != token)
                throw std::runtime_error("Unexpected token in order book");

            Book book{token, received_ms, std::nullopt, {}, {}};
            if (source.timestamp_ms > 0)
                book.source_ms = source.timestamp_ms;
            for (const auto &level : source.bids)
                book.bids.push_back({decimal(level.price), decimal(level.size)});
            for (const auto &level : source.asks)
                book.asks.push_back({decimal(level.price), decimal(level.size)});
            return book;
        }
    }

    void scan_opinion(int limit, Scan &scan)
    {
        opinion::ClobClient client(opinion::Environment::bnb_mainnet());
        opinion::MarketListQuery query;
        query.limit = limit;
        query.status = "activated";
        query.market_type = 0; // Binary markets.
        query.sort_by = 5;     // Highest 24-hour volume first.
        const auto page = client.list_markets(query);
        if (!page)
            throw std::runtime_error(page.error().message);

        for (const auto &market : page.value().markets)
        {
            if (scan.examined == limit)
                break;
            ++scan.examined;
            if (market.market_type != 0 || market.yes_token_id.empty() ||
                market.no_token_id.empty() || market.yes_token_id == market.no_token_id)
                continue;

            try
            {
                const auto yes = read_book(client, market.yes_token_id);
                const auto no = read_book(client, market.no_token_id);
                scan.emit({{"venue", "opinion"}, {"market_id", market.market_id}, {"slug", market.slug},
                           {"title", market.market_title}, {"rules", market.rules},
                           {"yes_label", market.yes_label}, {"no_label", market.no_label},
                           {"condition_id", market.condition_id}, {"collateral_address", market.quote_token},
                           {"chain_id", market.chain_id}, {"no_book_source", "token_orderbook"}}, yes, no);
            }
            catch (const std::exception &error)
            {
                scan.error(std::to_string(market.market_id), error.what());
            }
        }
    }
}
