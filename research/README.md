# Historical research artifacts

- [Summary CSVs](archive/summaries/) retain aggregates from earlier Polymarket runs.
- [Archived plots](archive/plots/) retain the original figures, including a historical order benchmark.
- [Research notes](../docs/research/) describe the investigations and current data collection.

## README figures

Run `python3 research/render_readme_figures.py` with Matplotlib installed.

The quote cutoff plot uses the latest `all` row in
[`stats-summary.csv`](archive/summaries/stats-summary.csv), dated
2026-01-10 at 17:54:09 UTC. It divides each selected cumulative threshold count
by the row's 1,465,559 quote updates. The denominator counts updates across BTC,
ETH, XRP, and SOL 15-minute markets. Several updates can come from one market or
quote episode. The archive contains aggregates and does not establish that both
quotes were simultaneously fillable.

The bundle figure illustrates the cost and gross payout for one pair of binary
shares using example prices.
