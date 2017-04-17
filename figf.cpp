#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <math.h>

/**********************************************************************************************************************************************************************************
File: figf.cpp
Contents: frequent item sets mining from datasets with high number of transactions by granular computing and fuzzy set theory
Author: Zhongjie Zhang, Jian Huang, Ying Wei
Citation: Zhang Z, Huang J, Wei Y. FI-FG: frequent item sets mining from datasets with high number of transactions by granular computing and fuzzy set theory[J]. Mathematical Problems in Engineering, 2015, 2015.
**********************************************************************************************************************************************************************************/

using namespace std;

#pragma warning(disable:4996) 
#define BUFFER 20000//this code only support those dataset with items less than 10000, the extra items will be ignored

double minsup;//minimum support
double transNum = 0;//the number of transaction
char str[BUFFER];
int Items[BUFFER] = { 0 };//the support of every item
int Id[BUFFER] = { 0 };//transform Items to BinaryItems
int BinaryItems[BUFFER] = { 0 };//the binary vector for every item with support higher than minimum support
int BinaryLen = 0;//the length of BinaryItems
int tnorm;//the t-norm used to calculate the support of the itemset, 1:minimum t-norm   else: product t-norm
bool Apri;//1: this algorithm becomes the standard apriori
int patternNum = 0;

double afa;//the parameter to make the granularity flexible

void PreScan(FILE *fIn)
{
	int value;//the variable to record the item in every transaction
	int i;
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
		if (Items[i] >= minsup*transNum)
		{
			BinaryLen++;
			Id[i] = BinaryLen;
			BinaryItems[Id[i]-1] = i;
		}
		else
		{
			Items[i] = 0;
		}
	}
}

class Granule
{
public:
	double *gtrans;//the granule
	int num = 0;//the number of transactions contained by this granule
	Granule *next;//the next granule
};

Granule *Gdataset;//create the granular dataset
int GdNum = 0;

void Granulation(FILE *fIn)//compress the dataset
{
	rewind(fIn);//reset the fIn to the begin of file
	int value;//the variable to record the item in every transaction
	int i,j;
	char * token;

	double *gtrans = new double[BinaryLen];//the granular transaction
	bool *btrans = new bool[BinaryLen];//create a new binary transaction
	bool *ntrans = new bool[BinaryLen];//create a new binary transaction

	int Gnum = 0;//the number of transactions of the current granules
	double diff = 0;//the accumulate hamming distance in the granules
	double specificity = 1;//the specificity of the current granules
	double coverage = 0;//the coverage of the current granules
	bool iMax = 0;//whether the specificity*coverage reaches the highest value

	double Col[BUFFER];//the array recording specificity*coverage

	memset(gtrans, 0, BinaryLen * sizeof(double));//initialize the granular transaction
	memset(btrans, false, BinaryLen * sizeof(bool));//initialize the binary transaction
	memset(ntrans, false, BinaryLen * sizeof(bool));//initialize the binary transaction
	memset(Col, 0, BUFFER * sizeof(double));//initialize the array of specificity*coverage

	Granule *cGranule = new Granule;//initialize the first granule
	Gdataset = cGranule;

	for (i = 0; fgets(str, BUFFER, fIn); i++)//Counting the support of every item
	{
		token = strtok(str, " \t\n");
		while (token != NULL)
		{
			if (isdigit(*token))
			{
				value = atoi(token);
				if (value < BUFFER && Items[value]>0)
				{
					ntrans[Id[value] - 1] = true;
					gtrans[Id[value] - 1]++;//update the granular transaction
				}
			}
			token = strtok(NULL, " \t\n");
		}

		if (Gnum > 0)
		{
			double intect = 0.0000000001;
			double unionn = 0.0000000001;//to make sure that the denominator is not zero
			for (j = 0; j < BinaryLen; j++)//computing the hamming distance between the new binary transaction and the old binary transaction
			{
				if (ntrans[j] + btrans[j] > 0)
				{
					unionn++;
				}
				if (ntrans[j] + btrans[j] == 2)
				{
					intect++;
				}
			}
			diff = diff + 1 - intect/unionn;//accumulate the hammining distance in the granule
			memcpy(btrans, ntrans, BinaryLen * sizeof(bool));//copy the new binary transaction to the old binary transaction
			memset(ntrans, false, BinaryLen * sizeof(bool));//initialize the new binary transaction
		}
		else
		{
			memcpy(btrans, ntrans, BinaryLen * sizeof(bool));//copy the new binary transaction to the old binary transaction
			memset(ntrans, false, BinaryLen * sizeof(bool));//initialize the new binary transaction
		}

		Gnum++;//the number of transactions in this granule plus one
		specificity = exp(-afa*diff);//calculate the specificity of this granule
		coverage = Gnum;//calculate the coverage of this granule
		Col[Gnum - 1] = specificity*coverage;//update the new specificity*coverage

		if (Gnum > 1 || Apri)//check whether specificity*coverage reaches the highest value
		{
			if (Col[Gnum - 1] - Col[Gnum - 2] <= 0 || Gnum >= BUFFER || Apri)
			{
				iMax = 1;
				for (j = 0; j < BinaryLen; j++)//fuzzylies the granular transaction
				{
					gtrans[j] = gtrans[j] / (Gnum);
				}
			}
		}
	

		if (iMax)//if the specificity*coverage reaches the highest value
		{
			GdNum ++ ;
			cGranule->gtrans = gtrans;
			cGranule->num = Gnum;
			cGranule->next = new Granule;//create a new transaction
			cGranule = cGranule->next;

			iMax = 0;
			specificity = 1;
			coverage = 0;
			Gnum = 0;
			diff = 0;

			gtrans = new double[BinaryLen];//the new granular transaction
			memset(gtrans, 0, BinaryLen * sizeof(double));//initialize the granular transaction
			memset(ntrans, false, BinaryLen * sizeof(bool));//initialize the new binary transaction
			memset(Col, 0, BUFFER * sizeof(double));//initialize the array of specificity*coverage
		}
	}
	if (iMax == 0)
	{
		for (j = 0; j < BinaryLen; j++)//fuzzylies the granular transaction
		{
			if (Gnum == 0)
			{
				Gnum++;
			}
			gtrans[j] = gtrans[j] / Gnum;
		}
		cGranule->gtrans = gtrans;
		cGranule->num = Gnum;
		cGranule->next = NULL;
	}
	delete[] btrans;
	delete[] ntrans;
}

class Candi
{
public:
	bool* c;
	Candi* next;
	double sup = 0;
};
int CandNum = 0;

Candi *Candidate;//the list of candidate itemsets

void Apriori()//mining the Granules and output the final results by Apriori
{
	int i, j, k = 0, m, ncLen;
	Candi *sCand = new Candi;
	while (CandNum > 0 || k==0)
	{
		if (k == 0)//initial the candidate itemsets with only one item
		{
			Candidate = sCand;
			CandNum = 0;
			for (i = 0; i < BinaryLen; i++)
			{
				if (i > 0)
				{
					sCand->next = new Candi;
					sCand = sCand->next;
				}
				bool *nc = new bool[BinaryLen];//build a new candidate itemset
				memset(nc, false, BinaryLen * sizeof(bool));//initial it as an empty set
				nc[i] = true;
				sCand->c = nc;
				sCand->next = NULL;
				CandNum++;
			}
		}
		else
		{
			Granule *sGran;
			Candi *jCand, *mCand, *nCand;
			sGran = Gdataset;//scanning the granular dataset from its head
			while (sGran!= NULL)//scanning the granular dataset to prune the candidate itemset
			{
				for (sCand=Candidate; sCand!=NULL; sCand=sCand->next)
				{
					double sss = 1;
					for (j = 0; j < BinaryLen; j++)
					{
						if (sCand->c[j])
						{
							if (sss > sGran->gtrans[j] && tnorm==1)
							{
								sss = sGran->gtrans[j];//the minimum support
							}
							else
							{
								sss = sss*sGran->gtrans[j];//the product support
							}
						}
						//s = s + sGran->gtrans[j] * Candidate[i][j];//calculate the support of pattern in this granular transaction
					}
					sCand->sup = sCand->sup + sGran->num*sss;//update the support of pattern
				}
				sGran = sGran->next;//scanning the next granule transaction
			}
			for (sCand = Candidate; sCand != NULL; sCand = sCand->next)//remove those infrequent candidate itemsets
			{
				if (sCand->sup < minsup*transNum)
				{
					delete[] sCand->c;
					sCand->c = NULL;
				}
				else//show the frequent itemset
				{
					for (j = 0; j < BinaryLen; j++)
					{
						if (sCand->c[j] > 0)
						{
							printf("%d ", BinaryItems[j]);
						}
					}
					printf(" gsupport %f\n", sCand->sup); patternNum++;
				}
			}
			Candi * newCand = NULL;//the new candidate itemset
			Candi *snCand = NULL;
			int nCandNum = 0;
			for (sCand = Candidate; sCand != NULL; sCand = sCand->next)//combining the new candidate itemsets
			{
				for (jCand = sCand->next; jCand!=NULL && sCand->c !=NULL; jCand=jCand->next)
				{
					if (jCand->c !=NULL)
					{
						ncLen = 0;//the length of the combined itemset
						bool *nc = new bool[BinaryLen];//build a new itemset to conbine two frequent itemsets
						memset(nc, false, BinaryLen * sizeof(bool));//the new itemset is initialized as an empyty set
						for (m = 0; m < BinaryLen; m++)//doing the union operation
						{
							if (sCand->c[m] || jCand->c[m])
							{
								ncLen++;//record the length of the new itemset
								nc[m] = true;
							}
						}
						if (ncLen == k + 1)//if this itemset with length equaling to k+1
						{
							bool rp = false;
							for (mCand=newCand; mCand!=NULL; mCand=mCand->next)//check whether this itemset has been already in the new candidiate
							{
								if (!memcmp(nc, mCand->c, BinaryLen * sizeof(bool)))//check if they are same
								{
									rp = true;
									break;
								}
							}
							for (m = 0; m < BinaryLen && !rp; m++)//check every subset with length k to find whether they are all frequent itemsets
							{
								if (nc[m])
								{
									bool *onc= new bool[BinaryLen];//build a new 
									memcpy(onc, nc, BinaryLen * sizeof(bool));
									onc[m] = false;
									bool exit = false;
									for (nCand=Candidate; nCand!=NULL; nCand = nCand->next)//check if any subset of the candidate itemsets is the frequent itemset
									{
										if (nCand->c != NULL)
										{
											if (!memcmp(onc, nCand->c, BinaryLen * sizeof(bool)))//check if they are same
											{
												exit = true;
												break;
											}
										}
									}
									delete []onc;//free the memory
									if (!exit)//if this subset is not a frequent itemset
									{
										delete[]nc;//delete this combined itemsets
										break;//ignore this itemset
									}
								}
							}
							if (m == BinaryLen)//if all the subsets are frequent itemsets
							{
								if (nCandNum > 0)
								{
									snCand->next = new Candi;
									snCand = snCand->next;
								}
								else
								{
									snCand = new Candi;
									newCand = snCand;
								}
								snCand->c = nc;
								snCand->next = NULL;
								nCandNum++;//the new candidate itemsets plus one
							}
						}
					}
				}
			}
			for (sCand = Candidate; sCand!=NULL; sCand = sCand->next)
			{
				delete[] sCand->c;//free the memory
			}
			Candidate = newCand;
			CandNum = nCandNum;
		}
		k++;
	}
}

void main()
{
	/*Input the parameter*/
	minsup = 0.01;//the minimum support
	afa = 0.01;//the parameter for specificity, the lower of it, the lower importance of specificity
	tnorm = 2;//the t-norm, 1: minimum t-norm, other: product t-norm
	Apri = false;//if Apri is true, this algorithm becomes the Apriori
	FILE *fIn;
	if (!(fIn = fopen("kosarak.dat", "r")))//input the data for mining
	{
		printf("Error in input file. The test data is kosarak.dat, and it can obtained from http://fimi.ua.ac.be/data/. \n");
	}
	/*Input the parameter completed*/

	PreScan(fIn);//delete those infrequent items
	Granulation(fIn);//generating those information granules
	Apriori();//applying Apriori to mine those granules
	printf("There are %d frequent pattern ooutput \n\n push the return to exit\n",patternNum);
	getchar();
}
