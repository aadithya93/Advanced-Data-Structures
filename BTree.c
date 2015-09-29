/*B-Tree implementation*/

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

typedef struct
{
	int n;
	int *key;
	long *child;
}btree_node;

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

long findParent(char* filename, long offset, long root,int order)
{
	btree_node node;
	int i;
	FILE *fp; /* Input file stream */ 
	long parent,found=0;
	node.n = 0; 
	node.key = (int*) calloc(order-1, sizeof(int)); 
	node.child = (long*) calloc(order, sizeof(long));
	fp = fopen( filename, "r+b" );

	
	if(offset == root)
	{
		fclose(fp);
		return 0;
	}
	else
	{
		fseek(fp,root,SEEK_SET);
		parent = ftell(fp);
		fread(&node.n,sizeof(int),1,fp);
		fread(node.key,sizeof(int),order-1,fp);
		fread(node.child,sizeof(long),order,fp);
		
		for(i=0;i<node.n;i++)
		{
			if(node.child[i] == offset)
				return parent;
		}
		if(node.child[i] == offset)
			return parent;
		
		for(i=0;i<node.n+1;i++)
		{
			//fclose(fp);
			if(node.child[i]!=-1)
			{
				found = findParent(filename,offset,node.child[i],order);
				if(found !=0) 
				{
					parent = found;
					break;
				}
			}
		}
		if(found == 0)
			return 0;
		else	
			return parent;
	}
}
					
int btreeFind(char* filename, int key, int order, int negativeOffset)
{
	btree_node node;
	int i;
	FILE *fp; /* Input file stream */ 
	long root;	/* Offset of B-tree root node */ 
	long current;
	node.n = 0; 
	node.key = (int*) calloc(order-1, sizeof(int)); 
	node.child = (long*) calloc(order, sizeof(long));
	fp = fopen( filename, "r+b" ); /* If file doesn't exist, set root offset 
										to unknown and create * file, 
										otherwise read the root offset at the 
										front of the file */ 
	if ( fp == NULL ) 
	{ 
		root = -1; 
		fp = fopen( filename, "w+b" ); 
		fwrite( &root, sizeof( long ), 1, fp ); 
	} 
	else
		fread( &root, sizeof( long ), 1, fp );
	if(root == -1) 
	{
		fclose(fp);
		return 0;
	}
	else
	{
		fseek(fp,root,SEEK_SET);
		while(5)
		{
			current = ftell(fp);
			fread(&node.n,sizeof(int),1,fp);
			fread(node.key,sizeof(int),order-1,fp);
			fread(node.child,sizeof(long),order,fp);
			
			for(i=0;i<node.n;i++)
			{
				if(key == node.key[i])
				{
					fclose(fp);
					return 1;
				}
				else if(key<node.key[i])
				{
					if(node.child[i]==-1)
					{
						fclose(fp);
						if(negativeOffset == 0)
							return 0;
						else if(negativeOffset == 1)
							return current;
					}
					else
					{
						fseek(fp,node.child[i],SEEK_SET);
					}
					break;
				}
			}
			if(i < node.n) continue;
			else
			{
				if(node.child[i] == -1)
				{
					fclose(fp);
					if(negativeOffset == 0)
						return 0;
					else if(negativeOffset == 1)
						return current;
				}
				else 
				{
					fseek(fp,node.child[i],SEEK_SET);
				}
			}
		}
	}
}

int findHeight(char* filename, int order)
{
	btree_node node;
	int i,height=0;
	long root;
	FILE *fp;
	
	fp = fopen( filename, "r+b" );
	
	node.n = 0; 
	node.key = (int*) calloc(order-1, sizeof(int)); 
	node.child = (long*) calloc(order, sizeof(long));
	
	fread( &root,sizeof(long),1,fp);
	if(root != -1)
	{
		fseek(fp,root,SEEK_SET);
		fread(&node.n,sizeof(int),1,fp);
		fread(node.key,sizeof(int),order-1,fp);
		fread(node.child,sizeof(long),order,fp);
		height++;
		while(node.child[0]!=-1)
		{
			fseek(fp,node.child[0],SEEK_SET);
			fread(&node.n,sizeof(int),1,fp);
			fread(node.key,sizeof(int),order-1,fp);
			fread(node.child,sizeof(long),order,fp);
			height++;
		}
		
	}
	fclose(fp);
	return height;
}

void displayHeight(char* filename, int order, long offset,int level)
{
	btree_node node;
	int i;
	FILE *fp;
	long root; /* Offset of B-tree root node */ 
	fp = fopen( filename, "r+b" );
	node.n = 0; 
	node.key = (int*) calloc(order-1, sizeof(int)); 
	node.child = (long*) calloc(order, sizeof(long));
	
	if(offset == -1)
	{
		fclose(fp);
		return;
	}
	else
	{
		fseek(fp,offset,SEEK_SET);
		fread(&node.n,sizeof(int),1,fp);
		fread(node.key,sizeof(int),order-1,fp);
		fread(node.child,sizeof(long),order,fp);
		
		if(level == 1)
		{
			for(i=0;i<node.n-1;i++)
				printf("%d,",node.key[i]);
			printf("%d ",node.key[i]);
		}
		else if(level > 1)
		{
			fclose(fp);
			for(i=0;i<node.n;i++)
			{
				displayHeight(filename,order,node.child[i],level-1);
			}
			displayHeight(filename,order,node.child[i],level-1);
		}
	}
}
	
void btreePrint(char* filename, int order)
{
	btree_node node;
	int i,height;
	FILE *fp; /* Input file stream */ 
	long root; /* Offset of B-tree root node */ 
	long offsets[50];
	node.n = 0; 
	node.key = (int*) calloc(order-1, sizeof(int)); 
	node.child = (long*) calloc(order, sizeof(long));
	fp = fopen( filename, "r+b" ); /* If file doesn't exist, set root offset 
										to unknown and create * file, 
										otherwise read the root offset at the 
										front of the file */ 
	if ( fp != NULL )
	{
		fread( &root, sizeof( long ), 1, fp );
		height = findHeight(filename,order);
		for(i=1;i<=height;i++)
		{
			printf(" %d: ",i);
			displayHeight(filename,order,root,i);
			printf("\n");
		}
		fclose(fp);
	}
}

void insert(char* filename, int order, long offset, int key, long lchild,long rchild)
{
	btree_node node;
	int i;
	FILE *fp;
	long root,parent; /* Offset of B-tree root node */ 
	fp = fopen( filename, "r+b" );
	node.n = 0; 
	node.key = (int*) calloc(order-1, sizeof(int)); 
	node.child = (long*) calloc(order, sizeof(long));
	fread( &root, sizeof( long ), 1, fp );
	//seek to the location
	fseek(fp,offset,SEEK_SET);
	fread(&node.n,sizeof(int),1,fp);
	fread(node.key,sizeof(int),order-1,fp);
	fread(node.child,sizeof(long),order,fp);

	if(node.n<order-1)  //node not full. Insert key in node
	{
		for(i=node.n-1;node.key[i]>key && i>=0;i--)
		{
			node.key[i+1]=node.key[i];
			node.child[i+2]=node.child[i+1];
		}
		if(node.n==0) i=-1;
		node.key[i+1] = key;
		node.child[i+1]=lchild;
		node.child[i+2]=rchild;
		node.n++;
		//overwrite the node
		fseek(fp,offset,SEEK_SET);
		fwrite(&node.n,sizeof(int),1,fp);
		fwrite(node.key,sizeof(int),order-1,fp);
		fwrite(node.child,sizeof(long),order,fp);
		fclose(fp);
	}
	else 	//Node full.Split required
	{
		btree_node lnode,rnode;
		int mid;
		int keyArray[order];
		long childArray[order+1],lc,rc;
		lnode.n = 0; 
		lnode.key = (int*) calloc(order-1, sizeof(int)); 
		lnode.child = (long*) calloc(order, sizeof(long));
		rnode.n = 0; 
		rnode.key = (int*) calloc(order-1, sizeof(int)); 
		rnode.child = (long*) calloc(order, sizeof(long));
		
		for(i=0;i<node.n;i++)
		{
			keyArray[i]=node.key[i];
			childArray[i]=node.child[i];
		}
		childArray[i]=node.child[i];
		
		for(i=node.n-1;node.key[i]>key && i>=0;i--)
		{
			keyArray[i+1]=keyArray[i];
			childArray[i+2]=childArray[i+1];
		}
		keyArray[i+1] = key;
		childArray[i+1]=lchild;
		childArray[i+2]=rchild;
		mid = ceil((order-1)/2.0);
		for(i=0;i<mid;i++)
		{
			lnode.key[lnode.n] = keyArray[i];
			lnode.child[lnode.n] = childArray[i];
			lnode.n++;
		}
		lnode.child[lnode.n] = childArray[i];
		for(i=mid+1;i<order;i++)
		{
			rnode.key[rnode.n] = keyArray[i];
			rnode.child[rnode.n] = childArray[i];
			rnode.n++;
		}
		rnode.child[rnode.n] = childArray[i];
		
		fseek(fp,offset,SEEK_SET);
		lc = ftell(fp);
		fwrite(&lnode.n,sizeof(int),1,fp);
		fwrite(lnode.key,sizeof(int),order-1,fp);
		fwrite(lnode.child,sizeof(long),order,fp);
		fseek(fp,0,SEEK_END);
		rc = ftell(fp);
		fwrite(&rnode.n,sizeof(int),1,fp);
		fwrite(rnode.key,sizeof(int),order-1,fp);
		fwrite(rnode.child,sizeof(long),order,fp);
		node.n = 0; 
		node.key = (int*) calloc(order-1, sizeof(int)); 
		node.child = (long*) calloc(order, sizeof(long));
		
		
		if(offset == root) //split and create new root
		{
			offset = ftell(fp);
			fwrite(&node.n,sizeof(int),1,fp);
			fwrite(node.key,sizeof(int),order-1,fp);
			fwrite(node.child,sizeof(long),order,fp);
			fseek(fp,0,SEEK_SET);
			fwrite(&offset,sizeof(long),1,fp);
			fclose(fp);
			insert(filename,order,offset,keyArray[mid],lc,rc);
		}
		else 
		{
			
			offset = findParent(filename,offset,root,order);
			//printf("%ld\n",offset);
			fclose(fp);
			insert(filename,order,offset,keyArray[mid],lc,rc);
		}//split,find parent and promote
	}
}

void btreeAdd(char* filename, int key, int order)
{
	btree_node node;
	int i;
	FILE *fp; /* Input file stream */ 
	long root; /* Offset of B-tree root node */ 
	fp = fopen( filename, "r+b" ); /* If file doesn't exist, set root offset 
										to unknown and create * file, 
										otherwise read the root offset at the 
										front of the file */ 
	if ( fp == NULL ) 
	{ 
		root = -1; 
		fp = fopen( filename, "w+b" ); 
		fwrite( &root, sizeof( long ), 1, fp ); 
	} 
	else
		fread( &root, sizeof( long ), 1, fp );
	//If empty tree
	if(root == -1)
	{
		node.n = 0; 
		node.key = (int*) calloc(order-1, sizeof(int)); 
		node.child = (long*) calloc(order, sizeof(long));
		
		for(i=0;i<order;i++)
			node.child[i] = -1;
		
		node.key[node.n] = key;
		node.n++;
		
		root = ftell(fp);
		fwrite(&node.n,sizeof(int),1,fp);
		fwrite(node.key,sizeof(int),order-1,fp);
		fwrite(node.child,sizeof(long),order,fp);
		
		//Update root
		fseek(fp,0,SEEK_SET);
		fwrite( &root, sizeof( long ), 1, fp ); 
		fclose(fp);
		
	}
	//Existing tree
	else
	{
		int result = btreeFind(filename,key,order,1);
		//Duplicate value
		if(result == 1)
			printf("Entry with key=%d already exists\n",key);
		//Insert node in leaf pointed by result
		else
		{
			fclose(fp);
			insert(filename,order,result,key,-1,-1);
		}	
	}
}
	
void bTreeOperations(char* filename, int order)
{
	char cmd[100];
	while(5)
	{
		gets(cmd);
		//printf("%s\n",cmd);
		if (strstr(cmd,"add"))
		{
			int i,key=0;
			//printf("Adding to Btree --");
			for(i=0;i<strlen(cmd);i++)
			{
				if(isdigit(cmd[i]))key = (key*10) + cmd[i] - '0';
				if(cmd[i-1]=='-')key*=-1;
			}
			//printf("%d\n",key);
			btreeAdd(filename,key,order);
		}
		else if (strstr(cmd,"find"))
		{
			int i,key=0;
			//printf("Finding a node --");
			for(i=0;i<strlen(cmd);i++)
			{
				if(isdigit(cmd[i]))key = (key*10) + cmd[i] - '0';
				if(cmd[i-1]=='-')key*=-1;
			}
			//printf("%d\n",key);
			int result = btreeFind(filename, key, order,0);
			if(result == 0) 
				printf("Entry with key=%d does not exist\n",key);
			else 
			{
				//printf("%ld\n",result);
				printf("Entry with key=%d exists\n",key);
			}
		}
		else if (strcmp(cmd,"print")==0)
		{
			//printf("Printing Btree\n");
			btreePrint(filename,order);
		}
		else if (strcmp(cmd,"end")==0)break;
	}
}

int main(int argc, char** argv)
{
	if(argc==3)
	{
		int order;
		order = atoi(argv[2]);
		bTreeOperations(argv[1],order);
	}
	return 0;
}
