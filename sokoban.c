#pragma warning(disable:4996)

#include <Windows.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Map_Size 10 //地图大小

char init[Map_Size][Map_Size]; //配置
const char * Init_File = "sokoban.init"; //配置文件

const char * Nul_Text = "  "; //空文本
const int Nul_Color = 0; //空颜色

const char * Map_Text = "■"; //地图文本
const int Map_Color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED; //地图颜色

const char * Box_Text = "★"; //盒子文本
const int Box_Color = FOREGROUND_RED; //盒子颜色

const char * Bin_Text = "■"; //箱子文本
const int Bin_Color = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; //箱子颜色

const char * Man_Text = "♀"; //人文本
const int Man_Color = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; //人颜色

/* 标志 */
enum Flag
{
	Flag_Nul,	 //空白标志
	Flag_Map,	 //地图标志
	Flag_Box,	 //盒子标志
	Flag_Bin,	 //箱子标志
	Flag_Man,	 //人身标志
	Flag_ManBox, //人入盒标志
	Flag_BinBox  //箱入盒标志
};

/* 方向 */
enum Move
{
	Move_Stop, //停止运动
	Move_Up,   //向上运动
	Move_Down, //向下运动
	Move_Left, //向左运动
	Move_Right //向右运动
};

int num = 0; //箱子数

/* 箱子 */
struct
{
	int x;
	int y;
} bin[Map_Size];

/* 人 */
struct
{
	int x;
	int y;
	enum Move move;
} man;

/* 打印文本 */
void PrintText(int x, int y, int color, const char * text)
{
	assert(x >= 0 && y >= 0 && color >= 0 && text != NULL);

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { x, y }); //设置光标位置
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); //设置文本颜色
	printf("%s", text);
}

/* 读取配置 */
void ReadInit(const char * initfile)
{
	assert(initfile != NULL);

	FILE * file = fopen(initfile, "r");
	if (file == NULL) exit(EXIT_FAILURE);
	char buf[Map_Size + 2];
	for (int i = 0; i < Map_Size; i++)
	{
		fgets(buf, Map_Size + 2, file);
		for (int j = 0; j < Map_Size; j++)
		{
			switch (buf[j])
			{
			case '0':
				init[j][i] = Flag_Nul;
				break;
			case '1':
				init[j][i] = Flag_Map;
				break;
			case '2':
				init[j][i] = Flag_Box;
				break;
			case '3':
				init[j][i] = Flag_Bin;
				break;
			case '4':
				init[j][i] = Flag_Man;
				break;
			case '5':
				init[i][j] = Flag_ManBox;
				break;
			case '6':
				init[i][j] = Flag_BinBox;
				break;
			default:
				break;
			}
		}
	}
	fclose(file);
}

/* 创建配置 */
void CreateInit()
{
	for (int i = 0; i < Map_Size; i++)
	{
		for (int j = 0; j < Map_Size; j++)
		{
			switch (init[i][j])
			{
			case Flag_Bin:
			case Flag_BinBox:
				bin[num].x = i;
				bin[num].y = j;
				num++;
				break;
			case Flag_Man:
			case Flag_ManBox:
				man.x = i;
				man.y = j;
				man.move = Move_Stop;
				break;
			default:
				break;
			}
		}
	}
}

/* 打印配置 */
void PrintInit()
{
	for (int i = 0; i < Map_Size; i++)
	{
		for (int j = 0; j < Map_Size; j++)
		{
			switch (init[i][j])
			{
			case Flag_Map:
				PrintText(i << 1, j, Map_Color, Map_Text);
				break;
			case Flag_Box:
				PrintText(i << 1, j, Box_Color, Box_Text);
				break;
			case Flag_Bin:
				PrintText(i << 1, j, Bin_Color, Bin_Text);
				break;
			case Flag_Man:
				PrintText(i << 1, j, Man_Color, Man_Text);
				break;
			case Flag_ManBox:
				PrintText(i << 1, j, Box_Color, Man_Text);
				break;
			case Flag_BinBox:
				PrintText(i << 1, j, Box_Color, Bin_Text);
				break;
			default:
				break;
			}
		}
	}
}

/* 检测按键 */
void CheckKey()
{
	if (GetAsyncKeyState(VK_UP))
	{
		man.move = Move_Up;
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		man.move = Move_Down;
	}
	else if (GetAsyncKeyState(VK_LEFT))
	{
		man.move = Move_Left;
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		man.move = Move_Right;
	}
	else
	{
		man.move = Move_Stop;
	}
	Sleep(100);
}

/* 检测人 */
int CheckMan()
{
	int x = man.x;
	int y = man.y;

	switch (man.move)
	{
	case Move_Up:
		y--;
		break;
	case Move_Down:
		y++;
		break;
	case Move_Left:
		x--;
		break;
	case Move_Right:
		x++;
		break;
	default:
		break;
	}

	switch (init[x][y])
	{
	case Flag_Map:
		man.move = Move_Stop;
		break;
	case Flag_Bin:
	case Flag_BinBox:
		for (int i = 0; i < num; i++)
		{
			if (x == bin[i].x && y == bin[i].y)
			{
				return i;
			}
		}
		break;
	default:
		break;
	}

	return -1;
}

/* 检测箱子 */
void CheckBin(int index)
{
	if (index < 0 || index >= num)
	{
		return;
	}
	
	int x = bin[index].x;
	int y = bin[index].y;

	switch (man.move)
	{
	case Move_Up:
		y--;
		break;
	case Move_Down:
		y++;
		break;
	case Move_Left:
		x--;
		break;
	case Move_Right:
		x++;
		break;
	default:
		break;
	}

	switch (init[x][y])
	{
	case Flag_Map:
	case Flag_Bin:
	case Flag_BinBox:
		man.move = Move_Stop;
		break;
	default:
		break;
	}
}

/* 移动箱子 */
void MoveBin(int index)
{
	if (index < 0 || index >= num)
	{
		return;
	}

	int x = bin[index].x;
	int y = bin[index].y;

	switch (init[x][y])
	{
	case Flag_Bin:
		init[x][y] = Flag_Nul;
		break;
	case Flag_BinBox:
		init[x][y] = Flag_Box;
		break;
	default:
		break;
	}

	switch (man.move)
	{
	case Move_Up:
		bin[index].y--;
		y--;
		break;
	case Move_Down:
		bin[index].y++;
		y++;
		break;
	case Move_Left:
		bin[index].x--;
		x--;
		break;
	case Move_Right:
		bin[index].x++;
		x++;
		break;
	default:
		break;
	}

	switch (init[x][y])
	{
	case Flag_Nul:
		init[x][y] = Flag_Bin;
		PrintText(x << 1, y, Bin_Color, Bin_Text);
		break;
	case Flag_Box:
		init[x][y] = Flag_BinBox;
		PrintText(x << 1, y, Box_Color, Bin_Text);
		break;
	default:
		break;
	}
}

/* 移动人 */
void MoveMan()
{
	int x = man.x;
	int y = man.y;

	switch (init[x][y])
	{
	case Flag_Man:
		init[x][y] = Flag_Nul;
		PrintText(x << 1, y, Nul_Color, Nul_Text);
		break;
	case Flag_ManBox:
		init[x][y] = Flag_Box;
		PrintText(x << 1, y, Box_Color, Box_Text);
		break;
	default:
		break;
	}

	switch (man.move)
	{
	case Move_Up:
		man.y--;
		y--;
		break;
	case Move_Down:
		man.y++;
		y++;
		break;
	case Move_Left:
		man.x--;
		x--;
		break;
	case Move_Right:
		man.x++;
		x++;
		break;
	default:
		break;
	}

	switch (init[x][y])
	{
	case Flag_Nul:
		init[x][y] = Flag_Man;
		PrintText(x << 1, y, Man_Color, Man_Text);
		break;
	case Flag_Box:
		init[x][y] = Flag_ManBox;
		PrintText(x << 1, y, Box_Color, Man_Text);
		break;
	default:
		break;
	}
}

/* 检测盒子 */
bool CheckBox()
{
	for (int i = 0; i < num; i++)
	{
		if (init[bin[i].x][bin[i].y] != Flag_BinBox)
		{
			return false;
		}
	}
	return true;
}

int main()
{
	ReadInit(Init_File);
	CreateInit();
	PrintInit();

	for (int index = -1; ; )
	{
		CheckKey();
		index = CheckMan();
		CheckBin(index);
		MoveBin(index);
		MoveMan();
		if (CheckBox()) break;
	}

	return 0;
}