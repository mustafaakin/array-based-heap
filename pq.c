#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct Node Node;
struct Node {
	char* word;
	int amount;
  	Node *left;
  	Node *right;
};

Node *root;
int currentSize = 0;

void rebuild(Node* head);

// Swaps the two nodes values, without touching the children pointers.
void swap(Node* n1, Node* n2){
	char* tmp1 = n1->word;
	n1->word = n2->word;
	n2->word = tmp1;
		
	int tmp2 = n1->amount;
	n1->amount = n2->amount;
	n2->amount = tmp2;
}

// Gets the n'th bit of the number: 1011 >> (3-1) = 10 & 1 = 0
int nthbit(int no, int n) { 
	return (no >> (n-1)) & 1;
} 

// Searchs the tree if the tree contains the word element, if finds it; 
// increments its counter and returns 1 so that the calle can understand 
// and do not produce a heap insert operation. 
int exists(Node* head, char* word){
	if ( head == NULL){
		return 0;
	} else {
		int comp = strcmp(head->word, word);
		if ( comp == 0){
			head->amount++;
			return 1;
		} else if ( comp > 0){
			// No need to go deeper, because anything below this is larger than what we seek.
			return 0; 
		} else {
		 	// Doesn't matter which one has; additonally helps short-circuit evaluation, if one of them is 1
		 	// no need to search the rest of the files.
			return exists(head->left, word) | exists(head->right, word);
		}
	}
}

// Rebuild the heap, place the smallest value of head-left-right to top, rebuild if neccessary.
void rebuild(Node* head){
	if ( head == NULL){
		return;
	}
	if ( head->left != NULL && strcmp(head->word, head->left->word) > 0){
		if ( head->right != NULL && strcmp(head->right->word, head->left->word) < 0){
			swap(head, head->right);
			rebuild(head->right);
		} else {
			swap(head, head->left);
			rebuild(head->left);
		}		
	} else if ( head->right != NULL && strcmp(head->word, root->right->word) > 0){
		if ( head->left != NULL && strcmp(head->left->word, head->right->word) < 0){
			swap(head, head->left);
			rebuild(head->left);
		} else {
			swap(head, head->right);
			rebuild(head->right);
		}		
	}
}

// Retrieves the last node. Level is given for eliminating unneccesary calculations. 
Node* retrieve(Node* head, int level){
	int bit = nthbit(currentSize, level);	
	if ( level == 1){ // We have reached the root.
		Node *lastOne = NULL;
		if ( bit == 0){
			lastOne = head->left;
			head->left = NULL;
		} else {
			lastOne = head->right;
			head->right = NULL;
		}		
		return lastOne;
	} else { // We need to go deeper!
		Node *path = bit == 0 ? head->left : head->right;		
		return retrieve(path, level-1); 
	}
}

/* 	The algorithm to find the the right place is simple:
 	Consider 1 and its children 2 3 in binary.
	1 :  1
	2 : 10
	3 : 11
	
	What is different? The bit scheme tells us where to go from 1 to 2 or 3. If we shift the 1
	for one times to the left, we see that if we want 2, its in the left, if we want 3, its in
	the right. It applies for all nodes.
	
	Consider 10001 = 17
	We do not count the highest bit because we start from 1 to build the heap, not 0.
	There fore 0-0-0-1 tells us the exact pattern. Left-Left-Left-Right
	
	So for the function, we provide a level variable which is actually the (logarithm of currentSize) - 1
	to find the closest n such that 2^n <= number.
	
	In each function call, we decrement it 1. And get the that bit. So we know if we should go to left
	or right. Then, when level is 0, we have reached the parent of the place that we need to insert
	the new node.  Again, depending on the last bit, we choose left or right. 
	
	Then, when function is returning; the swap functions are called. The beauty of the recursion is here:
	We keep calling insert and after we do not call insert no more; we are at the desired root element to
	swap the elements if needed. So, both insert & heap rebuilding upwards are complete in one function, with
	succesive recursive calls.
*/ 

void insert(Node* head, Node* desired, int level){
	int bit = nthbit(currentSize + 1, level); // We will place the *desired* to currentSize + 1
	if ( level == 1){ // We have reached the root.
		if ( bit == 0){
			head->left = desired;
		} else {
			head->right = desired;
		}
		currentSize++;
	} else { // We need to go deeper!
		Node *path = bit == 0 ? head->left : head->right;		
		insert(path, desired, level - 1); 
	}
	Node *path = bit == 0 ? head->left : head->right;		
	if ( strcmp(head->word, path->word) > 0){
		swap(head, path);
	}
}

int main (int argc, char *argv[])
{	
	if ( argc < 3 || argc > 3){
		printf("Wrong parameters!\n");
		return -2;
	}
	FILE *read = fopen(argv[1], "r");
	FILE *write = fopen(argv[2], "w");

	int problem = 1;
	if ( read == NULL){
		printf("Cannot open the file to read: %s \n", argv[1]);
	} else if ( read == NULL){
		printf("Cannot open the file to write: %s \n", argv[2]);		
	} else {
		problem = 0;
	}
	
	if ( problem){
		return;
	}
	
	char buffer[256]; // A word can be 255 chars + termninator char = 256
	while(1){
		if ( fscanf(read, "%s", buffer) == EOF){
			break;
		}
		
		// Find if exists, increment the counter.		
		int k = exists(root, buffer);
		if ( k == 1){
			continue; // No need for new memory
		}			
		// If does not exist, place a new node in proper place.
		Node* entry = (Node*) malloc(sizeof(Node));
		// Copying buffer with exact size needed for the word, no more, no less.
		entry->word = (char*) malloc(sizeof(char)*(strlen(buffer) + 1)); 
		strcpy(entry->word, buffer);
		entry->left = NULL;
		entry->right = NULL;
		entry->amount = 1;
		
		if ( currentSize == 0){ // Initial case
			currentSize++;
			root = entry;
		} else {
			int level = 0;
			insert(root, entry,(int)log2(currentSize+1));		
		}
	}
	// reading is now complete!
	while (1){
		while ( root->amount > 0){
			root->amount--;
			fprintf(write, "%s\n", root->word);
		}
		if ( root->left == NULL && root->right == NULL){
			break;
		}		
		Node *newRoot = retrieve(root,(int)log2(currentSize)); // Shows the last element
		currentSize--;
		// Move the last elements values to root
		root->word = newRoot->word;
		root->amount = newRoot->amount;
		free(newRoot); // Give the memory back
		rebuild(root); // Rebuild the tree after each deletion
	}	
	return 0;
}
