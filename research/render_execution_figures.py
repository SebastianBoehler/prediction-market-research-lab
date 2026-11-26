"""Render the dated cross-venue execution figures from the tracked summary."""

import csv
from math import ceil
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / "docs/assets"
SOURCE = ROOT / "research/archive/summaries/cross-platform-execution-2026-11-26.csv"
COLORS = ("#15685e", "#2b6091", "#a04c30")
INK = "#182b3c"
MUTED = "#465a6d"
PAPER = "#f5f8fa"


def read_summary():
    with SOURCE.open(newline="") as source:
        rows = []
        for row in csv.DictReader(source):
            paid = float(row["polymarket_cost"]) + float(row["opinion_cost"])
            net = float(row["modeled_net_at_opinion_cap"])
            rows.append((row["market"], paid, net))
        return rows


def canvas(title, subtitle):
    fig, ax = plt.subplots(figsize=(8.4, 4.25), facecolor=PAPER)
    fig.subplots_adjust(left=0.29, right=0.94, top=0.72, bottom=0.2)
    fig.text(0.055, 0.9, title, fontsize=17, weight="bold", color=INK)
    fig.text(0.055, 0.82, subtitle, fontsize=9.5, color=MUTED)
    ax.set_facecolor(PAPER)
    ax.set_axisbelow(True)
    ax.tick_params(colors=MUTED, length=0, labelsize=9)
    for spine in ax.spines.values():
        spine.set_visible(False)
    return fig, ax


def dollars(value, _):
    return f"${value:g}" if value >= 0 else f"−${abs(value):g}"


def fee_comparison(rows):
    fig, ax = canvas(
        "Quoted edge after fees and holding costs",
        "26 Nov 2026 • Synchronized quotes • Equal collateral value • No live fills",
    )
    y = list(range(len(rows)))
    fee_only = [net for _, _, net in rows]
    burden = 0.001 + 0.05 * 36 / 365
    scenario = [net - paid * burden for _, paid, net in rows]
    for offset, values, color, label in (
        (-0.16, fee_only, COLORS[0], "After modeled taker fees"),
        (0.16, scenario, COLORS[1], "Plus 10 bp and 36 days at 5% annual cost"),
    ):
        bars = ax.barh([i + offset for i in y], values, height=0.28, color=color, label=label)
        ax.bar_label(bars, labels=[f"{v:+.2f}" for v in values], padding=4, fontsize=9, color=INK)
    ax.set_yticks(y, [market for market, _, _ in rows])
    ax.invert_yaxis()
    ax.set_xlim(-3, 13.4)
    ax.xaxis.set_major_formatter(FuncFormatter(dollars))
    ax.axvline(0, linewidth=0.8, color=MUTED)
    ax.xaxis.grid(color="#d9e3eb")
    ax.legend(loc="lower left", bbox_to_anchor=(-0.02, -0.35), frameon=False, fontsize=8.5)
    fig.savefig(ASSETS / "cross-platform-execution-fees.png", dpi=160)
    plt.close(fig)


def cost_sensitivity(rows):
    fig, ax = canvas(
        "How much extra cost erases the quoted edge?",
        "Extra costs as a share of quoted purchase cost • Modeled fees already deducted",
    )
    fig.subplots_adjust(left=0.12, bottom=0.2, right=0.93)
    bps = list(range(0, ceil(max(net / paid * 10_000 for _, paid, net in rows)) + 16))
    for (market, paid, net), color in zip(rows, COLORS):
        breakeven = 10_000 * net / paid
        ax.plot(bps, [net - paid * b / 10_000 for b in bps], color=color, label=market)
        ax.scatter([breakeven], [0], color=color, zorder=5)
        ax.annotate(f"{breakeven:.0f} bp", (breakeven, 0), xytext=(0, 10),
                    textcoords="offset points", ha="center", color=color, fontsize=9)
    ax.set_xlabel("Additional costs (basis points of quoted purchase cost)", color=MUTED, labelpad=8)
    ax.yaxis.set_major_formatter(FuncFormatter(dollars))
    ax.axhline(0, linewidth=0.8, color=MUTED)
    ax.yaxis.grid(color="#d9e3eb")
    ax.legend(loc="upper right", frameon=False, fontsize=8.5)
    fig.savefig(ASSETS / "cross-platform-execution-cost-sensitivity.png", dpi=160)
    plt.close(fig)


if __name__ == "__main__":
    summary = read_summary()
    fee_comparison(summary)
    cost_sensitivity(summary)
