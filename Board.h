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

#define SHIP_HIT		5
#define SHIP_MISS		6
#define	SHIP_WON		7
#define CANT_GUESS	-1

static const int SHIP_SIZES[NUM_SHIPS] = {5,4,3,3,2};
static const char* SHIP_NAMES[NUM_SHIPS] = {"Aircraft Carrier","Battleship","Destroyer","Submarine","Patrol Boat"};

class Ship
{
public:
	int Xpos, Ypos;
	short rotation;
	short len;
	short num;
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
	int curShipPlace;
	
	//Helper functions
	bool placeShip(Ship* s);
	
public:
	Board();
	~Board();
	
	void reset();
	void randShipPlacement();
	
	short playerGuess(int guessX, int guessY);
	short AIGuess();
	
	void draw(bool bDrawShips = false);
	void drawShips();
	
	void 	setAIGuessLevel(short level)	{AIGuessLevel = level;};
	short getAIGuessLevel()						{return AIGuessLevel;};
	void 	setShipImages(Image* edge, Image* center)	{imgShipEdge = edge; imgShipCenter = center;};

	void 	findSpot(int spot[2]);
	int 	curShipLen();
	bool 	placeShip(int i, int j, short rotation);

};

























#endif //defined BOARD_H
