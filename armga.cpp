#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <math.h>
#include <time.h>

using namespace std;
#pragma warning(disable:4996)
#define BUFFER 20000

int Popsize;//the size of population
int GenNum;//the number of generations
double mp;//the probability for mutation
double sp;//the probability for selecting
double cp;//the probability for crossover
int length;//the length of pattern specified by users
int maxItem = 0;
int patternNum = 0;

double minconf;//minimum confidence
double minlift;//minimum lift
double transNum = 0;//the number of transaction

char str[BUFFER];
int Items[BUFFER] = { 0 };//the support of every item
int Itemsort[BUFFER] = { 0 };//sorted items by their support (descend)

bool compare(int a, int b)//the operator to sort items in the transaction
{
	return Items[a]>Items[b];
}

class trans
{
public:
	char *str = NULL;
	trans *next;
	int len = 0;
	int tid;
};

trans *Dataset;//the dataset sotred in the memory
trans *strans = new trans;

void ReadData(FILE *fIn)
{
	transNum = 0;
	int i, j;
	char str[BUFFER];
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
	int value;//the variable to record the item in every transaction
	int i;
	char * token;

	for (strans = Dataset; strans != NULL; strans = strans->next)//Counting the support of every item
	{
		//memset(str, 0, BUFFER);
		memcpy(str, strans->str, (strans->len)*sizeof(char));
		token = strtok(str, " \t\n");
		while (token != NULL)
		{
			if (isdigit(*token))
			{
				value = atoi(token);
				if (value < BUFFER)
				{
					Items[value]++;//the support of this item plus one
					if (value > maxItem)
					{
						maxItem = value;
					}
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

class chromosome//the structure for a chromosome
{
public:
	int *pat = NULL;
	double sup1 = 0;
	double sup2 = 0;
	double sup3 = 0;
	double fitness = 0;
	int tempfit1 = 0;
	int tempfit2 = 0;
	int tempfit3 = 0;
};

int mutate(int c, int *pat, double tmp)//doing the mutation operation
{
	int i, j;
	double r = (rand() % 100) / 100.0;
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
		else if(c > 0)
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

bool compareC(chromosome a, chromosome b)//the operator to sort chromosomes
{
	return a.fitness > b.fitness;
}

chromosome *Pop;

chromosome seed;

void initialise()//initialize the population
{
	srand(time(0));
	int i, j;
	seed.pat = new int[length + 1];
	seed.pat[0] = -(length-1);
	for (i = 1; i < length + 1; i++)
	{
		seed.pat[i] = Itemsort[length - 1 - i + 1];
	}
	Pop = new chromosome[Popsize];
	Pop[0] = seed;
	for (i = 1; i < Popsize; i++)
	{
		Pop[i].pat = new int[length + 1];
		memcpy(Pop[i].pat, seed.pat, (length + 1) * sizeof(int));
		for (j = 0; j < length + 1; j++)
		{
			Pop[i].pat[j] = mutate(Pop[i].pat[j], Pop[i].pat, mp);
		}
	}
}

void Select()//selsect the good chromosome;
{
	int i;
	for (i = 0; i < Popsize; i++)
	{
		double r = (rand() % 100) / 100.0;
		if (r > Pop[i].fitness*sp)
		{
			Pop[i].fitness = -1;
			delete[] Pop[i].pat;
			Pop[i].pat = NULL;
		}
	}
	sort(Pop, Pop + Popsize, compareC);
}

chromosome *corssover(int *pat1, int* pat2, chromosome* newPop, int k)//doing the crossover operation
{
	int r1 = rand() % (length + 1);
	int r2 = rand() % (length + 1);
	if (r1 > r2)
	{
		int r3 = r1;
		r1 = r2;
		r2 = r3;
	}

	int *npat1 = new int[length + 1];
	int *npat2 = new int[length + 1];
	//crossover, two point strategy
	memcpy(npat1, pat1, r1*sizeof(int));
	memcpy(npat1 + r1, pat2 + r1, (r2 - r1) * sizeof(int));
	memcpy(npat1 + r2, pat1 + r2, (length + 1 - r2) * sizeof(int));

	memcpy(npat2, pat2, r1 * sizeof(int));
	memcpy(npat2 + r1, pat1 + r1, (r2 - r1) * sizeof(int));
	memcpy(npat2 + r2, pat2 + r2, (length + 1 - r2) * sizeof(int));

	int i, j;
	for (i = 1; i < length + 1; i++)
	{
		for (j = i + 1; j < length + 1; j++)
		{
			if (npat1[i] == npat1[j])
			{
				npat1[j] = mutate(npat1[j], npat1, 1);
			}
			if (npat2[i] == npat2[j])
			{
				npat2[j] = mutate(npat2[j], npat2, 1);
			}
		}
	}

	newPop[k].pat = npat1;
	newPop[k+1].pat = npat2;
	return newPop;
}

void Cross()//doing the crossover operation
{
	chromosome *newPop = new chromosome[Popsize];
	int i, j, k;
	k = 0;
	for (i = 0; i < Popsize && Pop[i].pat != NULL && k<Popsize; i++)
	{
		for (j = i + 1; j < Popsize && Pop[j].pat != NULL && k<Popsize; j++)
		{
			double r = (rand() % 100) / 100.0;
			if (r < cp)
			{
				newPop = corssover(Pop[i].pat, Pop[j].pat, newPop, k);//doing the crossover operation
				k = k + 2;
			}
		}
	}
	for (i = 0; i < Popsize; i++)
	{
		delete[]Pop[i].pat;
	}
	delete Pop;
	Pop = newPop;//generate the new Population
}

void Mutat()//doing the mutation operation
{
	int i,j;
	for (i = 0; i < Popsize && Pop[i].pat!=NULL; i++)
	{
		for (j = 0; j < length + 1; j++)
		{
			Pop[i].pat[j] = mutate(Pop[i].pat[j], Pop[i].pat, mp);
		}
	}
}

void UpdateFitness()//Updating the fitness of every chromosome
{
	char * token;
	int value, i, j;

	for (i = 0; i < Popsize && Pop[i].fitness >= 0; i++)
	{
		Pop[i].sup1 = 0;
		Pop[i].sup2 = 0;
		Pop[i].sup3 = 0;
	}

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
					for (i = 0; i < Popsize && Pop[i].pat != NULL; i++)
					{
						for (j = 1; j < -Pop[i].pat[0]+1; j++)
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
			token = strtok(NULL, " \t\n");
		}
		for (i = 0; i < Popsize && Pop[i].pat != NULL; i++)
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
		Pop[i].sup1 = Pop[i].sup1 / transNum;
		Pop[i].sup2 = Pop[i].sup2 / transNum;
		Pop[i].sup3 = Pop[i].sup3 / transNum;
		Pop[i].fitness = (Pop[i].sup3- Pop[i].sup1*Pop[i].sup2)/(0.000000000001 + Pop[i].sup1*(1- Pop[i].sup2));
	}
}

class Rule//the structure to store rule
{
public:
	int *pat = NULL;
	Rule *next;
};

Rule *fRule=NULL;

void GenerateRules()
{
	Rule* sRule;
	int i, j;
	int* npat = new int[length + 1];
	for (i = 0; i < Popsize && Pop[i].fitness >= 0; i++)
	{
		if (Pop[i].sup3 / Pop[i].sup1 > minconf && Pop[i].sup3 / (Pop[i].sup1*Pop[i].sup2)>minlift)
		{
			memcpy(npat, Pop[i].pat, (length + 1)*sizeof(int));
			sort(npat+1, npat-npat[0]+1);
			sort(npat - npat[0] + 1, npat + length+1);
			for (sRule = fRule; sRule != NULL; sRule = sRule->next)
			{
				if (npat[0] == sRule->pat[0]
					&& !memcmp(npat+1, sRule->pat+1, -npat[0] * sizeof(int))
					&& !memcmp(npat + 1 - npat[0], sRule->pat + 1 - npat[0], (length + npat[0]) * sizeof(int)))
				{
					break;
				}
			}
			if (sRule == NULL)//show the 
			{
				sRule = new Rule;
				sRule->pat = npat;
				sRule->next = NULL;
				printf("IF ");
				for (j = 1; j < -npat[0] + 1; j++)
				{
					printf("%d ", npat[j]);
				}
				printf("THEN ");
				for (j = -npat[0] + 1; j < length+1; j++)
				{
					printf("%d ", npat[j]);
				}
				printf("       support %d confidence %f lift %f\n", int(Pop[i].sup3*transNum), Pop[i].sup3 / Pop[i].sup1, Pop[i].sup3 / (Pop[i].sup1*Pop[i].sup2));
				patternNum++;
			}
		}
	}
}

void armga()//the algorithm of armga
{
	int i;
	for (i = 0; i < GenNum; i++)
	{
		UpdateFitness();
		GenerateRules();
		Select();
		Cross();
		Mutat();
	}
	printf("There are %d rules generated\n", patternNum);
}


int main(int argc, char** argv)
{
	FILE *fIn;
	if (argc != 10)
	{
		printf("Usage: armga <input-filename> <minimum-confidence> <minimum-lift> <population size> <generation-number> <mutation-probability> <selection-probability> <crossover-probability> <length>\n");
	}
	else if (!(fIn = fopen(argv[1], "r")))
	{
		printf("Error in input file\n");
	}
	else if (atof(argv[2]) < 0 || atof(argv[2]) > 1)
	{
		printf("invalid minimum confidence");
	}
	else if (atof(argv[3]) < 0)
	{
		printf("invalid minimum lift");
	}
	else if (atoi(argv[4]) < 0)
	{
		printf("invalid populationn size");
	}
	else if (atoi(argv[5]) < 0)
	{
		printf("invalid generation number");
	}
	else if (atof(argv[6])<0 || atof(argv[7]) < 0 || atof(argv[8])< 0 || atof(argv[6])>1 || atof(argv[7]) >1 || atof(argv[8])>1)
	{
		printf("invalid probability");
	}
	else if (atoi(argv[9]) < 0)
	{
		printf("invalid length");
	}
	else
	{
		minconf = atof(argv[2]);
		minlift = atof(argv[3]);
		Popsize = atoi(argv[4]);
		GenNum = atoi(argv[5]);
		mp = atof(argv[6]);
		sp = atof(argv[7]);
		cp = atof(argv[8]);
		length = 10;
		if (Popsize % 20 > 0)
		{
			Popsize++;
		}
		ReadData(fIn);
		PreScan();
		initialise();
		armga();
	}

	printf("\npush retrun to exit");
	getchar();
	return 0;
}
