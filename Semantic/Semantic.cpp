#include "Semantic.h"
#include<windef.h>
HDC hDC;
extern double Parameter,//����T�Ĵ洢�ռ䣺��¼tÿ�μ�һ��ı仯  
Origin_x, Origin_y,//��������ƽ�ƾ���
Scale_x, Scale_y,//���ݱ�������
Rot_angle;//��ת�Ƕ�

double GetExprValue(struct ExprNode * root);//��ñ��ʽ��ֵ

void DrawPixel(unsigned long x, unsigned long y);//����һ����
//ͼ�λ���
void DrawLoop(double Start,
	double End,
	double Step,
	struct ExprNode *HorPtr,
	struct ExprNode *VerPtr);

static void Errmsg(char *string);//������

//����������
static void CalcCoord(struct ExprNode *Hor_Exp,
	struct ExprNode *Ver_Exp,
	double &Hor_x,
	double &Ver_y);


//  ������
void Errmsg(char *string) { exit(1); }

//----------���㱻���Ƶ������
static void CalcCoord(struct ExprNode *Hor_Exp,//��������ʽ�﷨���ĸ��ڵ�
	struct ExprNode * Ver_Exp,//��������ʽ�﷨���ĸ��ڵ�
	double &Hor_x,//�������ֵ���𷵻�ֵ������
	double &Ver_y)//��������ֵ���𷵻�ֵ������
{
	double HorCord, VerCord, Hor_tmp;

	//���ݱ��ʽ���﷨������ԭʼ����
	HorCord = GetExprValue(Hor_Exp);
	VerCord = GetExprValue(Ver_Exp);

	//���б����任
	HorCord *= Scale_x;
	VerCord *= Scale_y;

	//������ת�任
	Hor_tmp = HorCord * cos(Rot_angle) + VerCord * sin(Rot_angle);
	VerCord = VerCord * cos(Rot_angle) - HorCord * sin(Rot_angle);
	HorCord = Hor_tmp;

	//����ƽ�Ʊ任
	HorCord += Origin_x;
	VerCord += Origin_y;

	// ���ص��ֵ
	Hor_x = HorCord;
	Ver_y = VerCord;
}//û�з���ֵ

 //----------ѭ�����Ƶ������
void DrawLoop(double Start,//��ʼ
	double End,//��ֹ
	double Step,//����
	struct ExprNode *HorPtr,//��������ʽ�﷨���ĸ��ڵ�
	struct ExprNode *VerPtr)//��������ʽ�﷨���ĸ��ڵ�
{
	extern double Parameter;
	double x, y;
	for (Parameter = Start; Parameter <= End; Parameter += Step)//��t�ڷ�Χ�ڵ�ÿһ��ֵ�������
	{
		CalcCoord(HorPtr, VerPtr, x, y);//����Ҫ���Ƶ��ʵ������
		DrawPixel((unsigned long)x, (unsigned long)y);//���������
	}
}

//��ȡ���ʽ��ֵ
double GetExprValue(struct ExprNode *root)//�����Ǳ��ʽ�ĸ�
{//��������﷨��  ���ݲ�ͬ�Ľڵ����ͼ��㵱ǰ���ڵ��ֵ
	if (root == NULL) return 0.0;
	switch (root->OpCode)
	{
	//��Ԫ�����
	//����ֵ�Ǳ��ʽ��ֵ
	case PLUS:
		return GetExprValue(root->Content.CaseOperater.Left) +
			GetExprValue(root->Content.CaseOperater.Right);
	case MINUS:
		return GetExprValue(root->Content.CaseOperater.Left) -
			GetExprValue(root->Content.CaseOperater.Right);
	case MUL:
		return GetExprValue(root->Content.CaseOperater.Left) *
			GetExprValue(root->Content.CaseOperater.Right);
	case DIV:
		return GetExprValue(root->Content.CaseOperater.Left) /
			GetExprValue(root->Content.CaseOperater.Right);
	case POWER:
		return pow(GetExprValue(root->Content.CaseOperater.Left),
			GetExprValue(root->Content.CaseOperater.Right));
	case FUNC:
		return(*root->Content.CaseFunc.MathFuncPtr)(GetExprValue(root->Content.CaseFunc.Child));
	case CONST_ID:
		return root->Content.CaseConst;
	case T:
		return *(root->Content.CaseParmPtr);
	default:
		return 0.0;
	}
}



//----------����һ����
void DrawPixel(unsigned long x, unsigned long y)
{
	SetPixel(hDC, x, y, black);
}