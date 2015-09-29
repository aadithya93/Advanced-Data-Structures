/*Availability lists implementation using Best fit, Worst fit and Best Fit*/

#include<stdio.h>
#include<malloc.h>
#include<string.h>

typedef struct 
{
	int key;
	long offset;
}index_S;

typedef struct
{
	int size;
	long offset;
}availability_S;

index_S PriKeyIndex[10000];
availability_S AvailabilityList[10000];
int numIndex=0,numAL=0;

void loadIndex()
{
	FILE *f;
	index_S k;
	int i;
	
	f = fopen("index.bin","rb");
	fread(&numIndex,sizeof(int),1,f);
	for(i=0;i<numIndex;i++)
	{
		fread(&k,sizeof(index_S),1,f);
		PriKeyIndex[i] = k;
	}
	fclose(f);
}

void writeIndex()
{
	FILE *f;
	int i;
	
	f = fopen("index.bin","wb");
	fwrite(&numIndex,sizeof(int),1,f);
	for(i=0;i<numIndex;i++)
	{
		fwrite(&PriKeyIndex[i],sizeof(index_S),1,f);
	}
	fclose(f);
}

void loadAvailabilityList()
{
	FILE *f;
	availability_S k;
	int i;
	
	f = fopen("availability.bin","rb");
	fread(&numAL,sizeof(int),1,f);
	for(i=0;i<numAL;i++)
	{
		fread(&k,sizeof(availability_S),1,f);
		AvailabilityList[i] = k;
	}
	fclose(f);
}

void writeAL()
{
	FILE *f;
	int i;
	
	f = fopen("availability.bin","wb");
	fwrite(&numAL,sizeof(int),1,f);
	for(i=0;i<numAL;i++)
	{
		fwrite(&AvailabilityList[i],sizeof(availability_S),1,f);
	}
	fclose(f);
}

void printAL()
{
	int i;
	printf("Availability:\n");
	for(i=0;i<numAL;i++)
		printf( "size=%d: offset=%ld\n", AvailabilityList[i].size, AvailabilityList[i].offset );
}

void printIndex()
{
	int i;
	printf("Index:\n");
	for(i=0;i<numIndex;i++)
		printf( "key=%d: offset=%ld\n", PriKeyIndex[i].key, PriKeyIndex[i].offset );
}

int binarySearchIndex(int k)
{
	int l,u,mid,pos=-1;
	l=0;
	u=numIndex-1;
	while((l<=u)&&(pos==-1))
	{
		mid = (l+u)/2;
		if(PriKeyIndex[mid].key>k)u=mid-1;
		else if(PriKeyIndex[mid].key<k)l=mid+1;
		else if(PriKeyIndex[mid].key==k)pos=mid;
	}
	return pos;
}

long writeLocation(int size)
{
	int i,j,offset=0;
	for(i=0;i<numAL;i++)
	{
		if(AvailabilityList[i].size==size)
		{
			offset = AvailabilityList[i].offset;
			numAL--;
			for(j=i;j<numAL;j++)
				AvailabilityList[j]=AvailabilityList[j+1];
		}
		else if(AvailabilityList[i].size>size)
		{
			availability_S temp;
			offset = AvailabilityList[i].offset;
			AvailabilityList[i].size -= size;
			AvailabilityList[i].offset += size;
			temp.size = AvailabilityList[i].size;
			temp.offset = AvailabilityList[i].offset;
			for(j=i;j<numAL-1;j++)
				AvailabilityList[j] = AvailabilityList[j+1];
			AvailabilityList[j].size = temp.size;
			AvailabilityList[j].offset = temp.offset;
		}
	}
	return offset;
}

int cmpIndex(const void *a,const void *b)
{
	const index_S *elem1 = a;
	const index_S *elem2 = b;
	
	if(elem1->key < elem2->key)
		return -1;
	else if(elem1->key > elem2->key)
		return 1;
	else
		return 0;
}

int cmpALinc(const void *a,const void *b)
{
	const availability_S *elem1 = a;
	const availability_S *elem2 = b;
	
	if(elem1->size < elem2->size)
		return -1;
	else if(elem1->size > elem2->size)
		return 1;
	else
		return 0;
}

int cmpALdec(const void *a,const void *b)
{
	const availability_S *elem1 = a;
	const availability_S *elem2 = b;
	
	if(elem1->size < elem2->size)
		return 1;
	else if(elem1->size > elem2->size)
		return -1;
	else
		return 0;
}

void firstFit(char *filename)
{
	char *buf;
	int rec_size;
	long rec_offset;
	int k,hole_size=0;
	char cmd[200];
	int i,j;
	FILE *fp; //Input File Stream
	if((fp = fopen( filename, "r+b" )) == NULL ) 
	{ 
		fp = fopen( filename, "w+b" );
	}
	else
	{
		loadIndex();
		loadAvailabilityList();
	}
	while(5)
	{
		j=0;
		char key[20];
		gets(cmd);
		//printf("%s\n",cmd);
		if(strstr(cmd,"add"))
		{
			for(i=4;cmd[i]!=' ';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			i++;
			key[j]='\0';
			k = atoi(key);
			rec_size = strlen(cmd)-5-j;
			buf=(char*)malloc(sizeof(char)*(rec_size+1));
			j=0;
			while(i<strlen(cmd))
			{
				buf[j] = cmd[i];
				j++;i++;
			}
			buf[j]='\0';		
			if(binarySearchIndex(k)==-1)
			{
				rec_offset = writeLocation(rec_size+sizeof(rec_size));
				if(rec_offset == 0)
				{
					fseek(fp,0,SEEK_END);
					rec_offset = ftell(fp);
				}
				PriKeyIndex[numIndex].key = k;
				PriKeyIndex[numIndex].offset = rec_offset;
				numIndex++;
				fseek(fp,rec_offset,SEEK_SET);
				fwrite(&rec_size,sizeof(int),1,fp);
				fwrite(buf,sizeof(char),rec_size,fp);
			}
			else
			{
				printf("Record with SID=%d exists\n",k);
			}
			qsort(PriKeyIndex,numIndex,sizeof(index_S),cmpIndex);
		}	
		else if(strstr(cmd,"find"))
		{
			int pos;
			for(i=5;cmd[i]!='\0';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			key[j]='\0';
			k = atoi(key);
			pos = binarySearchIndex(k);
			if(pos==-1)
				printf("No record with SID=%d exists\n",k);
			else
			{
				rec_offset = PriKeyIndex[pos].offset; 
				fseek( fp, rec_offset, SEEK_SET ); 
				fread( &rec_size, sizeof( int ), 1, fp ); 

				buf = (char*)malloc( sizeof(char)*(rec_size + 1)); 
				fread( buf, sizeof(char), rec_size, fp ); 
				buf[ rec_size ] = '\0';
				printf("%s\n",buf);
			}
		}
		else if(strstr(cmd,"del"))
		{
			int pos;
			for(i=4;cmd[i]!='\0';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			key[j]='\0';
			k = atoi(key);
			pos = binarySearchIndex(k);
			if(pos==-1)
				printf("No record with SID=%d exists\n",k);
			else
			{
				rec_offset = PriKeyIndex[pos].offset; 
				fseek( fp, rec_offset, SEEK_SET ); 
				fread( &rec_size, sizeof( int ), 1, fp );

				AvailabilityList[numAL].size = rec_size+sizeof(int);
				AvailabilityList[numAL].offset = rec_offset;
				numAL++;

				numIndex--;
				for(i=pos;i<numIndex;i++)
					PriKeyIndex[i]=PriKeyIndex[i+1];
			}		
		}
		else if(strcmp(cmd,"end")==0)break;
	}
	printIndex();
	printAL();
	printf( "Number of holes: %d\n", numAL );
	for(i=0;i<numAL;i++)
		hole_size+=AvailabilityList[i].size;
	printf( "Hole space: %d\n", hole_size );
	writeIndex();
	writeAL();
}

void bestFit(char *filename)
{
	char *buf;
	int rec_size;
	long rec_offset;
	int k,hole_size=0;
	char cmd[200];
	int i,j;
	FILE *fp; //Input File Stream
	if((fp = fopen( filename, "r+b" )) == NULL ) 
	{ 
		fp = fopen( filename, "w+b" );
	}
	else
	{
		loadIndex();
		loadAvailabilityList();
	}
	while(5)
	{
		j=0;
		char key[20];
		gets(cmd);
		//printf("%s\n",cmd);
		if(strstr(cmd,"add"))
		{
			for(i=4;cmd[i]!=' ';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			i++;
			key[j]='\0';
			k = atoi(key);
			rec_size = strlen(cmd)-5-j;
			buf=(char*)malloc(sizeof(char)*(rec_size+1));
			j=0;
			while(i<strlen(cmd))
			{
				buf[j] = cmd[i];
				j++;i++;
			}
			buf[j]='\0';		
			if(binarySearchIndex(k)==-1)
			{
				rec_offset = writeLocation(rec_size+sizeof(rec_size));
				if(rec_offset == 0)
				{
					fseek(fp,0,SEEK_END);
					rec_offset = ftell(fp);
				}
				PriKeyIndex[numIndex].key = k;
				PriKeyIndex[numIndex].offset = rec_offset;
				numIndex++;
				fseek(fp,rec_offset,SEEK_SET);
				fwrite(&rec_size,sizeof(int),1,fp);
				fwrite(buf,sizeof(char),rec_size,fp);
			}
			else
			{
				printf("Record with SID=%d exists\n",k);
			}
			qsort(PriKeyIndex,numIndex,sizeof(index_S),cmpIndex);
			qsort(AvailabilityList,numAL,sizeof(availability_S),cmpALinc);			
		}	
		else if(strstr(cmd,"find"))
		{
			int pos;
			for(i=5;cmd[i]!='\0';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			key[j]='\0';
			k = atoi(key);
			pos = binarySearchIndex(k);
			if(pos==-1)
				printf("No record with SID=%d exists\n",k);
			else
			{
				rec_offset = PriKeyIndex[pos].offset; 
				fseek( fp, rec_offset, SEEK_SET ); 
				fread( &rec_size, sizeof( int ), 1, fp ); 

				buf = (char*)malloc( sizeof(char)*(rec_size + 1)); 
				fread( buf, sizeof(char), rec_size, fp ); 
				buf[ rec_size ] = '\0';
				printf("%s\n",buf);
			}
		}
		else if(strstr(cmd,"del"))
		{
			int pos;
			for(i=4;cmd[i]!='\0';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			key[j]='\0';
			k = atoi(key);
			pos = binarySearchIndex(k);
			if(pos==-1)
				printf("No record with SID=%d exists\n",k);
			else
			{
				rec_offset = PriKeyIndex[pos].offset; 
				fseek( fp, rec_offset, SEEK_SET ); 
				fread( &rec_size, sizeof( int ), 1, fp );

				AvailabilityList[numAL].size = rec_size+sizeof(int);
				AvailabilityList[numAL].offset = rec_offset;
				numAL++;

				numIndex--;
				for(i=pos;i<numIndex;i++)
					PriKeyIndex[i]=PriKeyIndex[i+1];
			}
			qsort(AvailabilityList,numAL,sizeof(availability_S),cmpALinc);		
		}
		else if(strcmp(cmd,"end")==0)break;
	}
	printIndex();
	printAL();
	printf( "Number of holes: %d\n", numAL );
	for(i=0;i<numAL;i++)
		hole_size+=AvailabilityList[i].size;
	printf( "Hole space: %d\n", hole_size );
	writeIndex();
	writeAL();
}

void worstFit(char *filename)
{
	char *buf;
	int rec_size;
	long rec_offset;
	int k,hole_size=0;
	char cmd[200];
	int i,j;
	FILE *fp; //Input File Stream
	if((fp = fopen( filename, "r+b" )) == NULL ) 
	{ 
		fp = fopen( filename, "w+b" );
	}
	else
	{
		loadIndex();
		loadAvailabilityList();
	}
	while(5)
	{
		j=0;
		char key[20];
		gets(cmd);
		//printf("%s\n",cmd);
		if(strstr(cmd,"add"))
		{
			for(i=4;cmd[i]!=' ';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			i++;
			key[j]='\0';
			k = atoi(key);
			rec_size = strlen(cmd)-5-j;
			buf=(char*)malloc(sizeof(char)*(rec_size+1));
			j=0;
			while(i<strlen(cmd))
			{
				buf[j] = cmd[i];
				j++;i++;
			}
			buf[j]='\0';		
			if(binarySearchIndex(k)==-1)
			{
				rec_offset = writeLocation(rec_size+sizeof(rec_size));
				if(rec_offset == 0)
				{
					fseek(fp,0,SEEK_END);
					rec_offset = ftell(fp);
				}
				PriKeyIndex[numIndex].key = k;
				PriKeyIndex[numIndex].offset = rec_offset;
				numIndex++;
				fseek(fp,rec_offset,SEEK_SET);
				fwrite(&rec_size,sizeof(int),1,fp);
				fwrite(buf,sizeof(char),rec_size,fp);
			}
			else
			{
				printf("Record with SID=%d exists\n",k);
			}
			qsort(PriKeyIndex,numIndex,sizeof(index_S),cmpIndex);
			qsort(AvailabilityList,numAL,sizeof(availability_S),cmpALdec);			
		}	
		else if(strstr(cmd,"find"))
		{
			int pos;
			for(i=5;cmd[i]!='\0';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			key[j]='\0';
			k = atoi(key);
			pos = binarySearchIndex(k);
			if(pos==-1)
				printf("No record with SID=%d exists\n",k);
			else
			{
				rec_offset = PriKeyIndex[pos].offset; 
				fseek( fp, rec_offset, SEEK_SET ); 
				fread( &rec_size, sizeof( int ), 1, fp ); 

				buf = (char*)malloc( sizeof(char)*(rec_size + 1)); 
				fread( buf, sizeof(char), rec_size, fp ); 
				buf[ rec_size ] = '\0';
				printf("%s\n",buf);
			}
		}
		else if(strstr(cmd,"del"))
		{
			int pos;
			for(i=4;cmd[i]!='\0';i++)
			{
				key[j] = cmd[i];
				j++;
			}
			key[j]='\0';
			k = atoi(key);
			pos = binarySearchIndex(k);
			if(pos==-1)
				printf("No record with SID=%d exists\n",k);
			else
			{
				rec_offset = PriKeyIndex[pos].offset; 
				fseek( fp, rec_offset, SEEK_SET ); 
				fread( &rec_size, sizeof( int ), 1, fp );

				AvailabilityList[numAL].size = rec_size+sizeof(int);
				AvailabilityList[numAL].offset = rec_offset;
				numAL++;

				numIndex--;
				for(i=pos;i<numIndex;i++)
					PriKeyIndex[i]=PriKeyIndex[i+1];
			}
			qsort(AvailabilityList,numAL,sizeof(availability_S),cmpALdec);		
		}
		else if(strcmp(cmd,"end")==0)break;
	}
	printIndex();
	printAL();
	printf( "Number of holes: %d\n", numAL );
	for(i=0;i<numAL;i++)
		hole_size+=AvailabilityList[i].size;
	printf( "Hole space: %d\n", hole_size );
	writeIndex();
	writeAL();
}

int main(int argc, char **argv)
{	
	if(strcmp(argv[1],"--first-fit")==0)
	{
		//printf("First fit\n");
		firstFit(argv[2]);		
	}
	else if(strcmp(argv[1],"--best-fit")==0)
	{
		//printf("Best fit\n");
		bestFit(argv[2]);
	}
	else if(strcmp(argv[1],"--worst-fit")==0)
	{
		//printf("Worst fit\n");
		worstFit(argv[2]);
	}
	return 0;
}
