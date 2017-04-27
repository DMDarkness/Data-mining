# Data-mining
Some algorithms to form frequent itemsets/association rules from datasets, where many techniques, such as FP-tree, Apriori, PSO, GA, Granular computing, Central limit theorem and so on, are applied
# For console in windows
## Usage
Every single `.cpp`, except for those file with prefix `py`, can be compiled to an `.exe` file. The details can be found in cDocument.md.
## Algorithms
1. FP-Growth: `fpgrowth.cpp` is an implementation.
2. Apriori: `figf.cpp`(parameter Apri>0) is an implementation.
2. BPSO-HD: `bpsohd.cpp` is an implementation, which is a BPSO based algorithm mining long frequent patterns.
3. ARMGA: `armga.cpp` is an implementation, which is a GA based association rules mining algorithm.
4. FI-GF: `figf.cpp`(parameter Apri<=0) is an implementation, which is a GrC and fuzzy theory based algorithm mining frequent patterns.
5. CLT: `clt.cpp` is an implementation, which is a Central Limit Theorem based algorithm mining frequent patterns.
# For Python 2.7
## Usage
Any `.pyd` and can be put in the working directory and be imported by Python. The details of usage can be found by the `help` of Python after importing.
## Algorithms
1. FP-Growth: `pyfpgrowth.pyd` is the corresponding `.pyd`, and `PythonExample.py` contains an example.
2. BPSO-HD: `pybpsohd.pyd` is the corresponding `.pyd`, and `PythonExample.py` contains an example.
3. CLT: `pyclt.pyd` is the corresponding `.pyd`, and `PythonExample.py` contains an example.
