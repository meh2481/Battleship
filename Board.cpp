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
  	pShips[i]->num = i;
  }
  	 
  AIGuessLevel = UNINTELLIGENT_GUESS;
  reset();
  loadSound();
}


Board::~Board()
{
	for(int i = 0; i < NUM_SHIPS; i++)
  	{
  		delete pShips[i];
	}
	Mix_FreeMusic(backMusic);
	Mix_FreeMusic(mainPageMusic);	
	Mix_FreeMusic(gameOver);

	Mix_FreeChunk(hitShip);
	Mix_FreeChunk(sunkShip);
	Mix_FreeChunk(missShip);

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
	curShipPlace = 0;
}

bool Board::placeShip(int i, int j, short rotation)
{
	Ship* s = pShips[curShipPlace];
	s->rotation = rotation;
	s->Xpos = i;
	s->Ypos = j;
	return placeShip(s);
}

bool Board::placeShip(Ship* s)
{	
	if(s->rotation == DIR_RIGHT)	//Place ship horizontally
	{
		//Test and make sure isn't off edge of map
		if(s->Xpos + s->len > BOARD_WIDTH)
			return false;
	
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
		//Test and make sure isn't off edge of map
		if(s->Ypos + s->len > BOARD_HEIGHT)
			return false;
			
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
	curShipPlace++;
	return true;
}

void Board::randShipPlacement()
{
	for(int i = 0; i < NUM_SHIPS; i++)
	{
		do	//Oh, look, a do-while loop has a use. Wonders never cease.
		{
			//Create coordinate to place at
			pShips[i]->rotation = DIR_RIGHT;
			pShips[i]->Xpos = rand() % (BOARD_WIDTH - pShips[i]->len + 1);
			pShips[i]->Ypos = rand() % (BOARD_HEIGHT);
			
			if(rand() % 2 == 0)
			{
				pShips[i]->rotation = DIR_DOWN;
				pShips[i]->Xpos = rand() % (BOARD_WIDTH);
				pShips[i]->Ypos = rand() % (BOARD_HEIGHT - pShips[i]->len + 1);
			}
		}
		while(!placeShip(pShips[i]));	//Place ship until we don't hit another (could theoretically hang here; oh, well)
	}
}
	
short Board::playerGuess(int guessX, int guessY)
{
	short retVal = SHIP_MISS;

	if(board[guessX][guessY].bGuessed)	//Can't guess the same spot twice
		return CANT_GUESS;
		
	numGuesses++;
	
	board[guessX][guessY].bGuessed = true;
	if(board[guessX][guessY].bShip)
	{

		//this is the part where a sound is made for hitting the ship
		Mix_PlayChannel(-1,hitShip,0);

		//TODO Some kind of cue for hitting ship
		retVal = SHIP_HIT;
		if(board[guessX][guessY].pShip != NULL)
		{
			if(++board[guessX][guessY].pShip->hits == board[guessX][guessY].pShip->len)
			{

				//this part sound made for sinking the ship
				Mix_PlayChannel(-1,sunkShip, 0);
			
				//TODO Some kind of cue for sinking ship
				retVal = board[guessX][guessY].pShip->num;
				cout << "Player sunk " << SHIP_NAMES[board[guessX][guessY].pShip->num] << endl;
				if(++numShipsSunk == NUM_SHIPS)
				{
					//sound for being game over
					Mix_PlayMusic(gameOver, 2);

					//TODO Some kind of game over state or somewhat
					cout << "Player won with " << numGuesses << " guesses." << endl;
					retVal = SHIP_WON;
				}	
			}	
		}
	}
	return retVal;
}

short Board::AIGuess()
{
	short retVal = SHIP_MISS;
	
	if(AIGuessLevel == UNINTELLIGENT_GUESS)
	{
		int row, col;
		do	//Does it say something bad about my programming style if I use more than one do-while loop in a program? Hmm...
		{
			row = rand() % BOARD_WIDTH;
			col = rand() % BOARD_HEIGHT;
		}
		while(board[row][col].bGuessed);	//While loop here so AI doesn't "give up" if it guesses the same spot twice
		
		numGuesses++;	//AI has # guesses count too

		board[row][col].bGuessed = true;
		
		if(board[row][col].bShip)
		{
			//this is the part where a sound is made for hitting the ship
			Mix_PlayChannel(-1,hitShip,0);

			retVal = SHIP_HIT;
			if(board[row][col].pShip != NULL)
			{
				if(++board[row][col].pShip->hits == board[row][col].pShip->len)
				{
					//this part sound made for sinking the ship
					Mix_PlayChannel(-1,sunkShip, 0);					

					retVal = board[row][col].pShip->num;
					cout << "Computer sunk " << SHIP_NAMES[board[row][col].pShip->num] << endl;
					
					if(++numShipsSunk == NUM_SHIPS)	//AI has same # of ships as player
					{

						//this part sound made for computer winning
						Mix_PlayMusic(gameOver, 2);
						retVal = SHIP_WON;
						cout << "Game Over." << endl << "Computer won with " << numGuesses << " guesses." << endl;
				}
				}
			}
		}
	}

	else if(AIGuessLevel == INTELLIGENT_GUESS)
	{
		int spot[2];
		int row, col;

		while(board[row][col].bGuessed)
		{
			findSpot(spot);
			row = spot[0];
			col = spot[1];
		}
		numGuesses++;

		board[row][col].bGuessed = true;

		if(board[row][col].bShip)
		{
			if(board[row][col].pShip != NULL)
			{
				if(++board[row][col].pShip->hits == board[row][col].pShip->len)
				{
					cout << "Computer sunk " << SHIP_NAMES[board[row][col].pShip->num] << endl;
					
					if(++numShipsSunk == NUM_SHIPS)	//AI has same # of ships as player
					{
						cout << "Game Over." << endl << "Computer won with " << numGuesses << " guesses." << endl;
						reset();
						randShipPlacement();
					}
				}
			}
		}
	}
	return retVal;
}

void Board::findSpot(int spot[2])
{
	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 10; j++)
		{
			if(board[i][j].bGuessed == true)
			{
				if(board[i-1][j].bGuessed == false)
				{
					//board[i-1][j].bGuessed = true;
					spot[0] = i-1;
					spot[1] = j;
					break;
				}
				else if(board[i+1][j].bGuessed == false)
				{
					//board[i+1][j].bGuessed = true;
					spot[0] = i+1;
					spot[1] = j;
					break;
				}
				else if(board[i][j-1].bGuessed == false)
				{
					//board[i][j-1] = true;
					spot[0] = i;
					spot[1] = j-1;
					break;
				}
				else if(board[i][j+1].bGuessed == false)
				{
					//board[i][j+1] = true;
					spot[0] = i;
					spot[1] = j+1;
					break;
				}
			}
		}
	}	
}

int Board::curShipLen()
{
	if(curShipPlace < NUM_SHIPS)
		return SHIP_SIZES[curShipPlace];
	return -1;
}
	
void Board::draw(bool bDrawShips)
{
	//Loop through the board
	for(int i = 0; i < BOARD_WIDTH; i++)
	{
		for(int j = 0; j < BOARD_HEIGHT; j++)
		{
			if(board[i][j].bShip && (bDrawShips || board[i][j].bGuessed))	//If we're supposed to draw ship outlines, or if they've guessed there
			{
				imgShipEdge->setColor(0.0,0.0,0.0);
				imgShipEdge->draw(i * TILE_WIDTH, j * TILE_HEIGHT);
			}
			if(board[i][j].bGuessed)						//And draw the guesses
			{
				if(board[i][j].bShip)
					imgShipCenter->setColor(1.0,0.0,0.0);
				else
					imgShipCenter->setColor(1.0,1.0,1.0);
				imgShipCenter->draw(i * TILE_WIDTH, j * TILE_HEIGHT);
			}
		}
	}
}

void Board::drawShips()	//Separate from above because of ship placement
{
	for(int i = 0; i < BOARD_WIDTH; i++)
	{
		for(int j = 0; j < BOARD_HEIGHT; j++)
		{
			if(board[i][j].bShip)
			{
				imgShipEdge->setColor(0.0,0.0,0.0);
				imgShipEdge->draw(i * TILE_WIDTH, j * TILE_HEIGHT);
			}
		}
	}
}
//loading the sounds to be played throughout the game
bool Board::loadSound()
{
	backMusic =Mix_LoadMUS("  PUTMUSICFILE " );
	mainPageMusic =Mix_LoadMUS("  PUTMUSICFILE " );
	gameOver = Mix_LoadMUS(" PUTMUSICFILE ");
	if((backMusic == NULL)||(mainPageMusic == NULL)||(gameOver == NULL))
	{
		return false;
	}
	hitShip =Mix_LoadWAV("  PUTMUSICFILE " );
	sunkShip =Mix_LoadWAV("  PUTMUSICFILE " );
	missShip =Mix_LoadWAV("  PUTMUSICFILE " );
	if(( hitShip ==NULL)||( sunkShip==NULL)||(missShip ==NULL))
	{
		return false;
	}
	return true;

}
