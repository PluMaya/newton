#!/bin/bash
#SBATCH --job-name=multivalued_heuristic_search
#SBATCH --output=logs/%x_%A_%a.out
#SBATCH --error=logs/%x_%A_%a.err
#SBATCH --time=02:00:00
#SBATCH --mem=4G
#SBATCH --nodelist=newton3,newton4,galileo1 # Request run only on nodes newton3/newton4/galileo1
#SBATCH --array=1-2 # TODO check this every time!
#SBATCH --mail-user=wo@cs.technion.ac.il
#SBATCH --mail-type=ALL           # Valid values are NONE, BEGIN, END, FAIL, REQUEUE, ALL
#SBATCH --cpus-per-task=1         # number of cores (treats)

# Go to the submission directory
cd $SLURM_SUBMIT_DIR

# Read the line corresponding to this array task
LINE=$(sed -n "${SLURM_ARRAY_TASK_ID}p" heuristic_queries.txt)

export LD_LIBRARY_PATH=$HOME/boost_install/lib:$LD_LIBRARY_PATH


# Split line into variables
read -r MAP ALG GOAL <<< "$LINE"
STARTS="$REST"

# Create unique output folder
OUTDIR=output

# Run your executable
./build/MultivaluedHeuristicSearch \
  --map resources/$MAP \
  --algorithm $ALG \
  --goal $GOAL \
  > "${OUTDIR}/output_${MAP}_${GOAL}_${ALG}.txt" 2>&1
