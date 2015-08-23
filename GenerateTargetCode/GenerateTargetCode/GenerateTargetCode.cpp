// GenerateTargetCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
class DataNode
{
public:
	string name;
	int offset;
	int length;//stand for the total bytes of the data takes
};
class CodeNode
{
public:
	string no;
	string op;
	string val1;
	string val2;
	string result;
};
int main()
{
	int no_code = 0;
	ifstream infile("output");
	ofstream outfile("asmfile.asm");
	//先输出汇编文件头
	outfile << "TITLE datatype.asm : example for datatypes\n"
		<< ".386\n"
		<< ".model flat, stdcall\n"
		<< ".stack 2048\n"
		<< "option casemap : none\n"
		<< "includelib	msvcrt.lib\n"
		<< "printf		proto c : ptr byte, : vararg\n"
		<< "scanf		proto c : ptr byte, : vararg\n"
		<< "_getche		proto c\n"
		<< "ExitProcess 	proto : dword\n"
		<< "chr$ MACRO any_text : VARARG\n"
		<< "LOCAL txtname\n"
		<< ".data\n"
		<< "txtname byte any_text, 0\n"
		<< ".code\n"
		<< "EXITM <OFFSET txtname>\n"
		<< "ENDM\n";
    //处理数据区域
    outfile<<".data"<<endl;
    string input_str="";
    DataNode datanode;
    infile >> input_str;
    if (input_str=="符号表")
    {
    	infile>>input_str;
    	while(input_str!="四元式")
    	{
            datanode.name=input_str;
            infile>>datanode.offset;
            infile>>datanode.length;
            if (datanode.length==4)
            {
            	outfile<<datanode.name+"  dword   ?"<<endl;
            }
			if (datanode.length == 1)
			{
				outfile << datanode.name + "  byte   ?" << endl;
			}
    		infile>>input_str;
    	}  
    }
    else
    {
    	cout<<"缺少符号表"<<endl;
		exit(0);
    }

	outfile << ".code" << endl
		<< "main proc" << endl;
	CodeNode codenode;
	while (infile >> input_str)
	{
		no_code++;
		codenode.no = input_str;
		outfile << "label" << codenode.no << endl;
		infile >> codenode.op;
		if (codenode.op[0] == 'j')//是跳转指令的情况
		{
			if (codenode.op == "j")
			{ 
				infile >> codenode.result;
				outfile << "     jmp label" <<codenode.result << endl;
			}
			else
			{
				infile >> codenode.val1;
				infile >> codenode.val2;
				infile >> codenode.result;
				outfile << "     mov eax," << codenode.val1 << endl;
				outfile << "     mov ebx," << codenode.val2 << endl;
				outfile <<"     cmp eax,ebx"<< endl;
				outfile << codenode.op << " label" <<codenode.result << endl;
			}
			
		}
		if (codenode.op == "+")
		{
			infile >> codenode.val1;
			infile >> codenode.val2;
			infile >> codenode.result;
			outfile << "     mov eax, " << codenode.val1 << endl;
			outfile << "     add eax, " << codenode.val2 << endl;
			outfile << "     mov " << codenode.result << " ,eax" << endl;
		}
		if (codenode.op == "-")
		{
			infile >> codenode.val1;
			infile >> codenode.val2;
			infile >> codenode.result;
			outfile << "     mov eax, " << codenode.val1 << endl;
			outfile << "     sub eax, " << codenode.val2 << endl;
			outfile << "     mov " << codenode.result << " ,eax" << endl;
		}
		if (codenode.op == "*")
		{
			infile >> codenode.val1;
			infile >> codenode.val2;
			infile >> codenode.result;
			outfile << "     mov eax, " << codenode.val1 << endl;
			outfile << "     mul " << codenode.val2 << endl;
			outfile << "     mov " << codenode.result << " ,eax" << endl;
		}
		if (codenode.op == "/")
		{
			infile >> codenode.val1;
			infile >> codenode.val2;
			infile >> codenode.result;
			outfile << "     mov eax, " << codenode.val1 << endl;
			outfile << "     mov ebx, " << codenode.val2 << endl;
			outfile << "     div ebx" << endl;
			outfile << "     mov " << codenode.result << " ,eax" << endl;
		}
		if (codenode.op == "=")
		{
			infile >> codenode.val1;
			infile >> codenode.result;
			outfile << "     mov eax, " << codenode.val1 << endl;
			outfile << "     mov " << codenode.result << " ,eax" << endl;
		}
		if (codenode.op == "out")
		{
			infile >> codenode.val1;
			outfile << "     mov eax," << codenode.val1 << endl;
			outfile << "     invoke printf,chr$(\"%d\"),eax" << endl;
		}
	}
	outfile <<"label"<<no_code<<":"<<endl<< "     invoke _getche\n     invoke ExitProcess,0" << endl;
	outfile << "main endp" << endl
		<< "end main" << endl;
	outfile.close();
	infile.close();
	return 0;
}

