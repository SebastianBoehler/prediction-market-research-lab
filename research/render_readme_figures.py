"""Render the README figures from a tracked summary and a worked example."""
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import PercentFormatter

ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / "docs" / "assets"
INK = "#172738"
MUTED = "#466071"
PAPER = "#f5f8fa"
TEAL = "#167d74"
BLUE = "#356ca5"


def canvas(title, subtitle):
    fig, ax = plt.subplots(figsize=(8.4, 3.45), facecolor=PAPER)
    fig.subplots_adjust(left=0.14, right=0.92, top=0.70, bottom=0.21)
    fig.text(0.065, 0.87, title, fontsize=18, weight="bold", color=INK)
    fig.text(0.065, 0.78, subtitle, fontsize=10.5, color=MUTED)
    ax.set_facecolor(PAPER)
    ax.tick_params(axis="both", colors=MUTED, length=0, labelsize=10)
    for spine in ax.spines.values():
        spine.set_visible(False)
    return fig, ax


def historical_thresholds():
    summary = ROOT / "research/archive/summaries/stats-summary.csv"
    with summary.open(newline="") as source:
        row = max(
            (row for row in csv.DictReader(source) if row["row_type"] == "all"),
            key=lambda row: row["timestamp"],
        )

    thresholds = ["0.9", "0.95", "0.98", "0.99"]
    samples = int(row["samples"])
    shares = [int(row[f"hits_le_{threshold}"]) / samples for threshold in thresholds]
    date = row["timestamp"][:10]
    fig, ax = canvas(
        "How often did quotes meet each cutoff?",
        f"Polymarket 15-minute crypto markets  •  {samples:,} quote updates  •  as of {date} UTC",
    )
    bars = ax.barh([f"${float(t):.2f}" for t in thresholds], shares, color=TEAL, height=0.53)
    ax.bar_label(bars, labels=[f"{share:.3%}" for share in shares], padding=7, color=INK, fontsize=10)
    ax.invert_yaxis()
    ax.set_axisbelow(True)
    ax.xaxis.grid(color="#d7e1e8")
    ax.xaxis.set_major_formatter(PercentFormatter(1, decimals=1))
    ax.set_xticks([i / 1000 for i in range(5)])
    ax.set_xlim(0, max(shares) * 1.23)
    ax.tick_params(axis="y", pad=9)
    ax.set_xlabel("Share of updates at or below the combined ask cutoff", labelpad=9, color=MUTED)
    fig.savefig(ASSETS / "historical-quote-cutoffs.png", dpi=160)
    plt.close(fig)


def bundle_example():
    fig, ax = canvas("What does a 98¢ bundle imply?",
                     "Illustrative purchase of one YES share and one NO share in the same market")
    ax.set_position([0.14, 0.18, 0.78, 0.51])
    ax.barh(1, 0.48, height=0.29, color=TEAL)
    ax.barh(1, 0.50, left=0.48, height=0.29, color=BLUE)
    ax.barh(0, 1, height=0.29, color=INK)
    ax.text(0.24, 1, "YES 48¢", color="white", ha="center", va="center", weight="bold")
    ax.text(0.73, 1, "NO 50¢", color="white", ha="center", va="center", weight="bold")
    ax.text(0.50, 0, "$1.00", color="white", ha="center", va="center", weight="bold")
    ax.plot([1, 1], [0.16, 1.3], linestyle=":", color=MUTED, linewidth=1.25)
    ax.annotate("2¢ gross difference", xy=(0.99, 1.08), xytext=(0.72, 1.53),
                arrowprops={"arrowstyle": "->", "color": MUTED}, ha="center", va="center", color=INK)
    ax.set_yticks([1, 0], labels=["Cost", "Payout"])
    ax.tick_params(axis="y", pad=9)
    ax.set_xlim(0, 1.05)
    ax.set_ylim(-0.35, 1.75)
    ax.set_xticks([])
    fig.savefig(ASSETS / "binary-bundle-example.png", dpi=160)
    plt.close(fig)


if __name__ == "__main__":
    historical_thresholds()
    bundle_example()
