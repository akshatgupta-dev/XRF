#!/usr/bin/env python3
import argparse
import csv
from pathlib import Path

import matplotlib.pyplot as plt


def parse_args():
    parser = argparse.ArgumentParser(
        description="Plot summed XRF spectrum from simulation CSV (all detector pixels combined)."
    )
    parser.add_argument(
        "csv_file",
        nargs="?",
        help="Path to input CSV. If omitted, newest CSV under output/G4_Fe is used.",
    )
    parser.add_argument(
        "-o",
        "--output",
        default=None,
        help="Output PNG path. Default: <csv_stem>_spectrum.png",
    )
    parser.add_argument(
        "--xmax",
        type=float,
        default=None,
        help="Optional max X (keV) for zooming.",
    )
    parser.add_argument(
        "--logy",
        action="store_true",
        help="Use logarithmic Y axis.",
    )
    return parser.parse_args()


def newest_csv(default_dir: Path) -> Path:
    files = sorted(default_dir.glob("*.csv"), key=lambda p: p.stat().st_mtime, reverse=True)
    if not files:
        raise FileNotFoundError(f"No CSV files found in {default_dir}")
    return files[0]


def load_spectrum(csv_path: Path):
    # Key: bin index, Value: [bin_low_keV, bin_high_keV, summed_count]
    bins = {}

    with csv_path.open("r", newline="") as f:
        reader = csv.reader(f)
        for row in reader:
            if not row or row[0].startswith("#"):
                continue

            # Expected columns:
            # det_id,theta_deg,phi_deg,x_mm,y_mm,z_mm,bin_idx,bin_low_keV,bin_high_keV,count
            if len(row) < 10:
                continue

            try:
                bin_idx = int(float(row[6]))
                bin_low = float(row[7])
                bin_high = float(row[8])
                count = float(row[9])
            except ValueError:
                continue

            if bin_idx not in bins:
                bins[bin_idx] = [bin_low, bin_high, 0.0]
            bins[bin_idx][2] += count

    if not bins:
        raise ValueError("No spectrum rows parsed from CSV. Check file format.")

    sorted_bins = [bins[k] for k in sorted(bins.keys())]
    x = [(b[0] + b[1]) * 0.5 for b in sorted_bins]
    y = [b[2] for b in sorted_bins]
    return x, y


def main():
    args = parse_args()

    if args.csv_file:
        csv_path = Path(args.csv_file).expanduser().resolve()
    else:
        csv_path = newest_csv(Path("output/G4_Fe").resolve())

    if not csv_path.exists():
        raise FileNotFoundError(f"CSV file not found: {csv_path}")

    x, y = load_spectrum(csv_path)

    out_path = Path(args.output).expanduser().resolve() if args.output else csv_path.with_name(csv_path.stem + "_spectrum.png")

    plt.figure(figsize=(10, 5))
    plt.plot(x, y, linewidth=1.2)
    plt.xlabel("Energy (keV)")
    plt.ylabel("Counts (summed over all detector pixels)")
    plt.title(f"XRF Spectrum: {csv_path.name}")
    plt.grid(alpha=0.25)

    if args.logy:
        plt.yscale("log")

    if args.xmax is not None:
        plt.xlim(left=0.0, right=args.xmax)

    plt.tight_layout()
    plt.savefig(out_path, dpi=180)
    print(f"Input CSV: {csv_path}")
    print(f"Saved plot: {out_path}")


if __name__ == "__main__":
    main()
