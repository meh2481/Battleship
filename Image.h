#ifndef IMAGE_H
#define IMAGE_H

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <iostream>
using namespace std;

class Image
{
protected:
	int texW, texH;
	GLuint texture;
	
public:
	Image();
	~Image();

	void load(string sFilename);
	void draw(int x, int y);

	int getWidth() {return texW;};
	int getHeight() {return texH;};
};









#endif //ifdef IMAGE_H
