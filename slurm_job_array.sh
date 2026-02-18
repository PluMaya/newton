#!/bin/bash
#SBATCH --job-name=multivalued_heuristic_search
#SBATCH --output=logs/%x_%A_%a.out
#SBATCH --error=logs/%x_%A_%a.err
#SBATCH --time=02:00:00
#SBATCH --mem=4G
#SBATCH --nodelist=newton3,newton4,galileo1 # Request run only on nodes newton3/newton4/galileo1
#SBATCH --array=1-$(wc -l < queries.txt)
#SBATCH --mail-user=wo@cs.technion.ac.il
#SBATCH --mail-type=ALL           # Valid values are NONE, BEGIN, END, FAIL, REQUEUE, ALL
#SBATCH --cpus-per-task=1         # number of cores (treats)

# Go to the submission directory
cd $SLURM_SUBMIT_DIR

# Read the line corresponding to this array task
LINE=$(sed -n "${SLURM_ARRAY_TASK_ID}p" queries.txt)

export LD_LIBRARY_PATH=$HOME/boost_install/lib:$LD_LIBRARY_PATH

# Split line into variables
read -r MAP ALG E1 E2 GOAL REST <<< "$LINE"
STARTS="$REST"

# Create unique output folder
OUTDIR=output/run_${SLURM_ARRAY_TASK_ID}_${MAP}_${ALG}_${GOAL}
mkdir -p "$OUTDIR"

# Run your executable
./MultivaluedHeuristicSearch \
  --map resources/$MAP \
  --goal $GOAL \
  --algorithm $ALG \
  --e1 $E1 \
  --e2 $E2 \
  --logging_file "$OUTDIR" \
  --multi_source $STARTS \
  > "$OUTDIR/output.txt" 2>&1