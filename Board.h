#ifndef BOARD_H
#define BOARD_H

#include "Image.h"

#define BOARD_WIDTH		10
#define BOARD_HEIGHT	10

#define TILE_WIDTH		64
#define TILE_HEIGHT		64

#define NUM_SHIPS			5

#define DIR_UP				0
#define DIR_RIGHT			1
#define DIR_DOWN			2
#define DIR_LEFT			3
#define NO_DIR				-1

#define INTELLIGENT_GUESS		0
#define UNINTELLIGENT_GUESS	1

static const int SHIP_SIZES[NUM_SHIPS] = {5,4,3,3,2};
static const char* SHIP_NAMES[NUM_SHIPS] = {"Aircraft Carrier","Battleship","Destroyer","Submarine","Patrol Boat"};

class Ship
{
public:
	int Xpos, Ypos;
	short rotation;
	short len;
	string name;
	short hits;
};

class BoardSlot
{
public:
	bool bShip;
	bool bGuessed;
	Ship* pShip;
};


class Board
{
protected:
	BoardSlot board[BOARD_WIDTH][BOARD_HEIGHT];
	short AILastGuessX, AILastGuessY;
	short AIGuessDir;
	short AIGuessLevel;
	short numShipsSunk;
	int numGuesses;
	Ship* pShips[NUM_SHIPS];
	Image* imgShipEdge, *imgShipCenter;
	
	//Helper functions
	bool placeShip(Ship* s);
	
public:
	Board();
	~Board();
	
	void reset();
	void randShipPlacement();
	
	void playerGuess(int guessX, int guessY);
	void AIGuess(short AIGuessLevel = UNINTELLIGENT_GUESS);
	
	void draw(bool bDrawShips = false);
	
	void setAIGuessLevel(short level)	{AIGuessLevel = level;};
	short getAIGuessLevel()						{return AIGuessLevel;};
	void setShipImages(Image* edge, Image* center)	{imgShipEdge = edge; imgShipCenter = center;};

	void findSpot();

};

























#endif //defined BOARD_H
