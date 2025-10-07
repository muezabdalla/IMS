#define SCREEN_W 1920
#define SCREEN_H 1080-60

#define handle_few_args() cerr <<ERR_COL "too few argument after "<< arg_current << NOR_COL<< endl; return 1;
#define handle_not_valid_arg(a) cerr <<ERR_COL  << a << " is not a valid input" << NOR_COL<< endl; return 1;

i = 1;
while (i < argc)
{
	string arg_current = argv[i];
	if (arg_current == "-h" || arg_current == "--help") {
		print_help();
	} else if (arg_current == "-p") {
		// checking if the arguments after -p are less than required
		if (argc-i-1 < 2) {handle_few_args();}

		arg_next = argv[i+1];
		arg_next2 = argv[i+2];

		if (arg_next == "right")
			POS_RIGHT = true;
		else if (arg_next == "left")
			X = 0;
		else 
		{
			// check if the input is include a char
			for (char c : arg_next) { if (!isdigit(c)) {handle_not_valid_arg(arg_next);} }
			X = atoi(argv[i+1]);
		}

		if (arg_next2 == "top")
			Y = 0;
		else if (arg_next2 == "buttom")
			POS_BUTTOM = true;
		else 
		{
			// check if the input is include a char
			for (char c : arg_next2) { if (!isdigit(c)) {handle_not_valid_arg(arg_next2);} }
			Y = atoi(argv[i+2]);
		}
		i+=2;

	} else if (arg_current == "-s") {
		// checking if the arguments after -S are less than required
		if (argc-i-1 < 2) {handle_few_args();}

		BUTTON_WIDTH = atoi(argv[i+1]);
		if (BUTTON_WIDTH == 0) {handle_not_valid_arg(argv[i+1]);}
		BUTTON_HIEGHT = atoi(argv[i+2]);
		if (BUTTON_HIEGHT == 0) {handle_not_valid_arg(argv[i+2]);}
		i+=2;

	} else if (arg_current == "-S") {
		// checking if the arguments after -S are less than required
		if (argc-i-1 < 2) {handle_few_args();}

		MOUSE_WIDTH = atoi(argv[i+1]);
		if (MOUSE_WIDTH == 0) {handle_not_valid_arg(argv[i+1]);}
		MOUSE_HIEGHT = atoi(argv[i+2]);
		if (MOUSE_HIEGHT == 0) {handle_not_valid_arg(argv[i+2]);}
		i+=2;

	} else if (arg_current == "-b") {
		SHOW_BORDERS = false;

	} else if (arg_current == "-i") {
		cout <<WARN_COL "the -i for entering the keyboard file has been removed. becuase it will automatically find the file." NOR_COL<< endl;
		i++;

	} else if (arg_current == "-I") {
		cout <<WARN_COL "the -I for entering the mouse file has been removed. becuase it will automatically find the file." NOR_COL<< endl;
		i++;

	} else if (arg_current == "-c") {
		SHOW_CTRL = false;
	} else if (arg_current == "-f") {
		SHOW_SHIFT = false;
	} else if (arg_current == "-t") {
		SHOW_SUPER = false;
	} else if (arg_current == "-a") {
		SHOW_ALT = false;
	} else if (arg_current == "-g") {
		SHOW_LETTERS = false;

	} else if (arg_current == "-k") {
		SHOW_CTRL = false;
		SHOW_SHIFT = false;
		SHOW_SUPER = false;
		SHOW_ALT = false;
		SHOW_LETTERS = false;

	} else if (arg_current == "-m") {
		SHOW_MOUSE = false;

	} else if (arg_current == "-T") {
		TRANSPARENT_MODE = true;

	} else if (arg_current == "-n") {
		CONFIG_GUI = false;

	} else if (arg_current == "-r") {
		cout <<WARN_COL "the -r flag for defining the refresh time has been removed. now it will refresh automatically" NOR_COL<< endl;
		i++;

	} else {
		cerr <<ERR_COL "argument:" << arg_current << " is not an allowable argument\nhere is the manual to see the allowable arguments\n" NOR_COL<< endl;
		print_help();
	}
	i++;
}

if (!SHOW_CTRL && !SHOW_SHIFT && !SHOW_SUPER && !SHOW_ALT && !SHOW_LETTERS && !SHOW_MOUSE)
{
	cerr <<ERR_COL "you turned of both mouse and keyboard. what do you want to see" NOR_COL<< endl;
	return 0;
}
