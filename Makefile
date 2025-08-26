VERSION = 1.0
CXX = g++

LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf

SRC = IMS.cpp
TARGET = IMS

IMS: $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(LIBS) 

clean :
	rm $(TARGET)

install: IMS
