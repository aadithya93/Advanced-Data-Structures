/*Disk based merge sort*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>
#include<time.h>
#include<sys/time.h>

void swap(int *a,int *b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void sift(int array[],int i, int size)
{
	int j,k,lg;
	while(i < size/2)
	{
		j = i*2;
		k = j+1;
		
		if(k<size && array[k] <= array[j])
			lg = k;
		else
			lg = j;
		
		if(array[i] <= array[lg])
			break;
		swap(&array[i],&array[lg]);
		i = lg;
	}
}
			
		
void heapify(int array[], int size)
{
	int i;
	i = (size-1)/2;
	while(i >= 0)
	{
		sift(array,i,size);
		i--;
	}
}

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

void basicMergesort(char input[], char output[])
{
	int iBuffer[1000];
	int oBuffer[1000];
	int run=0,i,j,end=0,min,k=0, minIndex;
	char runFiles[1000][50];
	int exec_s,exec_us,start_s,start_us,end_s,end_us;
	//START TIME HERE
	struct timeval ts; 
	gettimeofday( &ts, NULL ); 
	start_s = ts.tv_sec; 
	start_us = ts.tv_usec;
	//Step 1 - Read the blocks and create runs
	FILE *fp;
	fp = fopen( input, "rb" ); 
	
	while(fread(iBuffer,sizeof(int),1000,fp))
	{
		char n[4];
		if(!feof(fp))
		{
			//Step 2 - sort each block and create run files
			char runFile[50];
			strcpy(runFile,input);
			strcat(runFile,".");
			if(run/10 == 0)
				strcat(runFile,"00");
			else if(run/100 == 0)
				strcat(runFile,"0");
			sprintf(n,"%d",run);
			strcat(runFile,n);
			strcpy(runFiles[run],runFile);
			FILE *f;
			f = fopen(runFile,"wb");
			qsort(iBuffer,1000,sizeof(int),cmpfunc);
			fwrite(iBuffer,sizeof(int),1000,f);
			fclose(f);
			run++;
		}
	}
	fclose( fp );
	//Step 3 - Open each run file and load it to the buffer
	int bufferIndices[run];
	int fileOffsets[run];
	int size[run],fileEnd[run];
	int bufferRunSize = 1000/run;
	int bufferRunSizes[run];
	FILE* file;
	for(i = 0; i < run; i++)
	{
		file = fopen(runFiles[i],"rb");
		fseek(file,0,SEEK_END);
		size[i] = ftell(file);
		fclose(file);
		bufferIndices[i] = bufferRunSize;
		bufferRunSizes[i] = bufferRunSize;
		fileOffsets[i] = 0;
		fileEnd[i] = 0;
	}
	fp = fopen( output, "wb" ); 
	while(1)
	{
		end=0;
		//update buffers
		for(i = 0; i < run; i++)
		{
			if(bufferIndices[i] == bufferRunSize)
			{
				int cur;
				file = fopen(runFiles[i],"rb");
				fseek(file,fileOffsets[i],SEEK_SET);
				cur = ftell(file);
				for(j = 0; j < bufferRunSize; j++)
					if(cur < size[i])
					{
						fread(&iBuffer[bufferRunSize*i+j],sizeof(int),1,file);
						cur = ftell(file);
						bufferIndices[i] = 0;
					}
					else
					{
						fileEnd[i] = 1;
						bufferRunSizes[i] = j;
						break;
					}
				fileOffsets[i] = ftell(file);
				fclose(file);
			}
		}

		//compare first values and find min
		min = INT_MAX;
		for(i = 0; i < run; i++)
		{
			if(bufferIndices[i] < bufferRunSizes[i])
			{
				if(iBuffer[bufferRunSize*i+bufferIndices[i]] <= min) 
				{
					min = iBuffer[bufferRunSize*i+bufferIndices[i]];
					minIndex = i;
				}
			}
		}
		
		// write smallest value to output buffer
		oBuffer[k] = min;
		bufferIndices[minIndex]++;k++;
		//write buffer to file when full.
		if(k==1000)
		{
			fwrite( oBuffer, sizeof(int), 1000, fp ); 
			k=0;
		}
		//Terminating condition
		for(i = 0; i < run; i++)
		{
			end = end || !fileEnd[i] || (bufferIndices[i]<bufferRunSizes[i]);
		}
		if(end == 0) break;
	}
	fclose( fp );
	//END TIME HERE
	struct timeval te; 
	gettimeofday( &te, NULL ); 
	end_s = te.tv_sec; 
	end_us = te.tv_usec;
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

void multistepMergesort(char input[], char output[])
{
	int iBuffer[1000];
	int oBuffer[1000];
	int run=0,superRun=0,i,j,end=0,min,k=0, minIndex;
	char runFiles[1000][50];
	char superRunFiles[1000][50];
	int exec_s,exec_us,start_s,start_us,end_s,end_us;
	//START TIME HERE
	struct timeval ts; 
	gettimeofday( &ts, NULL ); 
	start_s = ts.tv_sec; 
	start_us = ts.tv_usec;
	//Step 1 - Read the blocks and create runs
	FILE *fp;
	fp = fopen( input, "rb" ); 
	
	while(fread(iBuffer,sizeof(int),1000,fp))
	{
		char n[4];
		if(!feof(fp))
		{
			//Step 2 - sort each block and create run files
			char runFile[50];
			strcpy(runFile,input);
			strcat(runFile,".");
			if(run/10 == 0)
				strcat(runFile,"00");
			else if(run/100 == 0)
				strcat(runFile,"0");
			sprintf(n,"%d",run);
			strcat(runFile,n);
			strcpy(runFiles[run],runFile);
			FILE *f;
			f = fopen(runFile,"wb");
			qsort(iBuffer,1000,sizeof(int),cmpfunc);
			fwrite(iBuffer,sizeof(int),1000,f);
			fclose(f);
			run++;
		}
	}
	fclose( fp );
	superRun = (run%15==0)? run/15:run/15 + 1;
	//printf("%d\n",superRun);
	for(i=0;i<superRun;i++)
	{
		char runFile[50],n[4];
		int numbRun;
		k=0;
		strcpy(runFile,input);
		strcat(runFile,".super.");
		if(i/10==0)
			strcat(runFile,"00");
		else if(i/100==0)
			strcat(runFile,"0");
		sprintf(n,"%d",i);
		strcat(runFile,n);
		strcpy(superRunFiles[i],runFile);
		FILE *f;
		numbRun = ((i+1)*15 <= run)? 15:run%15;
		//printf("%d\n",numbRun);
		int bufferIndices[numbRun];
		int fileOffsets[numbRun],size[numbRun],fileEnd[numbRun];
		int bufferRunSize = 1000/numbRun;
		int bufferRunSizes[run];
		//printf("reached here\n");
		FILE *file;
		for(j=0;j<numbRun;j++)
		{
			file = fopen(runFiles[i*15+j],"rb");
			fseek(file,0,SEEK_END);
			size[j] = ftell(file);
			fclose(file);
			bufferIndices[j] = bufferRunSize;
			bufferRunSizes[j] = bufferRunSize;
			fileOffsets[j] = 0;
			fileEnd[j] = 0;
		}
		//printf("reached here\n");
		f = fopen(superRunFiles[i],"wb");
		while(1)
		{
			end=0;
			//update buffers
			for(j = 0; j < numbRun; j++)
			{
				if(bufferIndices[j] == bufferRunSize)
				{
					int cur,l;
					file = fopen(runFiles[i*15+j],"rb");
					fseek(file,fileOffsets[j],SEEK_SET);
					cur = ftell(file);
					for(l = 0; l < bufferRunSize; l++)
						if(cur < size[j])
						{
							fread(&iBuffer[bufferRunSize*j+l],sizeof(int),1,file);
							cur = ftell(file);
							bufferIndices[j] = 0;
						}
						else
						{
							fileEnd[j] = 1;
							bufferRunSizes[j] = l;
							break;
						}
					fileOffsets[j] = ftell(file);
					fclose(file);
				}
			}

			//compare first values and find min
			min = INT_MAX;
			for(j = 0; j < numbRun; j++)
			{
				if(bufferIndices[j] < bufferRunSizes[j])
				{
					if(iBuffer[bufferRunSize*j+bufferIndices[j]] <= min) 
					{
						min = iBuffer[bufferRunSize*j+bufferIndices[j]];
						minIndex = j;
					}
				}
			}
		
			// write smallest value to output buffer
			oBuffer[k] = min;
			bufferIndices[minIndex]++;k++;
			//write buffer to file when full.
			if(k==1000)
			{
				fwrite( oBuffer, sizeof(int), 1000, f ); 
				k=0;
			}
			//Terminating condition
			for(j = 0; j < numbRun; j++)
			{
				int cur;
				end = end || !fileEnd[j] || (bufferIndices[j]<bufferRunSizes[j]);
			}
			if(end == 0)
 				break;

		}
		fclose(f);
	}

			
	//Step 3 - Open each run file and load it to the buffer
	int bufferIndices[superRun];
	int fileOffsets[superRun];
	int size[superRun],fileEnd[superRun];
	int bufferRunSize = 1000/superRun;
	int bufferRunSizes[superRun];
	//printf("%d\n",bufferRunSize);
	FILE* file;
	for(i = 0; i < superRun; i++)
	{
		file = fopen(superRunFiles[i],"rb");
		fseek(file,0,SEEK_END);
		size[i] = ftell(file);
		fclose(file);
		bufferIndices[i] = bufferRunSize;
		bufferRunSizes[i] = bufferRunSize;
		fileOffsets[i] = 0;
		fileEnd[i] = 0;
	}
	fp = fopen( output, "wb" ); 
	k=0;
	while(1)
	{
		end=0;
		//update buffers
		for(i = 0; i < superRun; i++)
		{
			if(bufferIndices[i] == bufferRunSize)
			{
				int cur;
				file = fopen(superRunFiles[i],"rb");
				fseek(file,fileOffsets[i],SEEK_SET);
				cur = ftell(file);
				for(j = 0; j < bufferRunSize; j++)
					if(cur < size[i])
					{
						fread(&iBuffer[bufferRunSize*i+j],sizeof(int),1,file);
						cur = ftell(file);
						bufferIndices[i] = 0;
					}
					else
					{
						fileEnd[i] = 1;
						bufferRunSizes[i] = j;
						break;
					}
				fileOffsets[i] = ftell(file);
				fclose(file);
			}
		}

		//compare first values and find min
		min = INT_MAX;
		for(i = 0; i < superRun; i++)
		{
			if(bufferIndices[i] < bufferRunSizes[i])
			{
				if(iBuffer[bufferRunSize*i+bufferIndices[i]] <= min) 
				{
					min = iBuffer[bufferRunSize*i+bufferIndices[i]];
					minIndex = i;
				}
			}
		}
		
		// write smallest value to output buffer
		oBuffer[k] = min;
		bufferIndices[minIndex]++;k++;
		//write buffer to file when full.
		if(k==1000)
		{
			fwrite( oBuffer, sizeof(int), 1000, fp ); 
			k=0;
		}
		//Terminating condition
		for(i = 0; i < superRun; i++)
		{
			end = end || !fileEnd[i] || (bufferIndices[i]<bufferRunSizes[i]);
		}
		if(end == 0) break;
	}
	fclose( fp );
	//END TIME HERE
	struct timeval te; 
	gettimeofday( &te, NULL ); 
	end_s = te.tv_sec; 
	end_us = te.tv_usec;
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

void replacementMergesort(char input[], char output[])
{
	int iBuffer[1000];
	int oBuffer[2000];
	int run=0,i,j,end=0,min,k=0, minIndex;
	char runFiles[1000][50];
	int pHeap = 750,sHeap = 0,buf=750;
	int exec_s,exec_us,start_s,start_us,end_s,end_us;
	//START TIME HERE
	struct timeval ts; 
	gettimeofday( &ts, NULL ); 
	start_s = ts.tv_sec; 
	start_us = ts.tv_usec;
	//Step 1 - Read the blocks and create runs
	FILE *fp;
	fp = fopen( input, "rb" ); 
	fread(iBuffer,sizeof(int),1000,fp);
	while(1)
	{
		heapify(iBuffer,pHeap);
		oBuffer[k] = iBuffer[0];
		k++;
		if(iBuffer[buf] >= iBuffer[0]) iBuffer[0] = iBuffer[buf];
		else
		{
			iBuffer[0] = iBuffer[pHeap-1];
			iBuffer[pHeap-1] = iBuffer[buf];
			pHeap--;sHeap++;
		}
		buf++;
		if(buf == 1000)
		{
			int j;
			buf = 750;
			for(j=0;(buf+j)<1000;j++)
				fread(&iBuffer[buf+j],sizeof(int),1,fp);
		}
		if(pHeap == 0)
		{
			int j;
			char n[4];
			//Step 2 - sort each block and create run files
			char runFile[50];
			strcpy(runFile,input);
			strcat(runFile,".");
			if(run/10 == 0)
				strcat(runFile,"00");
			else if(run/100 == 0)
				strcat(runFile,"0");
			sprintf(n,"%d",run);
			strcat(runFile,n);
			strcpy(runFiles[run],runFile);
			//printf("%d\n",k);
			FILE *f;
			f = fopen(runFile,"wb");
			for(j=0;j<k;j++)
				fwrite(&oBuffer[j],sizeof(int),1,f);
			fclose(f);
			pHeap = 750;sHeap = 0;k = 0;
			run++;
		}
		if(feof(fp))
		{
			int j;
			char n[4];
			//Step 2 - sort each block and create run files
			char runFile[50];
			strcpy(runFile,input);
			strcat(runFile,".");
			if(run/10 == 0)
				strcat(runFile,"00");
			else if(run/100 == 0)
				strcat(runFile,"0");
			sprintf(n,"%d",run);
			strcat(runFile,n);
			strcpy(runFiles[run],runFile);
			//printf("%d\n",k);
			FILE *f;
			f = fopen(runFile,"wb");
			for(j=0;j<k;j++)
				fwrite(&oBuffer[j],sizeof(int),1,f);
			fclose(f);
			run++;
			break;
		}
	}
	fclose( fp );
	char n[4];
	char runFile[50];
	strcpy(runFile,input);
	strcat(runFile,".");
	if(run/10 == 0)
		strcat(runFile,"00");
	else if(run/100 == 0)
		strcat(runFile,"0");
	sprintf(n,"%d",run);
	strcat(runFile,n);
	strcpy(runFiles[run],runFile);
	run++;
	qsort(iBuffer,750,sizeof(int),cmpfunc);
	FILE *f;
	f = fopen(runFile,"wb");
	fwrite(iBuffer,sizeof(int),750,f);
	fclose(f);
	                                                                                                                                                                                                                                                                                                                                                                                                              
	//Step 3 - Open each run file and load it to the buffer
	int bufferIndices[run];
	int fileOffsets[run];
	int size[run],fileEnd[run];
	int bufferRunSizes[run];
	int bufferRunSize = 1000/run;
	FILE* file;
	for(i = 0; i < run; i++)
	{
		file = fopen(runFiles[i],"rb");
		fseek(file,0,SEEK_END);
		size[i] = ftell(file);
		fclose(file);
		bufferIndices[i] = bufferRunSize;
		bufferRunSizes[i] = bufferRunSize;
		fileOffsets[i] = 0;
		fileEnd[i] = 0;
	}
	k=0;
	fp = fopen( output, "wb" ); 
	while(1)
	{
		end=0;
		//update buffers
		for(i = 0; i < run; i++)
		{
			if(bufferIndices[i] == bufferRunSize)
			{
				int cur;
				file = fopen(runFiles[i],"rb");
				fseek(file,fileOffsets[i],SEEK_SET);
				cur = ftell(file);
				for(j = 0; j < bufferRunSize; j++)
					if(cur < size[i])
					{
						fread(&iBuffer[bufferRunSize*i+j],sizeof(int),1,file);
						cur = ftell(file);
						bufferIndices[i] = 0;
					}
					else
					{
						fileEnd[i] = 1;
						bufferRunSizes[i] = j;
						break;
					}
				fileOffsets[i] = ftell(file);
				fclose(file);
			}
		}

		//compare first values and find min
		min = INT_MAX;
		for(i = 0; i < run; i++)
		{
			if(bufferIndices[i] < bufferRunSizes[i])
			{
				if(iBuffer[bufferRunSize*i+bufferIndices[i]] <= min) 
				{
					min = iBuffer[bufferRunSize*i+bufferIndices[i]];
					minIndex = i;
				}
			}
		}
		
		// write smallest value to output buffer
		oBuffer[k] = min;
		bufferIndices[minIndex]++;k++;
		//write buffer to file when full.
		if(k==1000)
		{
			fwrite( oBuffer, sizeof(int), 1000, fp ); 
			k=0;
		}
		//Terminating condition
		for(i = 0; i < run; i++)
		{
			end = end || !fileEnd[i] || (bufferIndices[i]<bufferRunSizes[i]);
		}
		if(end == 0) break;
	}
	fclose( fp );
	//END TIME HERE
	struct timeval te; 
	gettimeofday( &te, NULL ); 
	end_s = te.tv_sec; 
	end_us = te.tv_usec;
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

int main(int argc,char **argv)
{
	if (argc == 4)
	{
		//printf("%d\n",INT_MAX);
		if (strcmp(argv[1],"--basic")==0)
		{
			//printf("Basic Merge sort\n");
			basicMergesort(argv[2],argv[3]);
		}
		else if (strcmp(argv[1],"--multistep")==0)
		{
			//printf("Multistep Merge sort\n");
			multistepMergesort(argv[2],argv[3]);			
		}
		else if(strcmp(argv[1],"--replacement")==0)
		{
			//printf("Replacement Selection Merge sort\n");
			replacementMergesort(argv[2],argv[3]);
		}
	}
	return 0;
}
