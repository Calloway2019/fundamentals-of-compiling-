#include<windows.h>
#include<wingdi.h>

//用windows自带图形库实现的词法分析器
// 程序结果输出函数绘图语言解释器绘出的图像
#include"Parser.h"

#define red RGB(255,0,0)//红色
#define black RGB(0,0,0)//黑色

//外部函数声明
extern void DrawPixel(unsigned long x, unsigned long y);//绘制一个点
extern double GetExprValue(struct ExprNode * root);//获得表达式的值

//图形绘制
extern void DrawLoop(double Start,
	double End,
	double Step,
	struct ExprNode *HorPtr,
	struct ExprNode *VerPtr);

#pragma once
