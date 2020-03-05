#ifndef PARSER_H
#define PARSER_H

#include"Scanner.h"

//FuncPtr代表一个指向返回double值并带有一个double形参的函数的指针的类型
typedef double(*FuncPtr)(double);
struct ExprNode   //语法树节点类型
{
	enum Token_Type OpCode;  //PLUS,MINUS,MUL,DIV,POWER,FUNC,CONST_ID，T
	union
	{
		struct { ExprNode *Left, *Right; 
		}CaseOperater;     //二元运算

		//函数调用：只有一个孩子的内部节点，还有一个指向对应函数名的指针 MathFuncPtr
		struct { ExprNode *Child; FuncPtr MathFuncPtr; 
		}CaseFunc;

		double CaseConst;    // 右值 常数：叶子节点 
		double *CaseParmPtr; //左值：存放T的值的地址
	}Content;
};

extern void Parser(char *SrcFilePtr);                       //语法分析器对外接口

#endif 
