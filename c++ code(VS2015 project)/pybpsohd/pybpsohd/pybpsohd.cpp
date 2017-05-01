#include "Python.h"
#include <algorithm>
#include <math.h>
/**********************************************************************************************************************************************************************************
File: bpsohd.cpp
Contents: Mining long frequent patterns through an improved binary particle swarm optimization
Author: Zhongjie Zhang, Jian Huang, Ying Wei
Citation: Zhang Z, Huang J, Wei Y, et al. Frequent item sets mining from high-dimensional dataset based on a novel binary particle swarm optimization[J]. Journal of Central South University, 2016, 23(7): 1700-1708.
**********************************************************************************************************************************************************************************/
using namespace std;
#pragma warning(disable:4996) 
#define BUFFER 20000//this code only support those dataset with items less than 20000, the extra items will be ignored
#define MAXPOP 1000//the maximum of the size of population
#define pi 3.14159265358979

class Particle//the particle or chromosome
{
public:
	bool *spat;//the pattern
	long patLen;
	double *svol;//the speed
	double fitness; //the fitness
	bool *slbest;//local best solution
	double lfitness;//local best fitness
	long lLen;

	Particle()
	{
		spat = NULL;
		patLen = 0;
		svol = NULL;
		fitness = 0;
		slbest = NULL;
		lfitness = -1;
		lLen = -1;
	}
	~Particle()
	{
		delete[]spat;
		delete[]svol;
		delete[]slbest;
	}
};

class Result
{
public:
	Particle part;
};

long Popsize;//the size of population
long GenNum;//the number of generations
double omiga, c1, c2;//parameters of PSO
double minsup;//minimum support
double transNum;//the number of transaction
long length;//the length of pattern specified by users
long  globali;
long Psize;
Particle *Pop;//the population
Result *Output;
long    Patnum;
bool    *gbest;//the global best solution
double   gfitness;//the global best fitness
long *cItems;

bool compare(long a, long b)//the operator to sort items in the transaction
{
	return cItems[a]>cItems[b];
}

class FreqPat
{
public:
	long value;
	FreqPat *right;
	long len;
	long support;
public:
	FreqPat()
	{
		value = 0;
		right = NULL;
		len = 0;
	}
	~FreqPat()
	{
		delete right;
	}
};

class FrePatRsult
{
public:
	FreqPat *pat;
	FrePatRsult *next;
	long PatNum;
public:
	FrePatRsult()
	{
		pat = NULL;
		next = NULL;
		PatNum = 0;
	}
	~FrePatRsult()
	{
		delete pat;
		delete next;
	}
};

FrePatRsult* theResult;

static PyObject *pybpsohdError;

void Generate(long *Items, bool *validate)//initialize the population
{
	Psize = 0;//the variable to record the number of frequent items
	long i, j;
	for (i = 0; i < BUFFER; i++)//delete those items whose supports are lower than minimum support
	{
		if (Items[i] >= minsup*transNum)//if the support of this item is higher than minimum support           
		{
			Psize++;//the number of frequent items plus one
			validate[i] = 1;//this dim is remained
		}
		else
		{
			Items[i] = 0;//for those infrequent items, their supports are set to 0
		}
	}
	gbest = new bool[Psize];
	for (i = 0; i < Popsize; i++)//initial the population
	{
		//generate a new particle
		Pop[i].spat = new bool[Psize];
		memset(Pop[i].spat, false, Psize * sizeof(bool));
		Pop[i].svol = new double[Psize];
		memset(Pop[i].svol, 0, Psize * sizeof(double));
		Pop[i].slbest = new bool[Psize];
		memset(Pop[i].slbest, false, Psize * sizeof(bool));
		for (j = 0; j < Psize; j++)//for all the items
		{
			if (rand() % 2)//if this item is remained
			{
				Pop[i].spat[j] = true;//randomly decide whether it is contained in the particle
				Pop[i].patLen++;
			}
		}
	}
	for (i = 0; i < Popsize; i++)//prune the initial population, to make sure that half of the initial population have not too bad fitness
	{
		if (i < 1 + Popsize*0.9)//if this particle belonging to the first half of particles
		{
			for (j = 0; Pop[i].patLen>length && j<Psize; j++)//when its length is higher than the minimum length specified by users
			{
				if (Pop[i].spat[Psize - 1 - j] == 1)//for every item whose support is not very high, if it is contained by this particle
				{
					Pop[i].spat[Psize - 1 - j] = 0;//remove it
					Pop[i].patLen = Pop[i].patLen - 1;//the length of this particle minus one
				}
			}
		}
	}
}

void UpdateResults(PyObject* pyDataset, long *Items, bool *validate, long *ItemIndex)//Update the fitness, local best solution and the global best solution
{
	long value;
	long i, j, m, transdsize;
	for (i = 0; i < Popsize; i++)//reset the fitness of all the particle
	{
		Pop[i].fitness = 0;
	}
	for (i = 0; i < transNum; i++)//scanning the dataset to update the fitness of all the particle     
	{
		PyObject* tr;
		tr = PyList_GetItem(pyDataset, i);
		transdsize = PyList_Size(tr);

		for (j = 0; j < Popsize && transdsize >= length; j++)//for every particle
		{
			long exit = 0;
			for (m = 0; m < transdsize; m++)
			{
				PyObject *V = PyList_GetItem(tr, m);
				value = PyInt_AS_LONG(V);
				if (value < BUFFER && validate[value])
				{
					exit = exit + Pop[j].spat[ItemIndex[value]];//counting the items in both trans and particle
				}
			}
			if (exit == Pop[j].patLen)//if the trans contains the particle
			{
				Pop[j].fitness++;//Update the fitness of that particle
			}
		}
	}
	for (i = 0; i < Popsize; i++)//Update the local best and global best solutions
	{
		if (Pop[i].patLen < length)//if the length of particle is smaller than the specified minimum length, reset the fitness
		{
			Pop[i].fitness = 0;
		}
		if (Pop[i].fitness >= Pop[i].lfitness)//Update local best solution
		{
			Pop[i].lfitness = Pop[i].fitness;
			memcpy(Pop[i].slbest, Pop[i].spat, Psize * sizeof(bool));
			Pop[i].lLen = Pop[i].patLen;
		}
		if (Pop[i].fitness >= gfitness)//Update global best solution
		{
			gfitness = Pop[i].fitness;
			memcpy(gbest, Pop[i].spat, Psize * sizeof(bool));
		}
		if (Pop[i].fitness >= minsup*transNum)//Put the pattern into the final solution
		{
			bool exit = false;
			for (m = 0; m < Patnum && !exit && Patnum < MAXPOP; m++)
			{
				long equal = 0;
				for (j = 0; j < Psize; j++)
				{
					if (Pop[i].spat[j] == Output[m].part.spat[j])
					{
						equal++;
					}
					else
					{
						break;
					}
					if (equal == Psize)
					{
						exit = true;
						break;
					}
				}
			}
			if (!exit)
			{
				Output[m].part.spat = new bool[Psize];
				for (j = 0; j < Psize; j++)
				{
					Output[m].part.spat[j] = Pop[i].spat[j];
					Output[m].part.fitness = Pop[i].fitness;
				}
				Patnum++;
			}
		}
	}
}

void Reduce(long *Items, bool *validate, long *Itemsort)//reduce the dimenision of the dataset
{
	long i, j;
	for (i = 0; i < Psize; i++)
	{
		if (Items[Itemsort[i]] < gfitness)
		{
			validate[Itemsort[i]] = 0;
			for (j = 0; j < Popsize; j++)
			{
				if (Pop[j].spat[i])
				{
					Pop[j].spat[i] = 0;
					Pop[j].patLen--;
				}
				Pop[j].svol[i] = 0;
			}
		}
	}
}

void UpdatePos(bool *validate, long *Itemsort)//Update the position of particles
{
	long i, j;
	double r1, r2, r, V;
	for (i = 0; i < Popsize; i++)
	{
		r1 = (rand() % 100) / 100.0;
		r2 = (rand() % 100) / 100.0;
		for (j = 0; j < Psize; j++)
		{
			Pop[i].svol[j] = omiga*Pop[i].svol[j]
				+ r1*c1*(Pop[i].slbest[j] - Pop[i].spat[j])
				+ r2*c2*(gbest[j] - Pop[i].spat[j]);//Update the speed of every particle
			V = abs((2 / pi)*atan(pi*Pop[i].svol[j] / 2.0));//the V shape function
			r = (rand() % 100) / 100.0;
			if (r < V && validate[Itemsort[j]])
			{
				Pop[i].spat[j] = !(Pop[i].spat[j]);
				if (Pop[i].spat[j] == 1)
				{
					Pop[i].patLen++;
				}
				else
				{
					Pop[i].patLen--;
				}
			}
		}
	}
}

void Mining(PyObject* pyDataset, long *Items, bool *validate, long *Itemsort, long *ItemIndex)
{
	Generate(Items, validate);
	for (globali = 0; globali < GenNum; globali++)
	{
		UpdateResults(pyDataset, Items, validate, ItemIndex);
		UpdatePos(validate, Itemsort);
		Reduce(Items, validate, Itemsort);
	}
	long i, j;
	for (i = 0; i < Patnum; i++)//show the final results
	{
		long patLen = 0;
		//Output the frequent patterns
		theResult->PatNum++;
		FrePatRsult *sResult;
		for (sResult = theResult; sResult->next != NULL; sResult = sResult->next);//find the end of all the existed frequent patterns
		FreqPat *sPat;
		sResult->pat = new FreqPat;//create a new pattern
		sPat = sResult->pat;
		sPat->support = long(Output[i].part.fitness);//the support of this pattern
		for (j = 0; j < Psize; j++)
		{
			if (Output[i].part.spat[j])
			{
				patLen++;
				sPat->value = Itemsort[j];
				sPat->right = new FreqPat;
				sPat = sPat->right;
			}
		}
		sResult->pat->len = patLen;//the length of this pattern
		sResult->next = new FrePatRsult;
		sResult = sResult->next;
		//Complete the output
	}
}

long bpsohd(PyObject* pyDataset, long *Items, bool *validate, long *Itemsort, long *ItemIndex)
{
	Mining(pyDataset, Items, validate, Itemsort, ItemIndex);
	return 0;
}

static PyObject *
pybpsohd(PyObject *self, PyObject *args)
{
	srand(1);
	long Items[BUFFER] = { 0 };//the support of every item
	long Itemsort[BUFFER] = { 0 };//sorted items by their support (descend)
	long ItemIndex[BUFFER] = { 0 };//map Items to Itemsort
	bool validate[BUFFER] = { 0 };//the validate dimension of dataset
	theResult = new FrePatRsult;//the final result mined by fpgrowth
	PyObject* pyDataset;//define the dataset

	if (!PyArg_ParseTuple(args, "Odiidddi", &pyDataset, &minsup, &Popsize, &GenNum, &omiga, &c1, &c2, &length))//read the input to the dataset, and read the minimum support
		return NULL;

	transNum = 0;
	Patnum = 0;
	gfitness = -1;
	Pop = new Particle[Popsize];
	Output = new Result[MAXPOP];

	long len = PyList_Size(pyDataset);
	long i, j;
	transNum = len;
	for (i = 0; i < len; i++)
	{
		PyObject* trans;
		trans = PyList_GetItem(pyDataset, i);
		long len2 = PyList_Size(trans);
		for (j = 0; j < len2; j++)
		{
			PyObject *V = PyList_GetItem(trans, j);
			if (PyInt_AS_LONG(V) < BUFFER)
			{
				Items[PyInt_AS_LONG(V)]++;
			}
		}
	}

	cItems = Items;

	for (i = 0; i < BUFFER; i++)
	{
		Itemsort[i] = i;
	}
	sort(Itemsort, Itemsort + BUFFER, compare);//sort the Items through the support (descend)
	for (i = 0; i < BUFFER; i++)
	{
		ItemIndex[Itemsort[i]] = i;
	}

	bpsohd(pyDataset, Items, validate, Itemsort, ItemIndex);

	PyObject* pyOutlist = PyList_New(theResult->PatNum);

	FrePatRsult *sResult;
	i = 0;
	for (sResult = theResult; sResult->next != NULL; sResult = sResult->next)//transform the result to the Python List
	{
		PyObject *Pat = PyList_New(sResult->pat->len + 1);
		j = 0;
		FreqPat *sPat;
		for (sPat = sResult->pat; sPat->right != NULL; sPat = sPat->right)
		{
			Py_INCREF(Pat);
			PyList_SetItem(Pat, j, Py_BuildValue("i", sPat->value));
			j++;
		}
		Py_INCREF(Pat);
		PyList_SetItem(Pat, j, Py_BuildValue("i", sResult->pat->support));

		Py_INCREF(pyOutlist);
		PyList_SetItem(pyOutlist, i, Py_BuildValue("O", Pat));
		i++;
	}

	delete theResult;
	delete[]Pop;
	delete[]Output;
	delete[]gbest;
	return pyOutlist;
}

static PyMethodDef pybpsohdMethods[] = {
	{ "getFP", pybpsohd, METH_VARARGS,
	"\
<fps>=getFP(<data>,<minsup>,<Popsize>,<GenNum>,<omiga>,<c1>,<c2>,<length>)\n\n\
input: \n\
<data>: the list of transactions to be mined,\n\
        every transaction is a list containing integer higher than 0\n\
<minsup>: double higher than 0, the minimum support\n\
<Popsize>: integer higher than 0, the size of population\n\
<GenNum>: integer higher than 0, the number of genertions\n\
<omiga>: double higher than 0, the interia wieght\n\
<c1>: double higher than 0, the first accelaration\n\
<c2>: double higher than 0, the second accelaration\n\
<length>: integer higher than 0, the minimum length of pattern\n\n\
output: \n\
<fps>:a list of frequent itemsets, where every itemset is also\n\
      a list, whose last value is its absolute support\n\n\
example\n\
fi=getFP([[1],[1,2,3,5],[2,6,5,7],[1,5],[2,5,6],[1,3,2,7]],0.01,20,20,0.5,1,1,2)\n\n\
Citation:\n\
Zhang Z, Huang J, Wei Y, et al. Frequent item sets mining from high-dimensional\n\
dataset based on a novel binary particle swarm optimization[J]. Journal of Central\n\
South University, 2016, 23(7): 1700-1708." },
{ NULL, NULL, 0, NULL }        /* Sentinel */
};

PyMODINIT_FUNC
initpybpsohd(void)
{
	PyObject *m;

	m = Py_InitModule("pybpsohd", pybpsohdMethods);
	if (m == NULL)
		return;

	pybpsohdError = PyErr_NewException("pybpsohd.error", NULL, NULL);
	Py_INCREF(pybpsohdError);
	PyModule_AddObject(m, "error", pybpsohdError);
}