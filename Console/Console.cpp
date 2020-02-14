/* Standard error macro for reporting API errors */
#include <Windows.h>
#include <stdio.h>
#include <sddl.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <iostream>
#include <mmsystem.h>

#define PERR(bSuccess, api) {if(!(bSuccess)) printf("%s:Error %d from %s \
    on line %d\n", __FILE__, GetLastError(), api, __LINE__);}

#define	MAX_WAIT_MSEC		1000
#define	MAX_DISPLAY_LINES	28
#define	MAX_LINES			(MAX_DISPLAY_LINES - 3)

int frame[MAX_LINES][6];

void cls(HANDLE hConsole);
void print_frame(HANDLE handle);
void hidecursor(HANDLE handle);
void move_location();
void input_first_value(HANDLE handle);
void display_frame(HANDLE handle);
int get_point(HANDLE handle,int max_sec);
void display_total_point(HANDLE handle, int total_point);
void display_line(HANDLE handle, int y, int shape);

int
main()
{
	int total_point = 0;
	int max_msec = MAX_WAIT_MSEC;				// max milli-sec
	HANDLE handle;
	COORD pos;

	handle = GetStdHandle(STD_OUTPUT_HANDLE);

	memset(frame, 0, 4 * MAX_LINES * 4);

	hidecursor(handle);
	cls(handle);
	PlaySound(TEXT("HappyDay (2).wav"), NULL, SND_ASYNC | SND_LOOP);

	print_frame(handle); //frame ���
	display_total_point(handle, 0);

	srand((unsigned)time(NULL)); 

	while (1)
	{
		int point = 0;

		move_location();			//��ġ�̵�

		input_first_value(handle);	//�Է�

		display_frame(handle);		//���

		point = get_point(handle, max_msec); //Ű���� �Է��ؼ� ���� ���
		if (point > 0)
		{
			total_point += point;
			display_total_point(handle, total_point);
		}
	}

	pos.X = 1;
	pos.Y = 30;
	SetConsoleCursorPosition(handle, pos);

	return 0;
}


void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    /* here's where we'll home the
									 cursor */
	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
	DWORD dwConSize;                 /* number of character cells in
									 the current buffer */

									 /* get the number of character cells in the current buffer */

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	/* fill the entire screen with blanks */

	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
		dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");

	/* get the current text attribute */

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	PERR(bSuccess, "ConsoleScreenBufferInfo");

	/* now set the buffer's attributes accordingly */

	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
		dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputAttribute");

	/* put the cursor at (0, 0) */

	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
	PERR(bSuccess, "SetConsoleCursorPosition");
	return;
}

void hidecursor(HANDLE handle)
{
	CONSOLE_CURSOR_INFO info;

	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(handle, &info);
}

void print_frame(HANDLE handle)
{
	int i;
	COORD pos;

	pos.X = 2;
	pos.Y = 0;
	SetConsoleCursorPosition(handle, pos);

	printf( "\u250D\u2501\u2501\u2501\u2501\u2501\u2501"
			"\u252F\u2501\u2501\u2501\u2501\u2501\u2501"
			"\u252F\u2501\u2501\u2501\u2501\u2501\u2501"
			"\u252F\u2501\u2501\u2501\u2501\u2501\u2501\u2511");

	for (i = 0; i < (MAX_DISPLAY_LINES - 5); i++)
	{
		pos.X = 2;
		pos.Y = 1 + i;
		SetConsoleCursorPosition(handle, pos);
		printf("\u2502            \u2502            \u2502            \u2502            \u2502");
	}

	pos.X = 2;
	pos.Y = 1 + (MAX_DISPLAY_LINES - 5);
	SetConsoleCursorPosition(handle, pos);
	printf("\
\u251D\u2501\u2501\u2501\u2501\u2501\u2501\
\u253F\u2501\u2501\u2501\u2501\u2501\u2501\
\u253F\u2501\u2501\u2501\u2501\u2501\u2501\
\u253F\u2501\u2501\u2501\u2501\u2501\u2501\u2525");

	for (i = 0; i < 2; i++)
	{
		pos.X = 2;
		pos.Y = 1 + (MAX_DISPLAY_LINES - 5) + 1 + i;
		SetConsoleCursorPosition(handle, pos);
		printf("\u2502            \u2502            \u2502            \u2502            \u2502");
	}

	pos.X = 2;
	pos.Y = 1 + (MAX_DISPLAY_LINES - 5) + 1 + 2;
	SetConsoleCursorPosition(handle, pos);
	printf("\
\u2515\u2501\u2501\u2501\u2501\u2501\u2501\
\u2537\u2501\u2501\u2501\u2501\u2501\u2501\
\u2537\u2501\u2501\u2501\u2501\u2501\u2501\
\u2537\u2501\u2501\u2501\u2501\u2501\u2501\u2519");

}

void move_location()
{
	//�����ϴ� ����
	int x, y;

	for (y = (MAX_LINES-1); y > 0; y--)
	{
		for (x = 5; x >= 0; x--)
		{
			frame[y][x] = frame[y - 1][x];
		}
	}

	for (x = 5; x >= 0; x--)
		frame[0][x] = 0;
}

void input_first_value(HANDLE handle)
{
	//4ĭ �߿� �ϳ��� �־�� �ȴ�
	int rnd_space, rnd_point;

	rnd_space = rand() % 4; //rnd_space : 0~3
	frame[0][rnd_space] = 1; //exist_box = 1, no_exist_box = 0

	rnd_point = rand() % 9 + 1; // rnd = 1~10, �� rnd�� �����̴�.
	frame[0][4] = rnd_point; //����

	frame[0][5] = 0; //shape �� �ʱ�ȭ
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	//���� ���� ��
	int i;
	COORD pos;

	pos.X = 80;
	pos.Y = MAX_DISPLAY_LINES - 1;
	SetConsoleCursorPosition(handle, pos);
	for (i = 0; i < 40; i++)
		printf(" ");

	pos.X = 80;
	pos.Y = MAX_DISPLAY_LINES - 1;
	SetConsoleCursorPosition(handle, pos);
	for (i = 0; i < 5; i++)
		printf("%d  ", frame[0][i]);
	printf("/// %d", rnd_space);
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
}

void display_frame(HANDLE handle)
{
  //���� ��ġ�� ��������, ���� �����ϴ� ��������
	int y;

	for (y = (MAX_LINES-1); y >= 0; y--)
	{
		if (frame[y][4] == 0)
			continue;

		display_line(handle, y, frame[y][5]);
	}
}

char get_key()
{
	if (_kbhit()) //Ű�� �������� 0�� �ƴ� �� ��ȯ
		return _getch();

	return '\0';
}

int get_point(HANDLE handle, int max_msec)
{
	//Ű�� �Է¹ް� point ǥ��+���
	int point = 0;
	int wait_msec = 0;
	char key = 0;

	while (wait_msec <= max_msec)
	{
		int curr_wait_msec;

		key = get_key(); //Ű �Է�

		if (key == 'h')
		{
			if (frame[23][0] == 1)
			{
				point += frame[23][4];
				frame[23][5] = 1;
				break;
			}
		}
		else if (key == 'j')
		{
			if (frame[23][1] == 1)
			{
				point += frame[23][4];
				frame[23][5] = 1;
				break;
			}
		}
		else if (key == 'k')
		{
			if (frame[23][2] == 1)
			{
				point += frame[23][4];
				frame[23][5] = 1;
				break;
			}
		}
		else if (key == 'l')
		{
			if (frame[23][3] == 1)
			{
				point += frame[23][4];
				frame[23][5] = 1;
				break;
			}
		}

		curr_wait_msec = max_msec / 1000;

		Sleep(curr_wait_msec);
		wait_msec += curr_wait_msec;
	}

	if (frame[23][5] == 1)
		display_line(handle, 23, frame[23][5]);

	if (wait_msec < max_msec)
	{
		int temp;

		temp = max_msec - wait_msec;
		Sleep(temp);
	}

	return point;
}

void display_total_point(HANDLE handle, int total_point)
{
	COORD pos;

	pos.X = 70;
	pos.Y = 24;
	SetConsoleCursorPosition(handle, pos);
	printf("point : %d", total_point);
}

void display_line(HANDLE handle, int y, int shape)
{
	int x;
	COORD pos;

	pos.X = 2;
	if (y > (MAX_LINES - 1 - 2))
		pos.Y = y + 1 + 1;
	else
		pos.Y = y + 1;
	SetConsoleCursorPosition(handle, pos);

	for (x = 0; x < 4; x++)
	{
		printf("\u2502  ");

		if (frame[y][x] == 1)		//box�� �����Ѵٸ�
		{
			if (shape == 0)
				printf("\u25A0\u25A0\u25A0\u25A0");
			else
				printf("\u25C7\u25C7\u25C7\u25C7");
		}
		else
		{
			printf("        ");
		}

		if (x < 3)
			printf("  ");
		else
		{
			printf("  \u2502 %d  ..\n", frame[y][4]);
		}
	}
	fflush(stdout);
}