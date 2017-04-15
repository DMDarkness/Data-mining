#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <algorithm>

using namespace std;

#pragma warning(disable:4996) 
#define BUFFER 20000
#define MAXITEMNUM 200000//the maxinum number of items, items whose indexes higher than it will be ignored
int Items[MAXITEMNUM]= {0};//the array recording the support of every item
int itemsize = 0;


class Node//the node in FPtree
{
public:
	int value;//the value
	int num;//the number
	Node *parent;//parent node
	vector<Node*> Son;
	//Node *Son[BUFFER];
	//int sonNum;
};

class Header//the data struct of head table
{
public:
	int value;
	int num;
	Header *parent;
	Header *son;
	Node *Right[BUFFER];//the nodes linked with the head table
	int rNum;//the number of nodes linked with the head table
};

Header *fphead;
Node *fpTree;
double minsup;
double transNum;

Header *FreItem(FILE *fIn, double minsup)//Scanning the whole dataset once and output the headTable
{
	char string[BUFFER];
	char * token;
	int value;
	transNum = 0;
	int i;
	for (i = 0; fgets(string, BUFFER, fIn); i++)//Counting the support of every item
	{
		transNum++;
		token = strtok(string, " \t\n");
		while (token != NULL)
		{
			if (isdigit(*token))
			{
				value = atoi(token);
				if (value < MAXITEMNUM)
				{
					Items[value]++;
					if (value > itemsize)
					{
						itemsize=value;
					}
				}
			}
			token = strtok(NULL, " \t\n");
		}
	}

	Header *Head;
	Head = new Header;
	Head->num = -1;
	Head->parent = Head;
	Head->value = -1;
	Head->son = Head;
	Head->rNum = 0;

	for (i = 0; i < itemsize+1; i++)//building the head table
	{
		if (Items[i] >= minsup*transNum)
		{
			Header *Hsearch;
			Hsearch = Head;
			Header *Item;
			Item = new Header;
			Item->num = Items[i];
			Item->value = i;
			Item->rNum = 0;
			while(!(Item->num >= Hsearch->num && (Item->num<=Hsearch->parent->num || Hsearch->parent->num==-1)))
			{
				Hsearch = Hsearch->parent;
			}
			Item->son = Hsearch;
			Item->parent = Hsearch->parent;
			Hsearch->parent->son = Item;
			Hsearch->parent = Item;
		}
		else
		{
			Items[i] = -1;
		}
	}
	return Head;//Output the headTable
}

bool compare(int a, int b)//the operator to sort items in the transaction
{
	return Items[a]>Items[b];   
}

Node* FPtree(FILE *fIn, Header *Head)//Build FP-tree
{
	rewind(fIn);//reset the fIn to the begin of file
	Node *Tree;//The head of fp-tree
	Tree = new Node;
	Tree->num = -1;
	Tree->value = -1;
	Tree->parent = NULL;
	//Tree->sonNum = 0;
	char string[BUFFER];
	char * token;
	int value;
	int i,j,m,shnum;
	for (i = 0; fgets(string, BUFFER, fIn); i++)
	{
		int trans[BUFFER] = {0};
		j = 0;
		token = strtok(string, " \t\n");
		while (token != NULL)//read the transaction and put it into an array
		{
			if (isdigit(*token))
				value = atoi(token);
			if (value < itemsize+1)
			{
				if (Items[value] > 0)
				{
					trans[j] = value;
					j++;
				}
			}
			token = strtok(NULL, " \t\n");
		}
		sort(trans, trans + j, compare);//sort the items in the transactions according to the frequents
		Header *Hsearch;					
		Hsearch = Head->son;
		Node *Tsearch;
		Tsearch = Tree;
		for (shnum = 0; shnum < j && Items[trans[0]]>0 && Hsearch->value>0;)//Looking for items belonging to trans and Head
		{
			if (Hsearch->value == trans[shnum])//The item belonging to trans and Head
			{
				shnum++;
				int find = 0;
				for (m = 0; m < Tsearch->Son.size(); m++)//search the son of current node of tree
				{
					if (Tsearch->Son[m]->value == Hsearch->value)//if there is a node whose value is the item
					{
						Tsearch->Son[m]->num++;//its number puls one
						find = 1;
						Tsearch = Tsearch->Son[m];//the current node is changed
						break;
					}
				}
				if (find == 0)//if there is no son of the current node whose value is the item
				{
					//Tsearch->sonNum++;													
					Node *nN = new Node;//build a new son
					Tsearch->Son.push_back(nN);					
					Tsearch->Son[m]->value = Hsearch->value;	
					Tsearch->Son[m]->num = 1;
					//Tsearch->Son[m]->sonNum = 0;
					Tsearch->Son[m]->parent = Tsearch;
						
					Tsearch = Tsearch->Son[m];//the current node is changed
					Hsearch->Right[Hsearch->rNum] = Tsearch;//link this node to the head table
					Hsearch->rNum++;
				}
			}
			Hsearch = Hsearch->son;
		}
	}
	return Tree;
}

int sItems[MAXITEMNUM] = { 0 };

Header *condFreItem(Header *sH, double minsup)
{
	int i,Num;
	memset(sItems, 0, sizeof(sItems));
	for (i = 0; i < sH->rNum; i++)//sH->Right[0],sH->Right[1]....are the conditional basis
	{
		Node *sC;
		sC = sH->Right[i]->parent;
		Num = sH->Right[i]->num;
		while (sC->value != -1)
		{
			sItems[sC->value] = sItems[sC->value] + Num;
			sC = sC->parent;
		}
	}

	Header *sHead = new Header;
	sHead->num = -1;
	sHead->parent = sHead;
	sHead->value = -1;
	sHead->son = sHead;
	sHead->rNum = 0;

	for (i = 0; i < itemsize+1; i++)//building the conditional head table
	{
		if (sItems[i] >= minsup*transNum)
		{
			Header *Hsearch;
			Hsearch = sHead;
			Header *Item;
			Item = new Header;
			Item->num = sItems[i];
			Item->value = i;
			Item->rNum = 0;
			while (!(Item->num >= Hsearch->num && (Item->num <= Hsearch->parent->num || Hsearch->parent->num == -1)))
			{
				Hsearch = Hsearch->parent;
			}
			Item->son = Hsearch;
			Item->parent = Hsearch->parent;
			Hsearch->parent->son = Item;
			Hsearch->parent = Item;
		}
		else
		{
			sItems[i] = -1;
		}
	}
	return sHead;
}

bool comparecond(int a, int b)//the operator to sort items in the transaction
{
	return sItems[a]>sItems[b];
}

Node* condFPtree(Header *sH, Header *sHead)	//building the conditional fp tree
{
	Node *sTree;
	sTree = new Node;
	sTree->num = -1;
	sTree->value = -1;
	sTree->parent = NULL;
	//sTree->sonNum = 0;
	int i,j, m, shnum;

	for (i = 0; i < sH->rNum; i++)
	{
		Node *sC;
		sC = sH->Right[i]->parent;
		int trans[BUFFER] = { 0 };
		j = 0;
		while (sC->value != -1)	//store the transaction of conditional basis
		{
			trans[j] = sC->value;
			sC = sC->parent;
			j++;
		}
		sort(trans, trans + j, comparecond);//sort the items in the trans of the conditional basis according to their support in the conditional basis
		Header *Hsearch;
		Hsearch = sHead->son;
		Node *Tsearch;
		Tsearch = sTree;
		for (shnum = 0; shnum < j && sItems[trans[0]]>0 && Hsearch->value>0;)//Looking for items belonging to trans and Head
		{
			if (Hsearch->value == trans[shnum])//The item belonging to trans and Head
			{
				shnum++;
				int find = 0;
				for (m = 0; m < Tsearch->Son.size(); m++)//search the son of current node of tree
				{
					if (Tsearch->Son[m]->value == Hsearch->value)//if there is a node whose value is the item
					{
						Tsearch->Son[m]->num= Tsearch->Son[m]->num+sH->Right[i]->num;//its number puls one
						find = 1;
						Tsearch = Tsearch->Son[m];//the current node is changed
						break;
					}
				}
				if (find == 0)//if there is no son of the current node whose value is the item
				{
					//Tsearch->sonNum++;
					Node *nN = new Node;//build a new son
					Tsearch->Son.push_back(nN);
					Tsearch->Son[m]->value = Hsearch->value;
					Tsearch->Son[m]->num = sH->Right[i]->num;
					//Tsearch->Son[m]->sonNum = 0;
					Tsearch->Son[m]->parent = Tsearch;

					Tsearch = Tsearch->Son[m];//the current node is changed
					Hsearch->Right[Hsearch->rNum] = Tsearch;//link this node to the head table
					Hsearch->rNum++;
				}
			}
			Hsearch = Hsearch->son;
		}
	}
	return sTree;
}

void FPgrowth(Node *Tree, Header *Head, double minsup, int itemset[BUFFER],int len)	//mining the frequent itemsets
{
	Header *sH;
	sH = Head->parent;
	int i;
	while (sH->value != -1)
	{
		int beta[BUFFER] = { 0 };//show the frequent itemsets
		int length = len;
		for (i = 0; i < length; i++)
		{
			beta[i] = itemset[i];
		}
		beta[length] = sH->value;
		length++;
		for (i = 0; i < length; i++)
		{
			printf("%d ", beta[i]);
		}
		printf("support %d\n", sH->num);
		Header *sHead;
		sHead = condFreItem(sH, minsup);//building the headtable of the conditional basis
		Node *sTree;
		sTree = condFPtree(sH, sHead);//buildng the conditional fp tree
		FPgrowth(sTree, sHead, minsup, beta, length);//frquent itemsets are growthing
		sH = sH->parent;
	}
}

void main()
{
	minsup = 0.03;//setting the minimum support
	FILE *fIn;
	if(!(fIn = fopen("kosarak.dat", "r")))//input the data for mining
	{
		printf("Error in input file\n");
	}
	fphead = FreItem(fIn, minsup);//scanning the dataset and find those items with supports higher than minsup
	fpTree = FPtree(fIn, fphead);//building fp-tree
	int itemset[BUFFER] = { 0 };
	int len = 0;
	FPgrowth(fpTree, fphead, minsup, itemset,len);//mining the freqeunt itemsets
}
