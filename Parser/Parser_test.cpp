#include"Parser.h"
#pragma warning(disable: 4996)
extern void Parser(char *SrcFilePtr);
int main(int argc, char *argv[])
{
	char*p;
	//∂ØÃ¨…Í«Îø’º‰
	p = (char*)malloc(20 * sizeof(char));
	//∏≥≥ı÷µ
	memset(p, 0, sizeof(char) * 20);
	strcpy(p, "Text.txt");
	Parser(p);
	free(p);
	system("pause");

}