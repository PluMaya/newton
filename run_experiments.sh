#!/bin/bash

EXEC=./cmake-build-debug/MultivaluedHeuristicSearch
MAP_DIR=resources
RESULTS=output
PRECOMP=precomputed

mkdir -p $RESULTS

RUN=0

while read MAP ALG E1 E2 GOAL SVH MVH STARTS
do
    RUN=$((RUN+1))
    echo "Running $RUN"

    $EXEC \
      --map $MAP_DIR/$MAP \
      --start 0 \
      --goal $GOAL \
      --algorithm $ALG \
      --e1 $E1 \
      --e2 $E2 \
      --logging_file $RESULTS/run_${RUN}_${MAP}_${ALG}_${GOAL}_${E1}_${E2} \
      --multi_source $STARTS\
      > $RESULTS/output_${RUN}_${MAP}_${ALG}_${GOAL}_${E1}_${E2}.txt 2>&1


done < big_queries.txt