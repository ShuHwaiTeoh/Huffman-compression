#include <sys/types.h>
#include <sys/time.h> 
//#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "unhuff.h"
  
#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif
 
int main(int argc, char**argv){
	char filename[50];
	FILE *FptrIn, *FptrOut;//Pointer for input file and output file opening
	struct Node *tree= NULL, *current=NULL;
	double cstart, cend;
	unsigned short value;//The value of EOF
	int SEOF[16];
	int i=0, n=0, j=0;
	 
	for(i=0; i<16; i++) SEOF[i]=2;
	cstart = (double) clock();
	strcpy(filename, argv[1]);//Copy the file name of the nput file
	FptrIn = fopen(filename,"rb");
  strcat(filename,".unhuff"); //Let output filename as example.txt.huff.unhuff 
  FptrOut = fopen(filename,"wb");
	
	//Read the first 1 bytes to know the value of EOF 
  fread(&value, 1, 1, FptrIn);
  //Convert the value to know the code of EOF
  i=0;
  n=value;
  while (n > 0) { 
    SEOF[i]=n%2; 
    n = n/2; 
    i++; 
  }
  i=0;
	j=0;
	while(SEOF[i]!=2) {
		j++;
		i++;
	}
     
  //Reaf the header (huffman tree
  tree=ReadHeader(FptrIn, value);
  //Set the EOFflag
  current=tree;
  for(i=(j-1); i>=0; i--){
  	if(SEOF[i]==1 && (current->right)!=NULL) current=current->right;
  	else if(SEOF[i]==0 && (current->left) !=NULL) current=current->left;
 // 	printf("p=%d code=%d", i, SEOF[i]);
	}
	current->EOFflag=1;
 
	//Decompress the file 
  unhuff(FptrIn, FptrOut, tree, value);
  
	fclose(FptrIn);
  fclose(FptrOut);

  //Free the Huffman tree
  FreeTree(tree);
	cend = (double) clock();
	printf(" Elapsed Time (sec): %f\n", (cend - cstart)/CLOCKS_PER_SEC);
	return 0;
}

