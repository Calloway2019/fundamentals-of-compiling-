#include<windows.h>
#include<wingdi.h>

//��windows�Դ�ͼ�ο�ʵ�ֵĴʷ�������
// ���������������ͼ���Խ����������ͼ��
#include"Parser.h"

#define red RGB(255,0,0)//��ɫ
#define black RGB(0,0,0)//��ɫ

//�ⲿ��������
extern void DrawPixel(unsigned long x, unsigned long y);//����һ����
extern double GetExprValue(struct ExprNode * root);//��ñ��ʽ��ֵ

//ͼ�λ���
extern void DrawLoop(double Start,
	double End,
	double Step,
	struct ExprNode *HorPtr,
	struct ExprNode *VerPtr);

#pragma once
