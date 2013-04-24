#include "Image.h"

Image::Image()
{
	texture = texW = texH = 0;
}

Image::Image(string sFilename)
{
	load(sFilename);
}

Image::~Image()
{
}


void Image::load(string sFilename)
{
	texture = texW = texH = 0;
	SDL_Surface *surface;
  int mode;

  surface = IMG_Load(sFilename.c_str());	//Use SDL_image to load various formats

  // could not load filename
  if(!surface) 
  {
		std::cout << "No img " << sFilename << std::endl;
		texture = 0;
    return;
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
		texture = 0;
    return;
	}
	
	texW=surface->w;
  texH=surface->h;
  // create one texture name
  glGenTextures(1, &texture);

  // tell opengl to use the generated texture name
  glBindTexture(GL_TEXTURE_2D, texture);

  // this reads from the sdl surface and puts it into an opengl texture
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

  // these affect how this texture is drawn later on...
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  // clean up
  SDL_FreeSurface(surface);

}

void Image::draw(int x, int y)
{
	if(texture == 0)
		return;
	
	// tell opengl to use the generated texture
  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_TEXTURE_2D);

  // make a rectangle
  glBegin(GL_QUADS);
	glColor3f(m_col.r,m_col.g,m_col.b);	//Colorize according to how we've colorized this image
  // top left
  glTexCoord2i(0, 0);
  glVertex3f(x, y, 0);

  // top right
  glTexCoord2i(1, 0);
  glVertex3f(x+texW, y, 0);

  // bottom right
  glTexCoord2i(1, 1);
  glVertex3f(x+texW, y+texH, 0);

  // bottom left
  glTexCoord2i(0, 1);
  glVertex3f(x, y+texH, 0);

  glEnd();
        
  glDisable(GL_TEXTURE_2D);
}

Color::Color()
{
	r = g = b = 1.0;
}

void Color::from256(int ir, int ig, int ib)
{
	r = (float)ir/255.0;
	g = (float)ig/255.0;
	b = (float)ib/255.0;
}




