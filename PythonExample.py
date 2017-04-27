# -*- coding: utf-8 -*-
"""
Created on Thu Apr 27 16:27:17 2017

@author: ZhongjieZhang
"""

import pybpsohd#import the module of BPSO-HD
import pyfpgrowth#import the module of FP-growth
import pyclt#import the module of Central limit theorem

#The dataset used in this example, which is the first 10 transactions of 'mushroom'
dataset=[[1, 3, 9, 13, 23, 25, 34, 36, 38, 40, 52, 54, 59, 63, 67, 76, 85, 86, 90, 93, 98, 107, 113]
, [2, 3, 9, 14, 23, 26, 34, 36, 39, 40, 52, 55, 59, 63, 67, 76, 85, 86, 90, 93, 99, 108, 114]
, [2, 4, 9, 15, 23, 27, 34, 36, 39, 41, 52, 55, 59, 63, 67, 76, 85, 86, 90, 93, 99, 108, 115]
, [1, 3, 10, 15, 23, 25, 34, 36, 38, 41, 52, 54, 59, 63, 67, 76, 85, 86, 90, 93, 98, 107, 113]
, [2, 3, 9, 16, 24, 28, 34, 37, 39, 40, 53, 54, 59, 63, 67, 76, 85, 86, 90, 94, 99, 109, 114]
, [2, 3, 10, 14, 23, 26, 34, 36, 39, 41, 52, 55, 59, 63, 67, 76, 85, 86, 90, 93, 98, 108, 114]
, [2, 4, 9, 15, 23, 26, 34, 36, 39, 42, 52, 55, 59, 63, 67, 76, 85, 86, 90, 93, 98, 108, 115]
, [2, 4, 10, 15, 23, 27, 34, 36, 39, 41, 52, 55, 59, 63, 67, 76, 85, 86, 90, 93, 99, 107, 115]
, [1, 3, 10, 15, 23, 25, 34, 36, 38, 43, 52, 54, 59, 63, 67, 76, 85, 86, 90, 93, 98, 110, 114]
, [2, 4, 9, 14, 23, 26, 34, 36, 39, 42, 52, 55, 59, 63, 67, 76, 85, 86, 90, 93, 98, 107, 115]]

fi=pyfpgrowth.getFP(dataset, 0.25)#the example of pyfpgrowth
"""
<fps>=pyfpgrowth.getFP(<dataset>,<minsup>)
Input:
<dataset>: a list of transactions, where every transaction
           is list containing integers higher than 0, and
           every integer represents an item.
<minsup>:  a double value located in [0, 1], which is the mnimum support
           represented by fraction, where 0.25 represent 25%.
Output:
<fps>:     a list of frequent itemsets, where every frequent
           itemset is also a list, whose last value is the
           absolute support, and other values are the items
           of this frequent itemsets.
"""

fi2=pybpsohd.getFP(dataset, 0.05, 30, 30, 0.5, 1, 1, 10)#the example of pybpsohd
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
           of this frequent itemsets.
Paper:
-----------------------------------------------------------------------------------------------          
Zhang Z, Huang J, Wei Y, et al. Frequent item sets mining from high-dimensional
dataset based on a novel binary particle swarm optimization[J]. Journal of Central
South University, 2016, 23(7): 1700-1708.
-----------------------------------------------------------------------------------------------           
"""

[fi3, SampleN]=pyclt.getFP(dataset, 0.25, 0.01, 0.01)#the example of central limit theorem based algorithm
"""
[<fps>,<SampleN>]=pyfpgrowth.getFP(<dataset>,<minsup>,<er>,<prob>)
Input:
<dataset>: a list of transactions, where every transaction
           is list containing integers higher than 0, and
           every integer represents an item.
<minsup>:  a double value located in [0, 1], which is the mnimum support
           represented by fraction, where 0.25 represent 25%.
<er>:      a double value located in [0, 1], the maximum range of deviation
           of support of any itemsets caused by sampling. 0.01 means that
           the deviation of support of any itemset is limited in [-1%, 1%].
<prob>:    a double value located in [0, 1], the maximum probability for
           the deviation of support to not be located in [-er, er]. 0.01
           means 1%.
Output:
<fps>:     a list of frequent itemsets, where every frequent
           itemset is also a list, whose last value is the
           absolute support, and other values are the items
           of this frequent itemsets.
<SampleN>: the size of sample accorrding to <er> and <prob>           
"""
