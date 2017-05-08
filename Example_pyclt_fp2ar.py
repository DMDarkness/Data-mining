# -*- coding: utf-8 -*-
"""
Created on Mon May 08 10:01:31 2017

@author: ZhongjieZhang
"""

import pyclt#import the package of central limit theorem based algorithm
import fp2ar#import the package to form association rules from frequent pattern
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

#mine dataset with the minimum support 3%
#the range of deviation of any support is limited in [-0.5%, 0.5%] with
#probability at least 99%
#fi is the formed frequent pattern
#sN is the size of sample
[fi, sN]=pyclt.getFP(dataset, 0.03, 0.005, 0.01)

#form the association rules from fi,
#the minimum confidence is set to 75%
#the minimum lift is set to 1
ar=fp2ar.getAR(fi,sN,0.75,1)
