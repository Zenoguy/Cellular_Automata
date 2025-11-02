/* This program generates the states of a given CA. */

#include<stdio.h>
#include<stdlib.h>
//#include<math.h>

void	DecToBin(int k, char *s, int n)
{
	int	i;

	for(i=0;i<n;i++)
	{
		s[i] = (k&1);
		k = k/2;
	}
	if(k!=0)
		printf("WARNING!!! wrong conversion from decimal to binary.\n");
}

int power(int n)
{
	int i, product = 1;
	for(i=n;i>0;i--)
		product *= 2;
	return product;
}

int	BinToDec2(char *str, int n)
{
	int	i, sum=0;
	for(i=0;i<n;i++)
		sum += str[i]*power(n-i-1);
	return sum;
}

int	BinToDec(char *str, int n)
{
	int	i, sum=0;
	for(i=0;i<n;i++)
		sum += str[i]*(1<<i);
	return sum;
}

void	NextState(int size, char **Rule_Bin, char *Q)
{
	int     i,*temp_Q;

        temp_Q = (int*)malloc(size*sizeof(int));

        temp_Q[0]=Rule_Bin[0][2*Q[0]+Q[1]];

        for(i=1;i<size-1;i++)
                temp_Q[i]=Rule_Bin[i][4*Q[i-1]+2*Q[i]+Q[i+1]];

        temp_Q[i]=Rule_Bin[i][4*Q[i-1]+2*Q[i]];

        for(i=0;i<size;i++)
                Q[i] = temp_Q[i];

        free(temp_Q);
}


int	main(int argc, char **argv)
{
	int	i, j, k, n,l;
	char	**Rule, *check, *State;

	if(argc<2)
	{
		printf("Usage:%s <n> <rule 1> <rule 2> ... <rule n>\n",argv[0]);
		exit(0);
	}
	n = atoi(argv[1]);
	if(argc!=n+2)
	{
		printf("Usage:%s <n> <rule 1> <rule 2> ... <rule n>\n",argv[0]);
		exit(0);
	}

	if(n>=32)
	{
		printf("Size of CA must be less than 32.\n");
		exit(0);
	}

	check = (char*)calloc(1<<n,1);
	if(check==NULL)
	{
		perror("check");
		exit(0);
	}

	if((Rule = (char**)malloc(n*sizeof(char*)))==NULL)
	{
		perror("Rule");
		exit(0);
	}
	for(i=0;i<n;i++)
		if((Rule[i] = (char*)calloc(8,1))==NULL)
		{
			perror("Rule");
			exit(0);
		}

	if((State = (char*)calloc(n,1))==NULL)
	{
		perror("Rule");
		exit(0);
	}

	for(i=0;i<n;i++)
		DecToBin(atoi(argv[i+2]),Rule[i],8);

	DecToBin(0,State,n);
	for(i=0;i<n;i++)
		printf(" %d",State[i]);
	printf(" (0)\n");
	check[0] = 1;
	for(;;)
	{
		NextState(n,Rule,State);
		for(i=0;i<n;i++)
			printf(" %d",State[i]);
		k = BinToDec(State,n);
		//printf(" (%d)",k);
		l = BinToDec2(State,n);
		printf(" (%d)\n",l);
			

		if(check[k])
		{
			for(j=1;j<(1<<n) && check[j];j++);
			if(j==(1<<n))
				return 0;

			DecToBin(j,State,n);
			printf("\n");
			for(i=0;i<n;i++)
				printf(" %d",State[i]);
			//printf(" (%d)\n",j);
			l = BinToDec2(State,n);
			printf(" (%d)\n",l);
			check[j] = 1;
		}
		else
		{
			DecToBin(k,State,n);
			check[k] = 1;
		}
	}
}
