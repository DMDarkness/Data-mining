# -*- coding: utf-8 -*-
"""
Created on Sat Apr 29 13:29:46 2017

@author: ZhongjieZhang
"""

def getAR(fi,N,minconf,minlift):
    Rule=[]
    fiNum=len(fi)
    fid={}
    for i in range(0,fiNum):
        fii=fi[i][0:-1]
        fii.sort()
        fid[tuple(fii)]=fi[i][-1]
    for i in range(0,fiNum):
        pat1=fi[i][0:-1]
        pat1.sort()
        sp1=fi[i][-1]
        for j in range(i+1,fiNum):
            if(len(fi[j])!=len(fi[i])):
                pat2=fi[j][0:-1]
                pat2.sort()
                sp2=fi[j][-1]
                if(len(set(pat1).difference(pat2))==0):
                    pat3=list(set(pat2).difference(pat1))
                    pat3.sort()
                    sp3=fid[tuple(pat3)]
                    if(1.0*sp2/sp1>minconf and 1.0*N*sp2/(sp1*sp3)>minlift):
                        Rule.append([pat1,pat3, sp2, 1.0*sp2/sp1, 1.0*N*sp2/(sp1*sp3)])
                if(len(set(pat2).difference(pat1))==0):
                    pat3=list(set(pat1).difference(pat2))
                    pat3.sort()
                    sp3=fid[tuple(pat3)]
                    if(1.0*sp1/sp2>minconf and 1.0*N*sp1/(sp2*sp3)>minlift):
                        Rule.append([pat2,pat3, sp1, 1.0*sp1/sp2, 1.0*N*sp1/(sp2*sp3)])
    return Rule
                
