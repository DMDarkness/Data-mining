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

The test dataset in these implementations is 'kosarak.dat', which is a common used dataset to test frequent pattern mining algorithms, and can be obtained from http://fimi.ua.ac.be/data/.
# FP-growth
FP-growth is a common used and classical algorithm to form frequent pattern from transactional dataset. 'fpgrowth.cpp' is an implementation done by myself, and the function 'void mian()' contains an example to use it.

# BPSOHD: binary particle swarm optimization (PSO) for mining dataset with high dimension
This algorithm, called BPSO-HD, is designed for mining long patterns from dataset with high number of items. The file 'bpsohd.cpp' is an implementation of it.

BPSO-HD is proposed in my paper, 'Zhang Z, Huang J, Wei Y. Frequent item sets mining from high-dimensional dataset based on a novel binary particle swarm optimization[J]. Journal of Central South University, 2016, 23(7): 1700-1708.' This paper can be found at https://link.springer.com/article/10.1007/s11771-016-3224-8.

The original BPSO-HD outputs the top-k long patterns by running k times. This implementation only runs BPSO-HD once, and outputs any pattern, which has been detected by at least one particle, with support higher than the minimum support.

The 'void main()' in bpsohd.cpp contains an example to use it.
# FIGF
'figf.cpp' is an implementation of the algorithm, called FI-GF, proposed in my paper, 'Zhang Z, Huang J, Wei Y. FI-FG: frequent item sets mining from datasets with high number of transactions by granular computing and fuzzy set theory[J]. Mathematical Problems in Engineering, 2015, 2015.', which can be found at http://downloads.hindawi.com/journals/mpe/2015/623240.pdf.

This algorithm firstly reduces the scale of dataset by information granularity, and then, it extracts the approximate frequent patterns from those granules. The method for information granularity is based on the Principle of Justifiable Granularity, whose details can be found in the paper 'Pedrycz W, Homenda W. Building the fundamentals of granular computing: A principle of justifiable granularity[J]. Applied Soft Computing, 2013, 13(10): 4209-4218.'

In this implementation, some modifications are made to improve the performance of the original FI-GF, where the specificity and coverage is designed according to the equation (3) and equation (4) in the paper of Prof. Pedrycz. and the highest value of specificity multiple coverage is detected when its current value is smaller than its last value.

The 'void main()' in figf.cpp contains an example to use it.
