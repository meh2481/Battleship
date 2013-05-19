#ifndef IMAGE_H
#define IMAGE_H

#define WIDTH  640
#define HEIGHT 640

#define MAGIC_ZOOM_NUMBER -2.415f

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <iostream>
using namespace std;

class Color
{
public:
	float r,g,b;
	Color();
	
	void from256(int ir, int ig, int ib);
};

class Image
{
protected:
	int texW, texH;
	GLuint texture;
	Color m_col;
	
public:
	Image();
	Image(string sFilename);
	~Image();

	void load(string sFilename);
	void draw(int x, int y, float fOffset = 0.0);

	int getWidth() 					{return texW;};
	int getHeight() 				{return texH;};
	
	Color getColor()				{return m_col;};
	int setColor(Color col)	{m_col = col;};
	int setColor(float r, float g, float b)	{m_col.r = r; m_col.g = g; m_col.b = b;};
	int setColorFrom256(int r, int g, int b)	{m_col.from256(r,g,b);};
};









#endif //ifdef IMAGE_H
