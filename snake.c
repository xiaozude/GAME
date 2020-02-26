#include <Windows.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define Map_Size (30) //地图大小

char init[Map_Size][Map_Size]; //配置
const char * Init_File = "snake.init"; //配置文件

const char * Blank_Text = "  "; //空文本
const int Blank_Color = 0; //空颜色

const char * Map_Text = "■"; //地图文本
const int Map_Color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED; //地图颜色

const char * Snake_Text = "■"; //蛇文本
const int Snake_Color = FOREGROUND_GREEN | FOREGROUND_INTENSITY; //蛇身颜色

const char * Food_Text = "★"; //食物文本
const int Food_Color = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY; //食物颜色

/* 标志 */
enum Flag
{
	Flag_Blank, //空白标志
	Flag_Map,   //地图标志
	Flag_Snake  //蛇头标志
};

/* 方向 */
enum Move
{
	Move_Up,   //向上移动
	Move_Down, //向下移动
	Move_Left, //向左移动
	Move_Right //向右移动
};

/* 蛇身 */
struct SnakeNode
{
	struct SnakeNode * next;
	int x;
	int y;
};

/* 蛇 */
struct Snake
{
	struct SnakeNode * head;
	enum Move move;
	int length;
} snake;

/* 食物 */
struct
{
	int x;
	int y;
} food;

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
			init[j][i] = (buf[j] == '0') ? Flag_Blank : Flag_Map;
		}
	}

	fclose(file);
}

/* 打印配置 */
void PrintInit()
{
	for (int i = 0; i < Map_Size; i++)
	{
		for (int j = 0; j < Map_Size; j++)
		{
			if (init[i][j] == Flag_Map)
			{
				PrintText(i << 1, j, Map_Color, Map_Text);
			}
		}
	}
}

/* 创建蛇 */
void CreateSnake()
{
	srand((unsigned)time(NULL));

	snake.head = (struct SnakeNode *)malloc(sizeof(struct SnakeNode));
	if (snake.head == NULL) exit(EXIT_FAILURE);
	snake.head->next = NULL;
	do
	{
		snake.head->x = rand() % (Map_Size - 2) + 1;
		snake.head->y = rand() % (Map_Size - 2) + 1;
	} while (init[snake.head->x][snake.head->y]);
	snake.move = rand() % 4;
	snake.length = 1;
	init[snake.head->x][snake.head->y] = Flag_Snake;
}

/* 销毁蛇 */
void DestroySnake()
{
	struct SnakeNode * p = snake.head;
	while (p != NULL)
	{
		snake.head = snake.head->next;
		free(p);
		p = snake.head;
	}
}

/* 打印蛇 */
void PrintSnake()
{
	struct SnakeNode * p = snake.head;
	while (p != NULL)
	{
		PrintText(p->x << 1, p->y, Snake_Color, Snake_Text);
		p = p->next;
	}
}

/* 创建食物 */
void CreateFood()
{
	srand((unsigned)time(NULL));

	do
	{
		food.x = rand() % (Map_Size - 2) + 1;
		food.y = rand() % (Map_Size - 2) + 1;
	} while (init[food.x][food.y]);
}

/* 打印食物 */
void PrintFood()
{
	PrintText(food.x << 1, food.y, Food_Color, Food_Text);
}

/* 检测按键 */
void CheckKey()
{
	if (GetAsyncKeyState(VK_UP) && snake.move != Move_Down)
	{
		snake.move = Move_Up;
	}
	else if (GetAsyncKeyState(VK_DOWN) && snake.move != Move_Up)
	{
		snake.move = Move_Down;
	}
	else if (GetAsyncKeyState(VK_LEFT) && snake.move != Move_Right)
	{
		snake.move = Move_Left;
	}
	else if (GetAsyncKeyState(VK_RIGHT) && snake.move != Move_Left)
	{
		snake.move = Move_Right;
	}
	else
	{
		Sleep(100);
	}
	Sleep(100);
}

/* 移动蛇 */
void MoveSnake()
{
	struct SnakeNode * p = (struct SnakeNode *)malloc(sizeof(struct SnakeNode));
	if (p == NULL) exit(EXIT_FAILURE);

	switch (snake.move)
	{
	case Move_Up:
		p->x = snake.head->x;
		p->y = snake.head->y - 1;
		break;
	case Move_Down:
		p->x = snake.head->x;
		p->y = snake.head->y + 1;
		break;
	case Move_Left:
		p->x = snake.head->x - 1;
		p->y = snake.head->y;
		break;
	case Move_Right:
		p->x = snake.head->x + 1;
		p->y = snake.head->y;
		break;
	default:
		break;
	}

	p->next = snake.head;
	snake.head = p;
}

/* 检测蛇 */
bool CheckSnake()
{
	if (!init[snake.head->x][snake.head->y])
	{
		init[snake.head->x][snake.head->y] = Flag_Snake;
		return true;
	}
	return false;
}

/* 检测食物 */
void CheckFood()
{
	if (snake.head->x == food.x && snake.head->y == food.y)
	{
		snake.length++;
		CreateFood();
		return;
	}

	struct SnakeNode * p = snake.head;
	while (p->next->next != NULL)
	{
		p = p->next;
	}

	init[p->next->x][p->next->y] = Flag_Blank;
	PrintText(p->next->x << 1, p->next->y, Blank_Color, Blank_Text);
	free(p->next);
	p->next = NULL;
}

int main()
{
	ReadInit(Init_File);
	PrintInit();
	CreateSnake();
	CreateFood();
	
	while (true)
	{
		PrintSnake();
		PrintFood();
		CheckKey();
		MoveSnake();
		if (!CheckSnake()) break;
		CheckFood();
	}

	DestroySnake();

	return 0;
}