#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Structure of the node for Huffman Tree 
typedef struct Node {
  char character;
  int EOFflag;
  struct Node *left;
  struct Node *right;
}Node;

struct Node* CreateNode(int);
struct Node* ReadHeader(FILE *, unsigned short);
void unhuff(FILE *, FILE *, struct Node *, unsigned short);
void FreeTree(struct Node *);


//Create a new node of the huffman tree
struct Node* CreateNode(int character){
  struct Node *NewNode;
  
  NewNode=malloc(sizeof(struct Node));
  NewNode->character = (char)character;
  NewNode->EOFflag = 0; 
	NewNode->left = NULL;
  NewNode->right = NULL;
  return NewNode;
}

//Read header from file
struct Node* ReadHeader(FILE *FptrIn, unsigned short value){	
	struct Node *left= NULL, *right = NULL, *root = NULL;
	char c;

		fread(&c, 1, 1, FptrIn);//read a byte from the file
		if(c=='0'){//If it is 0, then the following byte is not a character of a leaf node
			left = ReadHeader(FptrIn, value);
			right = ReadHeader(FptrIn, value);
			root = CreateNode(0);
			root->left = left;
			root->right = right;
			return root;
		}
		else if (c=='1'){//If it is 1, then the following byte is a character of a leaf node
			fread(&c, 1, 1, FptrIn);
			return CreateNode(c);
		}
		else if((int)c==value){//If read the byte that has the value of EOF
			return NULL;
		}

}

//Decompress the data of the file 
void unhuff(FILE *FptrIn, FILE *FptrOut, struct Node* tree, unsigned short value){
	int i, n, flag=0, binary[8], read[8], rest[8], count=0;
  unsigned char c;
	struct Node *current= tree;
	
	fread(&c, 1, 1, FptrIn);
	//Decode the data
	while(flag==0){
		fread(&c, 1, 1, FptrIn);
		//Convert the value of c into bit sequence
		n = c;
		i=0;
		while (n > 0) { 
    	read[i]=n%2; 
    	n = n/2; 
    	i++; 
  	}
  	//Correct the order of the bit sequence
  	for(i=0; i<8; i++)binary[i]=read[7-i];
		for(i = 0;i < 8; i++){
			if(binary[i] == 1 )	{//If the bit is 1, go to right
				current = current->right;
				if( current->left == NULL && current->right == NULL){
					c = current->character;
					if (current->EOFflag==1 || current->character=='$'){//If the value of the bit sequence equals to EOF
						flag=1; 
						break;
					}
					else{
						fwrite(&c, 1, 1, FptrOut);
						current = tree;
						count=0;
					}
				}
			}
			else {//If the bit is 0, go to left
				current = current->left;
				if( current->left == NULL && current->right == NULL){
					c = current->character;
					if (current->EOFflag==1 || current->character=='$'){ 
						flag=1;
						break;
					}
					else{
						fwrite(&c, 1, 1, FptrOut);
						current = tree;
						count=0;
					}
				}
			}	
		}
	}
}

//Free huffman tree
void FreeTree(struct Node *tree){
  if(tree != NULL){
    if(tree->left != NULL) FreeTree(tree->left);
    free(tree);
    if(tree->right != NULL) FreeTree(tree->right);
	}
}
