# -*- coding: utf-8 -*-
"""
Created on Sat Apr 29 13:29:46 2017

@author: ZhongjieZhang

Transforming the frequent itemsets to the association rules based
on the specificed minimum confidence and minimum lift.
"""

"""
Getting all the subsets with length L and len(pat)-L of the object pattern pat
"""
def getSubSet(pat,sub1,sub2,L,temp,base):
    if(L<1):
        if(sub1.count(temp)==0):
            temp.sort()
            pat.sort()
            sub1.append(temp)
            sub2.append(pat)
    else:
        for i in range(base,len(pat)):
            getSubSet(pat[0:i]+pat[i+1:],sub1,sub2,L-1,temp+[pat[i]], i)

def getAR(fi,N,minconf,minlift):
    Rule=[]
    fiNum=len(fi)
    fid={}
    #transform the frequent pattern to a dictionary, where index is the pattern, and value is the support
    for i in range(0,fiNum):
        fii=fi[i][0:-1]
        fii.sort()
        fid[tuple(fii)]=fi[i][-1]
    for i in range(0,fiNum):
        psLen=int((len(fi[i])-1)/2)
        pat=fi[i][0:-1]
        sup1=fi[i][-1]
        sub1=[]
        sub2=[]
        #find all the subset of pat
        for j in range(1,psLen+1):
            getSubSet(pat,sub1,sub2,j,[],0)
        for j in range(0,len(sub1)):
            if(sub1.count(sub2[j])!=0):
                sub1[j]=[-1,-9]
                sub2[j]=[-2,-5]
            pat2=sub1[j]
            pat3=sub2[j]
            pat2.sort()
            pat3.sort()
            if(fid.has_key(tuple(pat2)) and fid.has_key(tuple(pat3))):
                sup2=fid[tuple(pat2)]
                sup3=fid[tuple(pat3)]
                #check the lift
                if(sup1*N*1.0/(sup2*sup3*1.0) > minlift):
                    #check the confidence
                    if(sup1*1.0/sup2 > minconf):
                        newrule=[pat2,pat3,sup1,sup1*1.0/sup2,sup1*N*1.0/(sup2*sup3*1.0)]
                        Rule.append(newrule)
                    if(sup1*1.0/sup3 > minconf):
                        newrule=[pat3,pat2,sup1,sup1*1.0/sup3,sup1*N*1.0/(sup2*sup3*1.0)]
                        Rule.append(newrule)     
    return Rule
                
