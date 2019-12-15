#include <sys/types.h>
#include <sys/time.h>
//#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "huff.h"

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif

int main(int argc, char **argv){ 
  int HEADflag=1; //1: the new node created is the head node. 
							//0: the new node created is other node in the linkes list.
	int ZEROflag=0;//1: All element in character-frequency table are 0
	char c;//Character read from the input file.
  int counter=0, i=0, p=0, j=0, min=0;//Counter for the number of nodes, position in table, and loopings.
  char *code, **CodeBook;//Store the bit sequence for each character.
  char filename[50] = "";//To save input filename
  FILE *FptrIn, *FptrOut;//Pointer for input file and output file opening
  int table[257]={0};//Arrays for character-frequency table 
  struct Node *head=NULL, *PreNode=NULL, *NewNode=NULL; //Nodes for linked list
	struct Node *tree=NULL;//Node for Huffman tree.
	double cstart, cend;
	unsigned short value;//the value of eof
	int CodeIndex=0;//the index of code array
 

	
	code = malloc(257*sizeof(char));
	//Assign 257*257 memory sapce for codebook
	CodeBook = malloc(257*sizeof(char*));
	for(i=0;i<257;i++) CodeBook[i] = malloc(257*sizeof(char*));
	strcpy(filename, argv[1]);//Copy the file name of the nput file
	cstart = (double) clock();
  //Read input file and build the table of characters and their frequencies.
	FptrIn = fopen(filename, "r");
  while(1==1) {
  	c=fgetc(FptrIn);
  	if(c==EOF) break;
  	else table[(int)c]++;
	}
	table[256]=1;
  fclose(FptrIn); 
	//Select EOF as the element that has frequency 1 as the min 
	min=table[256];
	p=256;
  //According to the character-frequency table, build a linked list.
  while(ZEROflag==0) { 
		//Find the character that has minimum frequency
  	for(j=0;j<257;j++){
			if(table[j] < min && table[j] != 0)	{
	  		min = table[j];
	  		p = j;
			}
  	}
  	//Create a new node for the character
		NewNode = CreateNode(p, table[p]);
		table[p]=0; //Replace the minimun with 0 to remove it from the table.
  	if(HEADflag==0){//Insert the node into the linked list
      PreNode->next=NewNode;
      PreNode=PreNode->next;
    }
  	else{
      head=NewNode;//Save the new node as the head node.
      PreNode=head;//Assign the head node to be the previous node to link other node.
      HEADflag=0;
    }
		counter++; //Count the number of nodes in the priority queue.
	  ZEROflag=1;
	  //Chech whether all the elements in the character frequency table are zeros
	  for(i=0; i<257; i++){
	  	if(table[i]!=0){
	  		ZEROflag=0;
	  		min=table[i];
	  		p=i;
			}
		}
  }
  
  tree=CreateHuff(head, counter);//Use the linked list to build a Huffman tree.
  //Initialize the CodeBook.
	for(i=0; i<257; i++) {
		code[i]='\0';
		for(p=0; p<257; p++) CodeBook[i][p]='\0';
	}
	//Encode each character read from the input file.
	encode(tree, code, CodeBook, tree, &CodeIndex);
	 
  //Write huffman tree header and the binary code to an output file
  FptrIn = fopen(filename,"r");
  strcat(filename,".huff"); //Let output filename as example.txt.huff
  FptrOut = fopen(filename,"wb");
   
	//Calculate the value of EOF
	i=0;
	j=0;
	while(CodeBook[256][i]!='\0') {
		j++;
		i++;
	}
	for(i=0; i<j; i++){
		if(CodeBook[256][i]=='1'){
			value=value+pow(2, j-i-1);
		}
	}

	//Write the value of EOF as first byte in the file
	fwrite(&value, 1, 1, FptrOut);
	
	//Write the header (huffman tree) to the output file
  CreateHeader(tree, FptrOut);
	
	//Write the value of EOF as the end of header
	fwrite(&value, 1, 1, FptrOut);
	
  //Write the compressed data to the outputfile
  WriteData(FptrIn,FptrOut,code,CodeBook);
  
  fclose(FptrIn);
  fclose(FptrOut);
	cend = (double) clock();
	printf(" Elapsed Time (sec): %f\n", (cend - cstart)/CLOCKS_PER_SEC);
  //Free the memory
  free(code);
  for(i=0;i<256;i++) free(CodeBook[i]);
  free(CodeBook);
  FreeTree(tree);
	
  return 0;
}
