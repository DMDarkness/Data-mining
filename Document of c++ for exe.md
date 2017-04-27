# Frequent pattern/frequent itemset/association rule
Some algorithms to fast form frequent itemsets/patterns or association rules
# Transactional dataset
Dataset used in these implementations all look like the follows

1 2 3

1 2 6

3 7 9 18 33

13 77

1 5 8 22

...

Every row is a transaction, which contains some items, where any item is represented by an integer.

The test dataset in these implementations is `kosarak.dat`, which is a common used dataset to test frequent pattern mining algorithms, and can be obtained from http://fimi.ua.ac.be/data/.
# Basic algorithms
Some algorithms which are commonly used
## FP-growth
FP-growth is a common used and classical algorithm to form frequent pattern from transactional dataset. `fpgrowth.cpp` is an implementation done by me.

Usage: `fpgrowth <input-filename> <minimum-support>`

Example: `>fpgrowth kosarak.dat 0.01`

1. `<input-filename>`: A `char*` value, the file name of the transactional data.

2. `<minimum-support>`:A `double` value, the minimum support. If it is 0.01, the minimum support is 1%.
## Apriori
Apriori is also a common used algorithm to form frequent pattern. In `figf.cpp`, there is a parameter `Apri`. If `Apri>0`, `figf.cpp` becomes an implementation of Apriori.

Usage: `figf <is-Apriori-or-not> <input-filename> <minimum-support> <alpha> <t-norm>`

Example: `>figf 1 kosarak.dat 0.01 0.01 2`

1. `<is-Apriori-or-not>`:  An `int` value, if it is higher than 0, the `figf` becomes Apriori. If it is higher than 0, the input of `alpha` and `t-norm` will not affect the algorithm.

2. `<input-filename>`:     A `char*` value, the file name of the transactional data.

3. `<minimum-support>`:    A `double` value, the minimum support. If it is 0.01, the minimum support is 1%.

4. `<alpha>`:              A `double` value, the parameter to control the importance of specificity.

5. `<t-norm>`:             An `int` value, 1: minimum t-norm, other value: product t-norm
# Evolutionary algorithm
Some algorithms mining patterns or rules by evolutionary algorithm, which can efficiently extract those patterns or rules with very samll supports. 

For example, if you want to got patterns with length at least 20 in `kosarak.dat`, the minimum support has to be set lower than 0.00003, and based on this minimum support, Apriori becomes extremely slow, and fp-growth also becomes slow and may run out of memory, because the fp-tree will be very large.
## BPSO-HD: binary particle swarm optimization (PSO) for mining dataset with high dimension
This algorithm is designed for mining long patterns from dataset with high number of items. The file `bpsohd.cpp` is an implementation of it.

BPSO-HD is proposed in my paper, 'Zhang Z, Huang J, Wei Y. Frequent item sets mining from high-dimensional dataset based on a novel binary particle swarm optimization[J]. Journal of Central South University, 2016, 23(7): 1700-1708.' This paper can be found at https://link.springer.com/article/10.1007/s11771-016-3224-8.

The original BPSO-HD outputs the top-k long patterns by running k times. This implementation only runs BPSO-HD once, and outputs any pattern, which has been detected by at least one particle, with support higher than the minimum support.

Usage: `bpsohd <input-filename> <minimum-support> <population-size> <generation-number> <inertia-weight> <acceleration-1> <acceleration-2> <length>`

Example: `>bpsohd kosarak.dat 0.00001 20 30 0.5 1 1 20`

1. `<input-filename>`:    A `char*` value, the file name of the transactional data.

2. `<minimum-support>`:   A `double` value, the minimum support. If it is 0.01, the minimum support is 1%.

3. `<population-size>`:   An `int` value, the population size of PSO.

4. `<generation-number>`: An `int` value, the number of generation of PSO.

5. `<inertia-weight>`:    A `double` value, the inertia weight of PSO.

6. `<acceleration-1>`:    A `double` value, the first acceleration of PSO.

7. `<acceleration-2>`:    A `double` value, the second acceleration of PSO.

8. `<length>`:            An `int` value, the minimum length of the patterns going to be extracted out.
## ARMGA: Association rules mining by genetic algorithm
ARMGA is an algorithm mining association rules without setting minimum support, and `armga.cpp` is an implementation of it. The detail of ARMGA can be found in the paper http://www.sciencedirect.com/science/article/pii/S0957417408000195.

Furthermore, the original ARMGA outputs the final population. In this implementation, ARMGA outputs any rules, which has been detected, with confidence and lift higher than the minimum confidence and minimum lift.

Usage: `Usage: armga <input-filename> <minimum-confidence> <minimum-lift> <population size> <generation-number> <mutation-probability> <selection-probability> <crossover-probability> <length>`

Example: `>armga kosarak.dat 0.5 1 20 30 0.3 0.95 0.95 10`

1. `<input-filename>`:    A `char*` value, the file name of the transactional data.

2. `<minimum-confidence>`:   A `double` value, the minimum confidence. If it is 0.5, the minimum support is 50%.

3. `<minimum-lift>`:   A `double` value, the minimum lift.

4. `<population-size>`:   An `int` value, the population size of GA.

5. `<generation-number>`: An `int` value, the number of generation of GA.

6. `<mutation probability>`:    A `double` value, the mutation probability of GA.

7. `<selection probability>`:    A `double` value, the selection probability of GA.

8. `<crossover probability>`:    A `double` value, the crossover probability of GA.

9. `<length>`:            An `int` value, the length of the rules going to be extracted out.
# Data reduction based algorithms
Some algorithms reduce the scale of dataset first to save the time for scanning.
## FIGF: frequent itemsets mining through granular computing (GrC) and fuzzy theory
`figf.cpp` is an implementation of the algorithm, called FI-GF, proposed in my paper, 'Zhang Z, Huang J, Wei Y. FI-FG: frequent item sets mining from datasets with high number of transactions by granular computing and fuzzy set theory[J]. Mathematical Problems in Engineering, 2015, 2015.', which can be found at http://downloads.hindawi.com/journals/mpe/2015/623240.pdf.

This algorithm firstly reduces the scale of dataset by information granularity, and then, it extracts the approximate frequent patterns from those granules. The method for information granularity is based on the Principle of Justifiable Granularity, whose details can be found in the paper 'Pedrycz W, Homenda W. Building the fundamentals of granular computing: A principle of justifiable granularity[J]. Applied Soft Computing, 2013, 13(10): 4209-4218.'

In this implementation, some modifications are made to improve the performance of the original FI-GF, where the specificity and coverage is designed according to the equation (3) and equation (4) in the paper of Prof. Pedrycz. and the highest value of specificity multiple coverage is detected when its current value is smaller than its last value.

Furthermore, instead of using the t-norm designed in the paper of FI-GF, we use the minimum t-norm and product t-norm in this implementation.

Usage: `figf <is-Apriori-or-not> <input-filename> <minimum-support> <alpha> <t-norm>`

Example: `>figf 0 kosarak.dat 0.01 0.01 2`

1. `<is-Apriori-or-not>`:  An `int` value, if it is higher than 0, the `figf` becomes Apriori. If it is higher than 0, the input of `alpha` and `t-norm` will not affect the algorithm.

2. `<input-filename>`:     A `char*` value, the file name of the transactional data.

3. `<minimum-support>`:    A `double` value, the minimum support. If it is 0.01, the minimum support is 1%.

4. `<alpha>`:              A `double` value, the parameter to control the importance of specificity.

5. `<t-norm>`:             An `int` value, 1: minimum t-norm, other value: product t-norm
## CLT: Mining frequent itemsets by central limit theorem
`clt.cpp` is an implementation of the algorithm proposed in https://link.springer.com/chapter/10.1007/978-3-540-30549-1_35?no-access=true. This algorithm uses sampling to reduce the scale of dataset. Meanwhile, through taking the central limit theorem, this algorithm can limit the error caused by sampling in a small range with high probability.

Usage: `clt <input-filename> <minimum-support> <error-range> <max-probabiltiy> <transaction-number>`

Example: `>clt kosarak.dat 0.03 0.005 0.005 990002`

1. `<input-filename>`:    A `char*` value, the file name of the transactional data.

2. `<minimum-support>`:   A `double` value, the minimum support. If it is 0.01, the minimum support is 1%.

3. `<error-range>`:   A `double` value, the maximum range of the deviation of frequency of any pattern caused by sampling, 0.005 means [-0.5%, 0.5%].

4. `<max-probabiltiy>`:   A `double` value, the maximum probability for the error to be out of the error range, 0.005 means 0.5%.

5. `<transaction-number>`:   An `int` value, the number of transactions before sampling.
