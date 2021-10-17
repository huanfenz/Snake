#define _M_C_
#include "m.h"
#include "res.h"

#define MAXX 105
#define MAXY 105

BYTE length = 21, width = 21, maxX = 20, maxY = 20;			//地图的长和宽，包含边框21行21列，最大下标20*20
BYTE dir = YOU, appleX, appleY;								//蛇运动的方向,果子的x，y坐标
DWORD curMs, prevMs;										//当前毫秒，上一次毫秒
BYTE over = 0, fmove = 1, fstop = 0, jout = 0;				//游戏失败的标志,移动了的标志,暂停的标志，跳出标志
BYTE key;													//用于存键值
DWORD score, topScore;										//用于存分数,存最高分数
UINT speed = 250;											//小蛇的速度，用于调整难度,速度为250ms移动一次
BYTE map[MAXX][MAXY];										//地图数据，存蛇身体的占用情况

/*
定义Snake结构体
三个参数,坐标和当前节点的下一个节点
*/
typedef struct SS
{
	BYTE x;
	BYTE y;
	struct SS* next;
}Snake;

Snake* head;	//全局的头指针，用于表示蛇头的位置，从而定位所有的节点

void HideCursor()//隐藏光标
{
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void setColor(unsigned short ForeColor, unsigned short BackGroundColor)//设置颜色,字体和背景
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//获取当前窗口句柄
	SetConsoleTextAttribute(handle, ForeColor + BackGroundColor * 0x10);//设置颜色
}

void gotoxy(BYTE x, BYTE y)//将光标定位到指定位置
{
	//COORD是Windows API中定义的一种结构，表示一个字符在控制台屏幕上的坐标
	COORD cor;
	//句柄 
	HANDLE hout;
	//设定我们要定位到的坐标 
	cor.X = x * 2;//双字节字符，x轴*2
	cor.Y = y;
	//GetStdHandle函数获取一个指向特定标准设备的句柄，包括标准输入，标准输出和标准错误。
	//STD_OUTPUT_HANDLE正是代表标准输出（也就是显示屏）的宏 
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetConsoleCursorPosition函数用于设置控制台光标的位置
	SetConsoleCursorPosition(hout, cor);
}

void FirstPage()//首界面，用于展示用户提示，基本操作等
{
	BYTE i;
	setColor(WHITE, BLACK);
	for (i = 0; i < 8; i++)
	{
		gotoxy(maxX / 2, i);
		printf(firstpg[i]);
	}
}

void WaitEnter()//等待enter键的按下执行以后的程序
{
	char ch;
	while (1)
	{
		ch = getchar();
		if (ch == '\n') break;//如果是换行符，那么跳出死循环
	}
	system("cls");//清屏
}

void DrawPoint(BYTE x, BYTE y, BYTE type)
{
	gotoxy(x, y);//将光标定位到指定位置
	//根据类型画出不同的图形
	switch (type)
	{
	default:
		break;
	case KONG://0为空白，表示空白
		setColor(WHITE, BLACK);
		printf("　");
		break;
	case BIAN://1表示边框
		setColor(WHITE, BLACK);
		printf("□");
		break;
	case SHE://2为实心方块，表示蛇体
		setColor(BLUE, YELL);
		printf("■");
		break;
	case GUO://3为圆形，表示果实
		setColor(YELL, BLACK);
		printf("●");
		break;
	case 4://3为圆形，表示果实
		setColor(RED, YELL);
		printf("■");
		break;
	}
}

void DrawFrame()//画边框
{
	BYTE x, y;
	for (x = 0; x <= maxX; x++)DrawPoint(x, 0, BIAN);//在第一行画空方块，类型为边
	for (x = 0; x <= maxX; x++)DrawPoint(x, maxY, BIAN);//在最后一行画空方块
	for (y = 1; y <= maxY - 1; y++)DrawPoint(0, y, BIAN);//在左边画竖
	for (y = 1; y <= maxY - 1; y++)DrawPoint(maxX, y, BIAN);//在右边画竖

	for (y = 0; y < 20; y++)
	{
		gotoxy(maxX + 2, y);
		printf(form[y]);
	}
}

void CreatFirstSnake()//创建第一个蛇
{
	BYTE i;
	Snake* cur, * prev;//cur为当前节点，p为上一个节点

	head = (Snake*)malloc(sizeof(Snake));//开辟头节点
	head->x = 8;	//头节点的数据
	head->y = 8;	//头节点的数据
	head->next = NULL;//下一个节点暂时为NULL
	//头节点处理完毕
	prev = head;//上一个节点为头节点

	for (i = 0; i < 3; i++)
	{
		cur = (Snake*)malloc(sizeof(Snake));//开辟当前节点,数据为头节点的前一个节点
		cur->x = prev->x - 1;//当前节点的x坐标为上一个点的x坐标-1
		cur->y = prev->y;//y坐标和上一个坐标的y坐标一样
		cur->next = NULL;//下一个节点暂时为NULL
		prev->next = cur;//上一个节点的下一个节点指向当前节点
		prev = cur;//上一个节点为当前节点
	}
	//蛇的链表搭建完成
}

void DrawSnake()//画蛇
{
	Snake* cur;
	cur = head;
	while (1)
	{
		if (cur == head)DrawPoint(cur->x, cur->y, 4);
		else DrawPoint(cur->x, cur->y, SHE);
		cur = cur->next;
		if (cur == NULL)return;
	}
}

void DrawScore()
{
	gotoxy(maxX + 8, 3); setColor(GREEN, BLACK); printf("游戏运行中……");
	gotoxy(maxX + 8, 4); setColor(WHITE, BLACK); printf("0");
	gotoxy(maxX + 8, 5); setColor(WHITE, BLACK); printf("0");
	gotoxy(maxX + 8, 6); setColor(GREEN, BLACK); printf("等级1 简单");
}

void DrawRandomApple()
{
	Snake* cur;
	srand((unsigned)time(NULL));//用时间做种子，每次产生的随机数不一样
	appleX = rand() % (maxX - 1) + 1;	//取1-边界-1的值
	appleY = rand() % (maxY - 1) + 1;	
	//清空地图
	memset(map, 0, sizeof map);
	//获取蛇身情况
	int count = 0;
	cur = head;
	while (cur != NULL) {
		count++;
		map[cur->y][cur->x] = 1;
		cur = cur->next;
	}
	if (count == (maxX-1) * (maxY-1)) {	//全是蛇身，游戏结束
		over = 1;
		return;
	}
	//如果果子位置是蛇身体，则循环找空位置
	while (map[appleY][appleX] == 1) {
		appleX++;
		if (appleX == maxX - 1) {
			appleX = 1;
			appleY++;
			if (appleY == maxY - 1) {
				appleY = 1;
			}
		}
	}
	DrawPoint(appleX, appleY, GUO);	//画果子
}

void InitGame()
{
	DrawFrame();		//画边框
	CreatFirstSnake();	//创建第一个蛇
	DrawSnake();		//画蛇
	DrawScore();		//画右侧得分
	DrawRandomApple();	//画随机苹果
}

void Inspect(BYTE x, BYTE y)//坐标进行检查,返回over
{
	Snake* cur;
	//不能碰到边框，检查是否在边框上
	if (y == 0 || x == 0 || y == maxY || x == maxX)
	{
		over = 1;
		return;
	}
	//不能碰到自己的蛇体,遍历链表
	cur = head;
	while (1)
	{
		if (x == cur->x && y == cur->y)
		{
			over = 1;
			return;
		}
		if (cur->next == NULL)break;
		cur = cur->next;
	}
}

void DrawScoreNanDu()
{
	gotoxy(maxX + 8, 6);
	switch (speed)
	{
	default:
		break;
	case 250:setColor(GREEN, BLACK); printf("等级1 简单"); break;
	case 225:setColor(GREEN, BLACK); printf("等级2 普通"); break;
	case 200:setColor(YELLOW, BLACK); printf("等级3 挑战"); break;
	case 175:setColor(YELLOW, BLACK); printf("等级4 困难"); break;
	case 150:setColor(RED, BLACK); printf("等级5 超难"); break;
	case 125:setColor(RED, BLACK); printf("等级6 变态"); break;
	case 100:setColor(BLUE, BLACK); printf("等级7 噩梦"); break;
	case 75:setColor(BLUE, BLACK); printf("等级8 绝望"); break;
	}
}

void AddScore()//加分数
{
	setColor(WHITE, BLACK);
	score += 1;//加分数
	//刷新当前分数
	gotoxy(maxX + 8, 4);
	printf("%d", score);
	//刷新最高分数
	if (score > topScore)
	{
		topScore = score;
		gotoxy(maxX + 8, 5);
		printf("%d", topScore);
	}
	if (score % 10 == 0)//每次满十分,增加难度
	{
		speed = speed - 25;
		DrawScoreNanDu();
	}
}

void MoveSnake()
{
	Snake* cur = NULL, * prev = NULL;
	cur = (Snake*)malloc(sizeof(Snake));
	switch (dir)
	{
	default:
		break;
	case SHANG:
		cur->x = head->x;
		cur->y = head->y - 1;
		break;
	case XIA:
		cur->x = head->x;
		cur->y = head->y + 1;
		break;
	case ZUO:
		cur->x = head->x - 1;
		cur->y = head->y;
		break;
	case YOU:
		cur->x = head->x + 1;
		cur->y = head->y;
		break;
	}
	Inspect(cur->x, cur->y);//坐标进行检查
	if (over)return;

	DrawPoint(head->x, head->y, SHE);
	DrawPoint(cur->x, cur->y, 4);//检查通过，画上节点

	cur->next = head;//新的节点的下一个节点是之前的头
	head = cur;//头变为新的节点

	if (cur->x == appleX && cur->y == appleY)//如果当前点是果子的点，那么不需要删除一个节点
	{
		AddScore();//加分数
		DrawRandomApple();//产生新的果子
		return;
	}

	//遍历链表，去掉最后一个节点,因为cur现在就是头，所以不需要cur=head
	while (1)
	{
		if (cur->next == NULL)break;//如果当前节点的下一个节点为空，那么当前节点时最后一个节点
		prev = cur;//跳出循环时，prev指向cur的上一个节点
		cur = cur->next;
	}

	DrawPoint(cur->x, cur->y, KONG);//在最后一个节点的位置画空
	prev->next = NULL;//最后一个节点的上一个节点的下一个节点指向空
	free(cur);//free最后一个节点
}

void DeleteThree()
{
	Snake* cur;
	BYTE i;
	for (i = 0; i < 3; i++)
	{
		if (head->next == NULL)break;//如果头没有了，那么GG 
		DrawPoint(head->x, head->y, KONG);//在当前头画空 
		cur = head->next;//存头的下一个 
		free(head);//删头 
		head = cur;//头等于 头的下一个 
	}
	DrawSnake();
	score -= 3;
}

void OverPage()
{
	setColor(RED, BLACK);
	gotoxy(maxX + 8, 3);
	printf("OVER!回车键重开");
}

void SnakeRun()
{
	//做一个定时时基
	curMs = GetTickCount();//取当前ms数
	if ((curMs - prevMs) >= speed)//如果到了速度的时间,初始是300ms
	{
		prevMs = curMs;//重置，即上次的ms数变为当前的ms数
		//移动小蛇
		MoveSnake();
		fmove = 1;//移动完成的标志
		if (over)
		{
			OverPage();//显示OVER
			return;
		}
	}
}

void InitVari()//初始化变量
{
	dir = YOU;
	over = 0;//over标志清0
	score = 0;
	speed = 250;//速度为250ms移动一次
	fmove = 1;
	fstop = 0;
}

void KeyOperate()
{
	//防止回头
	if (dir == ZUO && key == YOU || dir == YOU && key == ZUO)return;
	if (dir == SHANG && key == XIA || dir == XIA && key == SHANG)return;
	//如果key为方向键并且前一个按键已经移动过了
	if ((key == SHANG || key == XIA || key == ZUO || key == YOU) && fmove == 1)
	{
		fmove = 0;
		dir = key;//方向为键值
	}
	if (key == 32)//空格
	{
		if (fstop == 0)
		{
			fstop = 1;
			setColor(RED, BLACK);
			gotoxy(maxX + 8, 3);
			printf("游戏暂停中……");
		}
		else
		{
			fstop = 0;
			setColor(GREEN, BLACK);
			gotoxy(maxX + 8, 3);
			printf("游戏运行中……");
		}
	}
	if (over)
	{
		if (key == '\r')
		{
			jout = 1;//死亡退出
			return;
		}
		if (key == 'f')//复活
		{
			jout = 0;
			over = 0;
			DeleteThree();
		}
	}
	switch (key)
	{
	case '1':
	{
		speed = 250;
		DrawScoreNanDu();
	}break;
	case '2':
	{
		speed = 225;
		DrawScoreNanDu();
	}break;
	case '3':
	{
		speed = 200;
		DrawScoreNanDu();
	}break;
	case '4':
	{
		speed = 175;
		DrawScoreNanDu();
	}break;
	case '5':
	{
		speed = 150;
		DrawScoreNanDu();
	}break;
	case '6':
	{
		speed = 125;
		DrawScoreNanDu();
	}break;
	case '7':
	{
		speed = 100;
		DrawScoreNanDu();
	}break;
	case '8':
	{
		speed = 75;
		DrawScoreNanDu();
	}break;
	}
}

void KeyScan()
{
	while (_kbhit())//如果有按键按下
	{
		key = _getch();//不回显的获取按键
		KeyOperate();//按键操作
	}
}

int main()
{
	system("mode con cols=80 lines=22");	//设置控制台的大小，x轴80的字节的长度（40个汉字），y轴22个
	HideCursor();
	while (1) {
		FirstPage();	//绘制首界面
		WaitEnter();	//等待enter键之后清屏
		InitGame();		//初始化游戏
		//运行时
		prevMs = GetTickCount();//获取当前的ms
		while (1)
		{
			if (!fstop)SnakeRun();//蛇的运动
			KeyScan();//按键扫描
			if (jout)break;
		}
		jout = 0;
		//游戏结束
		system("cls");//清屏
		InitVari();//初始化变量
	}
	system("pause");//暂停，防止程序自动退出
	return 0;
}
