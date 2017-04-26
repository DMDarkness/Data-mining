# -*- coding: utf-8 -*-
"""
Created on Mon Apr 24 00:10:17 2017

@author: ZhongjieZhang
"""
import pyfpgrowth
import pybpsohd
import pyclt

fi = pyfpgrowth.getFP([[1],[1,3],[2,6,7],[1,5],[2,5,6],[1,2,7]],0.2)
fi = pybpsohd.getFP([[1],[1,3],[2,6,7],[1,5],[2,5,6],[1,2,7]], 0.2, 20, 20, 0.5, 1, 1, 2)
[fi, SampleN] = pyclt.getFP([[1],[1,3],[2,6,7],[1,5],[2,5,6],[1,2,7]], 0.2, 0.01, 0.01)
