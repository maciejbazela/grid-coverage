#!/usr/bin/env bash
set -euo pipefail

GEN_DIR="./build/generators"
DATA_DIR="./data/benchmark"

[[ -x "$GEN_DIR/random" ]] || { echo "Run 'make generators' first" >&2; exit 1; }

mkdir -p "$DATA_DIR"

gen() {
    local name="$1"; shift
    local out="$DATA_DIR/$name"
    if [[ -f "$out" ]]; then
        echo "  skip $name (exists)"
        return
    fi
    echo "  $name"
    "$GEN_DIR/$1" "${@:2}" "$out"
}

# --- random ---
for rows in 10 20 50 100 200; do
    for cols in 10 20 50 100 200; do
        [[ "$rows" == "$cols" ]] || continue
        for p in 0.1 0.3 0.5; do
            gen "random_${rows}_${cols}_${p}.ascii" random "$rows" "$cols" "$p"
        done
    done
done

# --- maze ---
for rows in 11 25 51 101 201; do
    for cols in 11 25 51 101 201; do
        [[ "$rows" == "$cols" ]] || continue
        for p in 0 0.1 0.3; do
            gen "maze_${rows}_${cols}_${p}.ascii" maze "$rows" "$cols" "$p"
        done
    done
done

# --- components ---
sizes=(10 20 50 100 200)
ks=(3 5 10 15 30)
minsizes=(4 8 15 25 40)
maxsizes=(12 25 50 80 150)

for i in "${!sizes[@]}"; do
    rows="${sizes[$i]}"
    cols="${sizes[$i]}"
    k="${ks[$i]}"
    minsize="${minsizes[$i]}"
    maxsize="${maxsizes[$i]}"
    gen "components_${rows}_${cols}_${k}_${minsize}_${maxsize}.ascii" \
        components "$rows" "$cols" "$k" "$minsize" "$maxsize"
done
