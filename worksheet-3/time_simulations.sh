#!/bin/bash

GENXYZ="./genxyz"
READXYZ="./readxyz"
FLAGS="--pure_output"

# <box-size> range
BOX_START=8
BOX_END=45
BOX_STEP=1

# <density> range
DENSITY_START=0.003
DENSITY_END=0.025
DENSITY_STEP=0.001

NUM_RUNS=5
OUTPUT_FILE="simulation_results.csv"

# CSV header
echo "box_size,density,ep_iter,ep_grid,iteration,grid,speedup" > "$OUTPUT_FILE"

# loop over box sizes and densities
for BOX_SIZE in $(seq $BOX_START $BOX_STEP $BOX_END); do
DENSITY=$DENSITY_START
    while (( $(echo "$DENSITY<=$DENSITY_END" | bc -l) )); do
        # generate position file
        POSITION_FILE="box_s${BOX_SIZE}_d${DENSITY}.xyz"
        $GENXYZ $BOX_SIZE $DENSITY $POSITION_FILE

        # initialize accumulators for averaging
        SUM_EP_ITER=0
        SUM_EP_GRID=0
        SUM_DT_ITER=0
        SUM_DT_GRID=0
        SUM_SPEEDUP=0

        # run the simulation multiple times
        for (( i=1; i<=NUM_RUNS; i++ )); do
            OUTPUT=$($READXYZ $BOX_SIZE $POSITION_FILE $FLAGS)
            read EP_ITER EP_GRID DT_ITER DT_GRID SPEEDUP <<< "$OUTPUT"

            SUM_EP_ITER=$(echo "$SUM_EP_ITER + $EP_ITER" | bc -l)
            SUM_EP_GRID=$(echo "$SUM_EP_GRID + $EP_GRID" | bc -l)
            SUM_DT_ITER=$(echo "$SUM_DT_ITER + $DT_ITER" | bc -l)
            SUM_DT_GRID=$(echo "$SUM_DT_GRID + $DT_GRID" | bc -l)
            SUM_SPEEDUP=$(echo "$SUM_SPEEDUP + $SPEEDUP" | bc -l)
        done

        # average over NUM_RUNS runs
        AVG_EP_ITER=$(echo "$SUM_EP_ITER / $NUM_RUNS" | bc -l)
        AVG_EP_GRID=$(echo "$SUM_EP_GRID / $NUM_RUNS" | bc -l)
        AVG_DT_ITER=$(echo "$SUM_DT_ITER / $NUM_RUNS" | bc -l)
        AVG_DT_GRID=$(echo "$SUM_DT_GRID / $NUM_RUNS" | bc -l)
        AVG_SPEEDUP=$(echo "$SUM_SPEEDUP / $NUM_RUNS" | bc -l)

        # append results to CSV file
        echo "$BOX_SIZE,$DENSITY,$AVG_EP_ITER,$AVG_EP_GRID,$AVG_DT_ITER,$AVG_DT_GRID,$AVG_SPEEDUP" >> "$OUTPUT_FILE"
        echo "Completed box size: $BOX_SIZE, density: $DENSITY (averaged over $NUM_RUNS runs)"
        rm -f "$POSITION_FILE"

        DENSITY=$(echo "$DENSITY + $DENSITY_STEP" | bc -l)
    done
done

echo "All simulations completed. Results saved in $OUTPUT_FILE"
