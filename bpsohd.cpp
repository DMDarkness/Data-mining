#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
#define BUFFER 10000//this code only support those dataset with items less than 10000, the extra items will be ignored
#define MAXPOP 500//the maximum of the size of population
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
double transNum = 0;//the number of transaction
long *Items;//the support of every item
long *Itemsort;//sorted items by their support (descend)
long *ItemIndex;//map Items to Itemsort
long length;//the length of pattern specified by users
bool *validate;//the validate dimension of dataset
long  globali;
long Psize;
Particle *Pop;//the population
Result *Output;
long    Patnum;
bool    *gbest;//the global best solution
double   gfitness;//the global best fitness

bool compare(long a, long b)//the operator to sort items in the transaction
{
	return Items[a]>Items[b];
}

class trans
{
public:
	char *str;
	trans *next;
	long len = 0;
	long tid;
};

trans *Dataset;//the dataset sotred in the memory
trans *strans = new trans;

void ReadData(FILE *fIn)
{
	transNum = 0;
	long i, j;
	char str[2*BUFFER];
	Dataset = strans;
	for (i = 0; fgets(str, BUFFER, fIn); i++)//Counting the support of every candidate itemset
	{
		if (transNum > 0)
		{
			strans->next = new trans;
			strans = strans->next;
		}
		for (j = 0; j < BUFFER && str[j] != '\0'; j++)
		{

		}
		strans->str = new char[j + 1];
		strans->len = j + 1;
		memcpy(strans->str, str, (j + 1) * sizeof(char));
		strans->str[j] = '\0';
		strans->next = NULL;
		strans->tid = i;
		transNum++;
	}
}

void PreScan()
{
	Patnum = 0;
	gfitness = -1;
	Items = new long[BUFFER];
	validate = new bool[BUFFER];
	Itemsort = new long[BUFFER];
	ItemIndex = new long[BUFFER];
	Pop = new Particle[Popsize];
	Output = new Result[MAXPOP];
	memset(Items, 0, BUFFER * sizeof(long));
	memset(validate, 0, BUFFER * sizeof(bool));

	long i, value;//the variable to record the item in every transaction
	char * token;
	char str[2*BUFFER];
	for (strans = Dataset; strans != NULL; strans = strans->next)//Counting the support of every item
	{
		//memset(str, 0, BUFFER);
		memcpy(str, strans->str, strans->len * sizeof(char));
		token = strtok(str, " \t\n");
		while (token != NULL)
		{
			if (isdigit(*token))
			{
				value = atoi(token);
				if (value < BUFFER)
				{
					Items[value]++;//the support of this item plus one
				}
			}
			token = strtok(NULL, " \t\n");
		}
	}

	for (i = 0; i < BUFFER; i++)
	{
		Itemsort[i] = i;
	}
	sort(Itemsort, Itemsort + BUFFER, compare);//sort the Items through the support (descend)
	for (i = 0; i < BUFFER; i++)
	{
		ItemIndex[Itemsort[i]] = i;
	}
}

void Generate()//initialize the population
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

void UpdateResults()//Update the fitness, local best solution and the global best solution
{
	//rewind(fIn);
	long value;
	long i, j, m, transdsize;
	char * token;
	for (i = 0; i < Popsize; i++)//reset the fitness of all the particle
	{
		Pop[i].fitness = 0;
	}
	char str[2*BUFFER];
	for (strans = Dataset; strans != NULL; strans = strans->next)//scanning the dataset to update the fitness of all the particle           
	{
		memcpy(str, strans->str, strans->len * sizeof(char));
		token = strtok(str, " \t\n");
		long trans[BUFFER] = { 0 };//store the transaction to a vector trans
		j = 0;
		while (token != NULL)
		{
			if (isdigit(*token))
			{
				value = atoi(token);
				if (value < BUFFER && validate[value])
				{
					trans[j] = value;
					j++;
				}
			}
			token = strtok(NULL, " \t\n");
		}
		transdsize = j;
		for (j = 0; j < Popsize; j++)//for every particle
		{
			long exit = 0;
			for (m = 0; m < transdsize; m++)
			{
				exit = exit + Pop[j].spat[ItemIndex[trans[m]]];//counting the items in both trans and particle
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
			memcpy(Pop[i].slbest, Pop[i].spat, Psize*sizeof(bool));
			Pop[i].lLen = Pop[i].patLen;
		}
		if (Pop[i].fitness >= gfitness)//Update global best solution
		{
			gfitness = Pop[i].fitness;
			memcpy(gbest, Pop[i].spat, Psize*sizeof(bool));
		}
		if (Pop[i].fitness >= minsup*transNum)//Put the pattern into the final solution
		{
			bool exit = false;
			for (m = 0; m < Patnum && !exit; m++)
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

void Reduce()//reduce the dimenision of the dataset
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

void UpdatePos()//Update the position of particles
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

void Mining()
{
	Generate();
	for (globali = 0; globali < GenNum; globali++)
	{
		UpdateResults();
		UpdatePos();
		Reduce();
		printf("generation %d is completed\n", globali + 1);
	}
	long i, j;
	for (i = 0 ; i < Patnum; i++)//show the final results
	{
		for (j = 0; j < Psize; j++)
		{
			if (Output[i].part.spat[j])
			{
				printf("%d ", Itemsort[j]);
			}
		}
		printf("support %f\n", Output[i].part.fitness);
	}
}

int main(int argc, char** argv)
{
	FILE *fIn;
	if (argc != 9)
	{
		printf("Usage: bpsohd <input-filename> <minimum-support> <population-size> <generation-number> <inertia-weight> <acceleration-1> <acceleration-2> <length>\n");
	}
	else if (!(fIn = fopen(argv[1], "r")))
	{
		printf("Error in input file\n");
	}
	else if (atof(argv[2]) < 0)
	{
		printf("invalid minimum support");
	}
	else if (atoi(argv[3]) < 0)
	{
		printf("invalid population size");
	}
	else if (atoi(argv[4]) < 0)
	{
		printf("invalid generation number");
	}
	else if (atof(argv[5]) < 0)
	{
		printf("invalid inertia weight");
	}
	else if (atof(argv[6]) < 0 || atof(argv[7]) < 0)
	{
		printf("invalid acceleration");
	}
	else if (atoi(argv[8]) < 0)
	{
		printf("invalid length");
	}
	else
	{
		minsup = atof(argv[2]);//setting the minimum support
		length = atoi(argv[8]);//setting the minimum length for the final patterns
		Popsize = atoi(argv[3]);//setting the size of population
		omiga = atof(argv[5]);//setting the inertia weight
		c1 = atof(argv[6]);//setting the first acceleration
		c2 = atof(argv[7]);//setting the second acceleration
		GenNum = atoi(argv[4]);//setting the number of generation
		ReadData(fIn);
		PreScan();
		Mining();
		printf("\n%d patterns are output\n", Patnum);
	}
	printf("push retrun to exit");
	getchar();
	delete []Items;
	delete []validate;
	delete []Itemsort;
	delete []ItemIndex;
	delete []Pop;
	delete []Output;
	return 0;
}
