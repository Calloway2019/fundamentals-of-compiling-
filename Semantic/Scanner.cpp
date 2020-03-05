#include "Scanner.h"
#define LEN 100        //�Ǻ����飬һ���Ǻ������Ϊ100
#pragma warning(disable: 4996) // ����Ҫ�С�����

unsigned int LineNo;//Դ�ļ��к� ��ʼֵΪ1
static FILE *InFile;//�����ļ�
static char TokenBuffer[LEN];//�Ǻ��ַ����壬����Ǻ�

//��ʼ���ʷ�������
extern int InitScanner(const char *FileName)
{
	LineNo = 1;//�к�
	InFile = fopen(FileName, "r");//ֻ����ʽ���ļ�
	if (InFile != NULL) return 1;//�򿪳ɹ�
	else return 0;//��ʧ��
}

//�رմʷ�������
extern void CloseScanner(void)
{
	if (InFile != NULL)
		fclose(InFile);//�ر��ļ�
}

//���ļ��ж����ַ�
static char GetChar(void)
{
	char Char = getc(InFile);//����ַ���
	return toupper(Char);//ȫ��ת��Ϊ��д��Ȼ��return
}

//���ոն������ַ��˻�
static void BackChar(char Char)
{
	if (Char != EOF)
		ungetc(Char, InFile);
}

//�����ַ����ǺŻ�����
static void AddCharTokenString(char Char)
{
	int TokenLength = strlen(TokenBuffer);
	//����Խ��
	if (TokenLength + 1 >= sizeof(TokenBuffer)) return;
	TokenBuffer[TokenLength] = Char;//д���ַ���������
}

//��ռǺŻ�����
static void EmptyTokenString()
{
	// ��������ÿ��Ԫ�ظ���ֵͬ0
	memset(TokenBuffer, 0, LEN);
}

//�жϼǺ��Ƿ��ڼǺű���
static Token JudgeKeyToken(const char *IDString)
{
	int i;
	//sizeof(TokenTab)/sizeof(TokenTab[0],��ʾ������һ���ж��ٸ�Ԫ��
	for (i = 0; i < sizeof(TokenTab) / sizeof(TokenTab[0]); i++)
	{	//����TokenTab��
		if (strcmp(TokenTab[i].lexeme, IDString) == 0)
			//�ж����ַ����оͷ��ظüǺ�
			return TokenTab[i];
	}
	Token errortoken;
	//�Ƚ�errortoken�ÿ�
	memset(&errortoken, 0, sizeof(Token));
	errortoken.type = ERRTOKEN;//Ȼ���������Ǻ�
	return errortoken;//���س���Ǻ�
}

//���һ���Ǻ�
extern Token GetToken(void)
{
	Token token;
	char Char;
	memset(&token, 0, sizeof(Token));//token�ÿ�!!!!
	EmptyTokenString();//��ռǺŻ�����!
	token.lexeme = TokenBuffer;
	for (;;)//��ѭ���������˵�Դ�����еĿո�TAB���س��ȷָ������ļ��������ؿռǺ�
	{
		Char = GetChar();//��Դ������ַ�
		if (Char == EOF)//����
		{
			// ���ؿռǺ�
			token.type = NONTOKEN;
			return token;
		}
		if (Char == '\n')
			LineNo++;//�к�+1
		if (!isspace(Char))//�����ո�üǺſ϶��Ѿ���ɣ��˳�ѭ��
			break;
	}
	AddCharTokenString(Char);//��������������Щ�ָ��������ȼ��뻺����
	if (isalpha(Char))//�����Ӣ����ĸ��һ���Ǻ������ؼ��֡�PI��E��
	{
		for (;;)
		{
			Char = GetChar();
			if (isalnum(Char))//�������ĸ������#<ctype.h> 
				AddCharTokenString(Char);//���뻺����
			else break;
		}
		BackChar(Char);//�˻ػ�����
		token = JudgeKeyToken(TokenBuffer);//�ж��Ƿ��ڼǺű���
		token.lexeme = TokenBuffer;
		return token;
	}
	else if (isdigit(Char))//����
	{
		for (;;)
		{
			Char = GetChar();
			if (isdigit(Char))
				AddCharTokenString(Char);
			else break;
		}
		// �����С��
		if (Char == '.')
		{
			AddCharTokenString(Char);
			//С�����Ժ�����������
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
		//���ַ���ת���ɸ�����
		token.value = atof(TokenBuffer);
		return token;
	}
	else//�������������
	{
		switch (Char)
		{
		case ';': token.type = SEMICO; break;
		case '(': token.type = L_BRACKET; break;
		case ')': token.type = R_BRACKET; break;
		case ',': token.type = COMMA; break;
		case '+': token.type = PLUS; break;
		case '-':
			// ����ǵ�Ŀ�����-
			Char = GetChar();
			if (Char == '-')
			{
				while (Char != '\n' && Char != EOF) Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			else
			{
				// �������ļ���
				BackChar(Char);
				token.type = MINUS;
				break;
			}
		case '/':
			Char = GetChar();
			// ע��
			if (Char == '/')
			{
				while (Char != '\n' && Char != EOF) Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			// ����
			else
			{
				BackChar(Char);
				token.type = DIV;
				break;
			}
		case '*':
			Char = GetChar();
			// �η�
			if (Char == '*')
			{
				token.type = POWER;
				break;
			}
			// �˺�
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