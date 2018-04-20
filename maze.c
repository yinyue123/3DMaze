/*hello.c*/
#include <windows.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib");
#define IDT_TIMER1 1
HWND hwnd;
int game_state=0;
int couple=0;
/*
 0:未初始化
 1:服务端
 2:客户端
 */
int leftTime=0;
int area_x,area_y,area_z;
int position_x=0,position_y=0,position_z=0;
int client_x=0,client_y=0,client_z=0;
int Army_n,Army_x[100],Army_y[100],Army_z[100];
//int data[100][100];
char ip[16]={0};
int data[100][100][100];

char text[][100]={
	"Escape from the prison maze!",
	"Congratulations!You pass the primary level!",
	"Congratulations!You pass the junior level!",
	"Congratulations!You pass the senior level!",
	"Time Up!Fail to pass the game!"
};
SOCKET sock;
int nAddrLen;
char sendData[65535];
char recvData[65535];

LRESULT CALLBACK wndproc(HWND,UINT,WPARAM,LPARAM);
void init_wndclass(HINSTANCE,TCHAR*,WNDCLASS*);
DWORD WINAPI Network(LPVOID);
void Send();

int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hprevinstance,LPSTR line,int cmd)
{
    TCHAR app_name[]=TEXT("Escape from the prison maze!");
    WNDCLASS wndclass;
    MSG msg;
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

/**三位迷宫生成*/
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

/**二维迷宫生成*/
/*
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
*/

void print()
{
	int i,j,k;

	if(couple==1){
		for(i=0;i<area_x;i++)
			for(j=0;j<area_y;j++)
				for(k=0;k<area_z;k++)
					data[i][j][k]=0;
					//path[i][j]=0;
		srand((int)time(NULL));
		if(game_state==1){
			area_x=area_y=20;
			area_z=7;
			Army_n=3;
			leftTime=300;
		}
		else if(game_state==2){
			area_x=area_y=area_z=40;
			area_z=10;
			leftTime=700;
			Army_n=10;
		}
		else if(game_state==3){
			area_x=area_y=area_z=60;
			area_z=15;
			Army_n=100;
			leftTime=2000;
		}
		DFS(area_x/2,area_y/2,area_z/2);					//生成迷宫
		for(i=0;i<area_x;i++)
			for(j=0;j<area_y;j++)
				for(k=0;k<area_z;k++)
					if(i==area_x-1||j==area_y-1||k==area_z-1||i==0||j==0||k==0)
						data[i][j][k]=0;

		data[area_x-2][area_y-2][area_z-2]=1;				//生成迷宫出口,在(area_x,area_y-1,area_z)处
	}
	printf("test-lefttime:%d\n",leftTime);
	while(!leftTime)
		Sleep(1);
/*		for(i=0;i<area_x;i++){
			for(j=0;j<area_y;j++){
				for(k=0;k<area_z;k++){
					printf("%d",data[i][j][k]);
				}
				printf("\n");
			}
			printf("\n");
		}*/
	do
	{
		position_x=rand()%area_x;
		position_y=rand()%area_y;
		position_z=rand()%area_z;
		printf("start position:%d,%d,%d\n",position_x,position_y,position_z);
	}
	while(data[position_x][position_y][position_z]==0);	//上面几行让小球开始的位置随机
	sprintf(sendData,"2%d,%d,%d",position_x,position_y,position_z);
	Send();
}

LRESULT CALLBACK wndproc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam)
{
    static int client_X,client_Y,char_x,char_y;
    TEXTMETRIC tm;
    PAINTSTRUCT ps;
    HDC hdc;
    HINSTANCE hinstance;
	char sztext[100];
	HFONT font,oldfont;
	int i,j,k;
	HBRUSH hBrush,holdbrush;
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
            client_X=LOWORD(lparam);
            client_Y=HIWORD(lparam);
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

				TextOut(hdc,client_X/2-strlen(text[-game_state])*char_x/2,client_Y/3-char_y/2,text[-game_state],strlen(text[-game_state]));
				if(game_state==0){
					if(strlen(ip)==0)
						TextOut(hdc,client_X/2-strlen("By:YinYue")*char_x/2,client_Y/2,"By:YinYue",strlen("By:YinYue"));
					else
						TextOut(hdc,client_X/2-strlen(ip)*char_x/2,client_Y/2,ip,strlen(ip));
					TextOut(hdc,client_X/2-strlen("START")*char_x/2,client_Y*2/3,"START",strlen("START"));
				}else{
					wsprintf(sztext,"Score:%d",(game_state==-1)?2*leftTime/10+40:(game_state==-2)?leftTime/10+30:(game_state==-3)?leftTime/10-100:0);
					TextOut(hdc,client_X/2-strlen(sztext)*char_x/2,client_Y/2,sztext,strlen(sztext));
					TextOut(hdc,client_X/2-strlen("CONTINUE")*char_x/2,client_Y*2/3,"CONTINUE",strlen("CONTINUE"));
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
				wsprintf(sztext,"(%d,%d,%d) (%d,%d,%d)  Left time:%d",position_x,position_y,position_z,client_x,client_y,client_z,leftTime);
				TextOut(hdc,5,5,sztext,strlen(sztext));
				DeleteObject(SelectObject(hdc,oldfont));
				//[x][y][position_z]
				hBrush=CreateSolidBrush(RGB(0,255,0));
				holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
				Ellipse(hdc,5+10*position_x,25+10*position_y,15+10*position_x,35+10*position_y);
				SelectObject(hdc,holdbrush);
				DeleteObject(hBrush);
				if(position_z==client_z){
					hBrush=CreateSolidBrush(RGB(0,0,255));
					holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
					Ellipse(hdc,5+10*client_x,25+10*client_y,15+10*client_x,35+10*client_y);
					SelectObject(hdc,holdbrush);
					DeleteObject(hBrush);
				}

				for(i=0;i<area_x;i++)
					for(j=0;j<area_y;j++)
						if(data[i][j][position_z]!=1)
						{
							hBrush=CreateSolidBrush(RGB(255,0,0));
							holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
							Rectangle(hdc,5+10*i,25+10*j,15+10*i,35+10*j);
							SelectObject(hdc,holdbrush);
							DeleteObject(hBrush);
						}
				//[x][position_y][z]  主图右边
				hBrush=CreateSolidBrush(RGB(0,255,0));
				holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
				Ellipse(hdc,5+10*position_x,25+50+10*area_x+10*position_z,15+10*position_x,35+50+10*area_x+10*position_z);
				SelectObject(hdc,holdbrush);
				DeleteObject(hBrush);
				if(position_y==client_y){
					hBrush=CreateSolidBrush(RGB(0,0,255));
					holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
					Ellipse(hdc,5+10*client_x,25+50+10*area_x+10*client_z,15+10*client_x,35+50+10*area_x+10*client_z);
					SelectObject(hdc,holdbrush);
					DeleteObject(hBrush);
				}

				for(i=0;i<area_x;i++)
					for(j=0;j<area_z;j++)
						if(data[i][position_y][j]!=1)
						{
							hBrush=CreateSolidBrush(RGB(255,0,0));
							holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
							Rectangle(hdc,5+10*i,25+50+10*area_x+10*j,15+10*i,35+50+10*area_x+10*j);
							SelectObject(hdc,holdbrush);
							DeleteObject(hBrush);
						}
				//[position_x][y][z] 主图下边
				hBrush=CreateSolidBrush(RGB(0,255,0));
				holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
				Ellipse(hdc,5+50+10*area_y+10*position_z,25+10*position_y,15+50+10*area_y+10*position_z,35+10*position_y);
				SelectObject(hdc,holdbrush);
				DeleteObject(hBrush);
				if(position_x==client_x){
					hBrush=CreateSolidBrush(RGB(0,0,255));
					holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
					Ellipse(hdc,5+50+10*area_y+10*client_z,25+10*client_y,15+50+10*area_y+10*client_z,35+10*client_y);
					SelectObject(hdc,holdbrush);
					DeleteObject(hBrush);
				}

				for(i=0;i<area_y;i++)
					for(j=0;j<area_z;j++)
						if(data[position_x][i][j]!=1)
						{
							hBrush=CreateSolidBrush(RGB(255,0,0));
							holdbrush=(HBRUSH)SelectObject(hdc,hBrush);
							Rectangle(hdc,5+50+10*area_y+10*j,25+10*i,15+50+10*area_y+10*j,35+10*i);
							SelectObject(hdc,holdbrush);
							DeleteObject(hBrush);
						}
			//End
			}
            EndPaint(hwnd,&ps);
            return 0;
		case WM_LBUTTONDOWN:
			if(game_state<=0){
				if(game_state==0){
					CreateThread(
						NULL	//default security attributes
						,0		//use default stack size
						,Network//thread function
						,NULL	//argument to thread function
						,0		//use default creation flags
						,NULL	//returns the thread identifier
						);
				}
				if(game_state==-4)
					game_state=0;
				else
				{
					game_state=-game_state;
					game_state++;
				}
				while(!couple)
					Sleep(1);
				print();
				SetTimer(hwnd,IDT_TIMER1,1000,NULL);
			}
			InvalidateRect(hwnd,NULL,true);
			return 0;
		case WM_KEYDOWN:
			RECT rt_x;
			RECT rt_y;
			RECT rt_z;
			rt_x.left=5+10*position_x;
			rt_x.top=25+10*position_y;
			rt_x.right=15+10*position_x;
			rt_x.bottom=35+10*position_y;
			rt_y.left=5+50+10*area_y+10*position_z;
			rt_y.top=25+10*position_y;
			rt_y.right=15+50+10*area_y+10*position_z;
			rt_y.bottom=35+10*position_y;
			rt_z.left=5+10*position_x;
			rt_z.top=25+50+10*area_x+10*position_z;
			rt_z.right=15+10*position_x;
			rt_z.bottom=35+50+10*area_x+10*position_z;
			//printf("%c\n,%d",LOWORD(wparam),LOWORD(wparam));
			switch(LOWORD(wparam))							//处理按键消息
			{
			//VK_LEFT:向左	VK_RIGHT:向右	VK_UP:向上键	VK_DOWN:向下键
			case 'A':
				if(data[position_x-1][position_y][position_z]==1)
					position_x=position_x-1;
				else
					return 0;
				rt_x.left-=10;
				rt_y.left=5+50+10*area_x;
				rt_y.top=25;
				rt_y.right=5+50+10*area_x+10*area_z;
				rt_y.bottom=25+10*area_y;
				rt_z.left-=10;
				break;
			case 'D':
				if(data[position_x+1][position_y][position_z]==1)
					position_x=position_x+1;
				else
					return 0;
				rt_x.right+=10;
				rt_y.left=5+50+10*area_x;
				rt_y.top=25;
				rt_y.right=5+50+10*area_x+10*area_z;
				rt_y.bottom=25+10*area_y;
				rt_z.right+=10;
				break;
			case 'W':
				if(data[position_x][position_y-1][position_z]==1)
					position_y=position_y-1;
				else
					return 0;
				rt_x.top-=10;
				rt_y.top-=10;
				rt_z.left=5;
				rt_z.top=25+50+10*area_y;
				rt_z.right=5+10*area_x;
				rt_z.bottom=25+50+10*area_y+10*area_z;
				break;
			case 'S':
				if(data[position_x][position_y+1][position_z]==1)
					position_y=position_y+1;
				else
					return 0;
				rt_x.bottom+=10;
				rt_y.bottom+=10;
				rt_z.left=5;
				rt_z.top=25+50+10*area_y;
				rt_z.right=5+10*area_x;
				rt_z.bottom=25+50+10*area_y+10*area_z;
				break;
			case 'Q':
				if(data[position_x][position_y][position_z-1]==1)
					position_z=position_z-1;
				else
					return 0;
				rt_x.left=5;
				rt_x.top=25;
				rt_x.right=5+10*area_x;
				rt_x.bottom=25+10*area_y;
				rt_y.left-=10;
				rt_z.top-=10;
				break;
			case 'E':
				if(data[position_x][position_y][position_z+1]==1)
					position_z=position_z+1;
				else
					return 0;
				rt_x.left=5;
				rt_x.top=25;
				rt_x.right=5+10*area_x;
				rt_x.bottom=25+10*area_y;
				rt_y.right+=10;
				rt_z.bottom+=10;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 190:
				if(strlen(ip)==15)
					return 0;
				if(LOWORD(wparam)==190)
					ip[strlen(ip)]='.';
				else
					ip[strlen(ip)]=LOWORD(wparam);
				InvalidateRect(hwnd,NULL,true);
				//break;
				return 0;
			case VK_BACK:
				ip[strlen(ip)-1]=0;
				InvalidateRect(hwnd,NULL,true);
				//break;
				return 0;
			default:
				return 0;
			}
			sprintf(sendData,"2%d,%d,%d",position_x,position_y,position_z);
			Send();
			if(game_state>0 && position_x==area_x-1)		//判断正在游戏中并且位置出迷宫,则游戏胜利
			{
				game_state=-game_state;
				KillTimer(hwnd,IDT_TIMER1);
				InvalidateRect(hwnd,NULL,true);
			}
			//printf("%d,%d,%d\n",position_x,position_y,position_z);
			InvalidateRect(hwnd,&rt_x,true);				//防止迷宫闪烁，只刷新小球和下个位置
			InvalidateRect(hwnd,&rt_y,true);				//防止迷宫闪烁，只刷新小球和下个位置
			InvalidateRect(hwnd,&rt_z,true);				//防止迷宫闪烁，只刷新小球和下个位置
			//InvalidateRect(hwnd,NULL,true);			//全屏重绘
			return 0;
		case WM_TIMER:
			if(wparam==IDT_TIMER1)
			{
				leftTime--;
				RECT rt;
				rt.left=5;
				rt.top=5;
				rt.bottom=25;
				rt.right=260;
				InvalidateRect(hwnd,&rt,true);			//重绘时间位置
				if(leftTime==0)
				{
					KillTimer(hwnd,IDT_TIMER1);
					game_state=-4;
					InvalidateRect(hwnd,NULL,true);
				}
			}

			return 0; 
        case WM_DESTROY:
			sprintf(sendData,"3");
			Send();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd,message,wparam,lparam);
}

sockaddr_in remoteAddr;

void Send(){
	printf("send:%s\n",sendData);
	sendto(sock,sendData,strlen(sendData),0,(sockaddr *)&remoteAddr,nAddrLen);
}

int act(){
	char* data_s=sendData;
	char* data_r=recvData;
	int i,j,k;
	if(couple==1)
		switch(*data_r++){	//server
		case '1':	//1.地图时间信息
			couple=1;
			*data_s++='1';
			sprintf(data_s,"%d,%d,%d,",area_x,area_y,area_z);
			for(i=0;i!=3;)
				if(*data_s++==',')
					i++;
			for(i=0;i<area_x;i++)
				for(j=0;j<area_y;j++)
					for(k=0;k<area_z;k++)
						*data_s++=(data[i][j][k]==1?'1':'0');
			//*data_s++='2';
			sprintf(data_s,"%d",leftTime);
			Send();
			sprintf(sendData,"2%d,%d,%d",position_x,position_y,position_z);
			Send();
			break;
		case '2':	//2.位置信息
			sscanf(data_r,"%d,%d,%d",&client_x,&client_y,&client_z);
			if(client_x==position_x || client_y==position_y || client_z==position_z)
				InvalidateRect(hwnd,NULL,true);			//重绘窗口
			break;
		case '3':	//3.退出游戏		case '3':	//3.退出游戏
			printf("exit");
			PostQuitMessage(0);
			exit(0);
			break;
		}
	else{
		switch(*data_r++){	//client
		case '1':	//1.加入游戏
			sscanf(data_r,"%d,%d,%d,",&area_x,&area_y,&area_z);
			for(i=0;i!=3;)
				if(*data_r++==',')
					i++;
			for(i=0;i<area_x;i++)
				for(j=0;j<area_y;j++)
					for(k=0;k<area_z;k++)
						data[i][j][k]=(*data_r++=='1'?1:0);
				sscanf(data_r,"%d",&leftTime);
				InvalidateRect(hwnd,NULL,true);			//全屏重绘
		case '2':	//2.位置移动
			sscanf(data_r,"%d,%d,%d",&client_x,&client_y,&client_z);
			if(client_x==position_x || client_y==position_y || client_z==position_z)
				InvalidateRect(hwnd,NULL,true);			//全屏重绘
			break;
		case '3':	//3.退出游戏
			printf("exit");
			PostQuitMessage(0);
			exit(0);
			break;
		}
	}	
}

DWORD WINAPI Network(LPVOID lpParam){
	WSADATA wsaData;
	WORD socketVersion=MAKEWORD(2,2);
	printf("ip%s\n",ip);	//输出界面输入的ip
	if(WSAStartup(socketVersion,&wsaData)!=0){
		return 0;
	}
	sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(sock==INVALID_SOCKET){
		printf("socket error!\n");
		return 0;
	}
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(8844);
	if(strlen(ip)==0)//server
		addr.sin_addr.S_un.S_addr=INADDR_ANY;	//Server
	else
		addr.sin_addr.S_un.S_addr=inet_addr(ip);//Client
	if(strlen(ip)==0)//server
		if(bind(sock,(sockaddr *)&addr,sizeof(addr))==SOCKET_ERROR){//server
			printf("bind error!\n");
			closesocket(sock);
			return 0;

		}
	
	nAddrLen=sizeof(remoteAddr);	//server远程ip地址大小*/
	
	if(strlen(ip)==0){	//server
		couple=1;		//收到客户数据
	}else{				//client
		couple=2;
		remoteAddr=addr;
		sprintf(sendData,"1");
		Send();
	}
/*	while(true){
		printf("scanf:");
		scanf("%s",recvData);
		printf("recv:%s\n",recvData);
		act();
	}*/
	while(true){
		int ret=recvfrom(sock,recvData,sizeof(recvData),0,(sockaddr *)&remoteAddr,&nAddrLen);
		if(ret>0)
		{
			recvData[ret]=0x00;
			printf("recv:%s\n",recvData);
			act();
		}
	}
}

