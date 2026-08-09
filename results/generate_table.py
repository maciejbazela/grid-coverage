#!/usr/bin/env python3
import csv
import sys
import re
from collections import defaultdict

ALGORITHMS = ["deep", "random", "warnsdorff", "heuristic"]


def main():
    if len(sys.argv) < 2:
        print("Usage: generate_table.py <result.csv>", file=sys.stderr)
        sys.exit(1)

    path = sys.argv[1]
    factor_match = re.search(r"benchmark_([\d.]+)_steps\.csv", path)
    factor = factor_match.group(1) if factor_match else "?"

    rows = defaultdict(dict)
    dataset_order = []

    with open(path, newline="") as f:
        reader = csv.DictReader(f, delimiter=";")
        for row in reader:
            ds = row["dataset"]
            alg = row["algorithm"]
            val = int(row["unique_cells"])
            if ds not in rows:
                dataset_order.append(ds)
            rows[ds][alg] = val

    print(f"## MAX_STEPS = {factor} * cell_count\n")
    header = "| dataset | " + " | ".join(ALGORITHMS) + " |"
    sep = "|---" * (len(ALGORITHMS) + 1) + "|"
    print(header)
    print(sep)

    for ds in dataset_order:
        values = {alg: rows[ds].get(alg, "-") for alg in ALGORITHMS}
        best = max(v for v in values.values() if isinstance(v, int))
        formatted = []
        for alg in ALGORITHMS:
            v = values[alg]
            if v == best:
                formatted.append(f"***{v}***")
            else:
                formatted.append(str(v))
        print(f"| {ds} | " + " | ".join(formatted) + " |")
    print()


if __name__ == "__main__":
    main()
