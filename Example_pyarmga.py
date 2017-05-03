# -*- coding: utf-8 -*-
"""
Created on Wed May 03 16:12:18 2017

@author: ZhongjieZhang

This is an example for pyarmga
"""
import pyarmga as ga#import the package of BPSO-HD
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

#mine long rules form dataset without minimum support
#with the minimum confidence 75% and minimum lift 1
#the size of population is set to 30
#the number of generation is set to 30
#the mutation probability is 25%
#the selection probability is 100%
#the crossover probability is 100%
#the length of rules is 10
#it may take some time
ar=ga.getAR(dataset, 0.7, 1, 30, 30, 0.25, 1, 1, 10)