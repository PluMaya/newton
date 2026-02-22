#!/bin/bash
#SBATCH --job-name=multivalued_heuristic_search_queries
#SBATCH --output=logs/%x_%A_%a.out
#SBATCH --error=logs/%x_%A_%a.err
#SBATCH --mem=4G
#SBATCH --nodelist=newton8,newton9,protagoras # Request run only on nodes newton3/newton4/galileo1
#SBATCH --array=1-10 # TODO check this every time!
#SBATCH --mail-user=wo@cs.technion.ac.il
#SBATCH --mail-type=ALL           # Valid values are NONE, BEGIN, END, FAIL, REQUEUE, ALL
#SBATCH --cpus-per-task=1         # number of cores (treats)

# Go to the submission directory
cd $SLURM_SUBMIT_DIR

# Read the line corresponding to this array task
LINE=$(sed -n "${SLURM_ARRAY_TASK_ID}p" queries.txt)

export LD_LIBRARY_PATH=$HOME/boost_install/lib:$LD_LIBRARY_PATH


# Split line into variables
read -r MAP ALG GOAL E1 E2 MVH REST <<< "$LINE"
STARTS="$REST"

# Create unique output folder
OUTDIR=output

# Run your executable
./build/MultivaluedHeuristicSearch \
  --map resources/$MAP \
  --goal $GOAL \
  --algorithm $ALG \
  --e1 $E1 \
  --e2 $E2 \
  --mvh $MVH \
  --logging_file "${OUTDIR}/run_${RUN}_${MAP}_${ALG}_${GOAL}_${E1}_${E2}" \
  --multi_source $STARTS \
  > "${OUTDIR}/output_${RUN}_${MAP}_${GOAL}_${ALG}_${E1}_${E2}.txt" 2>&1