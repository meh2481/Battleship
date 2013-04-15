#include "Image.h"
#include "Board.h"

#include <stdio.h>
#include <stdlib.h>

#define WIDTH  640
#define HEIGHT 640

Image* ship_edge;
Image* ship_center;
Image* bg;
int cursorX, cursorY;
Board gameBoards[2];
bool bShowShips;

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
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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
  //ship_edge->draw(64,64);
  gameBoards[0].draw(bShowShips);
  ship_center->draw(cursorX / 64 * 64, cursorY / 64 * 64);

  // swap the back and front buffers 
  SDL_GL_SwapBuffers();
}

//Main program loop
static void main_loop() 
{
	SDL_Event event;

  while(true)
  {
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
        	cursorX = event.motion.x;
        	cursorY = event.motion.y;
        	break;
        	
        case SDL_MOUSEBUTTONDOWN:
        	if(event.button.button == SDL_BUTTON_LEFT)
        		gameBoards[0].playerGuess(event.button.x / TILE_WIDTH, event.button.y / TILE_HEIGHT);
        	break;

        case SDL_QUIT:
          exit (0);
          break;
      }
    }
    
    // update the screen  
    repaint();

    // Run at about 60 fps
    SDL_Delay(16);	//Wait 16ms until next loop, for ~60fps framerate
  }
}

//Our main program
int main(int argc, char** argv)
{
	bShowShips = false;

	//Seed the random number generator
	srand(time(NULL));

	//Set up our viewport
  setup_sdl();
  setup_opengl();
    
	//Load textures
 	ship_edge = new Image("res/ship_edge.png");
 	ship_center = new Image("res/ship_center.png");
 	bg = new Image("res/board.png");
 	gameBoards[0].setShipImages(ship_edge, ship_center);
 	gameBoards[0].randShipPlacement();	//Start board
    
  //Start the main loop
  main_loop();
        
  return 0;
}
