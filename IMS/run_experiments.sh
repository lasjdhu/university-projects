#!/bin/bash

EXE="./main"
RUNS=150
OUTPUT_FILE="docs/results.txt"

if [ ! -f "$EXE" ]; then
    exit 1
fi

rm -f "$OUTPUT_FILE"

for i in $(seq 1 $RUNS); do
    if (( i % 5 == 0 )); then echo -n "."; fi

    $EXE >> "$OUTPUT_FILE"
done

awk '
/^Exp/ {
    # Key is Column 1 (Exp ID)
    count[$1]++;
    sum_time[$1] += $5;
    sum_wait[$1] += $6;
}
END {
    printf "%-15s %-15s %-15s\n", "EXPERIMENT", "AVG TIME (h)", "AVG WAIT (min)"
    print "-------------------------------------------------------"

    for (id in count) {
        avg_t = sum_time[id] / count[id];
        avg_w = sum_wait[id] / count[id];
        printf "%-15s %-15.2f %-15.2f\n", id, avg_t, avg_w;
    }
}
' "$OUTPUT_FILE" | sort
