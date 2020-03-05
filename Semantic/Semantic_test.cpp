#pragma warning(disable:4996)

#include "Semantic.h"
#include <tchar.h> 

#define MAX_CHARS 200

extern HDC hDC;							// 窗口句柄，全局变量
char SrcFilePath[MAX_CHARS];		// 用于存放测试文件路径
TCHAR Name[] = _T("卫彪彪的函数绘图语言解释器");	// 窗口名

// 检查源程序文件是否合法函数声明
static bool CheckSrcFile(LPSTR);

// 声明Windows程序
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// 定义程序名称
TCHAR szClassName[] = _T("卫彪彪的函数绘图语言解释器");

// windows 下面主程序
int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nFunsterStil)
{
	HWND hwnd;   //windows 的类
	MSG messages;  //这里保存到程序的内容
	WNDCLASSEX wincl;   // Windows数据结构
	int i;
	// 具体数据结构
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;  //这个函数由windows调用
	wincl.style = CS_DBLCLKS;             // 双击抓取拖动
	wincl.cbSize = sizeof(WNDCLASSEX);

	//使用默认图标和鼠标指针
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;      //没有菜单
	wincl.cbClsExtra = 0;     //在window类之后没有额外的字节
	wincl.cbWndExtra = 0;        //无额外结构或窗口实例
	//使用Windows的默认颜色作为窗口的背景
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	//注册窗口类，如果失败就退出程序
	if (!RegisterClassEx(&wincl))
		return 0;

	//类已注册，创建程序
	hwnd = CreateWindowEx(
		0,                   //无法扩大
		szClassName,         
		_T("戴红岩的函数绘图语言解释器"),   //名字
		WS_OVERLAPPEDWINDOW, //默认窗口
		CW_USEDEFAULT,       // 窗口决定位置 
		CW_USEDEFAULT,      //窗口在屏幕的什么位置结束
		740,                 //宽度
		490,                 //高度
		HWND_DESKTOP,        //窗口是桌面的子窗口
		NULL,                //没有菜单
		hThisInstance,       //实例
		NULL                 //没有窗口创建数据
	);

	//使窗口在屏幕上可见
	ShowWindow(hwnd, nFunsterStil);
	hDC = GetDC(hwnd);
	

	//文件路径
	strcpy(SrcFilePath, "Text.txt");

	if (!CheckSrcFile(SrcFilePath)) return 1;

	//调用绘图语言解释器
	//词法语法分析
	Parser(SrcFilePath);

	//运行消息循环。它将一直运行，直到GetMessage()返回0
	while (GetMessage(&messages, NULL, 0, 0))
	{
		// 将虚拟密钥消息转换为字符消息
		TranslateMessage(&messages);
		// 发送消息到WindowProcedure
		DispatchMessage(&messages);
	}
	// 程序返回值为0,是PostQuitMessage()给出的值
	return messages.wParam;
}


//  这个函数由Windows函数DispatchMessage()调用  

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)   //提取信息
	{
	case WM_DESTROY:
		PostQuitMessage(0);  //发送一个WM QUIT到消息队列
		break;
	default:    //不处理的信息
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
// 检查源程序文件是否合法函数实现
bool CheckSrcFile(LPSTR lpszCmdParam)
{
	FILE * file = NULL;

	if (strlen(lpszCmdParam) == 0)
	{
		MessageBoxA(NULL, "未指定源程序文件 !", "错误", MB_OK);
		return false;
	}
	if ((file = fopen(lpszCmdParam, "r")) == NULL)
	{
		MessageBoxA(NULL, "打开源程序文件出错 !", "错误", MB_OK);
		MessageBoxA(NULL, lpszCmdParam, "文件名", MB_OK);
		return false;
	}
	else fclose(file);
	return true;
}