#ifndef PARSER_H
#define PARSER_H

#include"Scanner.h"

//FuncPtr����һ��ָ�򷵻�doubleֵ������һ��double�βεĺ�����ָ�������
typedef double(*FuncPtr)(double);
struct ExprNode   //�﷨���ڵ�����
{
	enum Token_Type OpCode;  //PLUS,MINUS,MUL,DIV,POWER,FUNC,CONST_ID��T
	union
	{
		struct { ExprNode *Left, *Right; 
		}CaseOperater;     //��Ԫ����

		//�������ã�ֻ��һ�����ӵ��ڲ��ڵ㣬����һ��ָ���Ӧ��������ָ�� MathFuncPtr
		struct { ExprNode *Child; FuncPtr MathFuncPtr; 
		}CaseFunc;

		double CaseConst;    // ��ֵ ������Ҷ�ӽڵ� 
		double *CaseParmPtr; //��ֵ�����T��ֵ�ĵ�ַ
	}Content;
};

extern void Parser(char *SrcFilePtr);                       //�﷨����������ӿ�

#endif 
