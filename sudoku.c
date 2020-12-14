//Willis A. Hershey
//WillisAHershey@gmail.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*	This program models the numbers 1-9 that could occupy some cell of the Sudoku puzzle as the first through ninth bits of a short int
 *	Modeling them this ways allows these numbers to be bitwise-ored together to produce values representing multiple numbers at once
 */

#define ONE	0x1
#define TWO	(0x1<<1)
#define THREE	(0x1<<2)
#define FOUR	(0x1<<3)
#define FIVE	(0x1<<4)
#define SIX	(0x1<<5)
#define SEVEN	(0x1<<6)
#define EIGHT	(0x1<<7)
#define NINE	(0x1<<8)

//This tells the calling function if the short in question is set to a single value
static inline int set(short in){
  if(in==ONE||in==TWO||in==THREE||in==FOUR||in==FIVE||in==SIX||in==SEVEN||in==EIGHT||in==NINE)
	return 1;
  else
	return 0;
}

//Converts from representation of number to integer
static inline int translate(short in){
	if(in==ONE)
		return 1;
	else if(in==TWO)
		return 2;
	else if(in==THREE)
		return 3;
	else if(in==FOUR)
		return 4;
	else if(in==FIVE)
		return 5;
	else if(in==SIX)
		return 6;
	else if(in==SEVEN)
		return 7;
	else if(in==EIGHT)
		return 8;
	else if(in==NINE)
		return 9;
	else
		return 0;
}

//This function attempts to set cells to their numbers by checking if there exists a number this cell could hold that no other
//cell in its row, column ,or block could hold. If this function succeeds, it retuns 1, otherwise, 0.
int analyzePossibilities(short board[],int index){
  short val=board[index];
  int found=0;
  for(short bit=ONE;bit<=NINE;bit<<=1){
	if(!(val&bit))
		continue;
	for(int c=0,run=index-index%9;c<9;++c,++run){
		if(run==index||set(board[run]))
			continue;
		if(board[run]&bit){
			found=1;
			break;
		}
	}
	if(!found){
		board[index]=bit;
		return 1;
	}
	else
		found=0;
	for(int c=0,run=index%9;c<9;++c,run+=9){
		if(run==index||set(board[run]))
			continue;
		if(board[run]&bit){
			found=1;
			break;
		}
	}
	if(!found){
		board[index]=bit;
		return 1;
	}
	else
		found=0;
	for(int c=0,run=index-index%27+index%9-(index%9)%3;c<9;++c,run+=run%3==2?7:1){
		if(run==index||set(board[run]))
			continue;
		if(board[run]&bit){
			found=1;
			break;
		}
	}
	if(!found){
		board[index]=bit;
		return 1;
	}
  }
  return 0;
}

//This function sets the bits of a short to represent all of the numbers not currently present in some cell's row, column, or block
//Returns 1 if that cell can only hold one value, so that cell is now set
//Returns -1 if that cell in ineligable to hold any number, so we've encountered a contradiction, and need to revert with the logstack
//And returns 0 otherwise, to indicate that the cell's bits are properly set
int setPossibilities(short board[],int index){
  short val=ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE;
  for(int c=0,run=index-index%9;c<9;++c,++run){
	if(run==index)
		continue;
	if(set(board[run]))
		val&=~board[run];
  }
  for(int c=0,run=index%9;c<9;++c,run+=9){
	if(run==index)
		continue;
	if(set(board[run]))
		val&=~board[run];
  }
  for(int c=0,run=index-index%27+index%9-(index%9)%3;c<9;++c,run+=run%3==2?7:1){
	if(run==index)
		continue;
	if(set(board[run]))
		val&=~board[run];
  }
  board[index]=val;
  if(set(val))
	return 1;
  else if(!val)
	return -1;
  else
	return 0;
}

//Prints the current puzzle board, printing unset cells as zeroes
void printBoard(short board[]){
  printf("%d",translate(board[0]));
  for(int c=1;c<81;++c){
	if(!(c%9))
		printf("\n%d",translate(board[c]));
	else
		printf(",%d",translate(board[c]));
  }
  printf("\n\n");
}

//Verifies that each row column and block of the puzzle contains no more than one of each number
int verify(short board[]){
  for(int c=0;c<9;++c){
	char count[9]={0,0,0,0,0,0,0,0,0};
	for(int d=0;d<9;++d)
		if(set(board[c*9+d]))
			++count[translate(board[c*9+d])-1];
	for(int d=0;d<9;++d)
		if(count[d]>1)
			return 0;
  }
  for(int c=0;c<9;++c){
	char count[9]={0,0,0,0,0,0,0,0,0};
	for(int d=0;d<9;++d)
		if(set(board[c+d*9]))
			++count[translate(board[c+d*9])-1];
	for(int d=0;d<9;++d)
		if(count[d]>1)
			return 0;
  }
  for(int c=0;c<9;++c){
	char count[9]={0,0,0,0,0,0,0,0,0};
	for(int d=0;d<9;++d)
		if(set(board[(c/3)*27+(c%3)*3+(d/3)*9+(d%3)]))
			++count[translate(board[(c/3)*27+(c%3)*3+(d/3)*9+(d%3)])-1];
	for(int d=0;d<9;++d)
		if(count[c]>1)
			return 0;
  }
  return 1;
}

//This data structure allows the puzzle board to be reverted when a contradiction is encountered from a bad guess
typedef struct logStruct{
  int index;
  short val;
  short old[81];
  struct logStruct *next;
}log;

//This function is called when a contradiction is encountered. It works by attempting to change the guessed cell to the next number that it can hold
//If all numbers cause a contradiction in that cell, then that logfile is abandoned and an older one is used
//This function will call exit with an error status if it runs out of logfiles before it produces a new puzzle to be processed
//Returns the current value of the loghead
log* revert(short board[],log *head){
  while(head){
	memcpy(board,head->old,81*sizeof(short));
	for(short bit=head->val<<1;bit<=NINE;bit<<=1)
		if(board[head->index]&bit){
			board[head->index]=head->val=bit;
			return head;
		}
	log *hold=head->next;
	free(head);
	head=hold;
  }
  fprintf(stderr,"This puzzle has no solutions\n");
  exit(EXIT_FAILURE);
}

//This function sets the bits of every unset cell in the board, making sure to start over if some cell is set by happenstance
//It also reverts the board and starts over if it encounters a contradiction
//Returns the current value of the loghead
log* setAllPossibilities(short board[],log *head){
  for(int c=0;c<81;++c)
	if(!set(board[c])){
		int ret=setPossibilities(board,c);
		if(ret){
			c=-1;
			if(ret==-1)
				head=revert(board,head);
		}
	}
  return head;
}

//This loop sets the bits of every cell, then checks them against each other in an attempt to set them
//When these functions have done all they can do, it checks to see if there are any unset cells
//If there are, it produces a logfile for the top of the logstack, picks a number for the unset cell, and starts over
//Once all cells are set it frees the logstack and returns
void solve(short board[]){
  int solved=0;
  log *head=NULL;
  while(!solved){
	head=setAllPossibilities(board,head);
	for(int c=0;c<81;++c){
		if(!set(board[c]))
			if(analyzePossibilities(board,c)){
				head=setAllPossibilities(board,head);
				c=-1;
			}
	}
	solved=1;
	for(int c=0;c<81;++c)
		if(!set(board[c])){
			solved=0;
			for(short bit=ONE;bit<=NINE;bit<<=1)
				if(board[c]&bit){
					log *hold=malloc(sizeof(log));
					*hold=(log){.index=c,.val=bit,.next=head};
					memcpy(hold->old,board,81*sizeof(short));
					board[c]=bit;
					head=hold;
					bit=(NINE)<<1;
					c=81;
				}
		}
  }
  while(head){
	log *hold=head->next;
	free(head);
	head=hold;
  }
}

//Main reads the user input, produces its representation of the puzzle, checks to see if it's valid, prints the input, solves the puzzle
//verifies the solution, prints the output, and terminates
int main(int args,char *argv[]){
  if(args!=82){
	fprintf(stderr,"USAGE: %s {1,-,2,9,-,-...}\n",argv[0]);
	exit(EXIT_FAILURE);
  }
  short board[81];
  for(int c=0;c<81;++c)
	switch(argv[c+1][0]-'0'){
		case 1:
			board[c]=ONE;
			break;
		case 2:
			board[c]=TWO;
			break;
		case 3:
			board[c]=THREE;
			break;
		case 4:
			board[c]=FOUR;
			break;
		case 5:
			board[c]=FIVE;
			break;
		case 6:
			board[c]=SIX;
			break;
		case 7:
			board[c]=SEVEN;
			break;
		case 8:
			board[c]=EIGHT;
			break;
		case 9:
			board[c]=NINE;
			break;
		default:
			board[c]=0;
			break;
	}
  if(!verify(board)){
	fprintf(stderr,"Puzzle has a contradiction and cannot be solved\n");
	exit(EXIT_FAILURE);
  }
  printf("Puzzle is verified and accepted\n\nINPUT:\n");
  printBoard(board);
  solve(board);
  if(!verify(board)){
	fprintf(stderr,"Something went wrong :/\n");
	exit(EXIT_FAILURE);
  }
  printf("SOLUTION:\n");
  printBoard(board);
}
