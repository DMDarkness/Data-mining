# Approximate data mining for frequent pattern/association rules
Some algorithms to fast form high quality approxiamte frequent itemsets or association rules from datasets with large scales of transactions or items
# Transactional dataset
Dataset used in these implementations all look like the follows

1 2 3

1 2 6

3 7 9 18 33

13 77

1 5 8 22

...

Every row is a transaction, which contains some items, where any item is represented by an integar.

The test dataset in these implementations is kosarak.dat, which is a common used dataset to test frequent pattern mining algorithms, and can be obtained from http://fimi.ua.ac.be/data/.
# FP-growth
FP-growth is a common used and classical algorithm to form frequent pattern from transactional dataset. 'fpgrowth.cpp' is an implementation done by myself, and the function 'void mian()' contains an example to use it.

# BPSOHD: binary particle swarm optimization (PSO) for mining dataset with high dimension
This algorithm, called BPSO-HD, is designed for mining long patterns from dataset with high number of items. The file 'bpsohd.cpp' is an implemention of it.

BPSO-HD is proposed in my paper, 'Zhang Z, Huang J, Wei Y. Frequent item sets mining from high-dimensional dataset based on a novel binary particle swarm optimization[J]. Journal of Central South University, 2016, 23(7): 1700-1708.'

The original BPSO-HD outputs the top-k long patterns by running k times. This implementation only runs BPSO-HD once, and outputs any pattern, which has been detected by at least one particle, with support higher than the minimum support.

The 'void main()' in bpsohd.cpp contains an example to use it.
