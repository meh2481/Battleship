#include "Image.h"

#include <stdio.h>
#include <stdlib.h>

#define WIDTH  640
#define HEIGHT 640

Image ship_edge;
Image bg;

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
    
  bg.draw(0,0);
  ship_edge.draw(64, 64);
  ship_edge.draw(128, 128);

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
             
            default:
              //no default key processing
              break;
          }
          break;

        case SDL_QUIT:
          exit (0);
          break;
      }
    }
    
    // update the screen  
    repaint();

    // Run at about 60 fps
    SDL_Delay( 16 );	//Wait 16ms until next loop, for ~60fps framerate
  }
}

//Our main program
int main(int argc, char** argv)
{
	//Set up our viewport
  setup_sdl();
  setup_opengl();
    
	//Load textures
 	ship_edge.load("res/ship_edge.png");
 	bg.load("res/board.png");
    
  //Start the main loop
  main_loop();
        
  return 0;
}
