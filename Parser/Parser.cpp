#include"Parser.h"
#pragma warning(disable: 4996) // 错误信息：strcpy
#pragma warning(disable: 4703) // 错误信息：使用了可能未初始化的本地指针变量“address”

double Parameter = 0;

static Token token;//定义一个记号

//各种函数声明
//调用词法分析器的GetToken，把得到的当前记录保存起来。
//如果得到的记号是非法输入errtoken，则指出一个语法错误
static void getToken();

//匹配当前记号
static void MatchToken(enum Token_Type The_Token);

//处理语法错误的子程序。根据错误的性质打印相关信息并且终止程序运行。
//错误性质可以根据传参不同来区分：SyntaxError（1）词法错   SyntaxError（2）语法错
static void SyntaxError(int case_of);

//打印错误信息
static void ErrMsg(unsigned LineNo, char *descrip, char *string);

//先序遍历打印语法树
//非终结符递归子程序声明 有2类
//第一类是类型不匹配
static void PrintSyntaxTree(struct ExprNode *root, int indent);
static void Program();//程序
static void Statement();//语句
static void OriginStatement();//Origin语句
static void RotStatement();//Rot语句
static void ScaleStatement();//Scale语句
static void ForStatement();//For语句

//第2类，语法分析+构造语法树，因此表达式均设计为返回值为指向语法树节点的指针的函数。
static struct ExprNode *Expression();//表达式、二元加减运算表达式
static struct ExprNode *Term();//乘除运算表达式

static struct ExprNode *Factor();//一元加减运算表达式
//把项和因子独立开处理解决了加减号与乘除号的优先级问题。在这几个过程的反复调用中，始终传递fsys变量的值，保证可以在出错的情况下跳过出错的符号，使分析过程得以进行下去。
static struct ExprNode *Component();//幂运算表达式
static struct ExprNode *Atom();//原子表达式

//对外接口声明
extern void Parser(char *SrcFilePtr);

//语法树构造函数声明，变参的
static struct ExprNode *MakeExprNode(enum Token_Type opcode, ...);

//实现部分
//通过词法分析器接口GetToken获得一个记号
static void getToken()
{
	token = GetToken();
	// 如果得到的记号是非法输入errtoken，是第一类语法错误
	if (token.type == ERRTOKEN) SyntaxError(1); 
}

//匹配当前记号
static void MatchToken(enum Token_Type The_Token)
{
	if (token.type != The_Token)
	{
		SyntaxError(2);//若失败，第二类语法错误
	}
	getToken();//若成功，获取下一个
}

//语法错误处理
static void SyntaxError(int case_of)
{
	switch (case_of)
	{
	case 1: ErrMsg(LineNo, (char*)"错误记号 ", token.lexeme);
		break;
	case 2: ErrMsg(LineNo, (char*)"不是预期记号", token.lexeme);
		break;
	}
}

//打印错误信息，最后程序需要用到
void ErrMsg(unsigned LineNo, char *descrip, char *string)
{
	char msg[256];
	// 将数组中全部填充0
	memset(msg, 0, 256);

	//把格式化的数据写入字符串中
	sprintf(msg, "Line No %5d:%s %s !", LineNo, descrip, string);

	CloseScanner();
	//关闭程序
	exit(1);
}

//先序遍历打印语法树，根-左-右
// intend:缩进
void PrintSyntaxTree(struct ExprNode *root, int indent)
{
	int temp;
	for (temp = 1; temp <= indent; temp++) printf("\t");//缩进
	switch (root->OpCode)
	{
	case PLUS:		printf("%s\n", "+");	break;
	case MINUS:		printf("%s\n", "-");	break;
	case MUL:		printf("%s\n", "*");	break;
	case DIV:		printf("%s\n", "/");	break;
	case POWER:		printf("%s\n", "**");	break;
	// 如果是函数比如sin之类的，打印出来
	case FUNC:		printf("%x\n", root->Content.CaseFunc.MathFuncPtr);	break;
	// 如果是常数比如pi之类的，打印出来地址！
	case CONST_ID:	printf("%f\n", root->Content.CaseConst);	break;
	case T:			printf("%s\n", "T");	break;
	default:		printf("Error Tree Node !\n");	exit(0);
	}
	//常数和参数只有叶子节点 
	if (root->OpCode == CONST_ID || root->OpCode == T) //叶子节点返回
		return;

	//如果是函数名，递归打印一个孩子节点
	if (root->OpCode == FUNC)
		PrintSyntaxTree(root->Content.CaseFunc.Child, indent + 1);//函数有孩子节点和指向函数名的指针
	else//递归打印两个孩子节点
	{//二元运算：左右孩子的内部节点
		PrintSyntaxTree(root->Content.CaseOperater.Left, indent + 1);
		PrintSyntaxTree(root->Content.CaseOperater.Right, indent + 1);
	}
}
//----------删除一颗语法树
void DelExprTree(struct ExprNode *root)
{
	if (root == NULL) return;
	switch (root->OpCode)
	{
	//二元：：两个孩子的内部节点
	case PLUS:
	case MINUS:
	case MUL:
	case DIV:
	case POWER:
		DelExprTree(root->Content.CaseOperater.Left);
		DelExprTree(root->Content.CaseOperater.Right);
		break;
	case FUNC:
		DelExprTree(root->Content.CaseFunc.Child);//一元：：一个孩子的内部节点
		break;
	default:
		break;
	}
	delete(root);
}
//绘图语言解释器入口（与主程序的外部接口）
void Parser(char *SrcFilePtr)//语法分析器的入口
{
	if (!InitScanner(SrcFilePtr))//初始化词法分析器
	{
		printf("Open Source File Error !\n"); return;
	}

	getToken();//先获得一个记号
	Program();//然后进入Program递归子程序，递归下降分析
	CloseScanner();//关闭词法分析器
	return;
}

//程序Program的递归子程序
static void Program()
{
	while (token.type != NONTOKEN)//记号类型不为空
	{
		//程序的第一条语句
		Statement();
		MatchToken(SEMICO);//直到匹配到分隔符
	}
}

//语句Statment的递归子程序
static void Statement()
{
	switch (token.type)
	{
		//罗列四种语句类型，分别分析
	case ORIGIN: OriginStatement(); break;
	case SCALE: ScaleStatement(); break;
	case ROT:  RotStatement(); break;
	case FOR: ForStatement(); break;
	default: SyntaxError(2); //否则报错
	}
}

//语句OriginStatement的递归子程序
// 例如 origin is ( ,  ); 
static void OriginStatement(void)
{
	struct ExprNode *tmp;//语法树节点的类型
	MatchToken(ORIGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET); //(

	tmp = Expression();  
	PrintSyntaxTree(tmp, 1);
	DelExprTree(tmp);//删除一棵树

	MatchToken(COMMA);  // ,

	tmp = Expression();   
	PrintSyntaxTree(tmp, 1);
	DelExprTree(tmp);//删除一棵树

	MatchToken(R_BRACKET);
}

//语句ScaleStatement的递归子程序
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

//语句RotStatement的递归子程序
static void RotStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(ROT);
	MatchToken(IS);

	tmp = Expression();  
	PrintSyntaxTree(tmp, 1);
	DelExprTree(tmp);
}

//语句ForStatement的递归子程序
//对右部文法符号的展开->遇到终结符号直接匹配，遇到非终结符就调用相应子程序
//ForStatement中唯一的非终结符是Expression，他出现在5个不同位置，分别代表循环的起始、终止、步长、横坐标、纵坐标，需要5个树节点指针保存这5棵语法树
static void ForStatement(void)
{
	//eg:for T from 0 to 2*pi step pi/50 draw (t, -sin(t));
	double Start, End, Step;//绘图起点、终点、步长
	struct ExprNode *start_ptr, *end_ptr, *step_ptr, *x_ptr, *y_ptr;

	MatchToken(FOR);
	MatchToken(T);
	MatchToken(FROM);//eg:for T from

	start_ptr = Expression(); 
	PrintSyntaxTree(start_ptr, 1);//获得参数起点表达式的语法树
	DelExprTree(start_ptr);//释放参数起点语法树所占空间

	MatchToken(TO);

	end_ptr = Expression(); 
	PrintSyntaxTree(end_ptr, 1);//构造参数终点表达式语法树
	DelExprTree(end_ptr);//释放参数终点语法树所占空间

	MatchToken(STEP);

	step_ptr = Expression(); 
	PrintSyntaxTree(step_ptr, 1);//构造参数步长表达式语法树
	DelExprTree(step_ptr);//释放参数步长语法树所占空间

	MatchToken(DRAW);
	MatchToken(L_BRACKET);

	x_ptr = Expression(); 
	PrintSyntaxTree(x_ptr, 1);
	DelExprTree(x_ptr);//释放横坐标语法树所占空间

	MatchToken(COMMA);

	y_ptr = Expression();
	PrintSyntaxTree(y_ptr, 1);
	DelExprTree(y_ptr);//释放纵坐标语法树所占空间

	MatchToken(R_BRACKET);

}

//（二元加减运算）表达式Expression的递归子程序
//把函数设计为语法树节点的指针，在函数内引进2个语法树节点的指针变量，分别作为Expression左右操作数（Term）的语法树节点指针
//表达式应该是由正负号或无符号开头、由若干个项以加减号连接而成。
static struct ExprNode *Expression()//展开右部，并且构造语法树
{
	struct ExprNode *left, *right;//左右子树节点指针
	Token_Type token_tmp;//当前记号

	left = Term();//分析左操作数得到其语法树，左操作数是一个乘除运算表达式
	while (token.type == PLUS || token.type == MINUS)
	{
		token_tmp = token.type;//当前记号是加/减
		MatchToken(token_tmp);//匹配记号
		right = Term();//分析右操作数得到其语法树，右操作数是一个乘除运算表达式
		left = MakeExprNode(token_tmp, left, right);//构造运算的语法树，结果为左子树
	}
	return left;//返回的是当前记号节点
}

//乘除运算表达式Term的递归子程序
//项是由若干个因子以乘除号连接而成
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

//一元加减运算Factor的递归子程序
//因子则可能是一个标识符或一个数字，或是一个以括号括起来的子表达式
static struct ExprNode *Factor()
{
	struct ExprNode *left, *right;
	if (token.type == PLUS)//匹配一元加运算
	{
		MatchToken(PLUS);
		right = Factor();
	}
	else if (token.type == MINUS)//表达式退化为仅有右操作数的表达式
	{
		MatchToken(MINUS);
		right = Factor();
		left = new ExprNode;
		left->OpCode = CONST_ID;
		left->Content.CaseConst = 0.0;
		right = MakeExprNode(MINUS, left, right);
	}
	else right = Component();//匹配非终结符Component
	return right;
}

//幂运算表达式Component的递归子程序
static struct ExprNode *Component()
{
	//右结合
	struct ExprNode *left, *right;
	left = Atom();
	if (token.type == POWER)
	{
		MatchToken(POWER);
		right = Component();//递归调用Component以实现POWER的右结合
		left = MakeExprNode(POWER, left, right);
	}
	return left;
}

//原子表达式Atom的递归子程序，包括分隔符 函数 常数 参数
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
		//得到地址，等待打印
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

//生成语法树的一个节点，接收可变参数列表
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...)//注意这个函数是一个可变参数的函数
{
	struct ExprNode *ExprPtr = new(struct ExprNode);//为新节点开辟空间
	ExprPtr->OpCode = opcode;//向节点写入记号类别

	//读取可变参数操作
	va_list ArgPtr;//指向可变函数的参数的指针
	va_start(ArgPtr, opcode);//初始化va_list变量，第一个参数也就是固定参数为opcode

	switch (opcode)//根据记号的类别构造不同的节点
	{
	case CONST_ID://常数节点
		//返回可变参数，可变参数类型是常数
		ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);
		break;
	case T://参数T节点
		//返回可变参数，可变参数类型是参数T
		ExprPtr->Content.CaseParmPtr = &Parameter;
		break;
	case FUNC://函数调用节点
		//可变参数类型是对应函数的指针
		ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);
		//可变参数类型是节点
		ExprPtr->Content.CaseFunc.Child = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	default://二元运算节点
		//可变参数类型是节点
		ExprPtr->Content.CaseOperater.Left = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		//可变参数类型是节点
		ExprPtr->Content.CaseOperater.Right = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	}
	va_end(ArgPtr);//结束可变参数的获取

	return ExprPtr;
}