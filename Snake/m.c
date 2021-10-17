#define _M_C_
#include "m.h"
#include "res.h"

#define MAXX 105
#define MAXY 105

BYTE length = 21, width = 21, maxX = 20, maxY = 20;			//��ͼ�ĳ��Ϳ������߿�21��21�У�����±�20*20
BYTE dir = YOU, appleX, appleY;								//���˶��ķ���,���ӵ�x��y����
DWORD curMs, prevMs;										//��ǰ���룬��һ�κ���
BYTE over = 0, fmove = 1, fstop = 0, jout = 0;				//��Ϸʧ�ܵı�־,�ƶ��˵ı�־,��ͣ�ı�־��������־
BYTE key;													//���ڴ��ֵ
DWORD score, topScore;										//���ڴ����,����߷���
UINT speed = 250;											//С�ߵ��ٶȣ����ڵ����Ѷ�,�ٶ�Ϊ250ms�ƶ�һ��
BYTE map[MAXX][MAXY];										//��ͼ���ݣ����������ռ�����

/*
����Snake�ṹ��
��������,����͵�ǰ�ڵ����һ���ڵ�
*/
typedef struct SS
{
	BYTE x;
	BYTE y;
	struct SS* next;
}Snake;

Snake* head;	//ȫ�ֵ�ͷָ�룬���ڱ�ʾ��ͷ��λ�ã��Ӷ���λ���еĽڵ�

void HideCursor()//���ع��
{
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void setColor(unsigned short ForeColor, unsigned short BackGroundColor)//������ɫ,����ͱ���
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//��ȡ��ǰ���ھ��
	SetConsoleTextAttribute(handle, ForeColor + BackGroundColor * 0x10);//������ɫ
}

void gotoxy(BYTE x, BYTE y)//����궨λ��ָ��λ��
{
	//COORD��Windows API�ж����һ�ֽṹ����ʾһ���ַ��ڿ���̨��Ļ�ϵ�����
	COORD cor;
	//��� 
	HANDLE hout;
	//�趨����Ҫ��λ�������� 
	cor.X = x * 2;//˫�ֽ��ַ���x��*2
	cor.Y = y;
	//GetStdHandle������ȡһ��ָ���ض���׼�豸�ľ����������׼���룬��׼����ͱ�׼����
	//STD_OUTPUT_HANDLE���Ǵ����׼�����Ҳ������ʾ�����ĺ� 
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetConsoleCursorPosition�����������ÿ���̨����λ��
	SetConsoleCursorPosition(hout, cor);
}

void FirstPage()//�׽��棬����չʾ�û���ʾ������������
{
	BYTE i;
	setColor(WHITE, BLACK);
	for (i = 0; i < 8; i++)
	{
		gotoxy(maxX / 2, i);
		printf(firstpg[i]);
	}
}

void WaitEnter()//�ȴ�enter���İ���ִ���Ժ�ĳ���
{
	char ch;
	while (1)
	{
		ch = getchar();
		if (ch == '\n') break;//����ǻ��з�����ô������ѭ��
	}
	system("cls");//����
}

void DrawPoint(BYTE x, BYTE y, BYTE type)
{
	gotoxy(x, y);//����궨λ��ָ��λ��
	//�������ͻ�����ͬ��ͼ��
	switch (type)
	{
	default:
		break;
	case KONG://0Ϊ�հף���ʾ�հ�
		setColor(WHITE, BLACK);
		printf("��");
		break;
	case BIAN://1��ʾ�߿�
		setColor(WHITE, BLACK);
		printf("��");
		break;
	case SHE://2Ϊʵ�ķ��飬��ʾ����
		setColor(BLUE, YELL);
		printf("��");
		break;
	case GUO://3ΪԲ�Σ���ʾ��ʵ
		setColor(YELL, BLACK);
		printf("��");
		break;
	case 4://3ΪԲ�Σ���ʾ��ʵ
		setColor(RED, YELL);
		printf("��");
		break;
	}
}

void DrawFrame()//���߿�
{
	BYTE x, y;
	for (x = 0; x <= maxX; x++)DrawPoint(x, 0, BIAN);//�ڵ�һ�л��շ��飬����Ϊ��
	for (x = 0; x <= maxX; x++)DrawPoint(x, maxY, BIAN);//�����һ�л��շ���
	for (y = 1; y <= maxY - 1; y++)DrawPoint(0, y, BIAN);//����߻���
	for (y = 1; y <= maxY - 1; y++)DrawPoint(maxX, y, BIAN);//���ұ߻���

	for (y = 0; y < 20; y++)
	{
		gotoxy(maxX + 2, y);
		printf(form[y]);
	}
}

void CreatFirstSnake()//������һ����
{
	BYTE i;
	Snake* cur, * prev;//curΪ��ǰ�ڵ㣬pΪ��һ���ڵ�

	head = (Snake*)malloc(sizeof(Snake));//����ͷ�ڵ�
	head->x = 8;	//ͷ�ڵ������
	head->y = 8;	//ͷ�ڵ������
	head->next = NULL;//��һ���ڵ���ʱΪNULL
	//ͷ�ڵ㴦�����
	prev = head;//��һ���ڵ�Ϊͷ�ڵ�

	for (i = 0; i < 3; i++)
	{
		cur = (Snake*)malloc(sizeof(Snake));//���ٵ�ǰ�ڵ�,����Ϊͷ�ڵ��ǰһ���ڵ�
		cur->x = prev->x - 1;//��ǰ�ڵ��x����Ϊ��һ�����x����-1
		cur->y = prev->y;//y�������һ�������y����һ��
		cur->next = NULL;//��һ���ڵ���ʱΪNULL
		prev->next = cur;//��һ���ڵ����һ���ڵ�ָ��ǰ�ڵ�
		prev = cur;//��һ���ڵ�Ϊ��ǰ�ڵ�
	}
	//�ߵ��������
}

void DrawSnake()//����
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
	gotoxy(maxX + 8, 3); setColor(GREEN, BLACK); printf("��Ϸ�����С���");
	gotoxy(maxX + 8, 4); setColor(WHITE, BLACK); printf("0");
	gotoxy(maxX + 8, 5); setColor(WHITE, BLACK); printf("0");
	gotoxy(maxX + 8, 6); setColor(GREEN, BLACK); printf("�ȼ�1 ��");
}

void DrawRandomApple()
{
	Snake* cur;
	srand((unsigned)time(NULL));//��ʱ�������ӣ�ÿ�β������������һ��
	appleX = rand() % (maxX - 1) + 1;	//ȡ1-�߽�-1��ֵ
	appleY = rand() % (maxY - 1) + 1;	
	//��յ�ͼ
	memset(map, 0, sizeof map);
	//��ȡ�������
	int count = 0;
	cur = head;
	while (cur != NULL) {
		count++;
		map[cur->y][cur->x] = 1;
		cur = cur->next;
	}
	if (count == (maxX-1) * (maxY-1)) {	//ȫ��������Ϸ����
		over = 1;
		return;
	}
	//�������λ���������壬��ѭ���ҿ�λ��
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
	DrawPoint(appleX, appleY, GUO);	//������
}

void InitGame()
{
	DrawFrame();		//���߿�
	CreatFirstSnake();	//������һ����
	DrawSnake();		//����
	DrawScore();		//���Ҳ�÷�
	DrawRandomApple();	//�����ƻ��
}

void Inspect(BYTE x, BYTE y)//������м��,����over
{
	Snake* cur;
	//���������߿򣬼���Ƿ��ڱ߿���
	if (y == 0 || x == 0 || y == maxY || x == maxX)
	{
		over = 1;
		return;
	}
	//���������Լ�������,��������
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
	case 250:setColor(GREEN, BLACK); printf("�ȼ�1 ��"); break;
	case 225:setColor(GREEN, BLACK); printf("�ȼ�2 ��ͨ"); break;
	case 200:setColor(YELLOW, BLACK); printf("�ȼ�3 ��ս"); break;
	case 175:setColor(YELLOW, BLACK); printf("�ȼ�4 ����"); break;
	case 150:setColor(RED, BLACK); printf("�ȼ�5 ����"); break;
	case 125:setColor(RED, BLACK); printf("�ȼ�6 ��̬"); break;
	case 100:setColor(BLUE, BLACK); printf("�ȼ�7 ج��"); break;
	case 75:setColor(BLUE, BLACK); printf("�ȼ�8 ����"); break;
	}
}

void AddScore()//�ӷ���
{
	setColor(WHITE, BLACK);
	score += 1;//�ӷ���
	//ˢ�µ�ǰ����
	gotoxy(maxX + 8, 4);
	printf("%d", score);
	//ˢ����߷���
	if (score > topScore)
	{
		topScore = score;
		gotoxy(maxX + 8, 5);
		printf("%d", topScore);
	}
	if (score % 10 == 0)//ÿ����ʮ��,�����Ѷ�
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
	Inspect(cur->x, cur->y);//������м��
	if (over)return;

	DrawPoint(head->x, head->y, SHE);
	DrawPoint(cur->x, cur->y, 4);//���ͨ�������Ͻڵ�

	cur->next = head;//�µĽڵ����һ���ڵ���֮ǰ��ͷ
	head = cur;//ͷ��Ϊ�µĽڵ�

	if (cur->x == appleX && cur->y == appleY)//�����ǰ���ǹ��ӵĵ㣬��ô����Ҫɾ��һ���ڵ�
	{
		AddScore();//�ӷ���
		DrawRandomApple();//�����µĹ���
		return;
	}

	//��������ȥ�����һ���ڵ�,��Ϊcur���ھ���ͷ�����Բ���Ҫcur=head
	while (1)
	{
		if (cur->next == NULL)break;//�����ǰ�ڵ����һ���ڵ�Ϊ�գ���ô��ǰ�ڵ�ʱ���һ���ڵ�
		prev = cur;//����ѭ��ʱ��prevָ��cur����һ���ڵ�
		cur = cur->next;
	}

	DrawPoint(cur->x, cur->y, KONG);//�����һ���ڵ��λ�û���
	prev->next = NULL;//���һ���ڵ����һ���ڵ����һ���ڵ�ָ���
	free(cur);//free���һ���ڵ�
}

void DeleteThree()
{
	Snake* cur;
	BYTE i;
	for (i = 0; i < 3; i++)
	{
		if (head->next == NULL)break;//���ͷû���ˣ���ôGG 
		DrawPoint(head->x, head->y, KONG);//�ڵ�ǰͷ���� 
		cur = head->next;//��ͷ����һ�� 
		free(head);//ɾͷ 
		head = cur;//ͷ���� ͷ����һ�� 
	}
	DrawSnake();
	score -= 3;
}

void OverPage()
{
	setColor(RED, BLACK);
	gotoxy(maxX + 8, 3);
	printf("OVER!�س����ؿ�");
}

void SnakeRun()
{
	//��һ����ʱʱ��
	curMs = GetTickCount();//ȡ��ǰms��
	if ((curMs - prevMs) >= speed)//��������ٶȵ�ʱ��,��ʼ��300ms
	{
		prevMs = curMs;//���ã����ϴε�ms����Ϊ��ǰ��ms��
		//�ƶ�С��
		MoveSnake();
		fmove = 1;//�ƶ���ɵı�־
		if (over)
		{
			OverPage();//��ʾOVER
			return;
		}
	}
}

void InitVari()//��ʼ������
{
	dir = YOU;
	over = 0;//over��־��0
	score = 0;
	speed = 250;//�ٶ�Ϊ250ms�ƶ�һ��
	fmove = 1;
	fstop = 0;
}

void KeyOperate()
{
	//��ֹ��ͷ
	if (dir == ZUO && key == YOU || dir == YOU && key == ZUO)return;
	if (dir == SHANG && key == XIA || dir == XIA && key == SHANG)return;
	//���keyΪ���������ǰһ�������Ѿ��ƶ�����
	if ((key == SHANG || key == XIA || key == ZUO || key == YOU) && fmove == 1)
	{
		fmove = 0;
		dir = key;//����Ϊ��ֵ
	}
	if (key == 32)//�ո�
	{
		if (fstop == 0)
		{
			fstop = 1;
			setColor(RED, BLACK);
			gotoxy(maxX + 8, 3);
			printf("��Ϸ��ͣ�С���");
		}
		else
		{
			fstop = 0;
			setColor(GREEN, BLACK);
			gotoxy(maxX + 8, 3);
			printf("��Ϸ�����С���");
		}
	}
	if (over)
	{
		if (key == '\r')
		{
			jout = 1;//�����˳�
			return;
		}
		if (key == 'f')//����
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
	while (_kbhit())//����а�������
	{
		key = _getch();//�����ԵĻ�ȡ����
		KeyOperate();//��������
	}
}

int main()
{
	system("mode con cols=80 lines=22");	//���ÿ���̨�Ĵ�С��x��80���ֽڵĳ��ȣ�40�����֣���y��22��
	HideCursor();
	while (1) {
		FirstPage();	//�����׽���
		WaitEnter();	//�ȴ�enter��֮������
		InitGame();		//��ʼ����Ϸ
		//����ʱ
		prevMs = GetTickCount();//��ȡ��ǰ��ms
		while (1)
		{
			if (!fstop)SnakeRun();//�ߵ��˶�
			KeyScan();//����ɨ��
			if (jout)break;
		}
		jout = 0;
		//��Ϸ����
		system("cls");//����
		InitVari();//��ʼ������
	}
	system("pause");//��ͣ����ֹ�����Զ��˳�
	return 0;
}
