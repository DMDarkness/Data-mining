# Efficient-approximate-Frequent-patterns-association-rules-mining
Some algorithms to fast form high quality approxiamte frequent itemsets or association rules from datasets with large scales of transactions or items
# BPSOHD: binary particle swarm optimization (PSO) for mining dataset with high dimension
This algorithm, called BPSO-HD, is designed for mining long patterns from dataset with high number of items. The file 'bpsohd.cpp' is an implemention of it.

BPSO-HD is proposed in my paper, 'Zhang Z, Huang J, Wei Y. Frequent item sets mining from high-dimensional dataset based on a novel binary particle swarm optimization[J]. Journal of Central South University, 2016, 23(7): 1700-1708.'

The original BPSO-HD outputs the top-k long patterns by running k times. This implementation only runs BPSO-HD once, and outputs any pattern, which has been detected by at least one particle, with support higher than the minimum support.

The 'void main()' in bpsohd.cpp is actually an example to use it, where the test dataset, kosarak.dat, is a common used dataset to test freqeunt patterns mining algorithm, which can be obtained from http://fimi.ua.ac.be/data/.
