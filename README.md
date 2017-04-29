# Data-mining
Some algorithms to form frequent itemsets/association rules from datasets, where many techniques, such as FP-tree, Apriori, PSO, GA, Granular computing, Central limit theorem and so on, are applied
# For Python 2.7
## Usage
Any `.pyd` can be put in the working directory and be imported by Python, which is also implemented by c++. The details of usage can be found in `How to use.py`.
## Algorithms
1. FP-Growth: `pyfpgrowth.pyd` is the corresponding `.pyd`.
2. BPSO-HD: A BPSO (Binary Particle Swarm Optimization) based algorithm mining long frequent patterns. `pybpsohd.pyd` is the corresponding `.pyd`. This is the corresponding [paper](https://www.researchgate.net/publication/305370239_Frequent_item_sets_mining_from_high-dimensional_dataset_based_on_a_novel_binary_particle_swarm_optimization?ev=prf_high)
3. CLT: A Central Limit Theorem based algorithm mining frequent patterns. `pyclt.pyd` is the corresponding `.pyd`.
4. ARMGA: A GA (Genetic Algorithm) based algorithm mining association rules. `pyarmga.pyd` is the corresponding `.pyd`.
