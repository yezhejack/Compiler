#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
using namespace std;
const int Num_pre_words=43;
char buffer[512];
int pointer;//point to the next character
int end_index;//the last index of a word or a syntax
int size;//point to the last useful character
int num_line;
string temp;
string pre_words[50];
string target="";
int kind;
bool FillBuffer()
{
	pointer=0;
	size=0;
	if (scanf("%c",&buffer[size])!=EOF)
	{
        size++;
        while (buffer[size-1]!='\n')
        {
        	scanf("%c",&buffer[size]);
        	size++;
        }
        size--;
		return true;
	}
	else
	{
		return false;
	}
}
bool Compare(int index,int length,char*str)
{
    for (int i=0;i<length;i++)
    {
        if (buffer[index+i]!=str[i])
        {
            return false;
        }
    }
    return true;
}
void TokenOutput(int kind_id,string label)
{
    if (kind_id==0)
    {
        label="error";
        pointer=size;//出错之后跳过这一行
    }
    cout<<"( "<<kind_id<<" , "<<label<<" ) ";
    if (kind_id==3) cout<<" ( $ ) ";
}
//check the char is num or not
bool IsNum(char temp)
{
    if (temp>='0' && temp<='9') return true;
    else
        return false;
}
bool IsLetter(char temp)
{
    if ((temp>='a' && temp<='z')||(temp>='A' && temp<='Z'))
    {
        return true;
    }
    else
        return false;
}
bool IsSynatx(char temp)
{
    if (temp=='('||temp==')'||temp=='['||temp==']'||temp=='\"'||temp=='='||temp=='>'||temp=='<')
    {
        return true;
    }
    return false;
}
bool IsOp(char temp)
{
    if (temp=='&'||temp=='|'||temp=='='||temp=='>'||temp=='<'||temp=='+'||temp=='-'||temp=='*'||temp=='/')
        return true;
    else
        return false;
}
bool IsSeparate(char temp)
{
    if (IsSynatx(temp)||IsOp(temp)||temp==' '||temp==','||temp==';')
        return true;
    else
        return false;
}
int RecogID()
{
    int kind=34;
    while(pointer<size)
    {
        if (IsNum(buffer[pointer])||IsLetter(buffer[pointer])||buffer[pointer]=='.')
        {
            target.push_back(buffer[pointer]);
            pointer++;
        }
        else
        {
            if (buffer[pointer]==' '||IsSeparate(buffer[pointer]))
            {
                return kind;
            }
            else
            {
                kind=0;
                break;
            }
        }

    }
    return kind;
}
//识别保留字
void RecogWord(int kind_id,string str)
{
    kind=kind_id;
    temp=str;
    for (int i=0; i<(int)temp.size(); i++)
    {
        if (temp[i]==buffer[pointer])
        {
            if (pointer>=size)
            {
                kind=34;
                break;
            }
            target.push_back(buffer[pointer]);
            pointer++;
        }
        else
        {
            kind=RecogID();
            break;
        }
    }

}
int RecogNum()
{
    int kind_id=35;
    while(IsNum(buffer[pointer]))
    {
        target.push_back(buffer[pointer]);
        pointer++;
    }
    //识别float
    if (buffer[pointer]=='.')
    {
        kind_id=36;
        target.push_back(buffer[pointer]);
        pointer++;
        bool flag=false;//标志用于表示后面还有数字
        while(IsNum(buffer[pointer]))
        {
            flag=true;
            target.push_back(buffer[pointer]);
            pointer++;
        }
        if (!flag)
        {
            kind_id=0;
        }
    }
    return kind_id;
}
int RecogString()
{
    int kind_id=0;
    pointer++;//开头的引号
    while(pointer<size)
    {
        if (buffer[pointer]!='\"')
        {
            target.push_back(buffer[pointer]);
            pointer++;
        }
        else
        {
            pointer++;
            kind_id=37;
            break;
        }
    }
    return kind_id;
}
void Analyze()
{


    pointer=0;
	while(pointer<size)
	{
	    //separate a word(end with space ,'(',')','{','}')
		while((buffer[pointer]==' '||buffer[pointer]==',')&&pointer<size) pointer++;
		end_index=pointer;
        switch(buffer[pointer])
        {
        case '#':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            RecogWord(1,"include");
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'b':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            //begin
            if (buffer[pointer]=='e')
            {
                RecogWord(2,"egin");
            }
            else
            {
                //bool
                if (buffer[pointer]=='o')
                {
                    RecogWord(6,"ool");
                }
            }

            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'c':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            RecogWord(9,"onst");
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'd':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            RecogWord(14,"o");
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=34;
                switch (buffer[pointer])
                {
                case 'u':
                    RecogWord(8,"uble");
                    break;
                default:
                    break;
                }
            }
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'e':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            switch(buffer[pointer])
            {
            case 'n':
                RecogWord(3,"nd");
                if (!IsSeparate(buffer[pointer])&&pointer<size)
                {
                    kind=34;
                    switch (buffer[pointer])
                    {
                    case 'i':
                        RecogWord(40,"if");
                        break;
                    case 'e':
                        RecogWord(41,"else");
                        break;
                    case 'w':
                        RecogWord(42,"while");
                        break;
                    case 'd':
                        RecogWord(43,"dowhile");
                        break;
                    case 'f':
                        RecogWord(44,"for");
                        break;
                    case 'p':
                        RecogWord(45,"p");
                        break;
                    default:
                        break;
                    }
                }
                break;
            case 'l':
                RecogWord(13,"lse");
                break;
            }

            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'f':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            //begin
            if (buffer[pointer]=='o')
            {
                RecogWord(10,"or");
            }
            else
            {
                //bool
                if (buffer[pointer]=='l')
                {
                    RecogWord(7,"loat");
                }
            }

            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'i':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            //if
            if (buffer[pointer]=='f')
            {
                RecogWord(12,"f");
            }
            else
            {
                //bool
                if (buffer[pointer]=='n')
                {
                    RecogWord(5,"nt");
                }
            }
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'o':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            RecogWord(50,"ut");
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'r':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            RecogWord(46,"eturn");
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
         case 's':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            RecogWord(49,"tring");
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'v':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            RecogWord(4,"oid");
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case 'w':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=34;
            RecogWord(11,"hile");
            if (!IsSeparate(buffer[pointer])&&pointer<size)
            {
                kind=RecogID();
            }
            TokenOutput(kind,target);
            break;
        case '!':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=15;
            if (pointer<size&&buffer[pointer]=='=')
            {
                kind=25;
                target.push_back(buffer[pointer]);
                pointer++;
            }
            TokenOutput(kind,target);
            break;
        case '~':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=20;
            TokenOutput(kind,target);
            break;
        case '+':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=27;
            TokenOutput(kind,target);
            break;
        case '-':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=28;
            TokenOutput(kind,target);
            break;
        case '/':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=29;
            TokenOutput(kind,target);
            break;
        case '*':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=30;
            TokenOutput(kind,target);
            break;
        case '(':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=38;
            TokenOutput(kind,target);
            break;
        case ')':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=39;
            TokenOutput(kind,target);
            break;
        case '[':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=47;
            TokenOutput(kind,target);
            break;
        case ']':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=48;
            TokenOutput(kind,target);
            break;
        case ';':
            target="";
            target.push_back(buffer[pointer]);
            pointer++;
            kind=51;
            TokenOutput(kind,target);
            break;
        case '&':
            target="&";
            kind=18;
            pointer++;
            if (pointer<size&&buffer[pointer]=='&')
            {
                kind=16;
                target.push_back(buffer[pointer]);
                pointer++;
            }
            TokenOutput(kind,target);
            break;
        case '|':
            target="|";
            kind=19;
            pointer++;
            if (pointer<size&&buffer[pointer]=='|')
            {
                kind=17;
                target.push_back(buffer[pointer]);
                pointer++;
            }
            TokenOutput(kind,target);
            break;
        case '>':
            target=">";
            kind=21;
            pointer++;
            if (pointer<size&&buffer[pointer]=='=')
            {
                kind=22;
                target.push_back(buffer[pointer]);
                pointer++;
            }
            else
            {
                if (pointer<size&&buffer[pointer]=='>')
                {
                    kind=32;
                    target.push_back(buffer[pointer]);
                    pointer++;
                }
            }
            TokenOutput(kind,target);
            break;
        case '<':
            target="<";
            kind=23;
            pointer++;
            if (pointer<size&&buffer[pointer]=='=')
            {
                kind=24;
                target.push_back(buffer[pointer]);
                pointer++;
            }
            else
            {
                if (pointer<size&&buffer[pointer]=='<')
                {
                    kind=33;
                    target.push_back(buffer[pointer]);
                    pointer++;
                }
            }
            TokenOutput(kind,target);
            break;
        case '=':
            target="=";
            kind=31;
            pointer++;
            if (pointer<size&&buffer[pointer]=='=')
            {
                kind=26;
                target.push_back(buffer[pointer]);
                pointer++;
            }
            TokenOutput(kind,target);
            break;
        //字符串常量
        case '\"':
            target="";
            kind=RecogString();
            TokenOutput(kind,target);
            break;
        default:
            target="";
            if (IsLetter(buffer[pointer]))
            {
                kind=RecogID();
                TokenOutput(kind,target);
                break;
            }
            else
            {
                if (IsNum(buffer[pointer]))
                {
                    kind=RecogNum();
                    TokenOutput(kind,target);
                    break;
                }
                else
                {
                    kind=0;
                    TokenOutput(kind,target);
                    break;
                }
            }
        }

	}
}
int main()
{
    //input the pre_words.txt
    freopen("pre_words.txt","r",stdin);
    for (int i=1;i<=Num_pre_words;i++)
    {
        cin>>pre_words[i];
    }
    fclose(stdin);

    freopen("source.c","r",stdin);
    freopen("token.compile","w",stdout);
    while (FillBuffer())
    {
    	num_line++;
    	cout<<num_line<<": ";
        Analyze();
        cout<<endl;
    }
    fclose(stdin);
    fclose(stdout);
    return 0;
}
