// g++ IMS.cpp -o IMS.cpp.o -lSDL2 -lSDL2_image -lSDL2_ttf && ./IMS.cpp.o
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <filesystem> // to check if the input file exist
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
//#include <string>
#include <thread>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

char KEYBOARD_FILE[2][22] =	{"/dev/input/event","/dev/input/event"};
char MOUSE_FILE[2][22] =	{"/dev/input/event","/dev/input/event"};

int BUTTON_WIDTH =	100;
int BUTTON_HIEGHT =	90;
int MOUSE_WIDTH =	90;
int MOUSE_HIEGHT =	90;
int REFRESH_TIME =	50;

bool SHOW_CTRL =		true;
bool SHOW_SHIFT =		true;
bool SHOW_SUPER =		true;
bool SHOW_ALT =			true;
bool SHOW_LETTERS =		true;
bool SHOW_BORDERS = 	true;
bool SHOW_KEYBOARD =	true;
bool SHOW_MOUSE =		true;
bool TRANSPARENT_MODE =	false;
bool CONFIG_GUI =		true;
bool POS_RIGHT =		false; // when used the "-p right <something>"
bool POS_BUTTOM =		false; // when used the "-p <something> buttom"
bool NEED_REFRESH =		false; // to know if a refresh is needed
void RenderCustom(SDL_Texture * textureT, const SDL_Rect * dstRectT)
{
	SDL_RenderCopy(renderer, textureT,  NULL, dstRectT);
	NEED_REFRESH = true;
}
// to render the button and then the letter
void RenderLetters(SDL_Texture * textureT, const SDL_Rect * dstRectT, SDL_Texture * textureT2, const SDL_Rect * dstRectT2)
{
	SDL_RenderCopy(renderer, textureT,  NULL, dstRectT);
	SDL_RenderCopy(renderer, textureT2,  NULL, dstRectT2);
	NEED_REFRESH = true;
}

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

string home_dir = getenv("HOME"); // get the home directory in linux
string current_path;
TTF_Font* font;
SDL_Color font_color;
// the offsets for the characters inside the buttons
int horizontal_offset = 15;
int vertical_offset = 15;

// blank surface to hide the defects
SDL_Rect rect_blank;
SDL_Surface* sur_blank = NULL;
SDL_Texture* tex_blank = NULL;
// the backgraound color
#define BG_COLOR_R 110
#define BG_COLOR_G 188
#define BG_COLOR_B 197
#define BG_COLOR_A 255


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

// the offseted rectangles (inside the original) (used for the text)
SDL_Rect rect_offset_ctrl;
SDL_Rect rect_offset_shift;
SDL_Rect rect_offset_super;
SDL_Rect rect_offset_alt;
SDL_Rect rect_offset_letters;
SDL_Rect rect_offset_mouse;
// spetial rectangles for the letters that have factors
SDL_Rect rect_offset_backspace;
SDL_Rect rect_offset_enter;
SDL_Rect rect_offset_space;
SDL_Rect rect_offset_tab;

SDL_Texture * tex_checked = NULL;
SDL_Texture * tex_notchecked = NULL;
SDL_Texture * tex_btn_normal = NULL;
SDL_Texture * tex_btn_hoover = NULL;
SDL_Texture * tex_btn_holded = NULL;
// initializing outside the if statments to be able to use them
SDL_Texture* tex_ctrl =		NULL;
SDL_Texture* tex_shift =	NULL;
SDL_Texture* tex_super =	NULL;
SDL_Texture* tex_alt =		NULL;
SDL_Texture* tex_general =	NULL;
SDL_Texture* tex_generalP =	NULL;
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
		cerr << "error running the command:" << command << endl;
		return -1;
	}
	fread(&output, sizeof output, 1, fpipe);
	if (fread == NULL)
	{
		cerr << "error storing the output of the command:" << command << endl;
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
		rect_offset_letters.x = rect_tab.x + horizontal_offset;
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
			if (check_mouse_clicked() == 1) RenderCustom(tex_mouse, &rect_mouse);
		}
		else RenderCustom(tex_mouse, &rect_mouse);
		*/
		if ( (TRANSPARENT_MODE && check_mouse_clicked()) || (!TRANSPARENT_MODE) )
			RenderCustom(tex_mouse, &rect_mouse);

		if (PRESSED_MOUSE[0]) RenderCustom(tex_mouse_leftP, &rect_mouse);
		if (PRESSED_MOUSE[1]) RenderCustom(tex_mouse_rightP, &rect_mouse);
		if (PRESSED_MOUSE[2]) RenderCustom(tex_mouse_wheelP, &rect_mouse);
	}
	// updating the blank rectangle to use it
	rect_blank = {rect_letters.x, 0, int(BUTTON_WIDTH*factor), window_hieght};
}

void imageToTexture(string image_path_temp, SDL_Texture* &tex_temp)
{
	if (!filesystem::is_regular_file(image_path_temp))
	{
		cerr << "file\"" << image_path_temp << "\" does not exist" << endl;
		exit(1);
	}
	// making the surfaces from the images
	SDL_Surface* sur_temp =	IMG_Load(image_path_temp.c_str());
	// making the texture from the surfaces
	tex_temp = SDL_CreateTextureFromSurface(renderer, sur_temp);
	// we do not need the surfaces from now so we freed them
	SDL_FreeSurface(sur_temp);
}

void wordToTexture(const char* word, SDL_Texture* &tex_temp)
{
	// Render Latin1 text at fast quality to a new 8-bit surface.
	//SDL_Surface* sur_text = TTF_RenderText_Solid(font, "Text", font_color);
	// Render Latin1 text at high quality to a new ARGB surface.
	SDL_Surface* sur_text_temp = TTF_RenderText_Blended(font, word, font_color);

	tex_temp = SDL_CreateTextureFromSurface(renderer, sur_text_temp);

	SDL_FreeSurface(sur_text_temp);
}

void print_help() 
{
	// use the current_path to make run even from a differant directory
	if (filesystem::is_regular_file(current_path+"IMS.1"))
		system(("man -c "+current_path+"IMS.1").c_str());
	else
		system("man IMS");
	exit(0);
}
// to find if a file (image or font) is in ~/config/IMS/ or /usr/shar/IMS/ or ./resources/
string find_file(string file_name)
{
	if (filesystem::is_regular_file(home_dir+"/.config/IMS/"+file_name))
		return home_dir+"/.config/IMS/"+file_name;
	else if (filesystem::is_regular_file(current_path+"resources/"+file_name))
		return current_path+"resources/"+file_name;
	else 
		return "/usr/share/IMS/"+file_name;
}

SDL_Event sdl_input; // the input from SDL2 (the input when the application is in focus)
struct input_event global_keyboard[2]; // the global input from the keyboard input file
struct input_event global_mouse[2]; // the global input from the mouse input file
bool close_program = false; // if set to true the loop exit (to close the app)

int keyboard_input[2];
int mouse_input[2];

void keyboard_loop(const int fn)
{
	while (!close_program)
	{
		ssize_t keyboard_bytesRead = read(keyboard_input[fn], &global_keyboard[fn], sizeof(global_keyboard[fn]));
		if (keyboard_bytesRead == (ssize_t)-1) {
			cerr << "Failed to read from " << KEYBOARD_FILE[fn] << ". but it was opened successfuly" << endl;
			exit(1);
		}

		// checking if the type is pressed or released
		if (global_keyboard[fn].type == EV_KEY)
		{

			if (global_keyboard[fn].value == 0)
			{
				switch (global_keyboard[fn].code)
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
							else RenderLetters(tex_general, &rect_ctrl, tex_ctrl, &rect_offset_ctrl);
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
							else RenderLetters(tex_general, &rect_shift, tex_shift, &rect_offset_shift);
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
							else RenderLetters(tex_general, &rect_super, tex_super, &rect_offset_super);
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
							else RenderLetters(tex_general, &rect_alt, tex_alt, &rect_offset_alt);
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
							else RenderCustom(tex_general, &rect_letters);
							// returning the size of the window for the large keys
							if (global_keyboard[fn].code == KEY_BACKSPACE || 
								global_keyboard[fn].code == KEY_ENTER || 
								global_keyboard[fn].code == KEY_SPACE || 
								global_keyboard[fn].code == KEY_TAB)
								update_window_width(1);
						}
				} // switch (global_keyboard[fn].code)
			} // if (global_keyboard[fn].value == 0)
			if (global_keyboard[fn].value == 1)
			{
				switch (global_keyboard[fn].code)
				{
					case KEY_RIGHTCTRL:
					case KEY_LEFTCTRL:
						if (SHOW_CTRL)
						{
							if (TRANSPARENT_MODE)
							{
								rect_ctrl.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								rect_offset_ctrl.x = rect_ctrl.x + horizontal_offset;
								PRESSED_BUTTONS[0] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							RenderLetters(tex_generalP, &rect_ctrl, tex_ctrl, &rect_offset_ctrl);
						}
						break;
					case KEY_RIGHTSHIFT:
					case KEY_LEFTSHIFT:
						if (SHOW_SHIFT)
						{
							if (TRANSPARENT_MODE)
							{
								rect_shift.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								rect_offset_shift.x = rect_shift.x + horizontal_offset;
								PRESSED_BUTTONS[1] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							RenderLetters(tex_generalP, &rect_shift, tex_shift, &rect_offset_shift);
						}
						break;
					case KEY_LEFTMETA:
						if (SHOW_SUPER)
						{
							if (TRANSPARENT_MODE)
							{
								rect_super.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								rect_offset_super.x = rect_super.x + horizontal_offset;
								PRESSED_BUTTONS[2] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							RenderLetters(tex_generalP, &rect_super, tex_super, &rect_offset_super);
						}
						break;
					case KEY_RIGHTALT:
					case KEY_LEFTALT:
						if (SHOW_ALT)
						{
							if (TRANSPARENT_MODE)
							{
								rect_alt.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								rect_offset_alt.x = rect_alt.x + horizontal_offset;
								PRESSED_BUTTONS[3] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							RenderLetters(tex_generalP, &rect_alt, tex_alt, &rect_offset_alt);
						}
						break;
					// the boring part of checking all the letters
					default: if (SHOW_LETTERS)
						{
							if (TRANSPARENT_MODE)
							{
								rect_letters.x = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								rect_offset_letters.x = rect_letters.x + horizontal_offset;
								PRESSED_BUTTONS[4] = true;
								window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
								SDL_SetWindowSize(window, window_width, window_hieght);
							}
							RenderCustom(tex_generalP, &rect_letters);
							switch (global_keyboard[fn].code)
							{
								case KEY_A:				RenderCustom(tex_A,			&rect_offset_letters); break;
								case KEY_B:				RenderCustom(tex_B,			&rect_offset_letters); break;
								case KEY_C:				RenderCustom(tex_C,			&rect_offset_letters); break;
								case KEY_D:				RenderCustom(tex_D,			&rect_offset_letters); break;
								case KEY_E:				RenderCustom(tex_E,			&rect_offset_letters); break;
								case KEY_F:				RenderCustom(tex_F,			&rect_offset_letters); break;
								case KEY_G:				RenderCustom(tex_G,			&rect_offset_letters); break;
								case KEY_H:				RenderCustom(tex_H,			&rect_offset_letters); break;
								case KEY_I:				RenderCustom(tex_I,			&rect_offset_letters); break;
								case KEY_J:				RenderCustom(tex_J,			&rect_offset_letters); break;
								case KEY_K:				RenderCustom(tex_K,			&rect_offset_letters); break;
								case KEY_L:				RenderCustom(tex_L,			&rect_offset_letters); break;
								case KEY_M:				RenderCustom(tex_M,			&rect_offset_letters); break;
								case KEY_N:				RenderCustom(tex_N,			&rect_offset_letters); break;
								case KEY_O:				RenderCustom(tex_O,			&rect_offset_letters); break;
								case KEY_P:				RenderCustom(tex_P,			&rect_offset_letters); break;
								case KEY_Q:				RenderCustom(tex_Q,			&rect_offset_letters); break;
								case KEY_R:				RenderCustom(tex_R,			&rect_offset_letters); break;
								case KEY_S:				RenderCustom(tex_S,			&rect_offset_letters); break;
								case KEY_T:				RenderCustom(tex_T,			&rect_offset_letters); break;
								case KEY_U:				RenderCustom(tex_U,			&rect_offset_letters); break;
								case KEY_V:				RenderCustom(tex_V,			&rect_offset_letters); break;
								case KEY_W:				RenderCustom(tex_W,			&rect_offset_letters); break;
								case KEY_X:				RenderCustom(tex_X,			&rect_offset_letters); break;
								case KEY_Y:				RenderCustom(tex_Y,			&rect_offset_letters); break;
								case KEY_Z:				RenderCustom(tex_Z,			&rect_offset_letters); break;
								case KEY_0:				RenderCustom(tex_0,			&rect_offset_letters); break;
								case KEY_1:				RenderCustom(tex_1,			&rect_offset_letters); break;
								case KEY_2:				RenderCustom(tex_2,			&rect_offset_letters); break;
								case KEY_3:				RenderCustom(tex_3,			&rect_offset_letters); break;
								case KEY_4:				RenderCustom(tex_4,			&rect_offset_letters); break;
								case KEY_5:				RenderCustom(tex_5,			&rect_offset_letters); break;
								case KEY_6:				RenderCustom(tex_6,			&rect_offset_letters); break;
								case KEY_7:				RenderCustom(tex_7,			&rect_offset_letters); break;
								case KEY_8:				RenderCustom(tex_8,			&rect_offset_letters); break;
								case KEY_9:				RenderCustom(tex_9,			&rect_offset_letters); break;
								case KEY_APOSTROPHE:	RenderCustom(tex_APOSTROPHE,&rect_offset_letters); break;
								case KEY_BACKSLASH:		RenderCustom(tex_BACKSLASH,	&rect_offset_letters); break;
								case KEY_BACKSPACE:
														update_window_width(FAC_BACKSPACE);
														RenderCustom(tex_blank, &rect_blank);
														RenderLetters(tex_generalP,	&rect_backspace, tex_BACKSPACE, &rect_offset_backspace); break;

								case KEY_COMMA:			RenderCustom(tex_COMMA, &rect_letters); break;
								case KEY_DELETE:		RenderCustom(tex_DELETE, &rect_letters); break;
								case KEY_DOT:			RenderCustom(tex_DOT, &rect_letters); break;
								case KEY_ENTER:			
														update_window_width(FAC_ENTER);
														RenderCustom(tex_blank, &rect_blank);
														RenderLetters(tex_generalP,	&rect_enter, tex_ENTER,	&rect_offset_enter); break;
								case KEY_EQUAL:			RenderCustom(tex_EQUAL, &rect_offset_letters); break;
								case KEY_ESC:			RenderCustom(tex_ESC,		&rect_offset_letters); break;
								case KEY_GRAVE:			RenderCustom(tex_GRAVE,		&rect_offset_letters); break;
								case KEY_LEFTBRACE:		RenderCustom(tex_LEFTBRACE,	&rect_offset_letters); break;
								case KEY_MINUS:			RenderCustom(tex_MINUS,		&rect_offset_letters); break;
								case KEY_RIGHTBRACE:	RenderCustom(tex_RIGHTBRACE,&rect_offset_letters); break;
								case KEY_SEMICOLON:		RenderCustom(tex_SEMICOLON,	&rect_offset_letters); break;
								case KEY_SLASH:			RenderCustom(tex_SLASH,		&rect_offset_letters); break;
								case KEY_SPACE:			
														update_window_width(FAC_SPACE);
														RenderCustom(tex_blank, &rect_blank);
														RenderLetters(tex_generalP,	&rect_space, tex_SPACE,	&rect_offset_space); break;
								case KEY_TAB:			
														update_window_width(FAC_TAB);
														RenderCustom(tex_blank, &rect_blank);
														RenderLetters(tex_generalP,	&rect_tab, tex_TAB,	&rect_offset_tab); break;
			
								default: cout << "code:" << global_keyboard[fn].code << endl;
							} // switch (global_keyboard[fn].code)
						} // default: if (SHOW_LETTERS)
				} // switch (global_keyboard[fn].code)
			}
		}
		check_show_window();
	}
}

void mouse_loop(const int fn)
{
	while (!close_program)
	{
		ssize_t mouse_bytesRead = read(mouse_input[fn], &global_mouse[fn], sizeof(global_mouse[fn]));
		if (mouse_bytesRead == (ssize_t)-1) {
			cerr << "Failed to read from " << MOUSE_FILE[fn] << ". but it was opened successfuly" << endl;
			exit(1);
		}

		// checkin if the type is pressed or released
		if (global_mouse[fn].type == EV_KEY)
		{
			//checking if any of the three buttons release(right,left,wheel)
			if (global_mouse[fn].value == 0 && global_mouse[fn].code > 271 && global_mouse[fn].code < 275)
			{
				if (TRANSPARENT_MODE)
				{
					if (global_mouse[fn].code == 272) PRESSED_MOUSE[0] = false;
					if (global_mouse[fn].code == 273) PRESSED_MOUSE[1] = false;
					if (global_mouse[fn].code == 274) PRESSED_MOUSE[2] = false;

					window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
					update_window_hieght();
					SDL_SetWindowSize(window, window_width, window_hieght);
				}
				if (check_mouse_clicked() == 0) RenderCustom(tex_mouse, &rect_mouse);
			}
			if (global_mouse[fn].value == 1)
			{
				switch (global_mouse[fn].code)
				{
					case 272: // left click
						if (TRANSPARENT_MODE)
						{
							PRESSED_MOUSE[0] = true;
							rect_mouse.x = ntrues_keyboard() * BUTTON_WIDTH;
							window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
							update_window_hieght();
							SDL_SetWindowSize(window, window_width, window_hieght);
							if (ntrues_mouse() <= 1) RenderCustom(tex_mouse, &rect_mouse);
						}
						RenderCustom(tex_mouse_leftP, &rect_mouse);
						break;
					case 273: // right click
						if (TRANSPARENT_MODE)
						{
							PRESSED_MOUSE[1] = true;
							rect_mouse.x = ntrues_keyboard() * BUTTON_WIDTH;
							window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
							update_window_hieght();
							SDL_SetWindowSize(window, window_width, window_hieght);
							if (ntrues_mouse() <= 1) RenderCustom(tex_mouse, &rect_mouse);
						}
						RenderCustom(tex_mouse_rightP, &rect_mouse);
						break;
					case 274: // middle wheel
						if (TRANSPARENT_MODE)
						{
							PRESSED_MOUSE[2] = true;
							rect_mouse.x = ntrues_keyboard() * BUTTON_WIDTH;
							window_width = ntrues_keyboard() * BUTTON_WIDTH + check_mouse_clicked() * MOUSE_WIDTH;
							update_window_hieght();
							SDL_SetWindowSize(window, window_width, window_hieght);
							if (ntrues_mouse() <= 1) RenderCustom(tex_mouse, &rect_mouse);
						}
						RenderCustom(tex_mouse_wheelP, &rect_mouse);
						break;
				}
			}
		}
		// checking if the type is = 2 (relative)
		if (global_mouse[fn].type == EV_REL && global_mouse[fn].code == 11)
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
			if (global_mouse[fn].value == 120)
				RenderCustom(tex_mouse_wheelup, &rect_mouse);
			else // scrolled down
				RenderCustom(tex_mouse_wheeldown, &rect_mouse);


			// to make the arrow show for a small period of time we should sleep then show the normal mouse image
			// other wise the arrows will stay until a button is released
			SDL_Delay(200);
			RenderCustom(tex_mouse, &rect_mouse);
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

	SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland");
	if ( SDL_Init( SDL_INIT_EVENTS ) < 0 ) {
		cerr << "Error initializing SDL: " << SDL_GetError() << endl;
		return 1;
	} 

	// detecting the event files for keyboard and mouse
	char keyboard_number =	get_command_output("ls /dev/input/by-path/ -la | grep -o \"event-kbd -> ../event[0-9]*\" | grep -o \"[0-9]*\" | head -1");
	KEYBOARD_FILE[0][16] =		keyboard_number;
	char keyboard_number2 =	get_command_output("ls /dev/input/by-path/ -la | grep -o \"event-kbd -> ../event[0-9]*\" | grep -o \"[0-9]*\" | tail -1");
	KEYBOARD_FILE[1][16] =		keyboard_number2;
	char mouse_number =		get_command_output("ls /dev/input/by-path/ -la | grep -o \"event-mouse -> ../event[0-9]*\" | grep -o \"[0-9]*\" | head -1");
	MOUSE_FILE[0][16] =		mouse_number;
	char mouse_number2 =		get_command_output("ls /dev/input/by-path/ -la | grep -o \"event-mouse -> ../event[0-9]*\" | grep -o \"[0-9]*\" | tail -1");
	MOUSE_FILE[1][16] =		mouse_number2;

	bool use_second_keyboard_file = false, use_second_mouse_file = false;
	if ( keyboard_number != keyboard_number2 )
		use_second_keyboard_file = true;
	if ( mouse_number != mouse_number2 )
		use_second_mouse_file = true;

	current_path = argv[0];
	// remove last seven chars(IMS) by subtracting totall length - 3
	while (current_path.back() != '/')
		current_path.pop_back();
	//current_path.erase(current_path.length() - 3);

	TTF_Init();

	font = TTF_OpenFont(find_file("font.ttf").c_str(), 52);
	font_color = {255, 255, 255};

	// variables for handing arguments
	int i;
	string arg_next;
	string arg_next2;

	int X=0,Y=0; // position variables
	#include "arg.h"

	if (CONFIG_GUI)
	{
		#include "config_gui.h"
	}
	if (close_program)
	{
		#include "quit.h" // clear things and exit
	}

	if (SHOW_CTRL) 
	{
		rect_ctrl =		{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 
		rect_offset_ctrl = {rect_ctrl.x+horizontal_offset, rect_ctrl.y+vertical_offset, rect_ctrl.w-2*horizontal_offset, rect_ctrl.h-2*vertical_offset};    
		nButtons++;
	}
	if (SHOW_SHIFT)
	{
		rect_shift =	{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 
		rect_offset_shift = {rect_shift.x+horizontal_offset, rect_shift.y+vertical_offset, rect_shift.w-2*horizontal_offset, rect_shift.h-2*vertical_offset};    
		nButtons++;
	}
	if (SHOW_SUPER)
	{
		rect_super =	{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 
		rect_offset_super = {rect_super.x+horizontal_offset, rect_super.y+vertical_offset, rect_super.w-2*horizontal_offset, rect_super.h-2*vertical_offset};    
		nButtons++;
	}
	if (SHOW_ALT)
	{
		rect_alt =		{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 
		rect_offset_alt = {rect_alt.x+horizontal_offset, rect_alt.y+vertical_offset, rect_alt.w-2*horizontal_offset, rect_alt.h-2*vertical_offset};    
		nButtons++;
	}
	if (SHOW_LETTERS)
	{
		rect_letters =		{nButtons*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HIEGHT};
		rect_backspace =	{nButtons*BUTTON_WIDTH, 0, int(BUTTON_WIDTH*FAC_BACKSPACE), BUTTON_HIEGHT};
		rect_enter =		{nButtons*BUTTON_WIDTH, 0, int(BUTTON_WIDTH*FAC_ENTER), BUTTON_HIEGHT};
		rect_space =		{nButtons*BUTTON_WIDTH, 0, int(BUTTON_WIDTH*FAC_SPACE), BUTTON_HIEGHT};
		rect_tab =			{nButtons*BUTTON_WIDTH, 0, int(BUTTON_WIDTH*FAC_TAB), BUTTON_HIEGHT};
		rect_offset_letters = {rect_letters.x+horizontal_offset, rect_letters.y+vertical_offset, rect_letters.w-2*horizontal_offset, rect_letters.h-2*vertical_offset};    
		rect_offset_backspace = {rect_backspace.x+horizontal_offset, rect_backspace.y+vertical_offset, rect_backspace.w-2*horizontal_offset, rect_backspace.h-2*vertical_offset};    
		rect_offset_enter = {rect_enter.x+horizontal_offset, rect_enter.y+vertical_offset, rect_enter.w-2*horizontal_offset, rect_enter.h-2*vertical_offset};    
		rect_offset_space = {rect_space.x+horizontal_offset, rect_space.y+vertical_offset, rect_space.w-2*horizontal_offset, rect_space.h-2*vertical_offset};    
		rect_offset_tab = {rect_tab.x+horizontal_offset, rect_tab.y+vertical_offset, rect_tab.w-2*horizontal_offset, rect_tab.h-2*vertical_offset};    
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

	renderer = SDL_CreateRenderer(window, 3, 0);
	// initializing the blank texture
	sur_blank = SDL_GetWindowSurface(window);
	SDL_FillRect(sur_blank, NULL, SDL_MapRGB(sur_blank->format, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B));
	tex_blank = SDL_CreateTextureFromSurface(renderer, sur_blank);
	// display the background color
	SDL_SetRenderDrawColor(renderer, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A);
	SDL_RenderClear(renderer);

	if (SHOW_KEYBOARD)
	{
		imageToTexture(find_file("button.png"), tex_general);
		imageToTexture(find_file("buttonP.png"), tex_generalP);
	}
	if (SHOW_CTRL) 
	{
		// making the surfaces from the images
		wordToTexture("ctrl", tex_ctrl);
		// render the normal button at the beginning
		RenderLetters(tex_general,  &rect_ctrl, tex_ctrl, &rect_offset_ctrl);
	}
	if (SHOW_SHIFT) 
	{
		// making the surfaces from the images
		wordToTexture("shift", tex_shift);
		// render the normal button at the beginning
		RenderLetters(tex_general,  &rect_shift, tex_shift,  &rect_offset_shift);
	}
	if (SHOW_SUPER) 
	{
		// making the surfaces from the images
		imageToTexture(find_file("Meta.png"), tex_super);
		// render the normal button at the beginning
		RenderLetters(tex_general,  &rect_super, tex_super,  &rect_offset_super);
	}
	if (SHOW_ALT) 
	{
		// making the surfaces from the images
		wordToTexture("alt", tex_alt);
		// render the normal button at the beginning
		RenderLetters(tex_general,  &rect_alt, tex_alt,  &rect_offset_alt);
	}
	if (SHOW_LETTERS) 
	{
		// render the normal button at the beginning
		SDL_RenderCopy(renderer, tex_general,  NULL, &rect_letters);
	}
	if (SHOW_MOUSE)
	{
		// making the surfaces from the images
		imageToTexture(find_file("mouse.png"), tex_mouse);
		imageToTexture(find_file("mouse_rightP.png"), tex_mouse_rightP);
		imageToTexture(find_file("mouse_leftP.png"), tex_mouse_leftP);
		imageToTexture(find_file("mouse_wheelP.png"), tex_mouse_wheelP);
		imageToTexture(find_file("mouse_wheelup.png"), tex_mouse_wheelup);
		imageToTexture(find_file("mouse_wheeldown.png"), tex_mouse_wheeldown);
		// render the normal button at the beginning
		SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
	}

	SDL_RenderPresent(renderer);

	if (SHOW_LETTERS)
	{
		// the boring part of initializing all the letters
		wordToTexture("A", tex_A);
		wordToTexture("B", tex_B);
		wordToTexture("C", tex_C);
		wordToTexture("D", tex_D);
		wordToTexture("E", tex_E);
		wordToTexture("F", tex_F);
		wordToTexture("G", tex_G);
		wordToTexture("H", tex_H);
		wordToTexture("I", tex_I);
		wordToTexture("J", tex_J);
		wordToTexture("K", tex_K);
		wordToTexture("L", tex_L);
		wordToTexture("M", tex_M);
		wordToTexture("N", tex_N);
		wordToTexture("O", tex_O);
		wordToTexture("P", tex_P);
		wordToTexture("Q", tex_Q);
		wordToTexture("R", tex_R);
		wordToTexture("S", tex_S);
		wordToTexture("T", tex_T);
		wordToTexture("U", tex_U);
		wordToTexture("V", tex_V);
		wordToTexture("W", tex_W);
		wordToTexture("X", tex_X);
		wordToTexture("Y", tex_Y);
		wordToTexture("Z", tex_Z);
		wordToTexture("0", tex_0);
		wordToTexture("1", tex_1);
		wordToTexture("2", tex_2);
		wordToTexture("3", tex_3);
		wordToTexture("4", tex_4);
		wordToTexture("5", tex_5);
		wordToTexture("6", tex_6);
		wordToTexture("7", tex_7);
		wordToTexture("8", tex_8);
		wordToTexture("9", tex_9);
		wordToTexture("'", tex_APOSTROPHE);
		wordToTexture("\\", tex_BACKSLASH);
		wordToTexture("backspace", tex_BACKSPACE);
		wordToTexture(",", tex_COMMA);
		wordToTexture("delete", tex_DELETE);
		wordToTexture(".", tex_DOT);
		wordToTexture("enter", tex_ENTER);
		wordToTexture("=", tex_EQUAL);
		wordToTexture("esc", tex_ESC);
		wordToTexture("`", tex_GRAVE);
		wordToTexture("[", tex_LEFTBRACE);
		wordToTexture("-", tex_MINUS);
		wordToTexture("]", tex_RIGHTBRACE);
		wordToTexture(";", tex_SEMICOLON);
		wordToTexture("/", tex_SLASH);
		wordToTexture("space", tex_SPACE);
		wordToTexture("tab", tex_TAB);
	}

	if (keyboard_input[0] == -1) {
		cerr << "Cannot open " << KEYBOARD_FILE[0] << endl;
		return 1;
	}
	if (use_second_keyboard_file)
	{
		if (keyboard_input[1] == -1) {
			cerr << "Cannot open " << KEYBOARD_FILE[1] << endl;
			return 1;
		}
	}
	if (mouse_input[0] == -1) {
		cerr << "Cannot open " << MOUSE_FILE[0] << endl;
		return 1;
	}
	if (use_second_mouse_file)
	{
		if (mouse_input[1] == -1) {
			cerr << "Cannot open " << MOUSE_FILE[1] << endl;
			return 1;
		}
	}

	keyboard_input[0] = open(KEYBOARD_FILE[0], O_RDONLY);
	if (use_second_keyboard_file)
		keyboard_input[1] = open(KEYBOARD_FILE[1], O_RDONLY);
	if (SHOW_MOUSE)
	{
		mouse_input[0] = open(MOUSE_FILE[0], O_RDONLY);
		if (use_second_mouse_file)
			mouse_input[1] = open(MOUSE_FILE[1], O_RDONLY);
	}

	thread keyboard_thread;
	thread keyboard_thread2;
	thread mouse_thread;
	thread mouse_thread2;
	if (SHOW_KEYBOARD)
	{
		keyboard_thread = thread(&keyboard_loop,0);
		if (use_second_keyboard_file)
			keyboard_thread2 = thread(&keyboard_loop,1);
	}
	if (SHOW_MOUSE)
	{
		mouse_thread = thread(&mouse_loop,0);
		if (use_second_mouse_file)
			mouse_thread2 = thread(&mouse_loop,1);
	}
	while (!close_program)
	{
		SDL_Delay(REFRESH_TIME);
		if (NEED_REFRESH)
		{
			SDL_RenderPresent(renderer);
			NEED_REFRESH = false;
		}
		while ( SDL_PollEvent( &sdl_input ) != 0 ) 
		{
			// checking if you want to close the app
			if (sdl_input.type == SDL_QUIT) close_program =true;
			if (sdl_input.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				SDL_SetRenderDrawColor(renderer, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A);
				SDL_RenderClear(renderer);
				if (SHOW_CTRL)
				{
					if (PRESSED_BUTTONS[0])
						RenderLetters(tex_generalP,  &rect_ctrl, tex_ctrl, &rect_offset_ctrl);
					else
						RenderLetters(tex_general,  &rect_ctrl, tex_ctrl, &rect_offset_ctrl);
				}
				if (SHOW_SHIFT)
				{
					if (PRESSED_BUTTONS[1])
						RenderLetters(tex_generalP,  &rect_shift, tex_shift, &rect_offset_shift);
					else
						RenderLetters(tex_general,  &rect_shift, tex_shift, &rect_offset_shift);
				}
				if (SHOW_SUPER)
				{
					if (PRESSED_BUTTONS[2])
						RenderLetters(tex_generalP,  &rect_super, tex_super, &rect_offset_super);
					else
						RenderLetters(tex_general,  &rect_super, tex_super, &rect_offset_super);
				}
				if (SHOW_ALT)
				{
					if (PRESSED_BUTTONS[3])
						RenderLetters(tex_generalP,  &rect_alt, tex_alt, &rect_offset_alt);
					else
						RenderLetters(tex_general,  &rect_alt, tex_alt, &rect_offset_alt);
				}
				if (SHOW_LETTERS)
				{
					if (PRESSED_BUTTONS[3])
						SDL_RenderCopy(renderer, tex_generalP,  NULL, &rect_letters);
					else
						SDL_RenderCopy(renderer, tex_general,  NULL, &rect_letters);
				}
				if (SHOW_MOUSE)
				{
						SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
						if (PRESSED_MOUSE[0])
							RenderCustom(tex_mouse_leftP, &rect_mouse);
						if (PRESSED_MOUSE[1])
							RenderCustom(tex_mouse_rightP, &rect_mouse);
						if (PRESSED_MOUSE[2])
							RenderCustom(tex_mouse_wheelP, &rect_mouse);
				}

			}
		}
	}
	#include "quit.h" // clear things and exit
}
