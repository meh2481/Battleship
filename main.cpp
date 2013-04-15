#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

#define WIDTH  800
#define HEIGHT 600

GLuint myglu;
int textw,texth;

//Enable transparency across OpenGL
void EnableTransparency()
{
	glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//Load a GL texture from a filename
GLuint LoadTexture(char *filename, int *textw, int *texth) 
{
 	SDL_Surface *surface;
  GLuint textureid;
  int mode;

  surface = IMG_Load(filename);	//Use SDL_image to load various formats

  // could not load filename
  if(!surface) 
  {
		std::cout << "No img " << filename << std::endl;
    return 0;
	}
	
	// work out what format to tell glTexImage2D to use...
  if(surface->format->BytesPerPixel == 3) // RGB 24bit
  { 
		mode = GL_RGB;
	} 
	else if(surface->format->BytesPerPixel == 4)  // RGBA 32bit
	{
		mode = GL_RGBA;
	} 
	else //Unsupported format
	{
		SDL_FreeSurface(surface);
    return 0;
	}
	
	*textw=surface->w;
  *texth=surface->h;
  // create one texture name
  glGenTextures(1, &textureid);

  // tell opengl to use the generated texture name
  glBindTexture(GL_TEXTURE_2D, textureid);

  // this reads from the sdl surface and puts it into an opengl texture
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

  // these affect how this texture is drawn later on...
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  // clean up
  SDL_FreeSurface(surface);

  return textureid;
}

//Draw an image to our OpenGL buffer
void DrawTexture(int x, int y, GLuint textureid, int textw, int texth) 
{
	// tell opengl to use the generated texture name
  glBindTexture(GL_TEXTURE_2D, textureid);
  glEnable(GL_TEXTURE_2D);

  // make a rectangle
  glBegin(GL_QUADS);

  // top left
  glTexCoord2i(0, 0);
  glVertex3f(x, y, 0);

  // top right
  glTexCoord2i(1, 0);
  glVertex3f(x+textw, y, 0);

  // bottom right
  glTexCoord2i(1, 1);
  glVertex3f(x+textw, y+texth, 0);

  // bottom left
  glTexCoord2i(0, 1);
  glVertex3f(x, y+texth, 0);

  glEnd();
        
  glDisable(GL_TEXTURE_2D);
}

//Repaint our window
static void repaint()
{   
	// Clear the color plane and the z-buffer 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  DrawTexture(100,100,myglu,textw,texth);
  DrawTexture(200,200,myglu,textw,texth);

  // swap the back and front buffers 
  SDL_GL_SwapBuffers();
}

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


static void setup_opengl()
{
	// Make the viewport
  glViewport(0, 0, WIDTH, HEIGHT);

  // Set the camera projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
    
  glOrtho(0, WIDTH, HEIGHT, 0, 1, -1);
    
  glMatrixMode(GL_MODELVIEW);

  // set the clear color to black
  glClearColor(0.0, 0.0 ,0.0, 0);
    
  glEnable(GL_TEXTURE_2D);
  glLoadIdentity();
    
  //Enable image transparency
  EnableTransparency();
}


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
 	myglu=LoadTexture("res/foo.png",&textw,&texth);
    
  //Start the main loop
  main_loop();
        
  return 0;
}
