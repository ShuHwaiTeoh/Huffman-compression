#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//struct for the node of linked list 
typedef struct Node {
  char character;
  int freq;
  int EOFflag;
  struct Node *next;
  struct Node *left;
  struct Node *right;
}Node;

//Functions declared
struct Node* CreateNode(int , int);
struct Node* CreateHuff(struct Node*, int);
void encode(struct Node*, char*, char**, struct Node*, int*);
void CreateHeader(struct Node*, FILE*);
void WriteData(FILE *, FILE *, char*, char**);
void FreeTree(struct Node *tree);

//Creates a new node of the linked list
struct Node* CreateNode(int character, int frequency){
  struct Node *NewNode;
  
  NewNode=malloc(sizeof(struct Node));
  if(character!=256){//If the character is not EOF 
  	NewNode->character = (char)character;
  	NewNode->EOFflag=0;
	}
	else{//If the character is EOF
		NewNode->character = '$';
		NewNode->EOFflag=1;
	}
  NewNode->freq = frequency;
  NewNode->next=NULL;
  NewNode->left = NULL;
  NewNode->right = NULL;
  return NewNode;
}

//Functions to create Huffman tree
struct Node* CreateHuff(struct Node* head, int counter){
	struct Node *NewNode;
	struct Node *prev= NULL, *temp = NULL, *pres = NULL;
	
  while(counter > 1){
    NewNode = CreateNode(0,0);//Create a new node as a root node for the subtree.
    //Combine the first two nodes in the linked list to form a subtree.
    NewNode->freq = head->freq + head->next->freq;//The frequency of the root is the sum of its children's frequency
    NewNode->left = head;
    NewNode->right = head->next;
    //Delete the first two nodes in the linked list that was used as the children to form the subtree
    if(counter<=2)head = NULL;
		else head = head->next->next;
    //Insert the new node to the linked list according to its frequency
    if(head!=NULL){
    	if(head->freq < NewNode->freq){ //New node cannot be the head
		  	pres= head;//Store the head node of the list
				while(head != NULL  &&  head->freq<=NewNode->freq){
					//Find the right position in the priority queue to insert the new node
				  prev = head;
				  head = head->next;
				}
				if(head != NULL)	{//The position found is not at the end of the linked list
				  temp = prev->next;//Insert the new node
				  prev->next = NewNode;
				  NewNode->next = temp;
				}
			  else{//Insert the new node at the end of the linked list
			  	prev->next = NewNode;
				}
				head = pres; //reassigning head of the list
			}
			else{//New node is the new head
				temp = head;
			  head = NewNode;
			  head->next = temp;
			}
		}
		else head = NewNode;//There is no other node in the linked list
		counter = counter - 1; //Decrease the number of nodes by 1 each time.
	}
  return head;
}

//Function to traverse tree to find bit sequence for each character and build the codebook
void encode(struct Node *tree, char *code, char **CodeBook, struct Node *root, int* CodeIndex){
  int i;
  
  if(tree->left == NULL  &&  tree->right == NULL){//a leaf node, copy the code to codebook
    if(tree->EOFflag!=1){//character is not EOF
  		for(i=0; i<*CodeIndex; i++){
    	CodeBook[(int)tree->character][i]=code[i];
			}
		}
    else{//charracter is EOF
    	for(i=0; i<*CodeIndex; i++){
    		CodeBook[256][i]=code[i];
			}
		}
  }
  else{//not a leaf node
  	if(tree==root) *CodeIndex=0;
    code[*CodeIndex]='0';//When go to the left direction encode as 0
    *CodeIndex=*CodeIndex+1;
    encode(tree->left,code,CodeBook, root, CodeIndex); 
    *CodeIndex=*CodeIndex-1;
    if(tree==root) *CodeIndex=0;//Back to the root node of the whole tree
    code[*CodeIndex]='1';//When go to the right direction encode as 1
    *CodeIndex=*CodeIndex+1;
    encode(tree->right,code,CodeBook, root, CodeIndex);
    *CodeIndex=*CodeIndex-1;
  }
  return ;
}

//Wrute the header (huffman tree) to the output file
void CreateHeader(struct Node *tree, FILE *FptrOut){
	char c;
	//Use post-order to traverse the huffman tree
	if (tree->left != NULL || tree->right != NULL){
		c='0';
		fwrite(&c, 1, 1, FptrOut);
		CreateHeader(tree->left, FptrOut);
		CreateHeader(tree->right, FptrOut);
	}
	else{
		c='1';
		fwrite(&c, 1, 1, FptrOut);
		c = tree->character;
		fwrite(&c, 1, 1, FptrOut);
	}
}  

//Read characters from the input file, 
//write them in their huffman code to the compressed file
void WriteData(FILE *FptrIn, FILE *FptrOut, char *code, char **CodeBook){
  int value=0, index=0, BitCount=0;
  char ic, c;
	
  while(1==1){
  	//Read a character from the input file
  	c=fgetc(FptrIn);
  	ic=c;
  	if(c==EOF) break;
  	//Use its ascii valuse as its position in the codebook to find and copy its huffman code
  	index=0;
  	while(CodeBook[(int)ic][index]!='\0'){
  		code[BitCount]=CodeBook[(int)ic][index];
  		BitCount++;
			index++;
  		if(BitCount==8) {//When the bit sequence has 8 bits
  			for(BitCount=0; BitCount<8; BitCount++){//Claculate the bit sequence as a decimal value
					if(code[BitCount]=='1') value=value+pow(2, 7-BitCount);
				}
				c=value;
  			fwrite(&c, 1, 1, FptrOut);//write into the output file
  			value=0;
  			BitCount=0;
			}
		}
	}
	//write EOF at the end of the bit sequence
	index=0;
	while(CodeBook[256][index]!='\0'){
		code[BitCount]=CodeBook[256][index];
		BitCount++;
		index++;
		if(BitCount==8) {//When the bit sequence has 8 bits, write it to the output file
			for(BitCount=0; BitCount<8; BitCount++){
				if(code[BitCount]=='1') value=value+pow(2, 7-BitCount);
			}
			c=value;
			fwrite(&c, 1, 1, FptrOut);
			value=0;
			BitCount=0;
		}
	}
	//If there are extra bits, then it needs padding
	if(BitCount!=0){
		while(BitCount<8){
			code[BitCount]='0';//Padding with 0
			BitCount++;
		}
		BitCount=7;
		for(index=0; index<8; index++){
			if(code[index]=='1') value=value+pow(2, BitCount);
			BitCount--;		
		}
		c=value;
  	fwrite(&c, 1, 1, FptrOut);
  	value=0;
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
