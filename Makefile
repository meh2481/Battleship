#Slightly altered version of in-class example

objects := main.o Image.o Board.o
libs := -lSDL -lSDL_image -lGL -lGLU -lSDL_mixer

all: battleship

battleship: $(objects)
	g++ -o $@ $^ $(libs)

%.o: %.cpp
	g++ -c -MMD -o $@ $<

-include $(objects:.o=.d)

clean:
	rm -f *.o *.d battleship
