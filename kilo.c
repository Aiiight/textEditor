/***** INCLUDES *****/

#include <ctype.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

/***** DEFINES *****/
#define CTRL_KEY(k) ((k) & 0x1f)
/***** DATA *****/

struct termios orig_termios;

/***** TERMINAL *****/

void die (const char *s)
{
	perror(s);
	exit(1);
}

void disableRawMode()
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}


void enableRawMode()
{


	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) 
	{
		die("tcgetattr"); //reads terminal attriubtes
	}

	atexit(disableRawMode);

	struct termios raw = orig_termios;

	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); 
	raw.c_iflag &= ~(OPOST);
	raw.c_iflag &= ~(ICRNL | IXON);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	//IXON disables CTRL S and Q, ICANON turns off C and Z,
	//IEXTEN disables CTRL V, ICRNL fixes CNTRL M
	//OPOST turns off all output processing

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) 
	{
		die(tcsetattr);
	}
}

/***** INIT *****/

int main()
{
	enableRawMode();

	while (1)
	{
		char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");

		if (iscntrl(c))
		{
			printf("%d\n", c);
		}
		else
		{
			printf("%d ('%c')\n", c, c);
		}
		if ( c == CTRL_KEY('q')) break;
	}


	return 0;
}


//notes:
//iscntrl() tests whether a char is a control char. control chars
//are nonprintable chars that we dont want to print to the screen