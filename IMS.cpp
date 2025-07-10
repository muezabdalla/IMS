// g++ key-sun.cpp -o key-sun.cpp.o -lSDL2 -lSDL2_image && ./key-sun.cpp.o
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <filesystem> // to check if the input file exist
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//#include <string>
#include <thread>

char KEYBOARD_FILE[22] =	"/dev/input/event";
char MOUSE_FILE[22] =		"/dev/input/event";

int BUTTON_WIDTH =	100;
int BUTTON_HIEGHT =	60;
int MOUSE_WIDTH =	90;
int MOUSE_HIEGHT =	90;
int REFRESH_TIME =	100;

bool SHOW_CTRL =		true;
bool SHOW_SHIFT =		true;
bool SHOW_SUPER =		true;
bool SHOW_ALT =			true;
bool SHOW_LETTERS =		true;
bool SHOW_BORDERS = 	true;
bool SHOW_KEYBOARD =	true;
bool SHOW_MOUSE =		true;
bool TRANSPARENT_MODE =	false;
bool POS_RIGHT =		false; // when used the "-p right <something>"
bool POS_BUTTOM =		false; // when used the "-p <something> buttom"

// factors to multibly the width of the button to increase it
float FAC_BACKSPACE = 1.4;
float FAC_ENTER = 1.5;
float FAC_SPACE = 2;
float FAC_TAB = 1.2;

// a boolean for each button if it is pressed and the mouse
bool PRESSED_BUTTONS[5] = {false, false, false, false, false};
bool PRESSED_MOUSE[4] = {false, false, false, false}; // the scroll up/down are a single boolean

int window_hieght, window_width; // window sizes
short int nButtons = 0; // this shows the number of buttons including the general button

using namespace std;

string current_path;
string image_absolute_path;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// blank surface to hide the defects
SDL_Rect rect_blank;
SDL_Surface* sur_blank = NULL;
SDL_Texture* tex_blank = NULL;


// the x,y positons and width and hieght on the window
SDL_Rect rect_ctrl;
SDL_Rect rect_shift;
SDL_Rect rect_super;
SDL_Rect rect_alt;
SDL_Rect rect_letters;
SDL_Rect rect_mouse;
// spetial rectangles for the letters that have factors
SDL_Rect rect_backspace;
SDL_Rect rect_enter;
SDL_Rect rect_space;
SDL_Rect rect_tab;

// initializing outside the if statments to be able to use them
SDL_Texture* tex_ctrl =		NULL;
SDL_Texture* tex_ctrlP =	NULL;
SDL_Texture* tex_shift =	NULL;
SDL_Texture* tex_shiftP =	NULL;
SDL_Texture* tex_super =	NULL;
SDL_Texture* tex_superP =	NULL;
SDL_Texture* tex_alt =		NULL;
SDL_Texture* tex_altP =		NULL;
SDL_Texture* tex_general =	NULL;
SDL_Texture* tex_mouse =			NULL;
SDL_Texture* tex_mouse_rightP =		NULL;
SDL_Texture* tex_mouse_leftP =		NULL;
SDL_Texture* tex_mouse_wheelP =		NULL;
SDL_Texture* tex_mouse_wheelup =	NULL;
SDL_Texture* tex_mouse_wheeldown =	NULL;

// the boring part of initializing all the letters
SDL_Texture* tex_A =		NULL;
SDL_Texture* tex_B =		NULL;
SDL_Texture* tex_C =		NULL;
SDL_Texture* tex_D =		NULL;
SDL_Texture* tex_E =		NULL;
SDL_Texture* tex_F =		NULL;
SDL_Texture* tex_G =		NULL;
SDL_Texture* tex_H =		NULL;
SDL_Texture* tex_I =		NULL;
SDL_Texture* tex_J =		NULL;
SDL_Texture* tex_K =		NULL;
SDL_Texture* tex_L =		NULL;
SDL_Texture* tex_M =		NULL;
SDL_Texture* tex_N =		NULL;
SDL_Texture* tex_O =		NULL;
SDL_Texture* tex_P =		NULL;
SDL_Texture* tex_Q =		NULL;
SDL_Texture* tex_R =		NULL;
SDL_Texture* tex_S =		NULL;
SDL_Texture* tex_T =		NULL;
SDL_Texture* tex_U =		NULL;
SDL_Texture* tex_V =		NULL;
SDL_Texture* tex_W =		NULL;
SDL_Texture* tex_X =		NULL;
SDL_Texture* tex_Y =		NULL;
SDL_Texture* tex_Z =		NULL;
SDL_Texture* tex_0 =		NULL;
SDL_Texture* tex_1 =		NULL;
SDL_Texture* tex_2 =		NULL;
SDL_Texture* tex_3 =		NULL;
SDL_Texture* tex_4 =		NULL;
SDL_Texture* tex_5 =		NULL;
SDL_Texture* tex_6 =		NULL;
SDL_Texture* tex_7 =		NULL;
SDL_Texture* tex_8 =		NULL;
SDL_Texture* tex_9 =		NULL;
SDL_Texture* tex_APOSTROPHE=NULL;
SDL_Texture* tex_BACKSLASH =NULL;
SDL_Texture* tex_BACKSPACE =NULL;
SDL_Texture* tex_COMMA =	NULL;
SDL_Texture* tex_DELETE =	NULL;
SDL_Texture* tex_DOT =		NULL;
SDL_Texture* tex_ENTER =	NULL;
SDL_Texture* tex_EQUAL =	NULL;
SDL_Texture* tex_ESC =		NULL;
SDL_Texture* tex_GRAVE =	NULL;
SDL_Texture* tex_LEFTBRACE =NULL;
SDL_Texture* tex_MINUS =	NULL;
SDL_Texture* tex_RIGHTBRACE=NULL;
SDL_Texture* tex_SEMICOLON =NULL;
SDL_Texture* tex_SLASH =	NULL;
SDL_Texture* tex_SPACE =	NULL;
SDL_Texture* tex_TAB =		NULL;

char get_command_output(const char * command)
{
	char output;
	FILE *fpipe;
	fpipe = popen(command, "r");
	if (fpipe == NULL)
	{
		cout << "error running the command:" << command << endl;
		return -1;
	}
	fread(&output, sizeof output, 1, fpipe);
	if (fread == NULL)
	{
		cout << "error storing the output of the command:" << command << endl;
		return -1;
	}
	pclose(fpipe);
	return output;
}

int ntrues_keyboard()
{
	int count = 0;
	if (PRESSED_BUTTONS[0] && SHOW_CTRL)	count++;
	if (PRESSED_BUTTONS[1] && SHOW_SHIFT)	count++;
	if (PRESSED_BUTTONS[2] && SHOW_SUPER)	count++;
	if (PRESSED_BUTTONS[3] && SHOW_ALT)		count++;
	if (PRESSED_BUTTONS[4] && SHOW_LETTERS)	count++;
	return count;
}

int ntrues_mouse()
{
	int count = 0;
	for (int i=0;i<4;i++)
		if (PRESSED_MOUSE[i]) count++;
	return count;
}
int check_mouse_clicked()
{
	if ((PRESSED_MOUSE[0] || PRESSED_MOUSE[1] || PRESSED_MOUSE[2] || PRESSED_MOUSE[3]) && SHOW_MOUSE) return 1;
	else return 0;
}

void check_show_window()
{
	if (TRANSPARENT_MODE)
	{
		if (ntrues_keyboard() == 0 && check_mouse_clicked() == 0) SDL_HideWindow(window);
		else SDL_ShowWindow(window);
	}
}

void update_window_hieght()
{
	if (check_mouse_clicked() == 1) window_hieght = MOUSE_HIEGHT > BUTTON_HIEGHT? MOUSE_HIEGHT : BUTTON_HIEGHT;
	else window_hieght = BUTTON_HIEGHT;
}

// update the window width and the mouse x position using the factor
void update_window_width(float factor)
{
	// update window width
	if (TRANSPARENT_MODE)
	{
		window_width = ntrues_keyboard() * BUTTON_WIDTH;
		if (SHOW_MOUSE) window_width += check_mouse_clicked() * MOUSE_WIDTH;
		if (PRESSED_BUTTONS[4]) window_width = window_width - BUTTON_WIDTH + int(factor*BUTTON_WIDTH);
		rect_tab.x = rect_space.x = rect_enter.x = rect_backspace.x = window_width - int(factor*BUTTON_WIDTH);
	}
	else
	{
		window_width = (nButtons-1) * BUTTON_WIDTH + int(BUTTON_WIDTH*factor);
		if (SHOW_MOUSE) window_width += MOUSE_WIDTH;
	}
	SDL_SetWindowSize(window, window_width, window_hieght);

	// changing the mouse position
	if (SHOW_MOUSE)
	{
		rect_mouse.x = window_width - MOUSE_WIDTH;
		/*
		if (TRANSPARENT_MODE)
		{
			if (check_mouse_clicked() == 1) SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
		}
		else SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
		*/
		if ( (TRANSPARENT_MODE && check_mouse_clicked()) || (!TRANSPARENT_MODE) )
			SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);

		if (PRESSED_MOUSE[0]) SDL_RenderCopy(renderer, tex_mouse_leftP,  NULL, &rect_mouse);
		if (PRESSED_MOUSE[1]) SDL_RenderCopy(renderer, tex_mouse_rightP,  NULL, &rect_mouse);
		if (PRESSED_MOUSE[2]) SDL_RenderCopy(renderer, tex_mouse_wheelP,  NULL, &rect_mouse);
	}
	// updating the blank rectangle to use it
	rect_blank = {rect_letters.x, 0, int(BUTTON_WIDTH*factor), window_hieght};
}

void imageToTexture(string image_path, SDL_Texture* &tex_temp, SDL_Renderer* renderer_temp)
{
	image_absolute_path = current_path + image_path;
	if (!std::filesystem::is_regular_file(image_absolute_path))
	{
		cout << "file\"" << image_absolute_path << "\" does not exist" << endl;
		exit(0);
	}
	// this is to convert a string to a const char * to be able to use it in IMG_Load
	const char * image_path_converted = image_absolute_path.c_str();
	// making the surfaces from the images
	SDL_Surface* sur_temp =	IMG_Load(image_path_converted);
	// making the texture from the surfaces
	tex_temp = SDL_CreateTextureFromSurface(renderer_temp, sur_temp);
	// we do not need the surfaces from now so we freed them
	SDL_FreeSurface(sur_temp);
}

void print_help() 
{
	// use the current_path to make run even from a differant directory
	system(("man -c "+current_path+"IMS-sun.1").c_str());
	exit(0);
}

SDL_Event sdl_input; // the input from SDL2 (the input when the application is in focus)
struct input_event global_keyboard; // the global input from the keyboard input file
struct input_event global_mouse; // the global input from the mouse input file
bool close_program = false; // if set to true the loop exit (to close the app)

int keyboard_input;
int mouse_input;

void keyboard_loop()
{
	while (!close_program)
	{
		ssize_t keyboard_bytesRead = read(keyboard_input, &global_keyboard, sizeof(global_keyboard));
		if (keyboard_bytesRead == (ssize_t)-1) {
			cout << "Failed to read from " << KEYBOARD_FILE << ". but it was opened successfuly" << endl;
			exit(1);
		}

		// checking if the type is pressed or released
		if (global_keyboard.type == EV_KEY)
		{

			if (global_keyboard.value == 0)
			{
				switch (global_keyboard.code)
				{
					case KEY_RIGHTCTRL:
					case KEY_LEFTCTRL:
						if (SHOW_CTRL)
						{
							if (TRANSPARENT_MODE)
							{
								PRESSED_BUTTONS[0] = false;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							else SDL_RenderCopy(renderer, tex_ctrl,  NULL, &rect_ctrl);
						}
						break;
					case KEY_RIGHTSHIFT:
					case KEY_LEFTSHIFT:
						if (SHOW_SHIFT)
						{
							if (TRANSPARENT_MODE)
							{
								PRESSED_BUTTONS[1] = false;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							else SDL_RenderCopy(renderer, tex_shift,  NULL, &rect_shift);
						}
						break;
					case KEY_LEFTMETA:
						if (SHOW_SUPER)
						{
							if (TRANSPARENT_MODE)
							{
								PRESSED_BUTTONS[2] = false;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							else SDL_RenderCopy(renderer, tex_super,  NULL, &rect_super);
						}
						break;
					case KEY_RIGHTALT:
					case KEY_LEFTALT:
						if (SHOW_ALT)
						{
							if (TRANSPARENT_MODE)
							{
								PRESSED_BUTTONS[3] = false;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							else SDL_RenderCopy(renderer, tex_alt,  NULL, &rect_alt);
						}
						break;
					default:
						if (SHOW_LETTERS)
						{
							if(TRANSPARENT_MODE)
							{
								PRESSED_BUTTONS[4] = false;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							else SDL_RenderCopy(renderer, tex_general,  NULL, &rect_letters);
							// returning the size of the window for the large keys
							if (global_keyboard.code == KEY_BACKSPACE || 
								global_keyboard.code == KEY_ENTER || 
								global_keyboard.code == KEY_SPACE || 
								global_keyboard.code == KEY_TAB)
								update_window_width(1);
						}
				}
			}
			if (global_keyboard.value == 1)
			{
				switch (global_keyboard.code)
				{
					case KEY_RIGHTCTRL:
					case KEY_LEFTCTRL:
						if (SHOW_CTRL)
						{
							if (TRANSPARENT_MODE)
							{
								rect_ctrl.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								PRESSED_BUTTONS[0] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							SDL_RenderCopy(renderer, tex_ctrlP,  NULL, &rect_ctrl);
						}
						break;
					case KEY_RIGHTSHIFT:
					case KEY_LEFTSHIFT:
						if (SHOW_SHIFT)
						{
							if (TRANSPARENT_MODE)
							{
								rect_shift.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								PRESSED_BUTTONS[1] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							SDL_RenderCopy(renderer, tex_shiftP,  NULL, &rect_shift);
						}
						break;
					case KEY_LEFTMETA:
						if (SHOW_SUPER)
						{
							if (TRANSPARENT_MODE)
							{
								rect_super.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								PRESSED_BUTTONS[2] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							SDL_RenderCopy(renderer, tex_superP,  NULL, &rect_super);
						}
						break;
					case KEY_RIGHTALT:
					case KEY_LEFTALT:
						if (SHOW_ALT)
						{
							if (TRANSPARENT_MODE)
							{
								rect_alt.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								PRESSED_BUTTONS[3] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							SDL_RenderCopy(renderer, tex_altP,  NULL, &rect_alt);
						}
						break;
					// the boring part of checking all the letters
					default: if (SHOW_LETTERS)
						{
							if (TRANSPARENT_MODE)
							{
								rect_letters.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								PRESSED_BUTTONS[4] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							switch (global_keyboard.code)
							{
								case KEY_A:				SDL_RenderCopy(renderer, tex_A,			NULL, &rect_letters); break;
								case KEY_B:				SDL_RenderCopy(renderer, tex_B,			NULL, &rect_letters); break;
								case KEY_C:				SDL_RenderCopy(renderer, tex_C,			NULL, &rect_letters); break;
								case KEY_D:				SDL_RenderCopy(renderer, tex_D,			NULL, &rect_letters); break;
								case KEY_E:				SDL_RenderCopy(renderer, tex_E,			NULL, &rect_letters); break;
								case KEY_F:				SDL_RenderCopy(renderer, tex_F,			NULL, &rect_letters); break;
								case KEY_G:				SDL_RenderCopy(renderer, tex_G,			NULL, &rect_letters); break;
								case KEY_H:				SDL_RenderCopy(renderer, tex_H,			NULL, &rect_letters); break;
								case KEY_I:				SDL_RenderCopy(renderer, tex_I,			NULL, &rect_letters); break;
								case KEY_J:				SDL_RenderCopy(renderer, tex_J,			NULL, &rect_letters); break;
								case KEY_K:				SDL_RenderCopy(renderer, tex_K,			NULL, &rect_letters); break;
								case KEY_L:				SDL_RenderCopy(renderer, tex_L,			NULL, &rect_letters); break;
								case KEY_M:				SDL_RenderCopy(renderer, tex_M,			NULL, &rect_letters); break;
								case KEY_N:				SDL_RenderCopy(renderer, tex_N,			NULL, &rect_letters); break;
								case KEY_O:				SDL_RenderCopy(renderer, tex_O,			NULL, &rect_letters); break;
								case KEY_P:				SDL_RenderCopy(renderer, tex_P,			NULL, &rect_letters); break;
								case KEY_Q:				SDL_RenderCopy(renderer, tex_Q,			NULL, &rect_letters); break;
								case KEY_R:				SDL_RenderCopy(renderer, tex_R,			NULL, &rect_letters); break;
								case KEY_S:				SDL_RenderCopy(renderer, tex_S,			NULL, &rect_letters); break;
								case KEY_T:				SDL_RenderCopy(renderer, tex_T,			NULL, &rect_letters); break;
								case KEY_U:				SDL_RenderCopy(renderer, tex_U,			NULL, &rect_letters); break;
								case KEY_V:				SDL_RenderCopy(renderer, tex_V,			NULL, &rect_letters); break;
								case KEY_W:				SDL_RenderCopy(renderer, tex_W,			NULL, &rect_letters); break;
								case KEY_X:				SDL_RenderCopy(renderer, tex_X,			NULL, &rect_letters); break;
								case KEY_Y:				SDL_RenderCopy(renderer, tex_Y,			NULL, &rect_letters); break;
								case KEY_Z:				SDL_RenderCopy(renderer, tex_Z,			NULL, &rect_letters); break;
								case KEY_0:				SDL_RenderCopy(renderer, tex_0,			NULL, &rect_letters); break;
								case KEY_1:				SDL_RenderCopy(renderer, tex_1,			NULL, &rect_letters); break;
								case KEY_2:				SDL_RenderCopy(renderer, tex_2,			NULL, &rect_letters); break;
								case KEY_3:				SDL_RenderCopy(renderer, tex_3,			NULL, &rect_letters); break;
								case KEY_4:				SDL_RenderCopy(renderer, tex_4,			NULL, &rect_letters); break;
								case KEY_5:				SDL_RenderCopy(renderer, tex_5,			NULL, &rect_letters); break;
								case KEY_6:				SDL_RenderCopy(renderer, tex_6,			NULL, &rect_letters); break;
								case KEY_7:				SDL_RenderCopy(renderer, tex_7,			NULL, &rect_letters); break;
								case KEY_8:				SDL_RenderCopy(renderer, tex_8,			NULL, &rect_letters); break;
								case KEY_9:				SDL_RenderCopy(renderer, tex_9,			NULL, &rect_letters); break;
								case KEY_APOSTROPHE:	SDL_RenderCopy(renderer, tex_APOSTROPHE,NULL, &rect_letters); break;
								case KEY_BACKSLASH:		SDL_RenderCopy(renderer, tex_BACKSLASH,	NULL, &rect_letters); break;
								case KEY_BACKSPACE:
														update_window_width(FAC_BACKSPACE);
														SDL_RenderCopy(renderer, tex_blank,		NULL, &rect_blank);
														SDL_RenderCopy(renderer, tex_BACKSPACE,	NULL, &rect_backspace); break;

								case KEY_COMMA:			SDL_RenderCopy(renderer, tex_COMMA,		NULL, &rect_letters); break;
								case KEY_DELETE:		SDL_RenderCopy(renderer, tex_DELETE,	NULL, &rect_letters); break;
								case KEY_DOT:			SDL_RenderCopy(renderer, tex_DOT,		NULL, &rect_letters); break;
								case KEY_ENTER:			
														update_window_width(FAC_ENTER);
														SDL_RenderCopy(renderer, tex_blank,		NULL, &rect_blank);
														SDL_RenderCopy(renderer, tex_ENTER,		NULL, &rect_enter); break;
								case KEY_EQUAL:			SDL_RenderCopy(renderer, tex_EQUAL,		NULL, &rect_letters); break;
								case KEY_ESC:			SDL_RenderCopy(renderer, tex_ESC,		NULL, &rect_letters); break;
								case KEY_GRAVE:			SDL_RenderCopy(renderer, tex_GRAVE,		NULL, &rect_letters); break;
								case KEY_LEFTBRACE:		SDL_RenderCopy(renderer, tex_LEFTBRACE,	NULL, &rect_letters); break;
								case KEY_MINUS:			SDL_RenderCopy(renderer, tex_MINUS,		NULL, &rect_letters); break;
								case KEY_RIGHTBRACE:	SDL_RenderCopy(renderer, tex_RIGHTBRACE,NULL, &rect_letters); break;
								case KEY_SEMICOLON:		SDL_RenderCopy(renderer, tex_SEMICOLON,	NULL, &rect_letters); break;
								case KEY_SLASH:			SDL_RenderCopy(renderer, tex_SLASH,		NULL, &rect_letters); break;
								case KEY_SPACE:			
														update_window_width(FAC_SPACE);
														SDL_RenderCopy(renderer, tex_blank,		NULL, &rect_blank);
														SDL_RenderCopy(renderer, tex_SPACE,		NULL, &rect_space); break;
								case KEY_TAB:			
														update_window_width(FAC_TAB);
														SDL_RenderCopy(renderer, tex_blank,		NULL, &rect_blank);
														SDL_RenderCopy(renderer, tex_TAB,		NULL, &rect_tab); break;
			
								default: cout << "code:" << global_keyboard.code << endl;
							}
						}
				}
			}
		}
		check_show_window();
	}
}

void mouse_loop()
{
	while (!close_program)
	{
		ssize_t mouse_bytesRead = read(mouse_input, &global_mouse, sizeof(global_mouse));
		if (mouse_bytesRead == (ssize_t)-1) {
			cout << "Failed to read from " << MOUSE_FILE << ". but it was opened successfuly" << endl;
			exit(1);
		}

		// checkin if the type is pressed or released
		if (global_mouse.type == EV_KEY)
		{
			//checking if any of the three buttons release(right,left,wheel)
			if (global_mouse.value == 0 && global_mouse.code > 271 && global_mouse.code < 275)
			{
				if (TRANSPARENT_MODE)
				{
					if (global_mouse.code == 272) PRESSED_MOUSE[0] = false;
					if (global_mouse.code == 273) PRESSED_MOUSE[1] = false;
					if (global_mouse.code == 274) PRESSED_MOUSE[2] = false;

					window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
					update_window_hieght();
					SDL_SetWindowSize(window, window_width, window_hieght);
				}
				if (check_mouse_clicked() == 0) SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
			}
			if (global_mouse.value == 1)
			{
				switch (global_mouse.code)
				{
					case 272: // left click
						if (TRANSPARENT_MODE)
						{
							PRESSED_MOUSE[0] = true;
							rect_mouse.x = ntrues_keyboard() * BUTTON_WIDTH;
							window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
							update_window_hieght();
							SDL_SetWindowSize(window, window_width, window_hieght);
							if (ntrues_mouse() <= 1) SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
						}
						SDL_RenderCopy(renderer, tex_mouse_leftP,  NULL, &rect_mouse);
						break;
					case 273: // right click
						if (TRANSPARENT_MODE)
						{
							PRESSED_MOUSE[1] = true;
							rect_mouse.x = ntrues_keyboard() * BUTTON_WIDTH;
							window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
							update_window_hieght();
							SDL_SetWindowSize(window, window_width, window_hieght);
							if (ntrues_mouse() <= 1) SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
						}
						SDL_RenderCopy(renderer, tex_mouse_rightP,  NULL, &rect_mouse);
						break;
					case 274: // middle wheel
						if (TRANSPARENT_MODE)
						{
							PRESSED_MOUSE[2] = true;
							rect_mouse.x = ntrues_keyboard() * BUTTON_WIDTH;
							window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
							update_window_hieght();
							SDL_SetWindowSize(window, window_width, window_hieght);
							if (ntrues_mouse() <= 1) SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
						}
						SDL_RenderCopy(renderer, tex_mouse_wheelP,  NULL, &rect_mouse);
						break;
				}
			}
		}
		// checking if the type is = 2 (relative)
		if (global_mouse.type == EV_REL && global_mouse.code == 11)
		{
			if (TRANSPARENT_MODE)
			{
				PRESSED_MOUSE[3] = true;
				rect_mouse.x = ntrues_keyboard() * BUTTON_WIDTH;
				window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
				SDL_SetWindowSize(window, window_width, window_hieght);
				SDL_ShowWindow(window);
			}
			// checking if scrolled up
			if (global_mouse.value == 120)
				SDL_RenderCopy(renderer, tex_mouse_wheelup,  NULL, &rect_mouse);
			else // scrolled down
				SDL_RenderCopy(renderer, tex_mouse_wheeldown,  NULL, &rect_mouse);

			// to make the arrow show for a small period of time we should sleep then show the normal mouse image
			// other wise the arrows will stay until a button is released
			SDL_Delay(200);
			SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
			PRESSED_MOUSE[3] = false;
		}
		if (TRANSPARENT_MODE)
		{
			if (ntrues_keyboard() == 0 && check_mouse_clicked() == 0) SDL_HideWindow(window);
			else SDL_ShowWindow(window);
		}
	}
}

int main(int argc, char* argv[]) {

	// detecting the event files for keyboard and mouse
	char keyboard_number =	get_command_output("ls /dev/input/by-path/ -la | grep -o \"event-kbd -> ../event[0-9]*\" | grep -o \"[0-9]*\" | head -1");
	char mouse_number =		get_command_output("ls /dev/input/by-path/ -la | grep -o \"event-mouse -> ../event[0-9]*\" | grep -o \"[0-9]*\" | head -1");
	KEYBOARD_FILE[16] =		keyboard_number;
	MOUSE_FILE[16] =		mouse_number;

	current_path = argv[0];
	// remove last seven chars(IMS) by subtracting totall length - 3
	current_path.erase(current_path.length() - 3);

	// variables for handing arguments
	int i;
	string arg_next;
	string arg_next2;

	int X=0,Y=0; // position variables
	#include "arg.h"
	if (SHOW_CTRL) 
	{
		rect_ctrl =		{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 
		nButtons++;
	}
	if (SHOW_SHIFT)
	{
		rect_shift =	{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 
		nButtons++;
	}
	if (SHOW_SUPER)
	{
		rect_super =	{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 
		nButtons++;
	}
	if (SHOW_ALT)
	{
		rect_alt =		{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 
		nButtons++;
	}
	if (SHOW_LETTERS)
	{
		rect_letters =		{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT};
		rect_backspace =	{nButtons*BUTTON_WIDTH, 0, int(BUTTON_WIDTH*FAC_BACKSPACE), BUTTON_HIEGHT};
		rect_enter =		{nButtons*BUTTON_WIDTH, 0, int(BUTTON_WIDTH*FAC_ENTER), BUTTON_HIEGHT};
		rect_space =		{nButtons*BUTTON_WIDTH, 0, int(BUTTON_WIDTH*FAC_SPACE), BUTTON_HIEGHT};
		rect_tab =			{nButtons*BUTTON_WIDTH, 0, int(BUTTON_WIDTH*FAC_TAB), BUTTON_HIEGHT};
		nButtons++;
	}
	if (SHOW_MOUSE)
	{
		rect_mouse =	{nButtons*BUTTON_WIDTH, 0, MOUSE_WIDTH, MOUSE_HIEGHT}; 
		window_hieght = BUTTON_HIEGHT > MOUSE_HIEGHT ? BUTTON_HIEGHT : MOUSE_HIEGHT;
		window_width = BUTTON_WIDTH*nButtons + MOUSE_WIDTH;
	}
	else
	{
		window_hieght = BUTTON_HIEGHT;
		window_width = BUTTON_WIDTH*nButtons;
	}

	if (POS_RIGHT)
		X = SCREEN_W-window_width;
	if (POS_BUTTOM)
		Y = SCREEN_H-window_hieght;

	if (!TRANSPARENT_MODE)
	{
		if (SHOW_BORDERS)
			window = SDL_CreateWindow("IMS", X, Y, window_width, window_hieght, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY | SDL_WINDOW_ALWAYS_ON_TOP);
		else
			window = SDL_CreateWindow("IMS", X, Y, window_width, window_hieght, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_UTILITY | SDL_WINDOW_ALWAYS_ON_TOP);
	}
	else // transparent mode on then make the window as TOOLTIP to make it not focusable
	{
		window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
		update_window_hieght();
		window = SDL_CreateWindow("IMS", X, Y, window_width, window_hieght, SDL_WINDOW_TOOLTIP | SDL_WINDOW_ALWAYS_ON_TOP);
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	// initializing the blank texture
	sur_blank = SDL_GetWindowSurface(window);
	SDL_FillRect(sur_blank, NULL, SDL_MapRGB(sur_blank->format, 0, 0, 0));
	tex_blank = SDL_CreateTextureFromSurface(renderer, sur_blank);

	if (SHOW_CTRL) 
	{
		// making the surfaces from the images
		imageToTexture("resources/ctrl.png", tex_ctrl, renderer);
		imageToTexture("resources/ctrlP.png", tex_ctrlP, renderer); // P is short for pressed
		// render the normal button at the beginning
		SDL_RenderCopy(renderer, tex_ctrl,  NULL, &rect_ctrl);
	}
	if (SHOW_SHIFT) 
	{
		// making the surfaces from the images
		imageToTexture("resources/shift.png", tex_shift, renderer);
		imageToTexture("resources/shiftP.png", tex_shiftP, renderer); // P is short for pressed
		// render the normal button at the beginning
		SDL_RenderCopy(renderer, tex_shift,  NULL, &rect_shift);
	}
	if (SHOW_SUPER) 
	{
		// making the surfaces from the images
		imageToTexture("resources/super.png", tex_super, renderer);
		imageToTexture("resources/superP.png", tex_superP, renderer); // P is short for pressed
		// render the normal button at the beginning
		SDL_RenderCopy(renderer, tex_super,  NULL, &rect_super);
	}
	if (SHOW_ALT) 
	{
		// making the surfaces from the images
		imageToTexture("resources/alt.png", tex_alt, renderer);
		imageToTexture("resources/altP.png", tex_altP, renderer); // P is short for pressed
		// render the normal button at the beginning
		SDL_RenderCopy(renderer, tex_alt,  NULL, &rect_alt);
	}
	if (SHOW_LETTERS) 
	{
		// making the surfaces from the images
		imageToTexture("resources/general_button.png", tex_general, renderer);
		// render the normal button at the beginning
		SDL_RenderCopy(renderer, tex_general,  NULL, &rect_letters);
	}
	if (SHOW_MOUSE)
	{
		// making the surfaces from the images
		imageToTexture("resources/mouse.png", tex_mouse, renderer);
		imageToTexture("resources/mouse_rightP.png", tex_mouse_rightP, renderer);
		imageToTexture("resources/mouse_leftP.png", tex_mouse_leftP, renderer);
		imageToTexture("resources/mouse_wheelP.png", tex_mouse_wheelP, renderer);
		imageToTexture("resources/mouse_wheelup.png", tex_mouse_wheelup, renderer);
		imageToTexture("resources/mouse_wheeldown.png", tex_mouse_wheeldown, renderer);
		// render the normal button at the beginning
		SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
	}

	SDL_RenderPresent(renderer);

	if (SHOW_LETTERS)
	{
		// the boring part of initializing all the letters
		imageToTexture("resources/letters/A.png", tex_A, renderer);
		imageToTexture("resources/letters/B.png", tex_B, renderer);
		imageToTexture("resources/letters/C.png", tex_C, renderer);
		imageToTexture("resources/letters/D.png", tex_D, renderer);
		imageToTexture("resources/letters/E.png", tex_E, renderer);
		imageToTexture("resources/letters/F.png", tex_F, renderer);
		imageToTexture("resources/letters/G.png", tex_G, renderer);
		imageToTexture("resources/letters/H.png", tex_H, renderer);
		imageToTexture("resources/letters/I.png", tex_I, renderer);
		imageToTexture("resources/letters/J.png", tex_J, renderer);
		imageToTexture("resources/letters/K.png", tex_K, renderer);
		imageToTexture("resources/letters/L.png", tex_L, renderer);
		imageToTexture("resources/letters/M.png", tex_M, renderer);
		imageToTexture("resources/letters/N.png", tex_N, renderer);
		imageToTexture("resources/letters/O.png", tex_O, renderer);
		imageToTexture("resources/letters/P.png", tex_P, renderer);
		imageToTexture("resources/letters/Q.png", tex_Q, renderer);
		imageToTexture("resources/letters/R.png", tex_R, renderer);
		imageToTexture("resources/letters/S.png", tex_S, renderer);
		imageToTexture("resources/letters/T.png", tex_T, renderer);
		imageToTexture("resources/letters/U.png", tex_U, renderer);
		imageToTexture("resources/letters/V.png", tex_V, renderer);
		imageToTexture("resources/letters/W.png", tex_W, renderer);
		imageToTexture("resources/letters/X.png", tex_X, renderer);
		imageToTexture("resources/letters/Y.png", tex_Y, renderer);
		imageToTexture("resources/letters/Z.png", tex_Z, renderer);
		imageToTexture("resources/letters/0.png", tex_0, renderer);
		imageToTexture("resources/letters/1.png", tex_1, renderer);
		imageToTexture("resources/letters/2.png", tex_2, renderer);
		imageToTexture("resources/letters/3.png", tex_3, renderer);
		imageToTexture("resources/letters/4.png", tex_4, renderer);
		imageToTexture("resources/letters/5.png", tex_5, renderer);
		imageToTexture("resources/letters/6.png", tex_6, renderer);
		imageToTexture("resources/letters/7.png", tex_7, renderer);
		imageToTexture("resources/letters/8.png", tex_8, renderer);
		imageToTexture("resources/letters/9.png", tex_9, renderer);
		imageToTexture("resources/letters/APOSTROPHE.png", tex_APOSTROPHE, renderer);
		imageToTexture("resources/letters/BACKSLASH.png", tex_BACKSLASH, renderer);
		imageToTexture("resources/letters/BACKSPACE.png", tex_BACKSPACE, renderer);
		imageToTexture("resources/letters/COMMA.png", tex_COMMA, renderer);
		imageToTexture("resources/letters/DELETE.png", tex_DELETE, renderer);
		imageToTexture("resources/letters/DOT.png", tex_DOT, renderer);
		imageToTexture("resources/letters/ENTER.png", tex_ENTER, renderer);
		imageToTexture("resources/letters/EQUAL.png", tex_EQUAL, renderer);
		imageToTexture("resources/letters/ESC.png", tex_ESC, renderer);
		imageToTexture("resources/letters/GRAVE.png", tex_GRAVE, renderer);
		imageToTexture("resources/letters/LEFTBRACE.png", tex_LEFTBRACE, renderer);
		imageToTexture("resources/letters/MINUS.png", tex_MINUS, renderer);
		imageToTexture("resources/letters/RIGHTBRACE.png", tex_RIGHTBRACE, renderer);
		imageToTexture("resources/letters/SEMICOLON.png", tex_SEMICOLON, renderer);
		imageToTexture("resources/letters/SLASH.png", tex_SLASH, renderer);
		imageToTexture("resources/letters/SPACE.png", tex_SPACE, renderer);
		imageToTexture("resources/letters/TAB.png", tex_TAB, renderer);
	}

	if (keyboard_input == -1) {
		cout << "Cannot open " << KEYBOARD_FILE << endl;
		return 1;
	}
	if (mouse_input == -1) {
		cout << "Cannot open " << MOUSE_FILE << endl;
		return 1;
	}

	keyboard_input = open(KEYBOARD_FILE, O_RDONLY);
	if (SHOW_MOUSE)
		mouse_input = open(MOUSE_FILE, O_RDONLY);

	if (SHOW_MOUSE && SHOW_KEYBOARD) // both keyboard and mouse
	{
		thread keyboard_thread(keyboard_loop);
		thread mouse_thread(mouse_loop);
		while (!close_program)
		{
			SDL_Delay(REFRESH_TIME);
			SDL_RenderPresent(renderer);
			while ( SDL_PollEvent( &sdl_input ) != 0 ) 
			{
				// checking if you want to close the app
				if (sdl_input.type == SDL_QUIT) close_program =true;
			}
		}
		#include "quit.h" // clear things and exit
		mouse_thread.join();
		keyboard_thread.join();
	}
	else if (!SHOW_MOUSE && SHOW_KEYBOARD) // keyboard only
	{
		thread keyboard_thread(keyboard_loop);
		while (!close_program)
		{
			SDL_Delay(REFRESH_TIME);
			SDL_RenderPresent(renderer);
			while ( SDL_PollEvent( &sdl_input ) != 0 ) 
			{
				// checking if you want to close the app
				if (sdl_input.type == SDL_QUIT) close_program =true;
			}
		}
		keyboard_thread.join();
		#include "quit.h" // clear things and exit
	}
	else // mouse only
	{
		thread mouse_thread(mouse_loop);
		while (!close_program)
		{
			SDL_Delay(REFRESH_TIME);
			SDL_RenderPresent(renderer);
			while ( SDL_PollEvent( &sdl_input ) != 0 ) 
			{
				// checking if you want to close the app
				if (sdl_input.type == SDL_QUIT) close_program =true;
			}
		}
		mouse_thread.join();
		#include "quit.h" // clear things and exit
	}
}
