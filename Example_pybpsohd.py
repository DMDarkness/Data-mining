# -*- coding: utf-8 -*-
"""
Created on Wed May 03 16:02:51 2017

@author: ZhongjieZhang

This is an example for pybpsohd
"""

import pybpsohd as pybp#import the package of BPSO-HD
import re#import the regular expression, which is used to split the string and exact items from transaction

def read(dName):  
    f = open(dName)   
    Trans=[]
    content  = f.readlines()#read all the lines from the transactional dataset
    for line in content:
        sline=re.split(' |\n',line)#split the dataset by ' '
        trans=[]
        for item in sline:
            if item.isdigit():
                trans.append(int(item))
        if(len(trans)>0):
            Trans.append(trans)
    f.close()
    return Trans
    
#read the transactional dataset and store it in dataset
#the dataset kosarak.dat can be downloaded from http://fimi.ua.ac.be/data/
dataset=read("kosarak.dat")

#mine long patterns form dataset
#with the minimum support 0.001% with length 10
#the size of population is set to 30
#the number of generation is set to 30
#the interia weight is set to 0.5
#the two accelerations are both set to 1
fi=pybp.getFP(dataset, 0.00001, 30, 30, 0.5, 1, 1, 10)

"""
paper:
-----------------------------------------------------------------------------------------------          
Zhang Z, Huang J, Wei Y, et al. Frequent item sets mining from high-dimensional
dataset based on a novel binary particle swarm optimization[J]. Journal of Central
South University, 2016, 23(7): 1700-1708.
-----------------------------------------------------------------------------------------------           
"""