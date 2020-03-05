#ifndef SCANNER_H
#define SCANNER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> //该库定义了C语言字符分类函数，用于测试字符是否属于特定的字符类别
#include <stdarg.h>
#include <math.h>

enum Token_Type//记号类别枚举
{
	//保留字
	ORIGIN, // 0 
	SCALE, //1
	ROT, // 2 
	IS, // 3
	TO, // 4
	STEP, // 5
	DRAW, //6
	FOR, //7
	FROM, // 8

	T, // 9 参数

	//分层符号
	SEMICO/* 10 */,
	L_BRACKET/* 11 */,
	R_BRACKET/* 12 */,
	COMMA/* 13 */,

	//运算符
	PLUS, // 14
	MINUS, //15
	MUL, // 16
	DIV, //17
	POWER, //18

	FUNC, //19  函数
	CONST_ID, //20 常数
	NONTOKEN, //21 空记号，标记源程序文件的结束
	ERRTOKEN//22 出错记号，标记非法输入
};

struct Token
{
	Token_Type type;//记号类别
	char * lexeme;//定义字符串类型
	double value;//定义的常数的值
	double(*FuncPtr)(double);//属性，函数指针，代表一个指向返回double值并带有一个double形参的函数的指针的类型
};

typedef double(*MathFuncPtr)(double);//MathFuncPtr代表一个指向返回double值并带有一个double形参的函数的指针的类型
static Token TokenTab[] = //符号表内容
{
	{ CONST_ID,	(char*)"PI",	3.1415926,	NULL },
	{ CONST_ID,	(char*)"E",		2.71828,	NULL },
	{ T,		(char*)"T",		0.0,	NULL },
	{ FUNC,		(char*)"SIN",		0.0,	sin },
	{ FUNC,		(char*)"COS",		0.0,	cos },
	{ FUNC,		(char*)"TAN",		0.0,	tan },
	{ FUNC,		(char*)"LN",		0.0,	log },
	{ FUNC,		(char*)"EXP",		0.0,	exp },
	{ FUNC,		(char*)"SQRT",		0.0,	sqrt },
	{ ORIGIN,	(char*)"ORIGIN",	0.0,	NULL },
	{ SCALE,	(char*)"SCALE",		0.0,	NULL },
	{ ROT,		(char*)"ROT",		0.0,	NULL },
	{ IS,		(char*)"IS",		0.0,	NULL },
	{ FOR,		(char*)"FOR",		0.0,	NULL },
	{ FROM,		(char*)"FROM",		0.0,	NULL },
	{ TO,		(char*)"TO",		0.0,	NULL },
	{ STEP,		(char*)"STEP",		0.0,	NULL },
	{ DRAW,		(char*)"DRAW",		0.0,	NULL },
};

extern unsigned int LineNo;//跟踪记号所在源文件行号
extern int InitScanner(const char *FileName);//初始化词法分析器
extern Token GetToken(void);//获取记号函数
extern void CloseScanner(void);//关闭词法分析器

#endif