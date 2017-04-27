# -*- coding: utf-8 -*-
"""
Created on Thu Apr 27 16:27:17 2017

@author: ZhongjieZhang
"""

import pybpsohd#import the module of BPSO-HD
import pyfpgrowth#import the module of FP-growth
import pyclt#import the module of Central limit theorem

#The dataset used in this example, which is the first 10 transactions of 'kosarak.dat'
dataset=[[1, 2, 3]
, [1]
, [4, 5, 6, 7]
, [1, 8]
, [9, 10]
, [11, 6, 12, 13, 14, 15, 16]
, [1, 3, 7]
, [17, 18]
, [11, 6, 19, 20, 21, 22, 23, 24]
, [1, 25, 3]]

fi=pyfpgrowth.getFP(dataset, 0.1)#the example of pyfpgrowth
"""
<fps>=pyfpgrowth.getFP(<dataset>,<minsup>)
Input:
<dataset>: a list of transactions, where every transaction           is list containing integers higher than 0, and
           every integer represents an item.
<minsup>:  a double value located in [0, 1], which is the mnimum support
           represented by fraction, where 0.25 represent 25%.
Output:
<fps>:     a list of frequent itemsets, where every frequent
           itemset is also a list, whose last value is the
           absolute support, and other values are the items
           of this frequent itemset.
"""

fi2=pybpsohd.getFP(dataset, 0.01, 30, 30, 0.5, 1, 1, 2)#the example of pybpsohd
"""
<fps>=getFP(<dataset>,<minsup>,<Popsize>,<GenNum>,<omiga>,<c1>,<c2>,<length>)
Input:
<dataset>: a list of transactions, where every transaction
           is list containing integers higher than 0, and
           every integer represents an item.
<minsup>:  a double value located in [0, 1], which is the mnimum support
           represented by fraction, where 0.25 represent 25%.
<Popsize>: integer higher than 0, the size of population.
<GenNum>:  integer higher than 0, the number of genertions.
<omiga>:   double higher than 0, the interia wieght.
<c1>:      double higher than 0, the first accelaration.
<c2>:      double higher than 0, the second accelaration.
<length>:  integer higher than 0, the minimum length of pattern.
Output:
<fps>:     a list of frequent itemsets, where every frequent
           itemset is also a list, whose last value is the
           absolute support, and other values are the items
           of this frequent itemset.
Paper:
-----------------------------------------------------------------------------------------------          
Zhang Z, Huang J, Wei Y, et al. Frequent item sets mining from high-dimensional
dataset based on a novel binary particle swarm optimization[J]. Journal of Central
South University, 2016, 23(7): 1700-1708.
-----------------------------------------------------------------------------------------------           
"""

[fi3, SampleN]=pyclt.getFP(dataset, 0.1, 0.01, 0.01)#the example of central limit theorem based algorithm
"""
[<fps>,<SampleN>]=pyfpgrowth.getFP(<dataset>,<minsup>,<er>,<prob>)
Input:
<dataset>: a list of transactions, where every transaction
           is list containing integers higher than 0, and
           every integer represents an item.
<minsup>:  a double value located in [0, 1], which is the mnimum support
           represented by fraction, where 0.25 represent 25%.
<er>:      a double value located in [0, 1], the maximum range of deviation
           of support of any itemset caused by sampling. 0.01 means that
           the deviation of support of any itemset is limited in [-1%, 1%].
<prob>:    a double value located in [0, 1], the maximum probability for
           the deviation of support to not be located in [-er, er]. 0.01
           means 1%.
Output:
<fps>:     a list of frequent itemsets, where every frequent
           itemset is also a list, whose last value is the
           absolute support in the sample, and other values 
           are the items of this frequent itemset.
<SampleN>: the size of sample accorrding to <er> and <prob>           
"""
