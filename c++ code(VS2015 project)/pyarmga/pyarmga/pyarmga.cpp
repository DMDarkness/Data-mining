#include "Python.h"
#include <algorithm>
#include <math.h>

using namespace std;
#pragma warning(disable:4996)
#define BUFFER 20000

long Popsize;//the size of population
long GenNum;//the number of generations
double mp;//the probability for mutation
double sp;//the probability for selecting
double cp;//the probability for crossover
long length;//the length of pattern specified by users
long maxItem;
long patternNum;

double minconf;//minimum confidence
double minlift;//minimum lift
double transNum;//the number of transaction
long *cItems;

bool compare(long a, long b)//the operator to sort items in the transaction
{
	return cItems[a]>cItems[b];
}

class chromosome//the structure for a chromosome
{
public:
	long *pat;
	long sup1;
	long sup2;
	long sup3;
	double fitness;
	long tempfit1;
	long tempfit2;
	long tempfit3;
public:
	chromosome()
	{
		pat = NULL;
		sup1 = 0;
		sup2 = 0;
		sup3 = 0;
		fitness = 0;
		tempfit1 = 0;
		tempfit2 = 0;
		tempfit3 = 0;
	}
	~chromosome()
	{
		if (pat != NULL)
		{
			delete[]pat;
			pat = NULL;
		}
	}
};

long mutate(long c, long *pat, double tmp, long *Itemsort)//doing the mutation operation
{
	long i, j;
	double r = (rand() % 1000) / 1000.0;
	if (r < tmp)
	{
		if (c < 0 && c + 1<0)
		{
			c++;
		}
		else if (c < 0)
		{
			c = -length + 1;
		}
		else if (c > 0)
		{
			i = 0;
			while (true)
			{
				if ((rand() % 15) == 0)
				{
					for (j = 1; j < length + 1; j++)
					{
						if (Itemsort[i%maxItem] == pat[j])
						{
							break;
						}
					}
					if (j == length + 1)
					{
						c = Itemsort[i%maxItem];
						break;
					}
				}
				i++;
			}
		}
	}
	return c;
}

chromosome *Pop;

void initialise(long *Itemsort)//initialize the population
{
	srand(1);
	long i, j;
	Pop = new chromosome[Popsize];
	Pop[0].pat = new long[length + 1];
	Pop[0].pat[0] = -(length - 1);
	for (i = 1; i < length + 1; i++)
	{
		Pop[0].pat[i] = Itemsort[length - 1 - i + 1];
	}
	//Pop[0].pat[0] = -4;
	//Pop[0].pat[1] = 27;
	//Pop[0].pat[2] = 40;
	//Pop[0].pat[3] = 316;
	//Pop[0].pat[4] = 423;
	//Pop[0].pat[5] = 6;
	for (i = 1; i < Popsize; i++)
	{
		Pop[i].pat = new long[length + 1];
		memcpy(Pop[i].pat, Pop[0].pat, (length + 1) * sizeof(long));
		for (j = 0; j < length + 1; j++)
		{
			Pop[i].pat[j] = mutate(Pop[i].pat[j], Pop[i].pat, mp, Itemsort);
		}
	}
}

void Select()//selsect the good chromosome;
{
	long i;
	for (i = 0; i < Popsize; i++)
	{
		double r = (rand() % 1000) / 1000.0;
		if (r > Pop[i].fitness*sp)
		{
			Pop[i].fitness = -100000;
		}
	}
}

void Cross(long *Itemsort)//doing the crossover operation
{
	chromosome *newPop = new chromosome[Popsize];
	long i, j, k;
	k = 0;
	for (i = 0; i < Popsize && k<Popsize; i++)
	{
		if (Pop[i].fitness >= 0)
		{
			for (j = i + 1; j < Popsize && k<Popsize; j++)
			{
				if (Pop[j].fitness >= 0)
				{
					double r = (rand() % 1000) / 1000.0;
					if (r < cp)
					{
						long r1 = rand() % (length + 1);
						long r2 = rand() % (length + 1);
						if (r1 > r2)
						{
							long r3 = r1;
							r1 = r2;
							r2 = r3;
						}

						long *npat1 = new long[length + 1];
						long *npat2 = new long[length + 1];
						//crossover, two point strategy
						memcpy(npat1, Pop[i].pat, r1 * sizeof(long));
						memcpy(npat1 + r1, Pop[j].pat + r1, (r2 - r1) * sizeof(long));
						memcpy(npat1 + r2, Pop[i].pat + r2, (length + 1 - r2) * sizeof(long));

						memcpy(npat2, Pop[j].pat, r1 * sizeof(long));
						memcpy(npat2 + r1, Pop[i].pat + r1, (r2 - r1) * sizeof(long));
						memcpy(npat2 + r2, Pop[j].pat + r2, (length + 1 - r2) * sizeof(long));

						long ii, jj;
						for (ii = 1; ii < length + 1; ii++)
						{
							for (jj = ii + 1; jj < length + 1; jj++)
							{
								if (npat1[ii] == npat1[jj])
								{
									npat1[jj] = mutate(npat1[jj], npat1, 1, Itemsort);
								}
								if (npat2[ii] == npat2[jj])
								{
									npat2[jj] = mutate(npat2[jj], npat2, 1, Itemsort);
								}
							}
						}
						newPop[k].pat = npat1;
						newPop[k + 1].pat = npat2;//doing the crossover operation
						k = k + 2;
					}
				}
			}
		}
	}

	for (i = 0; i < Popsize; i++)//generate the new Population
	{
		Pop[i].fitness = 0;
		Pop[i].sup1 = 0;
		Pop[i].sup2 = 0;
		Pop[i].sup3 = 0;
		Pop[i].tempfit1 = 0;
		Pop[i].tempfit2 = 0;
		Pop[i].tempfit3 = 0;
		if (newPop[i].pat != NULL)
		{			
			memcpy(Pop[i].pat, newPop[i].pat, (length + 1) * sizeof(long));
		}
		else
		{
			Pop[i].fitness = -100000;
		}
	}
	delete[]newPop;
}

void Mutat(long *Itemsort)//doing the mutation operation
{
	long i, j;
	for (i = 0; i < Popsize; i++)
	{
		if (Pop[i].fitness >= 0)
		{
			for (j = 0; j < length + 1; j++)
			{
				Pop[i].pat[j] = mutate(Pop[i].pat[j], Pop[i].pat, mp, Itemsort);
			}
		}
	}
}

void UpdateFitness(PyObject* pyDataset)//Updating the fitness of every chromosome
{
	long value, i, j, di, dj;

	for (di = 0; di < transNum; di++)//Counting the support of every item
	{
		PyObject* trans;
		trans = PyList_GetItem(pyDataset, di);
		long len2 = PyList_Size(trans);
		for (dj = 0; dj < len2; dj++)
		{
			PyObject *V = PyList_GetItem(trans, dj);
			value = PyInt_AS_LONG(V);
			if (value < BUFFER)
			{
				for (i = 0; i < Popsize; i++)
				{
					if (Pop[i].fitness >= 0)
					{
						for (j = 1; j < -Pop[i].pat[0] + 1; j++)
						{
							if (value == Pop[i].pat[j])
							{
								Pop[i].tempfit1++;
								Pop[i].tempfit3++;
								break;
							}
						}
						if (j == -Pop[i].pat[0] + 1)
						{
							for (j = -Pop[i].pat[0] + 1; j < length + 1; j++)
							{
								if (value == Pop[i].pat[j])
								{
									Pop[i].tempfit2++;
									Pop[i].tempfit3++;
									break;
								}
							}
						}
					}
				}
			}
		}
		for (i = 0; i < Popsize; i++)
		{
			if (Pop[i].tempfit1 == -Pop[i].pat[0])
			{
				Pop[i].sup1++;
			}
			if (Pop[i].tempfit2 == length + Pop[i].pat[0])
			{
				Pop[i].sup2++;
			}
			if (Pop[i].tempfit3 == length)
			{
				Pop[i].sup3++;
			}
			Pop[i].tempfit1 = 0;
			Pop[i].tempfit2 = 0;
			Pop[i].tempfit3 = 0;
		}
	}

	for (i = 0; i < Popsize; i++)
	{
		Pop[i].fitness = (Pop[i].sup3 - Pop[i].sup1*Pop[i].sup2 / transNum) / (0.000000000001 + Pop[i].sup1 - Pop[i].sup1* Pop[i].sup2 / transNum);
	}
}

class Rule//the structure to store rule
{
public:
	long *pat;
	Rule *next;
	long support;
	double confidence;
	double lift;
public:
	Rule()
	{
		pat = NULL;
		next = NULL;
	}
	~Rule()
	{
		if (pat != NULL)
		{
			delete[]pat;
			pat = NULL;
		}
		if (next != NULL)
		{
			delete next;
			next = NULL;
		}
	}
};

Rule *fRule;
Rule* sRule;

void GenerateRules()
{
	long i;
	for (i = 0; i < Popsize && Pop[i].pat !=NULL; i++)
	{
		if (Pop[i].sup3*1.0 / Pop[i].sup1 > minconf && Pop[i].sup3 * transNum / (Pop[i].sup1*Pop[i].sup2) > minlift)
		{
			long* npat = new long[length + 1];
			memcpy(npat, Pop[i].pat, (length + 1) * sizeof(long));
			sort(npat + 1, npat - npat[0] + 1);
			sort(npat - npat[0] + 1, npat + length + 1);
			for (sRule = fRule; sRule->pat != NULL; sRule = sRule->next)
			{
				if (!memcmp(npat, sRule->pat, (length + 1) * sizeof(long)))
				{
					break;
				}
			}
			if (sRule->pat == NULL)//show the rules
			{
				sRule->pat = npat;
				sRule->next = new Rule;
				patternNum++;
				sRule->support = Pop[i].sup3;
				sRule->confidence = Pop[i].sup3*1.0 / Pop[i].sup1;
				sRule->lift = Pop[i].sup3 * transNum / (Pop[i].sup1*Pop[i].sup2);
			}
		}
	}
}

void armga(long* Itemsort, PyObject* pyDataset)//the algorithm of armga
{
	long i;
	for (i = 0; i < GenNum; i++)
	{
		UpdateFitness(pyDataset);
		GenerateRules();
		Select();
		Cross(Itemsort);
		Mutat(Itemsort);
	}
}

static PyObject *pyarmgaError;

static PyObject *
pyarmga(PyObject *self, PyObject *args)
{
	fRule = new Rule;
	maxItem = 0;
	transNum = 0;
	patternNum = 0;
	long Items[BUFFER] = { 0 };//the support of every item
	long Itemsort[BUFFER] = { 0 };//sorted items by their support (descend)

	srand(1);
	PyObject* pyDataset;//define the dataset

	if (!PyArg_ParseTuple(args, "Oddiidddi", &pyDataset, &minconf, &minlift, &Popsize, &GenNum, &mp, &sp, &cp, &length))//read the input to the dataset, and read the minimum support
		return NULL;

	long len = PyList_Size(pyDataset);
	long i, j, value;
	transNum = len;
	for (i = 0; i < len; i++)
	{
		PyObject* trans;
		trans = PyList_GetItem(pyDataset, i);
		long len2 = PyList_Size(trans);
		for (j = 0; j < len2; j++)
		{
			PyObject *V = PyList_GetItem(trans, j);
			value = PyInt_AS_LONG(V);
			if (value < BUFFER)
			{
				Items[value]++;
				if (value > maxItem)
				{
					maxItem = value;
				}
			}
		}
	}

	cItems = Items;

	for (i = 0; i < BUFFER; i++)
	{
		Itemsort[i] = i;
	}
	sort(Itemsort, Itemsort + BUFFER, compare);//sort the Items through the support (descend)

	if (Popsize % 2 > 0)
	{
		Popsize++;
	}
	initialise(Itemsort);
	armga(Itemsort, pyDataset);

	PyObject* pyOutlist = PyList_New(patternNum);

	i = 0;
	for (sRule = fRule; sRule->pat != NULL; sRule = sRule->next)//transform the result to the Python List
	{
		PyObject *Pat = PyList_New(5);
		PyObject *aPat = PyList_New(-sRule->pat[0]);
		PyObject *cPat = PyList_New(length + sRule->pat[0]);
		for (j = 0; j < -sRule->pat[0]; j++)
		{
			Py_INCREF(aPat);
			PyList_SetItem(aPat, j, Py_BuildValue("i", sRule->pat[j + 1]));
		}
		for (j = 0; j < length + sRule->pat[0]; j++)
		{
			Py_INCREF(cPat);
			PyList_SetItem(cPat, j, Py_BuildValue("i", sRule->pat[-sRule->pat[0] + j + 1]));
		}
		Py_INCREF(Pat);
		PyList_SetItem(Pat, 0, Py_BuildValue("O", aPat));

		Py_INCREF(Pat);
		PyList_SetItem(Pat, 1, Py_BuildValue("O", cPat));

		Py_INCREF(Pat);
		PyList_SetItem(Pat, 2, Py_BuildValue("i", sRule->support));

		Py_INCREF(Pat);
		PyList_SetItem(Pat, 3, Py_BuildValue("d", sRule->confidence));

		Py_INCREF(Pat);
		PyList_SetItem(Pat, 4, Py_BuildValue("d", sRule->lift));

		Py_INCREF(pyOutlist);
		PyList_SetItem(pyOutlist, i, Py_BuildValue("O", Pat));
		i++;
	}
	delete fRule;
	delete[]Pop;
	return pyOutlist;
}

static PyMethodDef pyarmgaMethods[] = {
	{ "getAR", pyarmga, METH_VARARGS,
	"\
<ars>=getAR(<data>,<minconf>,<minlift>,<Popsize>,<GenNum>,<mp>,<sp>,<cp>,<length>)\n\n\
input: \n\
<data>: the list of transactions to be mined,\n\
        every transaction is a list containing integer higher than 0\n\
<minconf>: double located in [0, 1], the minimum confidence\n\
<minlift>: double located in [0, 1], the minimum lift\n\
<Popsize>: integer higher than 0, the size of population\n\
<GenNum>: integer higher than 0, the number of genertions\n\
<mp>: double located in [0, 1], the mutation probability\n\
<sp>: double located in [0, 1], the selection probability\n\
<cp>: double located in [0, 1], the crossover probability\n\
<length>: integer higher than 0, the length of association rules\n\n\
output: \n\
<ars>:a list of association rules, where every rule is also\n\
      a list, whose last three values are its absolute support\n\
      , confidence and lift, and the absolute value lower than 0\n\
      is the antecedent\n\n\
example\n\
ars=getAR([[1],[1,2,3,5],[2,6,5,7],[1,5],[2,5,6],[1,3,2,7]],0.5,1,20,20,0.2,1,1,2)" },
{ NULL, NULL, 0, NULL }        /* Sentinel */
};

PyMODINIT_FUNC
initpyarmga(void)
{
	PyObject *m;

	m = Py_InitModule("pyarmga", pyarmgaMethods);
	if (m == NULL)
		return;

	pyarmgaError = PyErr_NewException("pyarmga.error", NULL, NULL);
	Py_INCREF(pyarmgaError);
	PyModule_AddObject(m, "error", pyarmgaError);
}