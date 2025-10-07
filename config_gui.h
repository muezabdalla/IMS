#define char_width 10
#define checkBox_size 20 // also used as the hieght of a letter
class checkBox
{
public:
	bool is_checked = true;
	const char* name;
	SDL_Texture* tex_name = NULL;
	SDL_Rect rect_checkBox = {0, 0, checkBox_size, checkBox_size};
	SDL_Rect rect_name = {0, 0, checkBox_size, checkBox_size};

	// to be able to define arrays of this class we need an empty constructor
	checkBox() {}
	checkBox(const char* name_temp, int Posx, int Posy, bool state, bool drawNow)
	{
		name = name_temp;
		font_color = {0, 0, 0};
		wordToTexture(name, tex_name);
		rect_checkBox.x = Posx;
		rect_checkBox.y = rect_name.y = Posy;
		rect_name.x = rect_checkBox.x + checkBox_size;
		rect_name.w = strlen(name) * char_width;
		is_checked = state;
		if (drawNow)
		{
			draw(true);
		}
	}
	~checkBox() {SDL_DestroyTexture(tex_name); tex_name = NULL;}
	void change_state()
	{
		is_checked = !is_checked;
		draw();
	}
	void draw(bool draw_name=false)
	{
		if (draw_name)
		{
			SDL_RenderCopy(renderer, tex_name,  NULL, &rect_name);
		}

		if (is_checked)
		{
			SDL_RenderCopy(renderer, tex_checked,  NULL, &rect_checkBox);
		}
		else
		{
			SDL_RenderCopy(renderer, tex_notchecked,  NULL, &rect_checkBox);
		}
		NEED_REFRESH = true;
	}
	void handle_input(SDL_Event * input_temp)
	{
		if (
			input_temp->type == SDL_MOUSEBUTTONUP && // check if a button is released
			input_temp->button.button == SDL_BUTTON_LEFT && // check if that button is left button
			input_temp->button.x > rect_checkBox.x &&
			input_temp->button.x < (rect_checkBox.x + rect_checkBox.w + rect_name.w) &&
			input_temp->button.y > rect_checkBox.y &&
			input_temp->button.y < (rect_checkBox.y + rect_checkBox.h)
			)
		{
			change_state();
		}
	}
};

#define clearanceX 10 // space between the text and button bouries on X-axis
#define clearanceY 6 // space between the text and button bouries on Y-axis
class button
{
public:
	// state represent the state of the button
	// 0: normal
	// 1: hoovered
	// 2: pressing (holding)
	// 3: pressed and released
	uint8_t state = 0;
	uint8_t old_state = 0;
	bool is_pressed = false;
	bool is_pressing = false;
	bool is_hoover = false;
	const char* name;
	SDL_Texture* tex_name = NULL;
	SDL_Rect rect_btn = {0, 0, checkBox_size, checkBox_size};
	SDL_Rect rect_name = {0, 0, checkBox_size, checkBox_size};

	// to be able to define arrays of this class we need an empty constructor
	button() {}
	button(const char* name_temp, int Posx, int Posy, bool drawNow)
	{
		name = name_temp;
		font_color = {0, 0, 0};
		wordToTexture(name, tex_name);

		rect_btn.x = Posx;
		rect_btn.y = Posy;
		rect_btn.h = checkBox_size + 2*clearanceY;
		rect_btn.w = strlen(name) * char_width + 2*clearanceX;
		rect_name.x = rect_btn.x + clearanceX;
		rect_name.y = rect_btn.y + clearanceY;
		rect_name.h = checkBox_size;
		rect_name.w = strlen(name) * char_width;

		if (drawNow)
			draw();
	}
	~button() {SDL_DestroyTexture(tex_name); tex_name = NULL;}
	void draw()
	{
		switch (state)
		{
			case 0: SDL_RenderCopy(renderer, tex_btn_normal,  NULL, &rect_btn); break;
			case 2: SDL_RenderCopy(renderer, tex_btn_holded,  NULL, &rect_btn); break;
			case 1:
			case 3: SDL_RenderCopy(renderer, tex_btn_hoover,  NULL, &rect_btn); break;
		}
		SDL_RenderCopy(renderer, tex_name,  NULL, &rect_name);
		NEED_REFRESH = true;
	}
	void handle_input(SDL_Event * input_temp)
	{
		old_state = state;
		if (
			input_temp->button.x > rect_btn.x &&
			input_temp->button.x < (rect_btn.x + rect_btn.w) &&
			input_temp->button.y > rect_btn.y &&
			input_temp->button.y < (rect_btn.y + rect_btn.h)
			)
		{
			switch (input_temp->type)
			{
				case SDL_MOUSEBUTTONDOWN: state = 2; break;
				case SDL_MOUSEBUTTONUP: state = 3; break;
				default: state = 1;
			}
		}
		else
			state = 0;

		if (old_state != state)
			draw();
	}
};

#define box_clearance 2
#define digits_limit 4
class int_input_box
{
public:
	bool in_focus = false;
	bool old_in_focus = false;
	bool need_redraw = false;
	bool left_mouse_button_hold = false;
	const char* name;
	string input_num = "0";
	SDL_Texture* tex_input_num = NULL;
	SDL_Texture* tex_name = NULL;
	SDL_Rect rect_input_box;
	SDL_Rect rect_input_box_frame;
	SDL_Rect rect_input_num;
	SDL_Rect rect_name;
	int cursor_pos = 0;
	int old_cursor_pos = 0;
	// the highlight is when selecting a part of the text/numbers
	int highlight_start = 0;
	int highlight_end = 0;
	int highlight_init = 0; // the initial position of the highlight
	SDL_Rect rect_highlight;


	// to be able to define arrays of this class we need an empty constructor
	int_input_box() {}
	int_input_box(const char* name_temp, int Posx, int Posy, bool drawNow, int def_value=0)
	{
		name = name_temp;
		input_num = to_string(def_value);
		font_color = {0, 0, 0};
		wordToTexture(name, tex_name);

		rect_name.x = Posx;
		rect_name.y = Posy;
		rect_name.h = checkBox_size;
		rect_name.w = strlen(name) * char_width;

		rect_input_box.x = rect_name.x + rect_name.w + box_clearance;
		rect_input_box.y = rect_name.y;
		rect_input_box.h = checkBox_size;
		rect_input_box.w = 6 * char_width;

		rect_input_box_frame.x = rect_input_box.x - box_clearance;
		rect_input_box_frame.y = rect_name.y - box_clearance;
		rect_input_box_frame.h = checkBox_size + box_clearance * 2;
		rect_input_box_frame.w = 6 * char_width + box_clearance * 2;

		rect_highlight = rect_input_box;
		rect_highlight.x = rect_input_box.x + highlight_start * char_width;
		rect_highlight.w = (highlight_end - highlight_start) * char_width;

		rect_input_num = rect_input_box;
		rect_input_num.w = input_num.size() * char_width;

		if (drawNow)
			draw(true);
	}
	~int_input_box()
	{
		SDL_DestroyTexture(tex_input_num);
		SDL_DestroyTexture(tex_name);
		tex_input_num = NULL;
		tex_name = NULL;
	}

	void draw(bool draw_name=false)
	{
		font_color = {0, 0, 0};
		wordToTexture(input_num.c_str(), tex_input_num);
		rect_input_num.w = input_num.size() * char_width;
		if (draw_name)
			SDL_RenderCopy(renderer, tex_name,  NULL, &rect_name);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rect_input_box_frame);
		if (in_focus)
		{
			SDL_SetRenderDrawColor(renderer, 110, 188, 197, 255);
		}
		else
			SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

		SDL_RenderDrawRect(renderer, &rect_input_box_frame);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rect_input_num);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		if (input_num.size() > 0 )
		{
			SDL_RenderCopy(renderer, tex_input_num,  NULL, &rect_input_num);
		}

		if (highlight_start != highlight_end) // there is a highlighted region
		{
			// first draw a blue rectangle
			SDL_SetRenderDrawColor(renderer, 48, 140, 198, 255); // highlight color
			rect_highlight.x = rect_input_box.x + highlight_start * char_width;
			rect_highlight.w = (highlight_end - highlight_start) * char_width;
			SDL_RenderFillRect(renderer, &rect_highlight);

			// second draw the number in white
			font_color = {255, 255, 255};

			wordToTexture(input_num.substr(highlight_start, (highlight_end-highlight_start)).c_str(), tex_input_num);
			SDL_RenderCopy(renderer, tex_input_num,  NULL, &rect_highlight);
		}

		if (in_focus)
		{
			SDL_RenderDrawLine(renderer, rect_input_box.x+cursor_pos*10, rect_input_box.y, rect_input_box.x+cursor_pos*10, rect_input_box.y+checkBox_size);
		}
		NEED_REFRESH = true;
		need_redraw = false;
	}
	void handle_input(SDL_Event * input_temp)
	{
		old_cursor_pos = cursor_pos;
		if (
			input_temp->type == SDL_MOUSEBUTTONUP && // check if a button is released
			input_temp->button.button == SDL_BUTTON_LEFT // check if that button is left button
			)
		{
			left_mouse_button_hold = false;
		}
		if (left_mouse_button_hold)
		{
			cursor_pos = (input_temp->button.x - rect_input_box.x) / char_width;

			if ((input_temp->button.x - rect_input_box.x) % char_width > char_width/2 )
				cursor_pos++;
			// make sure it does not exceed the limits
			if (cursor_pos > input_num.size())
				cursor_pos = input_num.size();
			if (cursor_pos < 0)
				cursor_pos = 0;

			if (cursor_pos > highlight_init)
				highlight_end = cursor_pos;
			else if (cursor_pos < highlight_init)
				highlight_start = cursor_pos;
			else
				highlight_start = highlight_end = cursor_pos;
		}
		if (
			input_temp->type == SDL_MOUSEBUTTONDOWN && // check if a button is pressed
			input_temp->button.button == SDL_BUTTON_LEFT // check if that button is left button
			)
		{
			old_in_focus = in_focus;
			if (
				input_temp->button.x > rect_input_box_frame.x &&
				input_temp->button.x < (rect_input_box_frame.x + rect_input_box_frame.w) &&
				input_temp->button.y > rect_input_box_frame.y &&
				input_temp->button.y < (rect_input_box_frame.y + rect_input_box_frame.h)
			   )
			{
				in_focus = true;
				left_mouse_button_hold = true;
				cursor_pos = (input_temp->button.x - rect_input_box.x) / char_width;

				// move one bit when the click is closer to that position
				if ((input_temp->button.x - rect_input_box.x) % char_width > char_width/2 )
					cursor_pos++;

				// handle input on a space
				cursor_pos = input_num.size() < cursor_pos ? input_num.size() : cursor_pos;

				// the highlight initial position
				highlight_init = cursor_pos;
			}
			else
			{
				in_focus = false;
			}

			highlight_end = highlight_start = cursor_pos;

			if (old_in_focus != in_focus)
				need_redraw = true;
			old_in_focus = in_focus;
		}
		if (in_focus && input_temp->type == SDL_KEYDOWN)
		{
			// remove when pressing backspace
			if ( input_temp->key.keysym.sym == SDLK_BACKSPACE)
			{
				if (highlight_start != highlight_end)
				{
					input_num.erase(highlight_start, (highlight_end - highlight_start) );
					cursor_pos = highlight_end = highlight_start;
					need_redraw = true;
				}
				else if (cursor_pos > 0)
				{
					input_num.erase(cursor_pos, 1);
					cursor_pos--;
					need_redraw = true;
				}
			}
			switch (input_temp->key.keysym.sym)
			{
				case SDLK_LEFT: cursor_pos--; break;
				case SDLK_RIGHT: cursor_pos++; break;
				case SDLK_DOWN:
				case SDLK_UP:
					int temp = stoi(input_num);

					if (input_temp->key.keysym.sym == SDLK_UP)
					{
						temp++;
						need_redraw = true;
					}
					else if (temp > 0)
					{
						temp--;
						need_redraw = true;
					}

					input_num = to_string(temp);

					highlight_start = 0;
					cursor_pos = highlight_end = input_num.size();
					break;
			}

			// add when pressing a number
			if ( (char)input_temp->key.keysym.sym >= '0' && (char)input_temp->key.keysym.sym <= '9' )
			{
				if (highlight_start != highlight_end)
				{
					input_num.erase(highlight_start, (highlight_end - highlight_start) );
					cursor_pos = highlight_end = highlight_start;
				}
				input_num.insert(cursor_pos, 1, (char)input_temp->key.keysym.sym);
				cursor_pos++;
				need_redraw = true;

				// remove the last digit if the number is over the digits_limit
				if (input_num.size() > digits_limit)
				{
					cursor_pos = cursor_pos > input_num.size() ? input_num.size() : cursor_pos;
					input_num.pop_back();
				}
			}
		}

		// if double clicked the input box highlight all the number
		if (
			input_temp->button.clicks >= 2 &&
			input_temp->button.button == SDL_BUTTON_LEFT &&
			input_temp->button.state == SDL_PRESSED &&
			input_temp->button.x > rect_input_box_frame.x &&
			input_temp->button.x < (rect_input_box_frame.x + rect_input_box_frame.w) &&
			input_temp->button.y > rect_input_box_frame.y &&
			input_temp->button.y < (rect_input_box_frame.y + rect_input_box_frame.h)
			)
		{
			highlight_start = 0;
			highlight_end = input_num.size();
			need_redraw = true;
		}

		if (old_cursor_pos != cursor_pos)
			need_redraw = true;
		old_cursor_pos = cursor_pos;

		if (need_redraw)
			draw();
	}
};

	window = SDL_CreateWindow("configuration panel of IMS", 100, 100, 600, 300, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
	renderer = SDL_CreateRenderer(window, 3, 0);


	SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
	SDL_RenderClear(renderer);

	imageToTexture(find_file("checkBox_checked.png"), tex_checked);
	imageToTexture(find_file("checkBox_notchecked.png"), tex_notchecked);

	imageToTexture(find_file("btn_normal.png"), tex_btn_normal);
	imageToTexture(find_file("btn_hoover.png"), tex_btn_hoover);
	imageToTexture(find_file("btn_holded.png"), tex_btn_holded);

	#define edge_cl 10 // the clearance between a button and the edge of the window
	#define col_dis 240 // the y distance to the 2nd coloum
	#define between 30 // the space between buttons on the y axis
	checkBox checkBox_arrays[8] {
		{"ctrl key", edge_cl, edge_cl, true, true},
		{"shift key", edge_cl, edge_cl+between, true, true},
		{"super key", edge_cl, edge_cl+between*2, true, true},
		{"alt key", edge_cl, edge_cl+between*3, true, true},
		{"general key", edge_cl, edge_cl+between*4, true, true},

		{"borders", col_dis, edge_cl, true, true},
		{"mouse", col_dis, edge_cl+between, true, true},
		{"trasparent mode", 240, edge_cl+between*2, false, true}
	};

	int_input_box int_input_box_array[] {
		// button size
		{"width of a key ", edge_cl, edge_cl+between*5, true, BUTTON_WIDTH},
		{"hieght of a key", edge_cl, edge_cl+between*6, true, BUTTON_HIEGHT},

		// mouse size
		{"width of the mouse        ", col_dis, edge_cl+between*3, true, MOUSE_WIDTH},
		{"hieght of the mouse       ", col_dis, edge_cl+between*4, true, MOUSE_HIEGHT},

		// window position
		{"position x                ", col_dis, edge_cl+between*5, true, X},
		{"position y                ", col_dis, edge_cl+between*6, true, Y},
	};

	button cancel ("cancel", 600-edge_cl-char_width*6-clearanceX*2, edge_cl+between*8, true);
	button start ("start", 600-edge_cl*2-char_width*11-clearanceX*4, edge_cl+between*8, true);

	bool close_config_gui = false;
	while (!close_config_gui && !close_program)
	{
		if (NEED_REFRESH)
		{
			SDL_RenderPresent(renderer);
			NEED_REFRESH = false;
		}

		usleep(150000);
		while ( SDL_PollEvent( &sdl_input ) != 0 ) 
		{
			for (int i=0;i<8;i++)
				checkBox_arrays[i].handle_input(&sdl_input);

			start.handle_input(&sdl_input);
			cancel.handle_input(&sdl_input);
			for (int i=0;i<6;i++)
				int_input_box_array[i].handle_input(&sdl_input);

			if (start.state == 3)
				close_config_gui = true;
			// checking if you want to close the app
			if (sdl_input.type == SDL_QUIT || cancel.state == 3) close_program =true;
			if (sdl_input.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
				SDL_RenderClear(renderer);
				for (int i=0;i<8;i++)
					checkBox_arrays[i].draw(true);
				for (int i=0;i<6;i++)
					int_input_box_array[i].draw(true);
				start.draw();
				cancel.draw();
			}
		}
	}

	SHOW_CTRL = checkBox_arrays[0].is_checked;
	SHOW_SHIFT = checkBox_arrays[1].is_checked;
	SHOW_SUPER = checkBox_arrays[2].is_checked;
	SHOW_ALT = checkBox_arrays[3].is_checked;
	SHOW_LETTERS = checkBox_arrays[4].is_checked;
	SHOW_BORDERS = checkBox_arrays[5].is_checked;
	SHOW_MOUSE = checkBox_arrays[6].is_checked;
	TRANSPARENT_MODE = checkBox_arrays[7].is_checked;

	BUTTON_WIDTH = stoi(int_input_box_array[0].input_num);
	BUTTON_HIEGHT = stoi(int_input_box_array[1].input_num);
	MOUSE_WIDTH = stoi(int_input_box_array[2].input_num);
	MOUSE_HIEGHT = stoi(int_input_box_array[3].input_num);
	X = stoi(int_input_box_array[4].input_num);
	Y = stoi(int_input_box_array[5].input_num);

	SDL_DestroyWindow(window);

	SDL_DestroyTexture(tex_checked);
	SDL_DestroyTexture(tex_notchecked);
	SDL_DestroyTexture(tex_btn_normal);
	SDL_DestroyTexture(tex_btn_hoover);
	SDL_DestroyTexture(tex_btn_holded);
	tex_checked = NULL;
	tex_notchecked = NULL;
	tex_btn_normal = NULL;
	tex_btn_hoover = NULL;
	tex_btn_holded = NULL;

	font_color = {255, 255, 255};
