#include"Parser.h"
#pragma warning(disable: 4996) // ������Ϣ��strcpy
#pragma warning(disable: 4703) // ������Ϣ��ʹ���˿���δ��ʼ���ı���ָ�������address��

double Parameter = 0;

static Token token;//����һ���Ǻ�

//���ֺ�������
//���ôʷ���������GetToken���ѵõ��ĵ�ǰ��¼����������
//����õ��ļǺ��ǷǷ�����errtoken����ָ��һ���﷨����
static void getToken();

//ƥ�䵱ǰ�Ǻ�
static void MatchToken(enum Token_Type The_Token);

//�����﷨������ӳ��򡣸��ݴ�������ʴ�ӡ�����Ϣ������ֹ�������С�
//�������ʿ��Ը��ݴ��β�ͬ�����֣�SyntaxError��1���ʷ���   SyntaxError��2���﷨��
static void SyntaxError(int case_of);

//��ӡ������Ϣ
static void ErrMsg(unsigned LineNo, char *descrip, char *string);

//���������ӡ�﷨��
//���ս���ݹ��ӳ������� ��2��
//��һ�������Ͳ�ƥ��
static void PrintSyntaxTree(struct ExprNode *root, int indent);
static void Program();//����
static void Statement();//���
static void OriginStatement();//Origin���
static void RotStatement();//Rot���
static void ScaleStatement();//Scale���
static void ForStatement();//For���

//��2�࣬�﷨����+�����﷨������˱��ʽ�����Ϊ����ֵΪָ���﷨���ڵ��ָ��ĺ�����
static struct ExprNode *Expression();//���ʽ����Ԫ�Ӽ�������ʽ
static struct ExprNode *Term();//�˳�������ʽ

static struct ExprNode *Factor();//һԪ�Ӽ�������ʽ
//��������Ӷ������������˼Ӽ�����˳��ŵ����ȼ����⡣���⼸�����̵ķ��������У�ʼ�մ���fsys������ֵ����֤�����ڳ�����������������ķ��ţ�ʹ�������̵��Խ�����ȥ��
static struct ExprNode *Component();//��������ʽ
static struct ExprNode *Atom();//ԭ�ӱ��ʽ

//����ӿ�����
extern void Parser(char *SrcFilePtr);

//�﷨�����캯����������ε�
static struct ExprNode *MakeExprNode(enum Token_Type opcode, ...);

//ʵ�ֲ���
//ͨ���ʷ��������ӿ�GetToken���һ���Ǻ�
static void getToken()
{
	token = GetToken();
	// ����õ��ļǺ��ǷǷ�����errtoken���ǵ�һ���﷨����
	if (token.type == ERRTOKEN) SyntaxError(1); 
}

//ƥ�䵱ǰ�Ǻ�
static void MatchToken(enum Token_Type The_Token)
{
	if (token.type != The_Token)
	{
		SyntaxError(2);//��ʧ�ܣ��ڶ����﷨����
	}
	getToken();//���ɹ�����ȡ��һ��
}

//�﷨������
static void SyntaxError(int case_of)
{
	switch (case_of)
	{
	case 1: ErrMsg(LineNo, (char*)"����Ǻ� ", token.lexeme);
		break;
	case 2: ErrMsg(LineNo, (char*)"����Ԥ�ڼǺ�", token.lexeme);
		break;
	}
}

//��ӡ������Ϣ����������Ҫ�õ�
void ErrMsg(unsigned LineNo, char *descrip, char *string)
{
	char msg[256];
	// ��������ȫ�����0
	memset(msg, 0, 256);

	//�Ѹ�ʽ��������д���ַ�����
	sprintf(msg, "Line No %5d:%s %s !", LineNo, descrip, string);

	CloseScanner();
	//�رճ���
	exit(1);
}

//���������ӡ�﷨������-��-��
// intend:����
void PrintSyntaxTree(struct ExprNode *root, int indent)
{
	int temp;
	for (temp = 1; temp <= indent; temp++) printf("\t");//����
	switch (root->OpCode)
	{
	case PLUS:		printf("%s\n", "+");	break;
	case MINUS:		printf("%s\n", "-");	break;
	case MUL:		printf("%s\n", "*");	break;
	case DIV:		printf("%s\n", "/");	break;
	case POWER:		printf("%s\n", "**");	break;
	// ����Ǻ�������sin֮��ģ���ӡ����
	case FUNC:		printf("%x\n", root->Content.CaseFunc.MathFuncPtr);	break;
	// ����ǳ�������pi֮��ģ���ӡ������ַ��
	case CONST_ID:	printf("%f\n", root->Content.CaseConst);	break;
	case T:			printf("%s\n", "T");	break;
	default:		printf("Error Tree Node !\n");	exit(0);
	}
	//�����Ͳ���ֻ��Ҷ�ӽڵ� 
	if (root->OpCode == CONST_ID || root->OpCode == T) //Ҷ�ӽڵ㷵��
		return;

	//����Ǻ��������ݹ��ӡһ�����ӽڵ�
	if (root->OpCode == FUNC)
		PrintSyntaxTree(root->Content.CaseFunc.Child, indent + 1);//�����к��ӽڵ��ָ��������ָ��
	else//�ݹ��ӡ�������ӽڵ�
	{//��Ԫ���㣺���Һ��ӵ��ڲ��ڵ�
		PrintSyntaxTree(root->Content.CaseOperater.Left, indent + 1);
		PrintSyntaxTree(root->Content.CaseOperater.Right, indent + 1);
	}
}
//----------ɾ��һ���﷨��
void DelExprTree(struct ExprNode *root)
{
	if (root == NULL) return;
	switch (root->OpCode)
	{
	//��Ԫ�����������ӵ��ڲ��ڵ�
	case PLUS:
	case MINUS:
	case MUL:
	case DIV:
	case POWER:
		DelExprTree(root->Content.CaseOperater.Left);
		DelExprTree(root->Content.CaseOperater.Right);
		break;
	case FUNC:
		DelExprTree(root->Content.CaseFunc.Child);//һԪ����һ�����ӵ��ڲ��ڵ�
		break;
	default:
		break;
	}
	delete(root);
}
//��ͼ���Խ�������ڣ�����������ⲿ�ӿڣ�
void Parser(char *SrcFilePtr)//�﷨�����������
{
	if (!InitScanner(SrcFilePtr))//��ʼ���ʷ�������
	{
		printf("Open Source File Error !\n"); return;
	}

	getToken();//�Ȼ��һ���Ǻ�
	Program();//Ȼ�����Program�ݹ��ӳ��򣬵ݹ��½�����
	CloseScanner();//�رմʷ�������
	return;
}

//����Program�ĵݹ��ӳ���
static void Program()
{
	while (token.type != NONTOKEN)//�Ǻ����Ͳ�Ϊ��
	{
		//����ĵ�һ�����
		Statement();
		MatchToken(SEMICO);//ֱ��ƥ�䵽�ָ���
	}
}

//���Statment�ĵݹ��ӳ���
static void Statement()
{
	switch (token.type)
	{
		//��������������ͣ��ֱ����
	case ORIGIN: OriginStatement(); break;
	case SCALE: ScaleStatement(); break;
	case ROT:  RotStatement(); break;
	case FOR: ForStatement(); break;
	default: SyntaxError(2); //���򱨴�
	}
}

//���OriginStatement�ĵݹ��ӳ���
// ���� origin is ( ,  ); 
static void OriginStatement(void)
{
	struct ExprNode *tmp;//�﷨���ڵ������
	MatchToken(ORIGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET); //(

	tmp = Expression();  
	PrintSyntaxTree(tmp, 1);
	DelExprTree(tmp);//ɾ��һ����

	MatchToken(COMMA);  // ,

	tmp = Expression();   
	PrintSyntaxTree(tmp, 1);
	DelExprTree(tmp);//ɾ��һ����

	MatchToken(R_BRACKET);
}

//���ScaleStatement�ĵݹ��ӳ���
static void ScaleStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(SCALE);
	MatchToken(IS);
	MatchToken(L_BRACKET);

	tmp = Expression();  
	PrintSyntaxTree(tmp, 1);
	DelExprTree(tmp);

	MatchToken(COMMA);

	tmp = Expression();    
	PrintSyntaxTree(tmp, 1);
	DelExprTree(tmp);

	MatchToken(R_BRACKET);
}

//���RotStatement�ĵݹ��ӳ���
static void RotStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(ROT);
	MatchToken(IS);

	tmp = Expression();  
	PrintSyntaxTree(tmp, 1);
	DelExprTree(tmp);
}

//���ForStatement�ĵݹ��ӳ���
//���Ҳ��ķ����ŵ�չ��->�����ս����ֱ��ƥ�䣬�������ս���͵�����Ӧ�ӳ���
//ForStatement��Ψһ�ķ��ս����Expression����������5����ͬλ�ã��ֱ����ѭ������ʼ����ֹ�������������ꡢ�����꣬��Ҫ5�����ڵ�ָ�뱣����5���﷨��
static void ForStatement(void)
{
	//eg:for T from 0 to 2*pi step pi/50 draw (t, -sin(t));
	double Start, End, Step;//��ͼ��㡢�յ㡢����
	struct ExprNode *start_ptr, *end_ptr, *step_ptr, *x_ptr, *y_ptr;

	MatchToken(FOR);
	MatchToken(T);
	MatchToken(FROM);//eg:for T from

	start_ptr = Expression(); 
	PrintSyntaxTree(start_ptr, 1);//��ò��������ʽ���﷨��
	DelExprTree(start_ptr);//�ͷŲ�������﷨����ռ�ռ�

	MatchToken(TO);

	end_ptr = Expression(); 
	PrintSyntaxTree(end_ptr, 1);//��������յ���ʽ�﷨��
	DelExprTree(end_ptr);//�ͷŲ����յ��﷨����ռ�ռ�

	MatchToken(STEP);

	step_ptr = Expression(); 
	PrintSyntaxTree(step_ptr, 1);//��������������ʽ�﷨��
	DelExprTree(step_ptr);//�ͷŲ��������﷨����ռ�ռ�

	MatchToken(DRAW);
	MatchToken(L_BRACKET);

	x_ptr = Expression(); 
	PrintSyntaxTree(x_ptr, 1);
	DelExprTree(x_ptr);//�ͷź������﷨����ռ�ռ�

	MatchToken(COMMA);

	y_ptr = Expression();
	PrintSyntaxTree(y_ptr, 1);
	DelExprTree(y_ptr);//�ͷ��������﷨����ռ�ռ�

	MatchToken(R_BRACKET);

}

//����Ԫ�Ӽ����㣩���ʽExpression�ĵݹ��ӳ���
//�Ѻ������Ϊ�﷨���ڵ��ָ�룬�ں���������2���﷨���ڵ��ָ��������ֱ���ΪExpression���Ҳ�������Term�����﷨���ڵ�ָ��
//���ʽӦ�����������Ż��޷��ſ�ͷ�������ɸ����ԼӼ������Ӷ��ɡ�
static struct ExprNode *Expression()//չ���Ҳ������ҹ����﷨��
{
	struct ExprNode *left, *right;//���������ڵ�ָ��
	Token_Type token_tmp;//��ǰ�Ǻ�

	left = Term();//������������õ����﷨�������������һ���˳�������ʽ
	while (token.type == PLUS || token.type == MINUS)
	{
		token_tmp = token.type;//��ǰ�Ǻ��Ǽ�/��
		MatchToken(token_tmp);//ƥ��Ǻ�
		right = Term();//�����Ҳ������õ����﷨�����Ҳ�������һ���˳�������ʽ
		left = MakeExprNode(token_tmp, left, right);//����������﷨�������Ϊ������
	}
	return left;//���ص��ǵ�ǰ�ǺŽڵ�
}

//�˳�������ʽTerm�ĵݹ��ӳ���
//���������ɸ������Գ˳������Ӷ���
static struct ExprNode *Term()
{
	struct ExprNode *left, *right;
	Token_Type token_tmp;
	left = Factor();
	while (token.type == MUL || token.type == DIV)
	{
		token_tmp = token.type;
		MatchToken(token_tmp);
		right = Factor();
		left = MakeExprNode(token_tmp, left, right);
	}
	return left;
}

//һԪ�Ӽ�����Factor�ĵݹ��ӳ���
//�����������һ����ʶ����һ�����֣�����һ�����������������ӱ��ʽ
static struct ExprNode *Factor()
{
	struct ExprNode *left, *right;
	if (token.type == PLUS)//ƥ��һԪ������
	{
		MatchToken(PLUS);
		right = Factor();
	}
	else if (token.type == MINUS)//���ʽ�˻�Ϊ�����Ҳ������ı��ʽ
	{
		MatchToken(MINUS);
		right = Factor();
		left = new ExprNode;
		left->OpCode = CONST_ID;
		left->Content.CaseConst = 0.0;
		right = MakeExprNode(MINUS, left, right);
	}
	else right = Component();//ƥ����ս��Component
	return right;
}

//��������ʽComponent�ĵݹ��ӳ���
static struct ExprNode *Component()
{
	//�ҽ��
	struct ExprNode *left, *right;
	left = Atom();
	if (token.type == POWER)
	{
		MatchToken(POWER);
		right = Component();//�ݹ����Component��ʵ��POWER���ҽ��
		left = MakeExprNode(POWER, left, right);
	}
	return left;
}

//ԭ�ӱ��ʽAtom�ĵݹ��ӳ��򣬰����ָ��� ���� ���� ����
static struct ExprNode *Atom()
{
	struct Token t = token;
	struct ExprNode *address, *tmp;
	switch (token.type)
	{
	case CONST_ID:
		MatchToken(CONST_ID);
		address = MakeExprNode(CONST_ID, t.value);
		break;
	case T:
		MatchToken(T);
		address = MakeExprNode(T);
		break;
	case FUNC:
		MatchToken(FUNC);
		MatchToken(L_BRACKET);
		tmp = Expression();  
		PrintSyntaxTree(tmp, 1);
		//�õ���ַ���ȴ���ӡ
		address = MakeExprNode(FUNC, t.FuncPtr, tmp);
		MatchToken(R_BRACKET);
		break;
	case L_BRACKET:
		MatchToken(L_BRACKET);
		address = Expression(); 
		PrintSyntaxTree(address, 1);
		MatchToken(R_BRACKET);
		break;
	default:
		SyntaxError(2);
	}
	return address;
}

//�����﷨����һ���ڵ㣬���տɱ�����б�
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...)//ע�����������һ���ɱ�����ĺ���
{
	struct ExprNode *ExprPtr = new(struct ExprNode);//Ϊ�½ڵ㿪�ٿռ�
	ExprPtr->OpCode = opcode;//��ڵ�д��Ǻ����

	//��ȡ�ɱ��������
	va_list ArgPtr;//ָ��ɱ亯���Ĳ�����ָ��
	va_start(ArgPtr, opcode);//��ʼ��va_list��������һ������Ҳ���ǹ̶�����Ϊopcode

	switch (opcode)//���ݼǺŵ�����첻ͬ�Ľڵ�
	{
	case CONST_ID://�����ڵ�
		//���ؿɱ�������ɱ���������ǳ���
		ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);
		break;
	case T://����T�ڵ�
		//���ؿɱ�������ɱ���������ǲ���T
		ExprPtr->Content.CaseParmPtr = &Parameter;
		break;
	case FUNC://�������ýڵ�
		//�ɱ���������Ƕ�Ӧ������ָ��
		ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);
		//�ɱ���������ǽڵ�
		ExprPtr->Content.CaseFunc.Child = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	default://��Ԫ����ڵ�
		//�ɱ���������ǽڵ�
		ExprPtr->Content.CaseOperater.Left = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		//�ɱ���������ǽڵ�
		ExprPtr->Content.CaseOperater.Right = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	}
	va_end(ArgPtr);//�����ɱ�����Ļ�ȡ

	return ExprPtr;
}