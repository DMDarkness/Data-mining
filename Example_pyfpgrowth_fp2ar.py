# -*- coding: utf-8 -*-
"""
Created on Tue May 02 18:05:42 2017

@author: ZhongjieZhang

This is an example for pyfpgrowth and fp2ar
"""
import pyfpgrowth as pyfp#import the package of fpgrowth
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

#mine dataset with the minimum support 1%
#after mining, there should be 383 frequent patterns
fi=pyfp.getFP(dataset, 0.01)

#form the association rules from fi,
#the minimum confidence is set to 75%
#the minimum lift is set to 1
#after running, there should be 431 association rules
ar=fp2ar.getAR(fi,len(dataset),0.75,1)