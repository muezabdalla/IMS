# IMS (Input Monitoring System)

![preview when clicked](./screenshots/example.png "image")

this is the image when you click the right button.

## The keyboard monitoring software for linux
this is a software to show the keyboard clicks on screen. it is useful when making a tutorial video so that learners know what you pressed.

[comment]: # (<img src="./screenshots/mouse.png" height="100" align="left"/>)

## how to install
first download the appimage file from the [releases](https://github.com/muezabdalla/IMS/releases)

then make it executable with this command:

```
chmod +x IMS*.AppImage
```

then run it:

```
./IMS*.AppImage
```

## building from source

### dependancies:

- SDL2-devel
- SDL2_image-devel
- SDL2_ttf-devel
- libinput-devel
- gcc (only for compilation)
- make (only for compilation)

installing dependancies on void linux:

```
sudo xbps-install SDL2-devel SDL2_image-devel SDL2_ttf-devel libinput-devel make gcc
```

installing dependancies on debian-based linux:

```
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libinput-dev make gcc
```

after installing the dependancies, download the repository and `cd` into it and run this command to build from source:

```
make install
```

if you want to remove the package run:

```
make clean
```

to run it:

```
./IMS
```

## common issues:

you can change the images of the buttons if you put an them in `~/.config/IMS/` and you should name them as fellow:
- button.png (the image of the button unpressed)
- buttonP.png (the image of the button pressed)
- Meta.png (the logo of the meta button)

## common issues:

- if when you use `cat /dev/input/event0` you get authentication failed or something like that. then you have two options. first: run the program as root (not recommended) second: add yourself to the input group using this command:

```
usermod -a -G input user
```

## similar projects:
- [key-mon](https://github.com/scottkirkwood/key-mon)
- [KmCaster](https://github.com/johantiden/KmCaster)
