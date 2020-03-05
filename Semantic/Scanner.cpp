#include "Scanner.h"
#define LEN 100        //记号数组，一个记号最长限制为100
#pragma warning(disable: 4996) // 必须要有。。。

unsigned int LineNo;//源文件行号 初始值为1
static FILE *InFile;//输入文件
static char TokenBuffer[LEN];//记号字符缓冲，缓存记号

//初始化词法分析器
extern int InitScanner(const char *FileName)
{
	LineNo = 1;//行号
	InFile = fopen(FileName, "r");//只读形式打开文件
	if (InFile != NULL) return 1;//打开成功
	else return 0;//打开失败
}

//关闭词法分析器
extern void CloseScanner(void)
{
	if (InFile != NULL)
		fclose(InFile);//关闭文件
}

//从文件中读入字符
static char GetChar(void)
{
	char Char = getc(InFile);//逐个字符读
	return toupper(Char);//全部转换为大写，然后return
}

//将刚刚读到的字符退回
static void BackChar(char Char)
{
	if (Char != EOF)
		ungetc(Char, InFile);
}

//加入字符到记号缓冲区
static void AddCharTokenString(char Char)
{
	int TokenLength = strlen(TokenBuffer);
	//数组越界
	if (TokenLength + 1 >= sizeof(TokenBuffer)) return;
	TokenBuffer[TokenLength] = Char;//写入字符到缓冲区
}

//请空记号缓冲区
static void EmptyTokenString()
{
	// 对数组中每个元素赋相同值0
	memset(TokenBuffer, 0, LEN);
}

//判断记号是否在记号表中
static Token JudgeKeyToken(const char *IDString)
{
	int i;
	//sizeof(TokenTab)/sizeof(TokenTab[0],表示数组中一共有多少个元素
	for (i = 0; i < sizeof(TokenTab) / sizeof(TokenTab[0]); i++)
	{	//遍历TokenTab表
		if (strcmp(TokenTab[i].lexeme, IDString) == 0)
			//判断在字符表中就返回该记号
			return TokenTab[i];
	}
	Token errortoken;
	//先将errortoken置空
	memset(&errortoken, 0, sizeof(Token));
	errortoken.type = ERRTOKEN;//然后填入出错记号
	return errortoken;//返回出错记号
}

//获得一个记号
extern Token GetToken(void)
{
	Token token;
	char Char;
	memset(&token, 0, sizeof(Token));//token置空!!!!
	EmptyTokenString();//清空记号缓冲区!
	token.lexeme = TokenBuffer;
	for (;;)//此循环用来过滤掉源程序中的空格、TAB、回车等分隔符，文件结束返回空记号
	{
		Char = GetChar();//从源程序读字符
		if (Char == EOF)//读空
		{
			// 返回空记号
			token.type = NONTOKEN;
			return token;
		}
		if (Char == '\n')
			LineNo++;//行号+1
		if (!isspace(Char))//遇到空格该记号肯定已经完成，退出循环
			break;
	}
	AddCharTokenString(Char);//如果不是上面的那些分隔符，就先加入缓冲区
	if (isalpha(Char))//如果是英文字母，一定是函数、关键字、PI、E等
	{
		for (;;)
		{
			Char = GetChar();
			if (isalnum(Char))//如果是字母或数字#<ctype.h> 
				AddCharTokenString(Char);//加入缓冲区
			else break;
		}
		BackChar(Char);//退回缓冲区
		token = JudgeKeyToken(TokenBuffer);//判断是否在记号表中
		token.lexeme = TokenBuffer;
		return token;
	}
	else if (isdigit(Char))//数字
	{
		for (;;)
		{
			Char = GetChar();
			if (isdigit(Char))
				AddCharTokenString(Char);
			else break;
		}
		// 如果是小数
		if (Char == '.')
		{
			AddCharTokenString(Char);
			//小数点以后的如果还是数
			for (;;)
			{
				Char = GetChar();
				if (isdigit(Char))
					AddCharTokenString(Char);
				else break;
			}
		}
		BackChar(Char);
		token.type = CONST_ID;
		//把字符串转换成浮点数
		token.value = atof(TokenBuffer);
		return token;
	}
	else//如果是其他符号
	{
		switch (Char)
		{
		case ';': token.type = SEMICO; break;
		case '(': token.type = L_BRACKET; break;
		case ')': token.type = R_BRACKET; break;
		case ',': token.type = COMMA; break;
		case '+': token.type = PLUS; break;
		case '-':
			// 如果是单目运算符-
			Char = GetChar();
			if (Char == '-')
			{
				while (Char != '\n' && Char != EOF) Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			else
			{
				// 是正常的减号
				BackChar(Char);
				token.type = MINUS;
				break;
			}
		case '/':
			Char = GetChar();
			// 注释
			if (Char == '/')
			{
				while (Char != '\n' && Char != EOF) Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			// 除号
			else
			{
				BackChar(Char);
				token.type = DIV;
				break;
			}
		case '*':
			Char = GetChar();
			// 次方
			if (Char == '*')
			{
				token.type = POWER;
				break;
			}
			// 乘号
			else
			{
				BackChar(Char);
				token.type = MUL;
				break;
			}
		default:token.type = ERRTOKEN; break;
		}
	}
	return token;
}