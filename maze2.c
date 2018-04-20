/*hello.c*/
#include <windows.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#define IDT_TIMER1 1
#define IDT_TIMER2 2
int game_state=0;
int leftTime;
int area_x,area_y,area_z;
int postion_x,postion_y,postion_z;
int data[100][100];
int Army_x[100],Army_y[100];
int Army_n;
//int data[100][100][100];
char text[][100]={
	"Welcome to the game!!Press left button to start!",
	"Congratulations!You pass the primary level!",
	"Congratulations!You pass the junior level!",
	"Congratulations!You pass the senior level!",
	"Time Up!Fail to pass the game!"
};

LRESULT CALLBACK wndproc(HWND,UINT,WPARAM,LPARAM);
void init_wndclass(HINSTANCE,TCHAR*,WNDCLASS*);
int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hprevinstance,LPSTR line,int cmd)
{
    TCHAR app_name[]=TEXT("Escape from the prison maze!");
    WNDCLASS wndclass;
    MSG msg;
    HWND hwnd;
    init_wndclass(hinstance,app_name,&wndclass);
    RegisterClass(&wndclass);
    hwnd=CreateWindow(app_name,app_name,WS_OVERLAPPEDWINDOW,\
            CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,\
            CW_USEDEFAULT,NULL,NULL,hinstance,NULL);
    ShowWindow(hwnd,cmd);
    UpdateWindow(hwnd);
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
void init_wndclass(HINSTANCE hinstance,TCHAR *app_name,WNDCLASS *wndclass)
{
    wndclass->cbClsExtra=0;
    wndclass->cbWndExtra=0;
    wndclass->hCursor=LoadCursor(hinstance,"IDC_CURSOR"); //在这里我们加载光标资源
    wndclass->hIcon=LoadIcon(hinstance,"IDI_ICON");      //加载图标资源
    wndclass->hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass->lpfnWndProc=wndproc;
    wndclass->lpszClassName=app_name;
    wndclass->lpszMenuName=NULL;
    wndclass->style=CS_VREDRAW|CS_HREDRAW;
    wndclass->hInstance=hinstance;
}
/*
void DFS(int x,int y,int z)
{
	int dir[6][3]={
		 1, 0, 0, //up
		-1, 0, 0, //down
		 0, 1, 0, //right
		 0,-1, 0, //left
		 0, 0, 1, //in
		 0, 0,-1};//out
	int t,i,k;
	for(i=0;i<6;i++)
	{
		//k=1;
		k=rand()%6;
	        t=dir[i][0];
		dir[i][0]=dir[k][0];
		dir[k][0]=t;
		t=dir[i][1];
		dir[i][1]=dir[k][1];
		dir[k][1]=t;
		t=dir[i][2];
		dir[i][2]=dir[k][2];
		dir[k][2]=t;

	}
	if(data[x][y][z]==0&&x>=1&&x<=area_x-2&&y>=1&&y<=area_y-2&&z>=1&&z<=area_z)
	{
		data[x][y][z]=1;
		for(i=0;i<6;i++)
		{
			if(data[x+2*dir[i][0]][y+2*dir[i][1]][z+2*dir[i][2]]==0)
			{
				data[x+dir[i][0]][y+dir[i][1]][z+dir[i][2]]=1;
				DFS(x+2*dir[i][0],y+2*dir[i][1],z+2*dir[i][2]);
			}
		}
	}
}
*/
void DFS(int m,int n)
{
	int dir[4][2]={1,0,-1,0,0,1,0,-1};
	int t,i,k;
	for(i=0;i<4;i++)
	{
		k=rand()%4;
        t=dir[i][0];
		dir[i][0]=dir[k][0];
		dir[k][0]=t;
		t=dir[i][1];
		dir[i][1]=dir[k][1];
		dir[k][1]=t;
	}
	if(data[m][n]==0&&m>=1&&m<=area_x-2&&n>=1&&n<=area_y-2)
	{
		data[m][n]=1;
		for(i=0;i<4;i++)
		{
			if(data[m+2*dir[i][0]][n+2*dir[i][1]]==0)
			{
				data[m+dir[i][0]][n+dir[i][1]]=1;
				DFS(m+2*dir[i][0],n+2*dir[i][1]);
			}
		}
	}
}

void print()
{
	int i,j;
	srand((int)time(NULL));
	if(game_state==1){
		area_x=area_y=area_z=20;
		leftTime=30;
		Army_n=5;
	}
	else if(game_state==2){
		area_x=area_y=area_z=40;
		leftTime=70;
		Army_n=10;
	}
	else if(game_state==3){
		area_x=area_y=area_z=60;
		leftTime=200;
		Army_n=20;
	}
	for(j=0;j<area_y;j++)
	{
		for(i=0;i<area_x;i++)
		{
			data[i][j]=0;
			//path[i][j]=0;
		}
	}
	DFS(area_x/2,area_y/2);
	for(i=0;i<area_x;i++)
		for(j=0;j<area_y;j++)
			if(i==area_x-1||j==area_y-1||i==0||j==0)
				data[i][j]=0;
	do
	{
		postion_x=rand()%area_x;
		postion_y=rand()%area_y;
		printf("my:%d,%d\n",postion_x,postion_y);
	}
	while(data[postion_x][postion_y]==0);
	i=0;
	do
	{
		Army_x[i]=rand()%area_x;
		Army_y[i]=rand()%area_y;
		printf("Army%d:%d,%d\n",i,Army_x[i],Army_y[i]);
	}
	while(!(data[Army_x[i]][Army_y[i]]==1&&i++==Army_n));
	data[area_x-1][area_y-2]=1;
}
LRESULT CALLBACK wndproc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam)
{
    static int client_x,client_y,char_x,char_y;
    TEXTMETRIC tm;
    PAINTSTRUCT ps;
    HDC hdc;
    HINSTANCE hinstance;
	char sztext[100];
	HFONT font,oldfont;
	int i,j,k;
	HBRUSH hBrush,holdbrush;
	static int direction[100]={1};
    switch(message)
    {
        case WM_CREATE:
            hinstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
            /*hdc=GetDC(hwnd);
            GetTextMetrics(hdc,&tm);
            char_x=tm.tmAveCharWidth;
            char_y=tm.tmHeight;
            ReleaseDC(hwnd,hdc);*/
            SetClassLong(hwnd,GCL_HICON,(LONG)LoadIcon(hinstance,"IDI_ICON")); //有时候会发现程序的图标是有了,但是在左上角却还是没有加载上,这样做保险点
            SetClassLong(hwnd,GCL_HCURSOR,(LONG)LoadCursor(hinstance,"IDC_CURSOR"));  //与上类似
			return 0;
        case WM_SIZE:
            client_x=LOWORD(lparam);
            client_y=HIWORD(lparam);
            return 0;
        case WM_PAINT:
            hdc=BeginPaint(hwnd,&ps);
			if(game_state<=0){
				font=CreateFont(40,0,0,0,900,FALSE,FALSE,
					0, OEM_CHARSET, OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH, "Segoe Script");
				oldfont=(HFONT)SelectObject(hdc,font);
				SetTextColor(hdc,RGB(255,0,255));

				GetTextMetrics(hdc,&tm);
				char_x=tm.tmAveCharWidth;
				char_y=tm.tmHeight;

				TextOut(hdc,client_x/2-strlen(text[-game_state])*char_x/2,client_y/3-char_y/2,text[-game_state],strlen(text[-game_state]));
				if(game_state==0){
					TextOut(hdc,client_x/2-strlen("START")*char_x/2,client_y*2/3,"START",strlen("START"));
				}else{
					wsprintf(sztext,"Score:%d",(game_state==-1)?2*leftTime+40:(game_state==-2)?leftTime+30:(game_state==-3)?leftTime-100:0);
					TextOut(hdc,client_x/2-strlen(sztext)*char_x/2,client_y/2,sztext,strlen(sztext));
					TextOut(hdc,client_x/2-strlen("CONTINUE")*char_x/2,client_y*2/3,"CONTINUE",strlen("CONTINUE"));
				}
				DeleteObject(SelectObject(hdc,oldfont));
			}
			else if(game_state>0){
				RECT rt;
				GetClientRect(hwnd,&rt);
				SetTextColor(hdc,RGB(0,0,255));
				font=CreateFont(20,0,0,0,900,FALSE,FALSE,
						0, OEM_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH, "Segoe Script");
				oldfont=(HFONT)SelectObject(hdc,font);
				wsprintf(sztext,"Left time:%d",leftTime);
				TextOut(hdc,5,5,sztext,strlen(sztext));
				DeleteObject(SelectObject(hdc,oldfont));

				hBrush=CreateSolidBrush(RGB(0,255,0));
				holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
				Ellipse(hdc,5+10*postion_x,25+10*postion_y,15+10*postion_x,35+10*postion_y);
				SelectObject(hdc,holdbrush);
				DeleteObject(hBrush);
				for(i=0;i<Army_n;i++){
					hBrush=CreateSolidBrush(RGB(255,0,0));
					holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
					Ellipse(hdc,5+10*Army_x[i],25+10*Army_y[i],15+10*Army_x[i],35+10*Army_y[i]);
					SelectObject(hdc,holdbrush);
					DeleteObject(hBrush);
				}

				for(i=0;i<area_x;i++)
					for(j=0;j<area_y;j++)
						if(data[i][j]!=1)
						{
							hBrush=CreateSolidBrush(RGB(255,0,0));
							holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
							Rectangle(hdc,5+10*i,25+10*j,15+10*i,35+10*j);
							SelectObject(hdc,holdbrush);
							DeleteObject(hBrush);
						}
			}
            EndPaint(hwnd,&ps);
            return 0;
		case WM_LBUTTONDOWN:
			if(game_state<=0){
				if(game_state==-4)
					game_state=0;
				else
				{
					game_state=-game_state;
					game_state++;
				}
				print();
				SetTimer(hwnd,IDT_TIMER1,1000,NULL);
				SetTimer(hwnd,IDT_TIMER2,250,NULL);
			}
			InvalidateRect(hwnd,NULL,true);
			return 0;
		case WM_KEYDOWN:
			RECT rt;
			rt.left=5+10*postion_x;
			rt.top=25+10*postion_y;
			rt.right=15+10*postion_x;
			rt.bottom=35+10*postion_y;
			switch(LOWORD(wparam))
			{
			case VK_LEFT:
				if(data[postion_x-1][postion_y]==1)
					postion_x=postion_x-1;
				rt.left-=10;
				break;
			case VK_RIGHT:
				if(data[postion_x+1][postion_y]==1)
					postion_x=postion_x+1;
				rt.right+=10;
				break;
			case VK_UP:
				if(data[postion_x][postion_y-1]==1)
					postion_y=postion_y-1;	
				rt.top-=10;
				break;
			case VK_DOWN:
				if(data[postion_x][postion_y+1]==1)
				postion_y=postion_y+1;
				rt.bottom+=10;
				break;
			}
			if(game_state>0 && postion_x==area_x-1)
			{
				game_state=-game_state;
				KillTimer(hwnd,IDT_TIMER1);
				KillTimer(hwnd,IDT_TIMER2);
				InvalidateRect(hwnd,NULL,true);
			}
			printf("%d,%d\n",postion_x,postion_y);
			InvalidateRect(hwnd,&rt,true);
			//InvalidateRect(hwnd,NULL,true);
			return 0;
		case WM_TIMER:
			if(wparam==IDT_TIMER1)
			{
				leftTime--;
				RECT rt;
				rt.left=50;
				rt.top=5;
				rt.bottom=25;
				rt.right=150;
				InvalidateRect(hwnd,&rt,true);
				if(leftTime==0)
				{
					KillTimer(hwnd,IDT_TIMER1);
					KillTimer(hwnd,IDT_TIMER2);
					game_state=-4;
					InvalidateRect(hwnd,NULL,true);
				}
			}
			if(wparam==IDT_TIMER2){
				RECT rt;
				for(i=0;i<Army_n;){
					rt.left=5+10*Army_x[i];
					rt.top=25+10*Army_y[i];
					rt.right=15+10*Army_x[i];
					rt.bottom=35+10*Army_y[i];
					/*U D
					  U L
					  U R
					  L R
					  L D
					  R D*/
					/*if(	direction[i]==0 ||
					   (data[Army_x[i]][Army_y[i]-1]==1&&data[Army_x[i]][Army_y[i]+1]==1) || 
					   (data[Army_x[i]][Army_y[i]-1]==1&&data[Army_x[i]-1][Army_y[i]]==1) || 
					   (data[Army_x[i]][Army_y[i]-1]==1&&data[Army_x[i]+1][Army_y[i]]==1) || 
					   (data[Army_x[i]-1][Army_y[i]]==1&&data[Army_x[i]+1][Army_y[i]]==1) || 
					   (data[Army_x[i]-1][Army_y[i]]==1&&data[Army_x[i]][Army_y[i]+1]==1) || 
					   (data[Army_x[i]+1][Army_y[i]]==1&&data[Army_x[i]][Army_y[i]+1]==1)
						)*/
					do{
						j=rand()%4;
					}
					while(j==direction[i]);//不走回头路
					direction[i]=j;			//记录上次方向
					
					if(Army_x[i]==postion_x){
						//printf("与%d处于同一横线上\n",i);
						for(k=Army_y[i];k!=postion_y;k>postion_y?k--:k++)//视线
							if(data[postion_x][k]==0)
								break;
						if(k==postion_y){//能看到对方
							printf("被%d发现\n",i);
							if(Army_y[i]>postion_y){//敌人在你右方
								direction[i]=0;//敌人向上
								//printf("敌人向上");
							}
							else{
								direction[i]=1;//敌人向下
								//printf("敌人向下");
							}
						}
					}
					if(Army_y[i]==postion_y){
						//printf("与%d处于同一竖线上\n",i);
						for(k=Army_x[i];k!=postion_x;k>postion_x?k--:k++)//视线
							if(data[k][postion_y]==0)
								break;
						if(k==postion_x){//能看到对方
							printf("被%d发现\n",i);
							if(Army_x[i]>postion_x){//敌人在你右面
								direction[i]=2;//敌人向左
								//printf("敌人向左");
							}
							else{
								direction[i]=3;//敌人向右
								//printf("敌人向右");
							}
						}
					}

					switch(direction[i]){
					case 0://up
						if(data[Army_x[i]][Army_y[i]-1]==1){
							Army_y[i++]--;
							rt.top-=10;
						}
						break;
					case 1://down
						if(data[Army_x[i]][Army_y[i]+1]==1){
							Army_y[i++]++;
							rt.bottom+=10;
						}
						break;
					case 2://left
						if(data[Army_x[i]-1][Army_y[i]]==1){
							Army_x[i++]--;
							rt.left-=10;
						}
						break;
					case 3://right
						if(data[Army_x[i]+1][Army_y[i]]==1){
							Army_x[i++]++;
							rt.right+=10;
						}
						break;
					}
					InvalidateRect(hwnd,&rt,true);
					//if(Army_x[i]==postion_x&&Army_y[i]==postion_y)
						//exit(0);
				}
				for(i=0;i<Army_n;i++)
					if(Army_x[i]==postion_x&&Army_y[i]==postion_y)
						exit(0);
			}

			//InvalidateRect(hwnd,NULL,true);
			return 0; 
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd,message,wparam,lparam);
}
