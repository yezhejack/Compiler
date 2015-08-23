#include "stdafx.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <set>
#include <map>
#include <sstream>
#include <stack>
#include <fstream>
using namespace std;
/*
语法分析器主程序
*/
int min(int a, int b, int c)
{
	int output;
	output = a;
	if (b < output) output = b;
	if (c < output) output = c;
	return output;
}
//文法符号包含终结符
ifstream infile2("token.compile");
int N;//产生式总数量
class Syntax
{
public:
	string str;//符号
	Syntax* next;
	Syntax()
	{
		next = NULL;
	}
	Syntax(string s)
	{
		str = s;
		next = NULL;
	}
};
//产生式类
class Statm
{
public:
	string left;//产生式左部
	Syntax* right;//产生式右部
	Syntax* curr;
	int num_right;//产生式右部的数量
	Statm()
	{
		right = NULL;
		num_right = 0;
	}
};
//项
class Item
{
public:
	string left;
	Syntax* right;
	string lookahead;
	Item* next;
	//初始一个标示符在最左端的
	Item(Statm statm, string la)
	{
		left = statm.left;
		if (statm.right != NULL)
		{
			statm.curr = statm.right;
			right = new Syntax();
			//插入@
			right->str = "@";
			right->next = new Syntax();
			Syntax* curr = right->next;
			while (statm.curr != NULL)
			{
				curr->str = statm.curr->str;
				curr->next = NULL;
				if (statm.curr->next != NULL)
				{
					curr->next = new Syntax();
					curr = curr->next;
					statm.curr = statm.curr->next;
				}
				else
				{
					break;
				}
			}
		}
		lookahead = la;
		next = NULL;
	}
	Item()
	{
		next = NULL;
	}
	Item(Item* item)
	{
		left = item->left;
		lookahead = item->lookahead;
		next = NULL;
		Syntax* curr1;
		Syntax* curr2;
		if (item->right != NULL)
		{
			right = new Syntax();
			right->str = item->right->str;
			curr1 = right;
			curr2 = item->right->next;
			while (curr2 != NULL)
			{
				curr1->next = new Syntax;
				curr1 = curr1->next;
				curr1->str = curr2->str;
				curr2 = curr2->next;
			}
		}
	}
};
Statm statm[100];
//输出表达式
void OutputStatm()
{
	//测试表达式输入
	for (int i = 0; i < N; i++)
	{
		cout << statm[i].left << " ";
		statm[i].curr = statm[i].right;
		while (statm[i].curr != NULL)
		{
			cout << statm[i].curr->str << " ";
			statm[i].curr = statm[i].curr->next;
		}
		cout << endl;
	}
}
bool IsTerminate(string str)
{
	char ch = str[0];
	if (ch >= 'A' && ch <= 'Z')
	{
		return false;
	}
	else
	{
		return true;
	}
}
class SyntaxSet
{
public:
	string parent;//代表集合的非终结符
	Syntax* child;//集合内的元素
	SyntaxSet* next;//下一个链
	bool nullable;
	SyntaxSet()
	{
		parent = "";
		child = NULL;
		next = NULL;
		nullable = false;
	}
};
SyntaxSet* syntax_set_head = NULL;
SyntaxSet* syntax_set_curr = NULL;

//从syntax_set_head开始查找符号的FIRST集
SyntaxSet* GetSetPointer(string str)
{
	SyntaxSet* curr = syntax_set_head;
	while (curr->parent != str)
	{
		curr = curr->next;
	}
	return curr;
}
//增加一个集合
void AddSyntaxSet(string str)
{
	if (syntax_set_head == NULL)
	{
		syntax_set_head = new SyntaxSet();
		syntax_set_curr = syntax_set_head;
	}
	else
	{
		syntax_set_curr->next = new SyntaxSet();
		syntax_set_curr = syntax_set_curr->next;
	}
	syntax_set_curr->parent = str;
}
//递归求取可空集合的子程序
bool CheckNullable(string str)
{
	if (GetSetPointer(str)->nullable == true) return true;
	for (int i = 0; i < N; i++)
	{
		if (statm[i].left == str)
		{
			//如果有终结符出现则说明这个表达式不是可空的
			statm[i].curr = statm[i].right;
			bool flag = false;
			while (statm[i].curr != NULL)
			{
				if (IsTerminate(statm[i].curr->str) == true)
				{
					flag = true;
					break;
				}
				statm[i].curr = statm[i].curr->next;
			}
			if (flag == true) continue;
			//全是非终结符
			flag = false;
			statm[i].curr = statm[i].right;
			while (statm[i].curr != NULL)
			{
				if (statm[i].curr->str != str)
				{
					if (CheckNullable(statm[i].curr->str) == false)
					{
						flag = true;
						break;
					}
				}
				statm[i].curr = statm[i].curr->next;
			}
			if (flag == true) continue;
			GetSetPointer(str)->nullable = true;
			return true;
		}
	}
	return false;
}
//将元素加到对应的FIRST集中,有变化则返回true,没有变化则返回false
bool AddToFirstSet(string parent, string toadd)
{
	SyntaxSet *temp = GetSetPointer(parent);
	if (temp->child == NULL)
	{
		temp->child = new Syntax();
		temp->child->str = toadd;
		return true;
	}
	else
	{
		Syntax* curr = temp->child;
		while (curr->str != toadd)
		{
			if (curr->next != NULL)
			{
				curr = curr->next;
			}
			else
			{
				curr->next = new Syntax();
				curr->next->str = toadd;
				return true;
			}
		}
		return false;
	}
}
//实现集合的并运算
bool UnionTwoFirstSet(string dest, string source)
{
	SyntaxSet* destset_curr = GetSetPointer(dest);
	SyntaxSet* sourset_curr = GetSetPointer(source);
	Syntax* sour_curr = sourset_curr->child;
	bool flag = false;//指示是否发生变化
	while (sour_curr != NULL)
	{
		if (AddToFirstSet(dest, sour_curr->str) == true)
		{
			flag = true;
		}
		sour_curr = sour_curr->next;
	}
	return flag;
}
//求解FIRST集合并将结果存在SyntaxSet链表内
void GetFirstSet()
{
	//先扫描一边获得所有的非终结符
	set<string> scanned;
	scanned.clear();
	for (int i = 0; i < N; i++)
	{
		if (scanned.count(statm[i].left) == 0)//不存在
		{
			scanned.insert(statm[i].left);
			AddSyntaxSet(statm[i].left);
		}
		statm[i].curr = statm[i].right;
		while (statm[i].curr != NULL)
		{
			if (IsTerminate(statm[i].curr->str) == false && scanned.count(statm[i].curr->str) == 0)
			{
				scanned.insert(statm[i].curr->str);
				AddSyntaxSet(statm[i].curr->str);
			}
			statm[i].curr = statm[i].curr->next;
		}
		//扫描是否有空产生式，如果有的话则列为可空
		if (statm[i].right->str == "empty")
		{
			syntax_set_curr = syntax_set_head;
			while (syntax_set_curr != NULL&&syntax_set_curr->parent != statm[i].left)
			{
				syntax_set_curr = syntax_set_curr->next;
			}
			syntax_set_curr->nullable = true;
		}
	}
	//递归求取可空集合
	syntax_set_curr = syntax_set_head;
	while (syntax_set_curr != NULL&&syntax_set_curr->nullable == false)
	{
		CheckNullable(syntax_set_curr->parent);
		syntax_set_curr = syntax_set_curr->next;
	}
	//求解FIRST集合
	bool changed;
	do{
		changed = false;//指示本次迭代是否有变化
		for (int i = 0; i < N; i++)
		{
			statm[i].curr = statm[i].right;
			bool flag = false;
			while (IsTerminate(statm[i].curr->str) == false && GetSetPointer(statm[i].curr->str)->nullable == true)
			{
				if (UnionTwoFirstSet(statm[i].left, statm[i].curr->str) == true)
				{
					changed = true;
				}
				statm[i].curr = statm[i].curr->next;
			}
			if (statm[i].curr != NULL)
			{
				if (IsTerminate(statm[i].curr->str) == true)
				{
					if (AddToFirstSet(statm[i].left, statm[i].curr->str) == true)
					{
						changed = true;
					}
				}
				else
				{
					if (UnionTwoFirstSet(statm[i].left, statm[i].curr->str) == true)
					{
						changed = true;
					}
				}
			}
		}
	} while (changed == true);

}

/*
计算LR（1）项集的部分
*/
Item* C[500];//集族
int size_C;//集族数组的实际大小
multimap<string, int> Item_C_Map;//存储着项与C中的下标对应关系
multimap<string, int> ItemFirst_C_Map;//存储着项的第一分量和下标对应关系
//将项改为字符串格式，用于multiplemap
string GetOutputItemForm(Item item)
{
	string output = "";
	output = output + item.left + "->";
	Syntax* syntax_curr = item.right;
	while (syntax_curr != NULL)
	{
		output = output + syntax_curr->str;
		syntax_curr = syntax_curr->next;
	}
	output = output + "," + item.lookahead;
	return output;
}
//获得C中的下标
int GetItemIndexOfC(multimap<string, int> set, string target)
{
	//先检查是否存在，不存在返回-1
	typedef multimap<string, int>::size_type sz_type;
	sz_type entries = set.count(target);
	if (entries < 1) return -1;
	//返回下标
	multimap<string, int>::iterator iter = set.find(target);
	return iter->second;
}
//获得在C中第一项的下标
int GetItemIndexOfC(string target)
{
	for (int i = 0; i < size_C; i++)
	{
		if (GetOutputItemForm(*C[i]) == target)
		{
			return i;
		}
	}
	return -1;
}
//将项的第一分量改写为字符串格式
string GetOutputItemFirstForm(Item item)
{
	string output = "";
	output = output + item.left + "->";
	Syntax* syntax_curr = item.right;
	while (syntax_curr != NULL)
	{
		output = output + syntax_curr->str;
		syntax_curr = syntax_curr->next;
	}
	return output;
}
//添加项到项集 set对应映射 statm_curr对应指向statm的指针,用于goto
int AddItemToC(Item item)
{
	//先查找是否存在
	string search = GetOutputItemForm(item);
	///////////////////////////2015.4.19
	int index = GetItemIndexOfC(Item_C_Map, search);
	//新建一个项集
	if (index == -1)
	{
		C[size_C] = new Item();
		*C[size_C] = item;
		C[size_C]->next = NULL;
		Item_C_Map.insert(make_pair(search, size_C));
		size_C++;
		return size_C - 1;
	}
	return index;
}
//添加到下表为C[index]中，用于求闭包
void AddItemToC(Item item, int index)
{
	string search = GetOutputItemForm(item);
	bool flag = false;;
	Item* curr = C[index];
	//检测是否在这个闭包中
	while (curr != NULL)
	{
		if (search == GetOutputItemForm(*curr))
		{
			flag = true;
			break;
		}
		else
		{
			curr = curr->next;
		}
	}
	if (flag == false)//不在当前闭包
	{
		Item*curr = C[index];
		while (curr->next != NULL)
		{
			curr = curr->next;
		}
		curr->next = new Item();
		*curr->next = item;
		curr->next->next = NULL;
		Item_C_Map.insert(make_pair(search, index));
	}
}
//初始化集族，将[S'->S,$]项加入到第一个集族I_0
void InitializeC()
{
	size_C = 0;
	Item item(statm[0], "$");
	AddItemToC(item);
}
//分析表节点
struct AnalyseTableNode
{
	string key;
	string op;
	AnalyseTableNode* next;
	AnalyseTableNode(string k, string o)
	{
		key = k;
		op = o;
		next = NULL;
	}
};
//分析表
class AnalyseTable
{
public:
	int row_size;//行尺寸
	AnalyseTableNode* node[1000];
	AnalyseTable(int rs)
	{
		row_size = rs;
		for (int i = 0; i < 1000; i++)
		{
			node[i] = NULL;
		}
	}
	void Write(int row, string syntax, string op_str)
	{
		AnalyseTableNode* curr = node[row];
		while (curr->key != syntax)
		{
			curr = curr->next;
		}
		//当有冲突的时候给出提示并结束程序
		if (curr->op != ""&&curr->op != op_str&&op_str != "acc")
		{
			cout << "文法不是LALR(1)" << endl;
			exit(0);
		}
		else
		{
			curr->op = op_str;
		}
	}
	string Read(int row, string syntax)
	{
		AnalyseTableNode* curr = node[row];
		while (curr->key != syntax)
		{
			curr = curr->next;
		}
		return curr->op;
	}
	void AddNewSyntax(string syntax)
	{
		if (row_size > 0)
		{
			if (node[0] == NULL)
			{
				for (int i = 0; i < row_size; i++)
				{
					node[i] = new AnalyseTableNode(syntax, "");
				}
			}
			else
			{
				AnalyseTableNode* curr = node[0];
				while (curr != NULL&&curr->key != syntax)
				{
					curr = curr->next;
				}
				if (curr == NULL)
				{
					for (int i = 0; i < row_size; i++)
					{
						curr = node[i];
						while (curr->next != NULL)
						{
							curr = curr->next;
						}
						curr->next = new AnalyseTableNode(syntax, "");
					}
				}
			}
		}
	}
};
//定义一个分析表
AnalyseTable table(600);
//求闭包时使用到的First集合
Syntax* GetFirstSet(Syntax* beta_a)
{
	Syntax* output = NULL;
	Syntax* output_curr = NULL;
	Syntax* curr = beta_a;
	while (curr != NULL)
	{
		//遇到终结符
		if (IsTerminate(curr->str))
		{
			if (output == NULL)
			{
				output = new Syntax();
				output_curr = output;
			}
			else
			{
				output_curr->next = new Syntax();
				output_curr = output_curr->next;
			}
			output_curr->str = curr->str;
			output_curr->next = NULL;
			break;
		}
		//遇到不可空非终结符
		if (CheckNullable(curr->str) == false)
		{
			if (output == NULL)
			{
				output = new Syntax();
				output_curr = output;
			}
			else
			{
				output_curr->next = new Syntax();
				output_curr = output_curr->next;
			}
			syntax_set_curr = syntax_set_head;
			while (syntax_set_curr->next != NULL&&syntax_set_curr->parent != curr->str)
			{
				syntax_set_curr = syntax_set_curr->next;
			}
			Syntax* temp_curr = syntax_set_curr->child;
			while (temp_curr != NULL)
			{
				output_curr->str = temp_curr->str;
				if (temp_curr->next != NULL)
				{
					output_curr->next = new Syntax();
					output_curr = output_curr->next;
					temp_curr = temp_curr->next;
				}
				else
				{
					break;
				}
			}
			break;
		}
		//遇到可空符号
		if (CheckNullable(curr->str) == true)
		{
			if (output == NULL)
			{
				output = new Syntax();
				output_curr = output;
			}
			else
			{
				output_curr->next = new Syntax();
				output_curr = output_curr->next;
			}
			syntax_set_curr = syntax_set_head;
			while (syntax_set_curr->next != NULL&&syntax_set_curr->parent != curr->str)
			{
				syntax_set_curr = syntax_set_curr->next;
			}
			Syntax* temp_curr = syntax_set_curr->child;
			while (temp_curr != NULL)
			{
				output_curr->str = temp_curr->str;
				if (temp_curr->next != NULL)
				{
					output_curr->next = new Syntax();
					output_curr = output_curr->next;
					temp_curr = temp_curr->next;
				}
				else
				{
					break;
				}
			}
		}
		curr = curr->next;
	}
	return output;
}
//求闭包
void Closure(int i)
{
	Item*item_curr = C[i];
	//遍历每个项集下的项
	while (item_curr != NULL)
	{
		Syntax* syntax_curr = item_curr->right;
		while (syntax_curr != NULL&&syntax_curr->str != string("@"))
		{
			syntax_curr = syntax_curr->next;
		}
		if (syntax_curr != NULL)
		{
			syntax_curr = syntax_curr->next;
			//@符号不在最右端
			if (syntax_curr != NULL)
			{
				for (int j = 0; j < N; j++)
				{
					if (statm[j].left == syntax_curr->str)
					{

						Syntax* beta_a = NULL;
						Syntax* beta_a_curr = NULL;
						Syntax* source = syntax_curr->next;
						//处理第一个
						if (source != NULL)
						{
							beta_a = new Syntax();
							beta_a_curr = beta_a;
							beta_a_curr->str = source->str;
							source = source->next;
						}
						//从第二个开始处理
						while (source != NULL)
						{
							beta_a_curr->next = new Syntax();
							beta_a_curr = beta_a_curr->next;
							beta_a_curr->str = source->str;
							source = source->next;
						}
						//加入向前搜索符
						if (beta_a == NULL)
						{
							beta_a = new Syntax();
							beta_a->str = item_curr->lookahead;

						}
						else
						{
							beta_a_curr->next = new Syntax();
							beta_a_curr->next->str = item_curr->lookahead;
						}

						Syntax* la = GetFirstSet(beta_a);
						//添加项完成闭包
						while (la != NULL)
						{
							Item tempitem(statm[j], la->str);
							AddItemToC(tempitem, i);
							la = la->next;
						}

					}
				}
			}
		}
		else
		{
			//错误提示
			cout << "项没有@符号" << endl;
			exit(0);
		}
		//处理下一个项
		item_curr = item_curr->next;

	}

}
//将item的@符号向后移动一个符号，如果可以移动则返回true，否则返回false，通过指针传递
string MoveNext(Item* item)
{
	Syntax* curr = item->right;
	while (curr != NULL&&curr->str != "@")
	{
		curr = curr->next;
	}
	if (curr == NULL)
	{
		cout << "产生式有错误导致的项集中不包含@符号" << endl;
		exit(0);
	}
	else
	{
		if (curr->next != NULL)//在@后还有符号
		{
			curr->str = curr->next->str;
			curr->next->str = "@";
			return curr->str;
		}
		else//在@后没有符号
		{
			return "";
		}
	}
}
//////////测试代码
//输出项集
void OutputItemSet()
{
	for (int i = 0; i < size_C; i++)
	{
		cout << "I(" << i << ")" << endl;
		Item* item = C[i];
		while (item != NULL)
		{
			string output = GetOutputItemForm(*item);
			cout << output << endl;
			item = item->next;
		}
	}
	cout << "------------------End-----------------" << endl;
}
//输出分析表
void OutputAnalyseTable()
{
	//打印列标签
	AnalyseTableNode*curr = table.node[0];
	while (curr != NULL)
	{
		if (IsTerminate(curr->key) == true)
		{
			cout << "\t" << curr->key;
		}
		curr = curr->next;
	}
	curr = table.node[0];
	while (curr != NULL)
	{
		if (IsTerminate(curr->key) == false)
		{
			cout << "\t" << curr->key;
		}
		curr = curr->next;
	}
	cout << endl;
	//输出内容
	for (int i = 0; i < table.row_size; i++)
	{
		cout << i << ": ";
		//先输出action部分
		curr = table.node[i];
		while (curr != NULL)
		{
			if (IsTerminate(curr->key) == true)
			{
				cout << "\t" << curr->op;
			}
			curr = curr->next;
		}
		//再输出goto部分
		curr = table.node[i];
		while (curr != NULL)
		{
			if (IsTerminate(curr->key) == false)
			{
				cout << "\t" << curr->op;
			}
			curr = curr->next;
		}
		cout << endl;
	}
}
//用于程序内比较的转换程序
string GetOutputStatmForm(Statm statm)
{
	string output = "";
	output = output + statm.left + "->";
	Syntax* syntax_curr = statm.right;
	while (syntax_curr != NULL)
	{
		output = output + syntax_curr->str;
		syntax_curr = syntax_curr->next;
	}
	return output;
}
//用于显示的产生式显示程序
string GetOutputStatmFormForDisplay(Statm statm)
{
	string output = "";
	output = output + statm.left + "->";
	Syntax* syntax_curr = statm.right;
	while (syntax_curr != NULL)
	{
		output = output + "<" + syntax_curr->str + ">";
		syntax_curr = syntax_curr->next;
	}
	return output;
}
//删除掉@符号
void DeleteMark(Item* item)
{
	if (item->right->str == "@")
	{
		item->right = item->right->next;
	}
	else
	{
		Syntax* curr = item->right;
		while (curr->next != NULL&&curr->next->str != "@")
		{
			curr = curr->next;
		}
		curr->next = curr->next->next;
	}

}
//找到产生式的序号
int FindStatm(Item item)
{
	Item ditem(&item);
	DeleteMark(&ditem);
	string target = GetOutputItemFirstForm(ditem);
	for (int i = 0; i < N; i++)
	{
		string search = GetOutputStatmForm(statm[i]);
		if (search == target)
		{
			return i;
		}
	}
	return -1;
}
class syntax_pair
{
public:
	string kind;
	string name;
	syntax_pair()
	{
		kind = "";
		name = "";
	}
	syntax_pair(string k, string n)
	{
		kind = k;
		name = n;
	}
};
syntax_pair InputNextCharacter()
{
	string str;
	syntax_pair output;
	if (infile2 >> str)
	{
		while (str != "(")
		{
			if (infile2 >> str) continue;
			else
			{
				output.kind = "end";
				output.name = "end";
				return output;
			}
		}
		infile2 >> output.kind;
		infile2 >> str;
		infile2 >> output.name;
		while (str != ")")
		{
			if (infile2 >> str) continue;
			else
			{
				output.kind = "end";
				output.name = "end";
				return output;
			}
		}
	}
	else
	{
		output.kind = "end";
		output.name = "end";
	}
	return output;
}
string IntToString(int i)
{
	stringstream stream;
	stream << i;
	return stream.str();
}
int StringToInt(string str)
{
	int output;
	stringstream stream(str);
	stream >> output;
	return output;
}
//语义分析部分
//符号表结构定义
class SyntaxTableNode
{
public:
	int width;
	string id;
	int addr;
	SyntaxTableNode* next;
	SyntaxTableNode(string i,int a,int w)
	{
		id = i;
		addr = a;
		width = w;
		next = NULL;
	}
};
//符号表
SyntaxTableNode* syntax_table=NULL;
int offset = 0;//记录当前分配的内存的偏移，用于声明语句的语义翻译
int num_tempvar = 0;//临时变量的数量
void AddToSyntax_table(string id, int addr,int width)
{
	if (syntax_table == NULL)
	{
		syntax_table = new SyntaxTableNode(id, addr,width);
		offset = offset + width;
	}
	else
	{
		SyntaxTableNode* curr = syntax_table;
		while (curr->next != NULL)
		{
			if (curr->id == id)
			{
				cout << "重复定义变量 " + id << endl;
				exit(0);
			}
			curr = curr->next;
		}
		curr->next = new SyntaxTableNode(id, addr, width);
		offset = offset + width;
	}
}
//查看符号表syntax_table中是否对应id的变量
bool ExistInsyntax_table(string target)
{
	bool flag = false;
	SyntaxTableNode* curr = syntax_table;
	while (curr != NULL)
	{
		if (curr->id == target)
		{
			flag = true;
			break;
		}
		curr = curr->next;
	}
	return flag;
}
//查找符号表
SyntaxTableNode* QuerySyntax_table(string target_id)
{
	SyntaxTableNode* curr = syntax_table;
	while (curr != NULL)
	{
		if (curr->id == target_id)
		{
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}
//语义分析栈的节点数据结构
class TranslationStackNode
{
public:
	int width;//数据的宽度
	int kind;//如果是终结符，对应的种类
	string name;//表明符号的是什么E1 B 之类的
	string value;//对应符号的值
	string id;//对应的在符号表中的名字，用于填写符号表
	int start;//对应的开始语句标号
	TranslationStackNode(int w,int k, string n, int v)
	{
		width = w;
		kind = k;
		name = n;
		value = v;
	}
	TranslationStackNode()
	{
		kind = 0;
	}
};
stack<TranslationStackNode> TranslationStack;

void Push_To_Translation_Stack(syntax_pair next_input)
{
	TranslationStackNode node;
	node.kind = StringToInt(next_input.kind);
	node.name = next_input.name;
	switch (node.kind)
	{
	case 5:
		node.width = 4;
		break;
	case 6:
		node.width = 4;
		break;
	case 7:
		node.width = 4;
		break;
	case 8:
		node.width = 8;
		break;
	case 49:
		node.width = 25;
		break;
	case 34:
		node.id = next_input.name;
		node.value = node.id;
		break;
	case 35:
		node.width = 4;
		node.value = next_input.name;
	default:
		break;
	}
	TranslationStack.push(node);
}
class Code
{
public:
	int no;
	string op;
	string val1;
	string val2;
	string des;
	Code* next;
	Code(int n,string o, string v1, string v2, string d)
	{
		no = n;
		op = o;
		val1 = v1;
		val2 = v2;
		des = d;
		next = NULL;
	}
	Code()
	{
		next = NULL;
	}
};
int no_code = 0;
Code* code = NULL;
Code* curr_code = NULL;
void GenCode(string op, string a, string b, string des)
{
	if (code == NULL)
	{
		code = new Code(no_code,op, a, b, des);
		curr_code = code;
		no_code++;
	}
	else
	{
		curr_code = new Code(no_code, op, a, b, des);
		curr_code->next = code;
		code = curr_code;
		no_code++;
	}
}
//翻译成四元式
void Translation(int index)//index表示产生式的序号
{
	string id;
	int width;
	int num_groups;
	string value;
	string val1;
	string val2;
	string des;
	string op;
	TranslationStackNode val1_node, val2_node,op_node;
	Code* temp_code;
	switch (index)
	{
	case 10://IF1->if (LOG)
		curr_code = code;
		while (curr_code != NULL && (curr_code->op[0] != 'j' || curr_code->des != ""))
		{
			curr_code = curr_code->next;
		}
		if (curr_code == NULL)
		{
			cout << "if语句没有形成闭包" << endl;
			exit(0);
		}
		temp_code = curr_code;//在GenCode中会改变curr_code所以需要预存起来
		GenCode("j", "", "", IntToString(no_code+1));
		temp_code->des = IntToString(no_code);
		break;
	case 11://IF2->IF1 else B endelse
		curr_code = code;
		while (curr_code != NULL && curr_code->op[0] != 'j')
		{
			curr_code = curr_code->next;
		}
		if (curr_code == NULL)
		{
			cout << "if语句没有形成闭包" << endl;
			exit(0);
		}
		curr_code->des = IntToString(no_code);
		break;
	case 12://LOG->E1 LOG_OP E1 跳转语句有while语句生成时需要回去填写
		val2_node = TranslationStack.top(); TranslationStack.pop();
		op_node = TranslationStack.top(); TranslationStack.pop();
		val1_node = TranslationStack.top(); TranslationStack.pop();
		GenCode(op_node.value, val1_node.value, val2_node.value, "");
		break;
	case 13://LOG_OP->>,>=,<,<=,!=,==
		TranslationStack.top().value = "jbe";
		break;
	case 14:
		TranslationStack.top().value = "jb";
		break;
	case 15:
		TranslationStack.top().value = "jea";
		break;
	case 16:
		TranslationStack.top().value = "ja";
		break;
	case 17:
		TranslationStack.top().value = "je";
		break;
	case 18:
		TranslationStack.top().value = "jne";
		break;
	case 19://WHILE->while (LOG) B
		curr_code = code;
		while (curr_code!=NULL&&(curr_code->op[0] != 'j' || curr_code->des != ""))
		{
			curr_code = curr_code->next;
		}
		if (curr_code == NULL)
		{
			cout << "while语句没有形成闭包" << endl;
			exit(0);
		}
		temp_code = curr_code;//在GenCode中会改变curr_code所以需要预存起来
		GenCode("j", "", "",IntToString(curr_code->no) );
		temp_code->des = IntToString(no_code);
		break;
	case 20://E->Left=E1
		op = "=";
		val1 = TranslationStack.top().value;
		TranslationStack.pop();
		TranslationStack.pop();
		des = TranslationStack.top().value;
		TranslationStack.pop();
		GenCode(op, val1, " ", des);
		break;
	case 21://Left->id
		id = TranslationStack.top().id;
		if (ExistInsyntax_table(id))
		{
			TranslationStack.top().name = "Left";
			TranslationStack.top().width = QuerySyntax_table(id)->width;
			TranslationStack.top().value = id;
			TranslationStack.top().start = no_code;
		}
		else
		{
			cout << "未定义" + id << endl;
			exit(0);
		}		
		break;
	case 22://E1->id op E1
		val2_node = TranslationStack.top(); TranslationStack.pop();
		op_node = TranslationStack.top(); TranslationStack.pop();
		val1_node = TranslationStack.top();
		//获取id的宽度
		val1_node.width = QuerySyntax_table(val1_node.id)->width;
		TranslationStack.top().width = val1_node.width;
		if (val1_node.width != val2_node.width)
		{
			cout << "类型不符合" << endl;
			exit(0);
		}
		//建立新的临时变量
		TranslationStack.top().name = "E1";
		TranslationStack.top().id = "t" + IntToString(num_tempvar);
		num_tempvar++;
		AddToSyntax_table(TranslationStack.top().id, offset, TranslationStack.top().width);
		GenCode(op_node.name, val1_node.value, val2_node.value, TranslationStack.top().id);
		TranslationStack.top().value = TranslationStack.top().id;
		TranslationStack.top().start = val2_node.start;
		break;
	case 23://E1->(E1)
		TranslationStack.pop();
		val1_node = TranslationStack.top();
		TranslationStack.pop();
		TranslationStack.pop();
		TranslationStack.push(val1_node);
		break;
	case 24://E1->id
		id = TranslationStack.top().id;
		if (ExistInsyntax_table(id))
		{
			TranslationStack.top().name = "E1";
			TranslationStack.top().width = QuerySyntax_table(id)->width;
			TranslationStack.top().start = no_code;
		}
		else
		{
			cout << "未定义" + id << endl;
			exit(0);
		}
		break;
	case 25://E1->constint
		TranslationStack.top().name = "E1";
		TranslationStack.top().id = "t" + IntToString(num_tempvar);
		num_tempvar++;
		AddToSyntax_table(TranslationStack.top().id, offset, TranslationStack.top().width);
		GenCode("=", TranslationStack.top().value, " ", TranslationStack.top().id);
		TranslationStack.top().value = TranslationStack.top().id;
		TranslationStack.top().start = no_code;
		break;
	case 26:
	case 27:
		break;
	case 40:
		id = TranslationStack.top().name;
		TranslationStack.pop();
		width = TranslationStack.top().width;
		TranslationStack.pop();
		AddToSyntax_table(id, offset, width);
		break;
	case 42:
		TranslationStack.pop();//弹出48 ]
		num_groups = StringToInt(TranslationStack.top().name);
		TranslationStack.pop();//弹出35 常数
		TranslationStack.pop();//弹出47 [
		id = TranslationStack.top().name;
		TranslationStack.pop();//弹出 id
		width = TranslationStack.top().width*num_groups;
		TranslationStack.pop();
		AddToSyntax_table(id, offset, width);
		break;
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
		TranslationStack.top().kind = 0;
		TranslationStack.top().name = "Type";
		break;
	case 53://B->out id
		val1 = TranslationStack.top().name;
		TranslationStack.pop();
		TranslationStack.pop();
		GenCode("out", val1, "", "");
		break;
	}
}
int main()
{
	//输入产生式
	ifstream infile0("Grammar.in");
	infile0 >> N;
	for (int i = 0; i < N; i++)
	{
		statm[i] = Statm();
		//输入左部
		infile0 >> statm[i].left;
		//输入右部
		statm[i].right = new Syntax();
		statm[i].curr = statm[i].right;
		infile0 >> statm[i].curr->str;
		string tempstr;
		infile0 >> tempstr;
		while (tempstr != "$")
		{
			statm[i].curr->next = new Syntax();
			statm[i].curr = statm[i].curr->next;
			statm[i].curr->str = tempstr;
			infile0 >> tempstr;
		}
	}
	infile0.close();
	//输入分析表
	ifstream infile("table.txt");
	infile >> table.row_size;
	string temp_str;
	infile >> temp_str;
	while (temp_str != "e" && temp_str[0] != 'r' && temp_str[0] != 's')
	{
		table.AddNewSyntax(temp_str);
		infile >> temp_str;
	}
	for (int i = 0; i < table.row_size; i++)
	{
		AnalyseTableNode* curr_tablenode = table.node[i];
		while (curr_tablenode != NULL)
		{
			if (temp_str == "e")
			{
				curr_tablenode->op = "";
			}
			else
			{
				curr_tablenode->op = temp_str;
			}
			curr_tablenode = curr_tablenode->next;
			infile >> temp_str;
		}
	}
	infile.close();
	OutputAnalyseTable();
	//主控程序
	//输入接受符号
	table.Write(1, "$", "acc");
	offset = 0;
	stack<int> state;
	stack<syntax_pair> syntax_stack;
	//讲0压入栈
	state.push(0);
	syntax_pair next_input = InputNextCharacter();
	string action;
	int go_to;
	while (next_input.kind != "end")
	{
		action = table.Read(state.top(), next_input.kind);
		if (action == "acc")
		{
			cout << "语法分析完成" << endl;
			next_input = InputNextCharacter();
			state.pop();
			continue;
		}
		if (action == "")
		{
			cout << "出现错误" << endl;
			break;
		}
		if (action[0] == 's')//移进
		{
			string::iterator it = action.begin();
			action.erase(it);
			state.push(StringToInt(action));
			syntax_stack.push(next_input);
			Push_To_Translation_Stack(next_input);
			next_input = InputNextCharacter();
		}
		else//规约
		{
			string::iterator it = action.begin();
			action.erase(it);
			int index = StringToInt(action);
			statm[index].curr = statm[index].right;
			while (statm[index].curr != NULL)
			{
				state.pop();
				syntax_stack.pop();
				statm[index].curr = statm[index].curr->next;
			}
			//语义分析
			Translation(index);
			syntax_pair temp_syntax_pair(statm[index].left, "");
			syntax_stack.push(temp_syntax_pair);
			//处理goto
			go_to = StringToInt(table.Read(state.top(), syntax_stack.top().kind));//将新状态压入
			state.push(go_to);
			//输出产生式
			cout << GetOutputStatmFormForDisplay(statm[index]) << endl;
		}
	}
	infile2.close();
	//输出语义分析结果以及四元式
	ofstream outfile("output");
	outfile << "符号表" << endl;
	SyntaxTableNode* curr_syntax_table = syntax_table;
	while (curr_syntax_table != NULL)
	{
		outfile << curr_syntax_table->id + "\t" + IntToString(curr_syntax_table->addr) + "\t" + IntToString(curr_syntax_table->width) + "\t\n";
		curr_syntax_table = curr_syntax_table->next;
	}
	outfile << "四元式" << endl;
	stack<Code*> code_output_stack;
	curr_code = code;
	while (curr_code != NULL)
	{
		code_output_stack.push(curr_code);
		curr_code = curr_code->next;
	}
	while (!code_output_stack.empty())
	{
		curr_code = code_output_stack.top();
		outfile << curr_code->no<<":\t";
		outfile << curr_code->op + "\t";
		outfile << curr_code->val1 + "\t";
		outfile << curr_code->val2 + "\t";
		outfile << curr_code->des + "\t\n";
		code_output_stack.pop();
	}
	outfile.close();
	return 0;
}