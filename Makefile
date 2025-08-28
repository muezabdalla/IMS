VERSION = 1.0
CXX = g++

LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf

SRC = IMS.cpp
TARGET = IMS
PREFIX = /usr/local

IMS: $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(LIBS) 

clean :
	rm $(TARGET)

install:
	$(CXX) $(SRC) -o $(TARGET) $(LIBS) 
	mkdir -p $(PREFIX)/bin
	cp -f IMS $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/IMS
uninstall:
	rm -r $(DESTDIR)$(PREFIX)/bin/IMS
