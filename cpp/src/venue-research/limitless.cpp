#include "quotes.hpp"

#include <limitless/clob_client.hpp>

namespace venue_research
{
    namespace
    {
        Book normalize(const limitless::OrderBook &source, const std::string &token, std::int64_t received_ms)
        {
            Book book{token, received_ms, std::nullopt, {}, {}};
            for (const auto &level : source.bids)
                book.bids.push_back({level.price, decimal(limitless::raw_to_shares(level.size_raw))});
            for (const auto &level : source.asks)
                book.asks.push_back({level.price, decimal(limitless::raw_to_shares(level.size_raw))});
            return book;
        }
    }

    void scan_limitless(int limit, Scan &scan)
    {
        limitless::ClobClient client(limitless::Environment::base());
        limitless::ActiveMarketsQuery query;
        query.limit = limit;
        query.trade_type = "clob";
        const auto page = client.get_active_markets(query);
        if (!page)
            throw std::runtime_error(page.error().message);

        for (const auto &market : page.value().markets)
        {
            if (scan.examined == limit)
                break;
            ++scan.examined;
            if (market.trade_type != "clob" || !market.tokens || market.slug.empty() ||
                market.tokens->yes.empty() || market.tokens->no.empty() ||
                market.tokens->yes == market.tokens->no)
                continue;

            try
            {
                const auto result = client.get_orderbook(market.slug);
                const auto received_ms = now_ms();
                if (!result)
                    throw std::runtime_error(result.error().message);
                const auto &book = result.value();
                if (!book.token_id.empty() && book.token_id != market.tokens->yes)
                    throw std::runtime_error("Unexpected YES token in order book");

                scan.emit({{"venue", "limitless"}, {"market_id", market.id}, {"slug", market.slug},
                           {"title", market.title}, {"no_book_source", "complement_of_yes"},
                           {"collateral", "USDC"}, {"market_metadata", market.raw}},
                          normalize(book, market.tokens->yes, received_ms),
                          normalize(limitless::derive_no_book(book), market.tokens->no, received_ms));
            }
            catch (const std::exception &error)
            {
                scan.error(market.slug, error.what());
            }
        }
    }
}
