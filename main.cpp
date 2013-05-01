#include "Image.h"
#include "Board.h"

#include <stdio.h>
#include <stdlib.h>

#define WIDTH  640
#define HEIGHT 640

#define STATE_PLAYER_PLACESHIPS		0
#define STATE_PLAYER2_PLACESHIPS	1
#define STATE_PLAYER_GUESS				2
#define STATE_AI_GUESS						3
#define STATE_GAMEOVER						4

Image* ship_edge;
Image* ship_center;
Image* bg;
Image* gameover;
int cursorX=0, cursorY=0;
Board gameBoards[2];
bool bShowShips;
char cState = STATE_PLAYER_PLACESHIPS;
short g_rot;

//Set up SDL window
static void setup_sdl() 
{
	const SDL_VideoInfo* video;

  if(SDL_Init(SDL_INIT_VIDEO) < 0) 
  {
  	fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }
        
  // Quit SDL properly on exit 
  atexit(SDL_Quit);

  // Get the current video information 
  video = SDL_GetVideoInfo();
  if(video == NULL) 
  {
  	fprintf(stderr, "Couldn't get video information: %s\n", SDL_GetError());
    exit(1);
  }

  // Set the minimum requirements for the OpenGL window
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  //Set icon for window
  SDL_Surface *image;
	image = IMG_Load("res/icon.png");
  SDL_WM_SetCaption("Battleship", NULL);
	SDL_WM_SetIcon(image, NULL);
	SDL_FreeSurface(image);

  // Create SDL window
  if(SDL_SetVideoMode(WIDTH, HEIGHT, video->vfmt->BitsPerPixel, SDL_OPENGL) == 0) 
  {
  	fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
    exit(1);
  }
}

//Set up OpenGL
static void setup_opengl()
{
	// Make the viewport
  glViewport(0, 0, WIDTH, HEIGHT);

  // Set the camera projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
    
  glOrtho(0, WIDTH, HEIGHT, 0, 1, -1);
    
  glMatrixMode(GL_MODELVIEW);

  // set the clear color to grey
  glClearColor(0.5, 0.5 ,0.5, 0);
    
  glEnable(GL_TEXTURE_2D);
  glLoadIdentity();
    
  //Enable image transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//Repaint our window
static void repaint()
{   
	// Clear the color plane and the z-buffer 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  bg->draw(0,0);
  if(cState == STATE_PLAYER_GUESS)
  {
  	ship_center->setColor(1.0,1.0,1.0);
  	ship_center->draw(cursorX / 64 * 64, cursorY / 64 * 64);
  	gameBoards[0].draw(bShowShips);
  }
  else if(cState == STATE_AI_GUESS)
  {
  	gameBoards[1].draw(true);
	}
	else if(cState == STATE_PLAYER_PLACESHIPS)
	{
		gameBoards[1].drawShips();
		int iLen = gameBoards[1].curShipLen();
		if(iLen != -1)
		{
			ship_edge->setColor(1.0,1.0,1.0);
			int x = cursorX / TILE_WIDTH;
			int y = cursorY / TILE_HEIGHT;
			ship_edge->draw(x * TILE_WIDTH, y * TILE_HEIGHT);
			if(g_rot == DIR_DOWN)
				for(int i = y; i - y < iLen; i++)
					ship_edge->draw(x * TILE_WIDTH, i * TILE_HEIGHT);
			else if(g_rot == DIR_RIGHT)
				for(int i = x; i - x < iLen; i++)
					ship_edge->draw(i * TILE_WIDTH, y * TILE_HEIGHT);
		}
	}
	else if(cState == STATE_GAMEOVER)
	{
		//Center "Game Over" text on the screen
		gameover->draw((WIDTH - gameover->getWidth()) / 2.0, (HEIGHT - gameover->getHeight()) / 2.0);
	}

  // swap the back and front buffers 
  SDL_GL_SwapBuffers();
}

//Main program loop
static void main_loop() 
{
	SDL_Event event;
	bool bDelay = false;
	bool bAIGuessed = false;

  while(true)
  {
  	if(bDelay)
  	{
  		bDelay = false;
  		switch(cState)
  		{
  			case STATE_AI_GUESS:
  				if(!bAIGuessed)
  				{
  					bAIGuessed = true;
  					short guessCode = gameBoards[1].AIGuess();
  					if(guessCode == SHIP_WON)
  					{
  						cState = STATE_GAMEOVER;
  						break;
  					}
						bDelay = true;
					}
					else
						cState = STATE_PLAYER_GUESS;
					break;
				case STATE_PLAYER_GUESS:
					cState = STATE_AI_GUESS;
					bAIGuessed = false;
					bDelay = true;
					break;
			}
  	}
  	
  	// process pending events 
    while(SDL_PollEvent(&event)) 
    {
    	switch(event.type) 
    	{
      	case SDL_KEYDOWN:
        	switch(event.key.keysym.sym)
        	{
          	case SDLK_ESCAPE:
            	exit(0);
              break;
              
            case SDLK_p:
            	bShowShips = !bShowShips;
            	break;
            	
            case SDLK_n:
            	gameBoards[0].reset();
            	gameBoards[0].randShipPlacement();
            	break;
             
            default:
              //no default key processing
              break;
          }
          break;
        
        case SDL_MOUSEMOTION:
        	if(cState == STATE_PLAYER_GUESS ||
        		 cState == STATE_PLAYER_PLACESHIPS)
        	{
        		cursorX = event.motion.x;
        		cursorY = event.motion.y;
        	}
        	break;
        	
        case SDL_MOUSEBUTTONDOWN:
        	if(cState == STATE_GAMEOVER)
        	{
        		cState = STATE_PLAYER_PLACESHIPS;
        		gameBoards[0].reset();
        		gameBoards[1].reset();
  					cursorX = cursorY = 0;
        	}
        	else if(cState == STATE_PLAYER_GUESS)
        	{
        		if(event.button.button == SDL_BUTTON_LEFT)
        		{
        			short guessCode = gameBoards[0].playerGuess(event.button.x / TILE_WIDTH, event.button.y / TILE_HEIGHT);
        			if(guessCode == SHIP_WON)
        				cState = STATE_GAMEOVER;
        			else if(guessCode != CANT_GUESS)
        				bDelay = true;
        		}
					}
        	else if(cState == STATE_PLAYER_PLACESHIPS)
        	{
        		if(event.button.button == SDL_BUTTON_LEFT)	//Left mouse button = place ship
        		{
        			gameBoards[1].placeShip(event.button.x / TILE_WIDTH, event.button.y / TILE_HEIGHT, g_rot);
        			if(gameBoards[1].curShipLen() == -1)	//Placed last ship
        			{
        				gameBoards[0].randShipPlacement();	//Place enemy ships
        				cState = STATE_PLAYER_GUESS;
        			}
        		}
        		else	//Right mouse button = rotate ship
        		{
        			if(g_rot == DIR_DOWN)
        				g_rot = DIR_RIGHT;
        			else
        				g_rot = DIR_DOWN;
						}
					}
        	break;

        case SDL_QUIT:
          exit (0);
          break;
      }
    }
    
		// update the screen  
    repaint();
    
   	//if(cState == STATE_AI_GUESS)
    //{
    	//Delay longer for AI guess so player can see where they're guessing
    //	bDelay = true;
		//}

    // Run at about 60 fps
    SDL_Delay(16);	//Wait 16ms until next loop, for ~60fps framerate
		
		if(bDelay)
			SDL_Delay(500);
  }
}

//Our main program
int main(int argc, char** argv)
{
	bShowShips = false;
	g_rot = DIR_DOWN;

	//Seed the random number generator
	srand(time(NULL));

	//Set up our viewport
  setup_sdl();
  setup_opengl();
    
	//Load textures
 	ship_edge = new Image("res/ship_edge.png");
 	ship_center = new Image("res/ship_center.png");
 	bg = new Image("res/board.png");
 	gameover = new Image("res/gameover.png");
 	gameBoards[0].setShipImages(ship_edge, ship_center);
 	gameBoards[1].setShipImages(ship_edge, ship_center);
    
  //Start the main loop
  main_loop();
        
  return 0;
}
