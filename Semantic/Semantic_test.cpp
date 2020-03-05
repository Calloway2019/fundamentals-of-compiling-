#pragma warning(disable:4996)

#include "Semantic.h"
#include <tchar.h> 

#define MAX_CHARS 200

extern HDC hDC;							// ���ھ����ȫ�ֱ���
char SrcFilePath[MAX_CHARS];		// ���ڴ�Ų����ļ�·��
TCHAR Name[] = _T("�����ĺ�����ͼ���Խ�����");	// ������

// ���Դ�����ļ��Ƿ�Ϸ���������
static bool CheckSrcFile(LPSTR);

// ����Windows����
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// �����������
TCHAR szClassName[] = _T("�����ĺ�����ͼ���Խ�����");

// windows ����������
int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nFunsterStil)
{
	HWND hwnd;   //windows ����
	MSG messages;  //���ﱣ�浽���������
	WNDCLASSEX wincl;   // Windows���ݽṹ
	int i;
	// �������ݽṹ
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;  //���������windows����
	wincl.style = CS_DBLCLKS;             // ˫��ץȡ�϶�
	wincl.cbSize = sizeof(WNDCLASSEX);

	//ʹ��Ĭ��ͼ������ָ��
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;      //û�в˵�
	wincl.cbClsExtra = 0;     //��window��֮��û�ж�����ֽ�
	wincl.cbWndExtra = 0;        //�޶���ṹ�򴰿�ʵ��
	//ʹ��Windows��Ĭ����ɫ��Ϊ���ڵı���
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	//ע�ᴰ���࣬���ʧ�ܾ��˳�����
	if (!RegisterClassEx(&wincl))
		return 0;

	//����ע�ᣬ��������
	hwnd = CreateWindowEx(
		0,                   //�޷�����
		szClassName,         
		_T("�����ҵĺ�����ͼ���Խ�����"),   //����
		WS_OVERLAPPEDWINDOW, //Ĭ�ϴ���
		CW_USEDEFAULT,       // ���ھ���λ�� 
		CW_USEDEFAULT,      //��������Ļ��ʲôλ�ý���
		740,                 //���
		490,                 //�߶�
		HWND_DESKTOP,        //������������Ӵ���
		NULL,                //û�в˵�
		hThisInstance,       //ʵ��
		NULL                 //û�д��ڴ�������
	);

	//ʹ��������Ļ�Ͽɼ�
	ShowWindow(hwnd, nFunsterStil);
	hDC = GetDC(hwnd);
	

	//�ļ�·��
	strcpy(SrcFilePath, "Text.txt");

	if (!CheckSrcFile(SrcFilePath)) return 1;

	//���û�ͼ���Խ�����
	//�ʷ��﷨����
	Parser(SrcFilePath);

	//������Ϣѭ��������һֱ���У�ֱ��GetMessage()����0
	while (GetMessage(&messages, NULL, 0, 0))
	{
		// ��������Կ��Ϣת��Ϊ�ַ���Ϣ
		TranslateMessage(&messages);
		// ������Ϣ��WindowProcedure
		DispatchMessage(&messages);
	}
	// ���򷵻�ֵΪ0,��PostQuitMessage()������ֵ
	return messages.wParam;
}


//  ���������Windows����DispatchMessage()����  

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)   //��ȡ��Ϣ
	{
	case WM_DESTROY:
		PostQuitMessage(0);  //����һ��WM QUIT����Ϣ����
		break;
	default:    //���������Ϣ
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
// ���Դ�����ļ��Ƿ�Ϸ�����ʵ��
bool CheckSrcFile(LPSTR lpszCmdParam)
{
	FILE * file = NULL;

	if (strlen(lpszCmdParam) == 0)
	{
		MessageBoxA(NULL, "δָ��Դ�����ļ� !", "����", MB_OK);
		return false;
	}
	if ((file = fopen(lpszCmdParam, "r")) == NULL)
	{
		MessageBoxA(NULL, "��Դ�����ļ����� !", "����", MB_OK);
		MessageBoxA(NULL, lpszCmdParam, "�ļ���", MB_OK);
		return false;
	}
	else fclose(file);
	return true;
}