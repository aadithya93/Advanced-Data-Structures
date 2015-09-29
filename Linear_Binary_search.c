/*In memory and On disk Linear and binary search*/

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<sys/time.h>

int fileLength(char* filename)
{
	FILE *input; //Inout File Stream
	int k,count=0;		//Key to read
	
	input = fopen(filename,"rb");
	
	while(!feof(input))
	{
		fread(&k, sizeof(int), 1, input);
		count++;
	}
	count--;
	fclose(input);
	return count;
}

void fileRead(char* filename, int *keys)
{
	FILE *input; //Inout File Stream
	int k,i=0;		//Key to read
	
	input = fopen(filename,"rb");

	while(!feof(input))
	{
		fread(&k, sizeof(int), 1, input);
		keys[i++]=k;
	}
	fclose(input);
}

void memLin(char *keyfile, char* seekfile,int k, int s)
{
	struct timeval ts,te;
	int *Keys,*Seeks,*Hits,i,j,exec_s,exec_us,start_s,start_us,end_s,end_us;
	Hits = (int*) malloc(s*sizeof(int));
	for(i=0;i<s;i++)
		Hits[i] = 0;
	Seeks = (int*) malloc(s*sizeof(int));
	fileRead(seekfile,Seeks);
	gettimeofday(&ts,NULL);
	start_s = ts.tv_sec;
	start_us = ts.tv_usec;//Start Time here
	Keys = (int*) malloc(k*sizeof(int));
        fileRead(keyfile,Keys);
	for(i=0;i<s;i++)
	{
		for(j=0;j<k;j++)
		{
			if(Keys[j] == Seeks[i])
				Hits[i]= 1;
		}
	}
	gettimeofday(&te,NULL);
	end_s = te.tv_sec;
	end_us = te.tv_usec;//Stop Time here
	for(i=0;i<s;i++)
	{
		if(Hits[i] == 1)
			printf("%12d: Yes\n", Seeks[i]);
		else
			printf("%12d: No\n", Seeks[i]);
	}
	if(end_us>=start_us)
	{
		exec_us = end_us - start_us;
		exec_s = end_s - start_s;
	}
	else
	{
		exec_us = start_us - end_us;
                exec_s = end_s - start_s - 1;
	}
	printf("Time: %ld.%06ld\n",exec_s,exec_us);
}

void memBin(char* keyfile, char* seekfile, int k, int s)
{
	struct timeval ts,te;
	int *Keys,*Seeks,*Hits,i,l,u,mid,exec_s,exec_us,start_s,start_us,end_s,end_us;
	Hits = (int*) malloc(s*sizeof(int));
	for(i=0;i<s;i++)
		Hits[i] = 0;
	Seeks = (int*) malloc(s*sizeof(int));
	fileRead(seekfile,Seeks);
	gettimeofday(&ts,NULL);
	start_s = ts.tv_sec;
	start_us = ts.tv_usec;//Start Time here
	Keys = (int*) malloc(k*sizeof(int));
        fileRead(keyfile,Keys);
	for(i=0;i<s;i++)
	{
		l=0;
		u=k-1;
		while((l<=u)&&(Hits[i] == 0))
		{
			mid = (l+u)/2;
			if(Seeks[i]<Keys[mid])u=mid-1;
			else if(Seeks[i]>Keys[mid])l=mid+1;
			else if(Seeks[i]==Keys[mid])Hits[i]=1;
		}
	}
	gettimeofday(&te,NULL);
	end_s = te.tv_sec;
	end_us = te.tv_usec;//Stop Time here
	for(i=0;i<s;i++)
	{
		if(Hits[i] == 1)
			printf("%12d: Yes\n", Seeks[i]);
		else
			printf("%12d: No\n", Seeks[i]);
	}
	if(end_us>=start_us)
	{
		exec_us = end_us - start_us;
		exec_s = end_s - start_s;
	}
	else
	{
		exec_us = start_us - end_us;
                exec_s = end_s - start_s - 1;
	}
	printf("Time: %ld.%06ld\n",exec_s,exec_us);
}

void diskLin(char *keyfile, char* seekfile, int s)
{
	struct timeval ts,te;
        int *Seeks,*Hits,i,j,k,mid,exec_s,exec_us,start_s,start_us,end_s,end_us;
	FILE *input;
        Hits = (int*) malloc(s*sizeof(int));
        for(i=0;i<s;i++)
                Hits[i] = 0;
        Seeks = (int*) malloc(s*sizeof(int));
        fileRead(seekfile,Seeks);
	input = fopen(keyfile, "rb");
        gettimeofday(&ts,NULL);
	start_s = ts.tv_sec;
	start_us = ts.tv_usec;//Start Time here
	for(i=0;i<s;i++)
       	{
		while(!feof(input))
		{
			fread(&k,sizeof(int),1,input);
			if(k == Seeks[i])
				Hits[i]= 1;       
		}
		fseek(input, 0, SEEK_SET);                                             	
	}
	fclose(input);
	gettimeofday(&te,NULL);
	end_s = te.tv_sec;
	end_us = te.tv_usec;//Stop Time here
	for(i=0;i<s;i++)
	{
		if(Hits[i] == 1)
			printf("%12d: Yes\n", Seeks[i]);
		else
			printf("%12d: No\n", Seeks[i]);
	}
        if(end_us>=start_us)
	{
		exec_us = end_us - start_us;
		exec_s = end_s - start_s;
	}
	else
	{
		exec_us = start_us - end_us;
                exec_s = end_s - start_s - 1;
	}
	printf("Time: %ld.%06ld\n",exec_s,exec_us);                                                                                                                
}

void diskBin(char* keyfile,char* seekfile, int k, int s)
{
	struct timeval ts,te;
	int *Seeks,*Hits,i,key,l,u,mid,exec_s,exec_us,start_s,start_us,end_s,end_us;
	FILE *input;
        Hits = (int*) malloc(s*sizeof(int));
        for(i=0;i<s;i++)
                Hits[i] = 0;
        Seeks = (int*) malloc(s*sizeof(int));
        fileRead(seekfile,Seeks);
	input = fopen(keyfile, "rb");
        gettimeofday(&ts,NULL);
	start_s = ts.tv_sec;
	start_us = ts.tv_usec;//Start Time here
	for(i=0;i<s;i++)
       	{
		l=0;
		u=k-1;
		while((l<=u)&&(Hits[i] == 0))
		{
			mid = (l+u)/2;
			fseek(input,mid*sizeof(int),SEEK_SET);
			fread(&key,sizeof(int),1,input);
			if(Seeks[i]<key)u=mid-1;
			else if(Seeks[i]>key)l=mid+1;
			else if(Seeks[i]==key)Hits[i]=1;
		}
	}
	fclose(input);
	gettimeofday(&te,NULL);
	end_s = te.tv_sec;
	end_us = te.tv_usec;//Stop Time here
	for(i=0;i<s;i++)
	{
		if(Hits[i] == 1)
			printf("%12d: Yes\n", Seeks[i]);
		else
			printf("%12d: No\n", Seeks[i]);
	}
	if(end_us>=start_us)
	{
		exec_us = end_us - start_us;
		exec_s = end_s - start_s;
	}
	else
	{
		exec_us = start_us - end_us;
                exec_s = end_s - start_s - 1;
	}
	printf("Time: %ld.%06ld\n",exec_s,exec_us);
}
	

int main(int *argc, char *argv[])
{
	if(argc==4)
	{
		int key_len,seek_len;
		if(strcmp(argv[1],"--mem-lin")==0)
		{
			//printf("In memory linear search\n");
			key_len = fileLength(argv[2]);
                	seek_len = fileLength(argv[3]);
			memLin(argv[2],argv[3],key_len,seek_len);
		}
		else if(strcmp(argv[1],"--mem-bin")==0)
		{
			//printf("In memory binary search\n");
			key_len = fileLength(argv[2]);
                	seek_len = fileLength(argv[3]);
			memBin(argv[2],argv[3],key_len,seek_len);
		}
		else if(strcmp(argv[1],"--disk-lin")==0)
		{
			//printf("On disk linear search\n");
			seek_len = fileLength(argv[3]);
			diskLin(argv[2],argv[3],seek_len);
		}
		else if(strcmp(argv[1],"--disk-bin")==0)
		{
			//printf("On disk binary search\n");
			key_len = fileLength(argv[2]);
			seek_len = fileLength(argv[3]);
			diskBin(argv[2],argv[3],key_len,seek_len);
		}
	}
	return 0;
}
	
	
	
