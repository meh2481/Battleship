#include "Board.h"
#include <stdio.h>
#include <stdlib.h>

Board::Board()
{
  imgShipEdge = imgShipCenter = NULL;
  for(int i = 0; i < NUM_SHIPS; i++)
  {
  	pShips[i] = new Ship();
  	pShips[i]->Xpos = pShips[i]->Ypos = -1;
  	pShips[i]->rotation = NO_DIR;
  	pShips[i]->len = SHIP_SIZES[i];
  	pShips[i]->name = SHIP_NAMES[i];
  }
  
  reset();
}

Board::~Board()
{
	for(int i = 0; i < NUM_SHIPS; i++)
  	{
  		delete pShips[i];
	}
}

void Board::reset()
{
	for(int i = 0; i < BOARD_WIDTH; i++)
	{
		for(int j = 0; j < BOARD_HEIGHT; j++)
		{
			board[i][j].bGuessed = false;
			board[i][j].bShip = false;
			board[i][j].pShip = NULL;
		}
	}
	for(int i = 0; i < NUM_SHIPS; i++)
		pShips[i]->hits = 0;
	AILastGuessX = AILastGuessY = AIGuessDir = NO_DIR;
	numShipsSunk = 0;
	numGuesses = 0;
}

bool Board::placeShip(Ship* s)
{	
	if(s->rotation == DIR_RIGHT)	//Place ship horizontally
	{
		//Create coordinate to place at
		s->Xpos = rand() % (BOARD_WIDTH - s->len + 1);
		s->Ypos = rand() % (BOARD_HEIGHT);
		
		//Test and make sure this doesn't overlap any other ships
		for(int i = s->Xpos; i < s->Xpos + s->len; i++)
		{
			if(board[i][s->Ypos].bShip)
				return false;	//Bad location; choose another
		}
		
		//Fill ship in here
		for(int i = s->Xpos; i < s->Xpos + s->len; i++)
		{
			board[i][s->Ypos].bShip = true;
			board[i][s->Ypos].pShip = s;
		}
		
	}
	else	//Place ship vertically
	{
		//Create coordinate to place at
		s->Xpos = rand() % (BOARD_WIDTH);
		s->Ypos = rand() % (BOARD_HEIGHT - s->len + 1);
		
		//Test and make sure this doesn't overlap any other ships
		for(int i = s->Ypos; i < s->Ypos + s->len; i++)
		{
			if(board[s->Xpos][i].bShip)
				return false;	//Bad location; choose another
		}
		
		//Fill ship in here
		for(int i = s->Ypos; i < s->Ypos + s->len; i++)
		{
			board[s->Xpos][i].bShip = true;
			board[s->Xpos][i].pShip = s;
		}
	}
	return true;
}

void Board::randShipPlacement()
{
	for(int i = 0; i < NUM_SHIPS; i++)
	{
		pShips[i]->rotation = DIR_RIGHT;
		if(rand() % 2 == 0)
			pShips[i]->rotation = DIR_DOWN;
		while(!placeShip(pShips[i]));	//Place ship until we don't hit another (could theoretically hang here; oh, well)
	}
}
	
void Board::playerGuess(int guessX, int guessY)
{
	if(board[guessX][guessY].bGuessed)	//Can't guess the same spot twice
		return;
		
	numGuesses++;
	
	board[guessX][guessY].bGuessed = true;
	if(board[guessX][guessY].bShip)
	{
		//TODO Some kind of cue for hitting ship
		if(board[guessX][guessY].pShip != NULL)
		{
			if(++board[guessX][guessY].pShip->hits == board[guessX][guessY].pShip->len)
			{
				//TODO Some kind of cue for sinking ship
				cout << "Player sunk " << board[guessX][guessY].pShip->name << endl;
				if(++numShipsSunk == NUM_SHIPS)
				{
					//TODO Some kind of game over state or somewhat
					cout << "Player won with " << numGuesses << " guesses." << endl;
					reset();
					randShipPlacement();
				}	
			}	
		}
	}
}

void Board::AIGuess(short AIGuessLevel)
{
	if(AIGuessLevel == 1)
	{
		int row = rand % 10;
		int col = rand % 10;

		if(board[row][col].AIGuessed)
			return;

		board[row][col].AIGuessed = true;
		
		if(board[row][col].AIShip)
		{
			if(board[row][col].aShip != NULL)
			{
				if(++board[row][col].aShip->hits == board[row][col].aShip->len)
				{
					cout << "Computer sunk " << board[row][col].aShip->name << endl;
					
					if(++numPCSunk == NUM_PC_SHIPS)
					{
						cout << "Game Over" << endl << "Computer won" << endl;
						reset();
					}
				}
			}
		}
	}

	else if(AIGuessLevel == 2)
	{

	}
}

void findSpot(BoardSlot board[BOARD_WIDTH][BOARD_HEIGHT])
{
	int hitCount[20];
	int h = 0;
	int max = -1;
	
	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 10; j++)
		{
			if(board[BOARD_WIDTH][BOARD_HEIGHT] == true)
			{
				hitCount[h] = hitCount[h] + 1;
				hitCount[j+10] = hitCount[j+10] + 1;
			}
		}
		
		h++;
	}

	for(int k = 1; k < 20; k++)
	{
		if(hitCount[k] > hitCount[k-1])
		{
			max = k;
		}
	}

	
	
}
	
void Board::draw(bool bDrawShips)
{
	//Loop through the board
	for(int i = 0; i < BOARD_WIDTH; i++)
	{
		for(int j = 0; j < BOARD_HEIGHT; j++)
		{
			if(board[i][j].bShip && (bDrawShips || board[i][j].bGuessed))	//If we're supposed to draw ship outlines, or if they've guessed there
				imgShipEdge->draw(i * TILE_WIDTH, j * TILE_HEIGHT);
			if(board[i][j].bGuessed)						//And draw the guesses
				imgShipCenter->draw(i * TILE_WIDTH, j * TILE_HEIGHT);
		}
	}
}
