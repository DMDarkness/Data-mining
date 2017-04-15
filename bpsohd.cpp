#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <math.h>

/****************************************************************************
File: bpsohd.cpp
Contents: Mining long frequent patterns through an improved binary particle swarm optimization
Author: Zhongjie Zhang, Jian Huang, Ying Wei
Citation: Zhang Z, Huang J, Wei Y, et al. Frequent item sets mining from high-dimensional dataset based on a novel binary particle swarm optimization[J]. Journal of Central South University, 2016, 23(7): 1700-1708.
****************************************************************************/

using namespace std;
#pragma warning(disable:4996) 
#define BUFFER 10000//this code only support those dataset with items less than 10000, the extra items will be ignored
#define MAXPOP 500//the maximum of the size of population
#define pi 3.14159265358979

int Popsize;//the size of population
int GenNum;//the number of generations
double omiga, c1, c2;//parameters of PSO
double minsup;//minimum support
double transNum = 0;//the number of transaction
int Items[BUFFER] = { 0 };//the support of every item
int Itemsort[BUFFER] = { 0 };//sorted items by their support (descend)                 
int length;//the length of pattern specified by users
bool validate[BUFFER] = { 0 };//the validate dimension of dataset
char str[BUFFER];
int  globali;

class Particle//the particle or chromosome
{
public:
	int pat[BUFFER] = { 0 };//the pattern
	int patLen = 0;
	double vol[BUFFER] = { 0 };//the speed
	double fitness; //the fitness
	int lbest[BUFFER] = { 0 };//local best solution
	double lfitness = -1;//local best fitness
	int lLen = -1;
};

Particle  Pop[MAXPOP];//the population
int    Output[BUFFER][BUFFER];//the Patterns which are used to putput
double   Outsupp[BUFFER];//the supports of the outpur patterns
int    Patnum = 0;
int    gbest[BUFFER] = { 0 };//the global best solution
double   gfitness = -1;//the global best fitness

bool compare(int a, int b)//the operator to sort items in the transaction
{
	return Items[a]>Items[b];
}

void PreScan(FILE *fIn)
{
	int value;//the variable to record the item in every transaction
	int i, j;
	char * token;

	for (i = 0; fgets(str, BUFFER, fIn); i++)//Counting the support of every item
	{
		transNum++;//the number of transaction plus one
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
}

void Generate()//initialize the population
{
	int Psize = 0;//the variable to record the number of frequent items
	int i, j;
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
	for (i = 0; i < Popsize; i++)//initial the population
	{
		Particle pati;//generate a new particle
		for (j = 0; j < BUFFER; j++)//for all the items
		{
			if (validate[j] > 0 && (rand() % 2)>0)//if this item is remained
			{
				pati.pat[j] = 1;//randomly decide whether it is contained in the particle
				pati.patLen++;
			}
		}
		Pop[i] = pati;
	}

	for (i = 0; i < Popsize; i++)//prune the initial population, to make sure that half of the initial population have not too bad fitness
	{
		if (i < 1 + Popsize*0.8)//if this particle belonging to the first half of particles
		{
			for (j = 0; Pop[i].patLen>length; j++)//when its length is higher than the minimum length specified by users
			{
				if (Pop[i].pat[Itemsort[Psize - 1 - j]] == 1)//for every item whose support is not very high, if it is contained by this particle
				{
					Pop[i].pat[Itemsort[Psize - 1 - j]] = 0;//remove it
					Pop[i].patLen = Pop[i].patLen - 1;//the length of this particle minus one
				}
			}
		}
	}
}

void UpdateResults(FILE *fIn)//Update the fitness, local best solution and the global best solution
{
	rewind(fIn);
	int value;
	int i, j, m, transdsize;
	char * token;
	for (i = 0; i < Popsize; i++)//reset the fitness of all the particle
	{
		Pop[i].fitness = 0;
	}
	for (i = 0; fgets(str, BUFFER, fIn); i++)//scanning the dataset to update the fitness of all the particle           
	{
		token = strtok(str, " \t\n");
		int trans[BUFFER] = { 0 };//store the transaction to a vector trans
		j = 0;
		while (token != NULL)
		{
			if (isdigit(*token))
			{
				value = atoi(token);
				if (value < BUFFER && validate[value]>0)
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
			int exit = 0;
			for (m = 0; m < transdsize; m++)
			{
				exit = exit + Pop[j].pat[trans[m]];//counting the items in both trans and particle
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
			for (j = 0; j < BUFFER; j++)
			{
				Pop[i].lbest[j] = Pop[i].pat[j];
			}
			Pop[i].lLen = Pop[i].patLen;
		}
		if (Pop[i].fitness >= gfitness)//Update global best solution
		{
			gfitness = Pop[i].fitness;
			for (j = 0; j < BUFFER; j++)
			{
				gbest[j] = Pop[i].pat[j];
			}
		}
		if (Pop[i].fitness >= minsup*transNum)//Put the pattern into the final solution
		{
			int exit = 0;
			for (m = 0; m < Patnum; m++)
			{
				int equal = 0;
				for (j = 0; j < BUFFER; j++)
				{
					if (Pop[i].pat[j] == Output[m][j])
					{
						equal++;
					}
					else
					{
						break;
					}
					if (equal == BUFFER)
					{
						exit = 1;
						break;
					}
				}
			}
			if (exit == 0)
			{
				for (j = 0; j < BUFFER; j++)
				{
					Output[Patnum][j] = Pop[i].pat[j];
					Outsupp[Patnum] = Pop[i].fitness;
				}
				Patnum++;
			}
		}
	}
}

void Reduce()//reduce the dimenision of the dataset
{
	int i, j;
	for (i = 0; i < BUFFER; i++)
	{
		if (Items[i] < gfitness)
		{
			validate[i] = 0;
			for (j = 0; j < Popsize; j++)
			{
				if (Pop[j].pat[i] > 0)
				{
					Pop[j].pat[i] = 0;
					Pop[j].patLen--;
				}
				Pop[j].vol[i] = 0;
			}
		}
	}
}

void UpdatePos()//Update the position of particles
{
	int i, j;
	double r1, r2, r, V;
	for (i = 0; i < Popsize; i++)
	{
		r1 = (rand() % 100) / 100.0;
		r2 = (rand() % 100) / 100.0;
		for (j = 0; j < BUFFER; j++)
		{
			Pop[i].vol[j] = omiga*Pop[i].vol[j]
				+ r1*c1*(Pop[i].lbest[j] - Pop[i].pat[j])
				+ r2*c2*(gbest[j] - Pop[i].pat[j]);//Update the speed of every particle
			V = abs((2 / pi)*atan(pi*Pop[i].vol[j] / 2.0));//the V shape function
			r = (rand() % 100) / 100.0;
			if (r < V && validate[j]>0)
			{
				Pop[i].pat[j] = 1 - Pop[i].pat[j];
				if (Pop[i].pat[j] == 1)
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

void Mining(FILE *fIn)
{
	Generate();
	for (globali = 0; globali < GenNum; globali++)
	{
		UpdateResults(fIn);
		UpdatePos();
		Reduce();
		printf("generation %d is completed\n", globali + 1);
	}
	int i, j;
	for (i = 0; i < Patnum; i++)//show the final results
	{
		for (j = 0; j < BUFFER; j++)
		{
			if (Output[i][j] > 0)
			{
				printf("%d ", j);
			}
		}
		printf("support %f\n", Outsupp[i]);
	}
}

void main()
{
	minsup = 0.00001;//setting the minimum support
	length = 10;//setting the minimum length for the final patterns
	Popsize = 20;//setting the size of population
	omiga = 0.5;//setting the inertia weight
	c1 = 1;//setting the first acceleration
	c2 = 1;//setting the second acceleration
	GenNum = 30;//setting the number of generation
	FILE *fIn;
	if (!(fIn = fopen("kosarak.dat", "r")))//input the data for mining
	{
		printf("Error in input file\n");
	}
	PreScan(fIn);
	Mining(fIn);
}
