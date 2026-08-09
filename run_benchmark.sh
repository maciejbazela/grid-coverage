#!/usr/bin/env bash
set -euo pipefail

MAIN="./build/main"
DATA_DIR="./data/benchmark"
RESULTS_DIR="./results"
PATHS_DIR="$RESULTS_DIR/paths"
ALGORITHMS=("deep" "random" "warnsdorff" "heuristic")
SAVE_PATHS=true

for arg in "$@"; do
    case "$arg" in
        --no-paths) SAVE_PATHS=false ;;
        *) echo "Unknown argument: $arg" >&2; exit 1 ;;
    esac
done

cleanup() { rm -f /tmp/gc_stdout.txt /tmp/gc_time.txt /tmp/gc_valgrind.txt; }
trap cleanup EXIT

[[ -d "$DATA_DIR" ]] || { echo "Directory $DATA_DIR not found" >&2; exit 1; }
[[ -x "$MAIN" ]] || { echo "Binary not found, run 'make' first" >&2; exit 1; }
[[ -n "${MAX_STEPS:-}" ]] || { echo "MAX_STEPS is not set, run this script with MAX_STEPS=<value> at the beginning of the command" >&2; exit 1; }

shopt -s nullglob # If below glob fails, files will be empty
files=("$DATA_DIR"/*)
[[ ${#files[@]} -gt 0 ]] || { echo "No files in $DATA_DIR" >&2; exit 1; }

if $SAVE_PATHS; then mkdir -p "$PATHS_DIR"; fi

compute_max_steps() {
    local file="$1"
    local name
    name="$(basename "$file")"
    # Filename format should be, see generate_benchmark_data: <gridtype>_<rows>_<cols>_...
    local rest="${name#*_}"
    local rows="${rest%%_*}"
    local cols="${rest#*_}"
    cols="${cols%%_*}"
    local total=$(( rows * cols ))
    if [[ "$MAX_STEPS" == *.* ]]; then
        awk "BEGIN { printf \"%d\", $MAX_STEPS * $total }"
    else
        echo "$MAX_STEPS"
    fi
}

if $SAVE_PATHS; then
    echo "dataset;algorithm;max_steps;unique_cells;elapsed_sec;bytes_allocated;leaks;result"
else
    echo "dataset;algorithm;max_steps;unique_cells;elapsed_sec;bytes_allocated;leaks"
fi

for data_file in "${files[@]}"; do
    name="$(basename "$data_file")"
    for algo in "${ALGORITHMS[@]}"; do
        elapsed="N/A"
        bytes_alloc="N/A"
        unique_cells="N/A"
        leaks="N/A"
        result=""

        steps=$(compute_max_steps "$data_file")

        if /usr/bin/time -o /tmp/gc_time.txt -f '%e' "$MAIN" "$algo" "$MAX_STEPS" "$data_file" > /tmp/gc_stdout.txt 2>&1; then
            result=$(grep -oP '^\(.*\)' /tmp/gc_stdout.txt | tr -d '\n')
            unique_cells=$(grep -oP ',\s*\K\d+$' /tmp/gc_stdout.txt || echo "0")
            elapsed="$(cat /tmp/gc_time.txt)"
        fi

        if $SAVE_PATHS; then cp /tmp/gc_stdout.txt "$PATHS_DIR/${name%.*}_${algo}_${MAX_STEPS}.txt"; fi

        if command -v valgrind &>/dev/null; then
            set +e
            valgrind --leak-check=full --error-exitcode=100 "$MAIN" "$algo" "$MAX_STEPS" "$data_file" > /dev/null 2> /tmp/gc_valgrind.txt
            retcode=$?
            set -e
            if [[ $retcode -eq 0 ]]; then leaks="no"; else leaks="yes"; fi
            bytes_alloc=$(grep -oP 'frees,\s*\K[\d,]+(?=\s*bytes allocated)' /tmp/gc_valgrind.txt | tr -d ',')
        fi

        if $SAVE_PATHS; then
            echo "$name;$algo;$steps;$unique_cells;$elapsed;$bytes_alloc;$leaks;$result"
        else
            echo "$name;$algo;$steps;$unique_cells;$elapsed;$bytes_alloc;$leaks"
        fi
    done
done
