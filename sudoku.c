//Willis A. Hershey
//WillisAHershey@gmail.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*	"Sudoku" is a number puzzle with a 9x9 grid of "cells". Each "cell" must hold one number between 1 and 9 inclusive.
 *
 *	For the sake of a reader understanding the comments in this program, the "board" refers to the playing field of the standard         0   1   2 | 3   4   5 | 6   7   8
 *	Sudoku game with 81 cells. The "rows" are the nine horizontal linear subsections of the "board", the "columns" are the nine          9  10  11 |12  13  14 |15  16  17
 *	vertical linear subsections of the "board", and the "blocks" are the nine 3x3 square subsections of the "board". Each of the        18  19  20 |21  22  23 |24  25  26
 *	"rows", "columns", and "blocks" contains exactly nine cells, and each cell in the board is within exactly one "row", "column"       ----------------------------------
 *	and "block". If I've lost you you should perhaps do some reading on Sudoku puzzles before continuing.                               27  28  29 |30  31  32 |33  34  35
 *                                                                                                                                          36  37  38 |39  40  41 |42  43  44
 *	This program models the board as an array of 81 short ints, with indices from 0-80. Rows begin at indices 0,9,18,27,36,45,54,       45  46  47 |48  49  50 |51  52  53
 *	63, and 72. Columns begin at indices 0,1,2,3,4,5,6,7, and 8. Blocks begin at indices 0,3,6,27,30,33,54,57, and 60.                  ----------------------------------
 *                                                                                                                                          54  55  56 |57  58  59 |60  61  62
 *	In accordace with the rules of Sudoku, each row, column and block of the board must contain each of the numbers from 1-9, and       63  64  65 |66  67  68 |69  70  71
 *	since each of these subsections contains only 9 cells, there can be no duplicates of any number in any row, column, or block.       72  73  74 |75  76  77 |78  79  80
 *
 *	This program takes a list of 81 integers as a representation of a puzzle board, finds a solution, and prints it out.
 */


/*	This program models the numbers 1-9 that could occupy some cell of the Sudoku puzzle as the first through ninth bits of a short int.
 *	Modeling them this ways allows these numbers to be bitwise-ored together to produce values representing multiple numbers at once
 *	The use of short ints potentially saves memory, as short is the smallest int type guaranteed to be at least ten bits wide
 */


//Values defined in bitshift-notation to avoid typos

#define ONE	0x1
#define TWO	(0x1<<1)
#define THREE	(0x1<<2)
#define FOUR	(0x1<<3)
#define FIVE	(0x1<<4)
#define SIX	(0x1<<5)
#define SEVEN	(0x1<<6)
#define EIGHT	(0x1<<7)
#define NINE	(0x1<<8)
//The definition of SET as the tenth bit saves computational time for set() defined below
#define SET	(0x1<<9)

//This tells the calling function if the parameter's short is set to a single value. If it is, but the set bit is unset, it ors it in.
static inline _Bool set(short *i){
  short in=*i;
  if(in&SET)
	return 1;
  _Bool found=0;
  for(short bit=ONE;bit<=NINE;bit<<=1)
	if(in&bit){
		if(found)
			return 0;
		else
			found=1;
	}
  if(found)
	*i|=SET;
  return found;
}

//Converts from bit-representation of number to standard binary integer
static inline int translate(short in){
	in=in&~SET;
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
//cell in its row, column ,or block could hold. If this function succeeds in setting the cell, it retuns 1, otherwise, 0.
_Bool analyzePossibilities(short board[],int index){
  short val=board[index];				//This value is the current value of what I will call the focus cell. It has a 1 in the corresponding position of every value that could legally be stored there
  for(short bit=ONE;bit<=NINE;bit<<=1){			//For each possible value, focus value, from ONE to NINE
	if(!(val&bit))					//If the focus cell doesn't have a 1 in that bit position skip it
		continue;
	_Bool found=0;
	for(int c=0,run=index-index%9;c<9;++c,++run){ 	//For each cell in the focus cell's row
		if(run==index||set(&board[run]))	//Skip over the focus cell and any other cell that is already set
			continue;
		if(board[run]&bit){			//If some other cell has a 1 in focus value's position then we can't set the focus cell's position on account of it's row
			found=1;
			break;
		}
	}
	if(!found){					//If no other cell in focus cell's row can hold some focus value then set focus cell to that value ored with SET and return 1
		board[index]=bit|SET;
		return 1;
	}
	else						//Otherwise try again with focus cell's column
		found=0;
	for(int c=0,run=index%9;c<9;++c,run+=9){
		if(run==index||set(&board[run]))
			continue;
		if(board[run]&bit){
			found=1;
			break;
		}					//... same algorithm as before
	}
	if(!found){
		board[index]=bit|SET;
		return 1;
	}
	else
		found=0;				//Otherwise try again with focus cell's block
	for(int c=0,run=index-index%27+index%9-index%3;c<9;++c,run+=run%3==2?7:1){				//Blocks are hard to iterate through so this loop is super confusing. Let me explain:
		if(run==index||set(&board[run]))								//For any index on the board from 0-80, index-index%27 is 0, 26, or 54, which represents a block
			continue;										//in the top row, middle row, or bottom row, respectively. adding to that index%9-index%3 pushes
		if(board[run]&bit){										//us forward 0, 3, or 6 cells for left, center and right. This puts us in the top left corner of
			found=1;										//whatever block the focus cell is in. Each iteration we add 1 to index unless our index%3 is 2, 
			break;											//because in that case we've reached the right edge of the block, so we must add 7 to wrap around
		}					//... same algorithm once again				//to the left edge of the next row.
	}
	if(!found){
		board[index]=bit|SET;
		return 1;
	}
  }
  return 0;						//If we cannot set focus cell's value on account of the cells in its row column or block we return zero
}

//This function sets the bits of a short to represent all of the numbers not currently present in some cell's row, column, or block
//Returns 1 if that cell can only hold one value, so that cell is now set
//Returns -1 if that cell in ineligable to hold any number, so we've encountered a contradiction, and need to revert with the logstack
//And returns 0 otherwise, to indicate that the cell's bits are properly set
//
//For instance if some cell has a 4,5,7,8 and 9 somewhere in its row/column/block, but not 1,2,3 and 6, this function will store the value
//ONE|TWO|THREE|SIX or 0x27 in that cell, and return zero
//
//If some cell has a 1,2,3,5,6,7,8, and 9 somewhere in its row/column/block, but not 4, this function will store the value FOUR|SET or 0x208 in
//that cell, and return one
//
//Lastly if some cell has all 1,2,3,4,5,6,7,8 and 9 in its row/column/block, this function will store the value 0x0 in the cell and return -1

int setPossibilities(short board[],int index){
  //We begin with the value 0x1FF, which represents a cell that could hold any value and is not set
  short val=ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE;
  //For each other cell in the row that is set, we remove that bit from the value
  for(int c=0,run=index-index%9;c<9;++c,++run){
	if(run==index)
		continue;
	if(set(&board[run]))
		val&=~board[run];
  }
  //For each other cell in the column that is set we remove that bit from the value
  for(int c=0,run=index%9;c<9;++c,run+=9){
	if(run==index)
		continue;
	if(set(&board[run]))
		val&=~board[run];
  }
  //For each other cell in the block that is set we remove that bit from the value
  for(int c=0,run=index-index%27+index%9-index%3;c<9;++c,run+=run%3==2?7:1){
	if(run==index)
		continue;
	if(set(&board[run]))
		val&=~board[run];
  }
  //What remains is the bitwise or of the values not present, which we store in the cell
  board[index]=val;
  //Returning 1 if it's one possible value, -1 if there are no possible values, and 0 if we have more than one
  if(!val)
	return -1;   //-1 indicates that this puzzle has a contradiction and needs to be reverted
  else if(set(&val)) //set() will or in SET for us
	return 1;    //1 indicates that control should start the loop over and call this function on every value again because circumstances have changed
  else
	return 0;    //0 indicates that control can continue as it was
}

//Prints the current puzzle board, printing unset cells as dashes
void printBoard(short board[]){
  int t = translate(board[0]);
  if(t)
  	printf("%d",t);
  else
	printf("-");
  for(int c=1;c<81;++c){
	t = translate(board[c]);
	if(!(c%9)){
		if(t)
			printf("\n%d",t);
		else
			printf("\n-");
	}
	else{
		if(t)
			printf(",%d",t);
		else
			printf(",-");
	}
  }
  printf("\n\n");
}

//Verifies that each row column and block of the puzzle contains no more than one of each number
_Bool verify(short board[]){
  for(int c=0;c<9;++c){
	char count[9]={0,0,0,0,0,0,0,0,0};
	for(int d=0;d<9;++d)
		if(set(&board[c*9+d]))
			++count[translate(board[c*9+d])-1];
	for(int d=0;d<9;++d)
		if(count[d]>1)
			return 0;
  }
  for(int c=0;c<9;++c){
	char count[9]={0,0,0,0,0,0,0,0,0};
	for(int d=0;d<9;++d)
		if(set(&board[c+d*9]))
			++count[translate(board[c+d*9])-1];
	for(int d=0;d<9;++d)
		if(count[d]>1)
			return 0;
  }
  for(int c=0;c<9;++c){
	char count[9]={0,0,0,0,0,0,0,0,0};
	for(int d=0;d<9;++d)
		if(set(&board[(c/3)*27+(c%3)*3+(d/3)*9+(d%3)]))
			++count[translate(board[(c/3)*27+(c%3)*3+(d/3)*9+(d%3)])-1];
	for(int d=0;d<9;++d)
		if(count[c]>1)
			return 0;
  }
  return 1;
}

//This data structure allows the puzzle board to be reverted when a contradiction is encountered from a bad guess
//The next pointer allows them to form a stack
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
			board[head->index]=bit|SET;
			head->val=bit;
			return head;
		}
	log *hold=head->next;
	free(head);
	head=hold;
  }
  fprintf(stderr,"This puzzle has no solutions\n");
  exit(EXIT_FAILURE);
}

//This function sets the bits of every unset cell in the board, making sure to start over if some cell is set in the process
//It also reverts the board and starts over if it encounters a contradiction
//Returns the current value of the loghead
log* setAllPossibilities(short board[],log *head){
  for(int c=0;c<81;++c)
	if(!set(&board[c])){
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
  _Bool solved=0;
  log *head=NULL;
  while(!solved){
	//We begin by setting all unset cells to their bitwise ors of possibilities
	head=setAllPossibilities(board,head);
	for(int c=0;c<81;++c)
		//Then for each unset cell in the board, we try to set it using analyzePossibilities()
		if(!set(&board[c]))
			if(analyzePossibilities(board,c)){
				//If it succeeds, we reset the bitwise ors for the whole board, and start over
				head=setAllPossibilities(board,head);
				c=-1;
			}
	//Eventually we reach a point where either the puzzle is solved, or we must guess to continue to make progress
	solved=1;
	for(int c=0;c<81;++c)
		if(!set(&board[c])){
			//If some cell is not set, the puzzle is not solved
			solved=0;
			int smallestParity=9;
			int index=c;
			int val=ONE;
			for(;c<81;++c){						//This for loop chooses which cell to guess on by picking the cell with the fewest possibilities
				if(set(&board[c]))
					continue;
				int parity=0;
				int lowestBit=NINE;
				for(short bit=NINE;bit!=0;bit>>=1)
					if(board[c]&bit){
						if(++parity>smallestParity)
							break;
						lowestBit=bit;
					}
				if(parity<smallestParity){
					smallestParity=parity;
					index=c;
					val=lowestBit;
					if(parity==2)				//2 is the lowest possible parity of a cell that is not set
						break;
				}
			}
			log *hold=malloc(sizeof(log));				//Make a record of which cell was changed and to what and put it on the log stack
			*hold=(log){.index=index,.val=val,.next=head};
			memcpy(hold->old,board,81*sizeof(short));
			board[index]=val|SET;
			head=hold;
			break;
		}
  }
  //Control makes it here when the puzzle is solved
  while(head){
	//Free any logfiles if there are any
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
			board[c]=ONE|SET;
			break;
		case 2:
			board[c]=TWO|SET;
			break;
		case 3:
			board[c]=THREE|SET;
			break;
		case 4:
			board[c]=FOUR|SET;
			break;
		case 5:
			board[c]=FIVE|SET;
			break;
		case 6:
			board[c]=SIX|SET;
			break;
		case 7:
			board[c]=SEVEN|SET;
			break;
		case 8:
			board[c]=EIGHT|SET;
			break;
		case 9:
			board[c]=NINE|SET;
			break;
		default:
			board[c]=0;
			break;
	}
  if(!verify(board)){
	fprintf(stderr,"Puzzle has a contradiction and cannot be solved\n");
	exit(EXIT_FAILURE);
  }
  else
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
