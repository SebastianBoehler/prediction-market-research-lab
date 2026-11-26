# Quote monitoring and strategy replays, 26 November 2026

## Same-market bundle monitor

The public Polymarket market stream and independently polled batched books
covered 11 CTF markets, including crypto, sports and event contracts. Two
bounded runs totaled 270 seconds of polling and returned 135 REST responses.
The first WebSocket closed with code 1013 after 52.3 seconds. The rerun's
stream covered 119.4 seconds using the narrower subscription.

- 0 of 1,448 REST comparisons had both asks below $1 combined. The lowest
  observed total was $1.001 before fees.
- 2 of 223,314 WebSocket comparisons appeared below $1. Each reverted within
  the same local millisecond. Both legs had to have a visible ask and recent
  updates for this check. A mixed feed state can create such a reading.
  The public feed gives no matching-engine execution timestamp for a pair.
- On two different venues, three fresh snapshots each reproduced the
  SHA-256 and magnitude 10.0+ earthquake gaps. The pricing model still
  shows +$6.11 and +$11.68 at displayed depth using the
  assumptions in the execution check. Separate matching engines mean these gaps can persist.
  Actual fills, joint settlement, and all-in fees remain unverified.
  [Execution check](cross-platform-execution-2026-11-26.md)

[Polymarket public market stream](https://docs.polymarket.com/market-data/realtime-data) ·
[Polymarket fee schedule](https://docs.polymarket.com/trading/fees)

## Sequential entry and averaging down

The May archive had 78 crypto contracts with observed quotes during the
active window. Gamma mapped their outcome tokens and on-chain Conditional
Tokens payout data supplied 62 final payout vectors. One truncated archive
line was excluded. Each modeled entry used the next available quote after
selecting the cheaper side early in the active window. The averaging
policy could add once, five shares lower by at least five cents. A hedge
needed enough displayed liquidity and combined after-fee cost of $0.99 or
less within three minutes. Open positions carried through verified settlement.
The 0.05 taker curve coefficient is a scenario for comparability, since the
historical snapshots lacked per-market fee records.

| Variant | Market entries | Paired | Paired PnL | Unpaired PnL | Net |
| --- | ---: | ---: | ---: | ---: | ---: |
| Buy once, then hedge | 33 | 17 | +$6.49 | −$13.44 | −$6.95 |
| Average down once, then hedge | 33 | 16 | +$6.00 | −$15.23 | −$9.23 |
| Buy once, require price to survive to next quote | 33 | 11 | +$8.70 | −$15.87 | −$7.17 |
| Average down, require price to survive to next quote | 33 | 12 | +$10.02 | −$23.25 | −$13.23 |

Two regimes differed. The delayed averaging policy lost $13.94 in the
earlier 20 entries and gained $0.71 in the later 13. The sample uses
recorded quote size and conditional settlement, with no verified fills.
The archive interval was around 32 seconds, which cannot model order
queue and routing latency.

[Polymarket Conditional Tokens contract](https://docs.polymarket.com/resources/contracts)

## Other mechanisms

- **Near-expiry favorites.** A fixed late entry into the 95–99¢ ask band
  found four markets with adequate quote size. All four won and yielded
  +$0.24 under the fee scenario. The sample is too small to estimate
  losses from rare wrong outcomes.
- **Short-horizon momentum and reversal.** A 12-second price-move screen
  entered at the next 5-share ask and priced an exit at the subsequent
  30-second bid. Momentum was positive in the first window and negative
  in the rerun. The fade lost in both. Exits without sufficient depth or
  observations stayed unresolved.
- **Passive quotes after trade-through.** A five-share resting bid proxy
  required a later five-share SELL print strictly below the proposed
  entry price. Eight qualified cases had priced 15-second exits for a
  total +$0.31 under the markout model. One qualifying event had no
  priced exit. Public prints do not establish queue priority or validate
  the counterfactual fill.
- **Independent weather observations.** The current rule text uses
  Aviation Weather Center routine METAR observations for the named
  station. A rule-based lower bound on daily high put nine brackets below
  at least two observed readings across London City, Munich, and
  LaGuardia. None of the nine NO books had a displayed ask when checked.
  Corrections can still change published station readings.

A forward test with verified fills and costs remains open for these
strategies. All work was read-only.

[Aviation Weather Center data API](https://aviationweather.gov/data/api/)
