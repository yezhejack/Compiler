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
�﷨������������
*/
int min(int a, int b, int c)
{
	int output;
	output = a;
	if (b < output) output = b;
	if (c < output) output = c;
	return output;
}
//�ķ����Ű����ս��
ifstream infile2("token.compile");
int N;//����ʽ������
class Syntax
{
public:
	string str;//����
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
//����ʽ��
class Statm
{
public:
	string left;//����ʽ��
	Syntax* right;//����ʽ�Ҳ�
	Syntax* curr;
	int num_right;//����ʽ�Ҳ�������
	Statm()
	{
		right = NULL;
		num_right = 0;
	}
};
//��
class Item
{
public:
	string left;
	Syntax* right;
	string lookahead;
	Item* next;
	//��ʼһ����ʾ��������˵�
	Item(Statm statm, string la)
	{
		left = statm.left;
		if (statm.right != NULL)
		{
			statm.curr = statm.right;
			right = new Syntax();
			//����@
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
//������ʽ
void OutputStatm()
{
	//���Ա��ʽ����
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
	string parent;//�����ϵķ��ս��
	Syntax* child;//�����ڵ�Ԫ��
	SyntaxSet* next;//��һ����
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

//��syntax_set_head��ʼ���ҷ��ŵ�FIRST��
SyntaxSet* GetSetPointer(string str)
{
	SyntaxSet* curr = syntax_set_head;
	while (curr->parent != str)
	{
		curr = curr->next;
	}
	return curr;
}
//����һ������
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
//�ݹ���ȡ�ɿռ��ϵ��ӳ���
bool CheckNullable(string str)
{
	if (GetSetPointer(str)->nullable == true) return true;
	for (int i = 0; i < N; i++)
	{
		if (statm[i].left == str)
		{
			//������ս��������˵��������ʽ���ǿɿյ�
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
			//ȫ�Ƿ��ս��
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
//��Ԫ�ؼӵ���Ӧ��FIRST����,�б仯�򷵻�true,û�б仯�򷵻�false
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
//ʵ�ּ��ϵĲ�����
bool UnionTwoFirstSet(string dest, string source)
{
	SyntaxSet* destset_curr = GetSetPointer(dest);
	SyntaxSet* sourset_curr = GetSetPointer(source);
	Syntax* sour_curr = sourset_curr->child;
	bool flag = false;//ָʾ�Ƿ����仯
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
//���FIRST���ϲ����������SyntaxSet������
void GetFirstSet()
{
	//��ɨ��һ�߻�����еķ��ս��
	set<string> scanned;
	scanned.clear();
	for (int i = 0; i < N; i++)
	{
		if (scanned.count(statm[i].left) == 0)//������
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
		//ɨ���Ƿ��пղ���ʽ������еĻ�����Ϊ�ɿ�
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
	//�ݹ���ȡ�ɿռ���
	syntax_set_curr = syntax_set_head;
	while (syntax_set_curr != NULL&&syntax_set_curr->nullable == false)
	{
		CheckNullable(syntax_set_curr->parent);
		syntax_set_curr = syntax_set_curr->next;
	}
	//���FIRST����
	bool changed;
	do{
		changed = false;//ָʾ���ε����Ƿ��б仯
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
����LR��1����Ĳ���
*/
Item* C[500];//����
int size_C;//���������ʵ�ʴ�С
multimap<string, int> Item_C_Map;//�洢������C�е��±��Ӧ��ϵ
multimap<string, int> ItemFirst_C_Map;//�洢����ĵ�һ�������±��Ӧ��ϵ
//�����Ϊ�ַ�����ʽ������multiplemap
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
//���C�е��±�
int GetItemIndexOfC(multimap<string, int> set, string target)
{
	//�ȼ���Ƿ���ڣ������ڷ���-1
	typedef multimap<string, int>::size_type sz_type;
	sz_type entries = set.count(target);
	if (entries < 1) return -1;
	//�����±�
	multimap<string, int>::iterator iter = set.find(target);
	return iter->second;
}
//�����C�е�һ����±�
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
//����ĵ�һ������дΪ�ַ�����ʽ
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
//����� set��Ӧӳ�� statm_curr��Ӧָ��statm��ָ��,����goto
int AddItemToC(Item item)
{
	//�Ȳ����Ƿ����
	string search = GetOutputItemForm(item);
	///////////////////////////2015.4.19
	int index = GetItemIndexOfC(Item_C_Map, search);
	//�½�һ���
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
//��ӵ��±�ΪC[index]�У�������հ�
void AddItemToC(Item item, int index)
{
	string search = GetOutputItemForm(item);
	bool flag = false;;
	Item* curr = C[index];
	//����Ƿ�������հ���
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
	if (flag == false)//���ڵ�ǰ�հ�
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
//��ʼ�����壬��[S'->S,$]����뵽��һ������I_0
void InitializeC()
{
	size_C = 0;
	Item item(statm[0], "$");
	AddItemToC(item);
}
//������ڵ�
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
//������
class AnalyseTable
{
public:
	int row_size;//�гߴ�
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
		//���г�ͻ��ʱ�������ʾ����������
		if (curr->op != ""&&curr->op != op_str&&op_str != "acc")
		{
			cout << "�ķ�����LALR(1)" << endl;
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
//����һ��������
AnalyseTable table(600);
//��հ�ʱʹ�õ���First����
Syntax* GetFirstSet(Syntax* beta_a)
{
	Syntax* output = NULL;
	Syntax* output_curr = NULL;
	Syntax* curr = beta_a;
	while (curr != NULL)
	{
		//�����ս��
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
		//�������ɿշ��ս��
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
		//�����ɿշ���
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
//��հ�
void Closure(int i)
{
	Item*item_curr = C[i];
	//����ÿ����µ���
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
			//@���Ų������Ҷ�
			if (syntax_curr != NULL)
			{
				for (int j = 0; j < N; j++)
				{
					if (statm[j].left == syntax_curr->str)
					{

						Syntax* beta_a = NULL;
						Syntax* beta_a_curr = NULL;
						Syntax* source = syntax_curr->next;
						//�����һ��
						if (source != NULL)
						{
							beta_a = new Syntax();
							beta_a_curr = beta_a;
							beta_a_curr->str = source->str;
							source = source->next;
						}
						//�ӵڶ�����ʼ����
						while (source != NULL)
						{
							beta_a_curr->next = new Syntax();
							beta_a_curr = beta_a_curr->next;
							beta_a_curr->str = source->str;
							source = source->next;
						}
						//������ǰ������
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
						//�������ɱհ�
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
			//������ʾ
			cout << "��û��@����" << endl;
			exit(0);
		}
		//������һ����
		item_curr = item_curr->next;

	}

}
//��item��@��������ƶ�һ�����ţ���������ƶ��򷵻�true�����򷵻�false��ͨ��ָ�봫��
string MoveNext(Item* item)
{
	Syntax* curr = item->right;
	while (curr != NULL&&curr->str != "@")
	{
		curr = curr->next;
	}
	if (curr == NULL)
	{
		cout << "����ʽ�д����µ���в�����@����" << endl;
		exit(0);
	}
	else
	{
		if (curr->next != NULL)//��@���з���
		{
			curr->str = curr->next->str;
			curr->next->str = "@";
			return curr->str;
		}
		else//��@��û�з���
		{
			return "";
		}
	}
}
//////////���Դ���
//����
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
//���������
void OutputAnalyseTable()
{
	//��ӡ�б�ǩ
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
	//�������
	for (int i = 0; i < table.row_size; i++)
	{
		cout << i << ": ";
		//�����action����
		curr = table.node[i];
		while (curr != NULL)
		{
			if (IsTerminate(curr->key) == true)
			{
				cout << "\t" << curr->op;
			}
			curr = curr->next;
		}
		//�����goto����
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
//���ڳ����ڱȽϵ�ת������
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
//������ʾ�Ĳ���ʽ��ʾ����
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
//ɾ����@����
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
//�ҵ�����ʽ�����
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
//�����������
//���ű�ṹ����
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
//���ű�
SyntaxTableNode* syntax_table=NULL;
int offset = 0;//��¼��ǰ������ڴ��ƫ�ƣ����������������巭��
int num_tempvar = 0;//��ʱ����������
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
				cout << "�ظ�������� " + id << endl;
				exit(0);
			}
			curr = curr->next;
		}
		curr->next = new SyntaxTableNode(id, addr, width);
		offset = offset + width;
	}
}
//�鿴���ű�syntax_table���Ƿ��Ӧid�ı���
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
//���ҷ��ű�
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
//�������ջ�Ľڵ����ݽṹ
class TranslationStackNode
{
public:
	int width;//���ݵĿ��
	int kind;//������ս������Ӧ������
	string name;//�������ŵ���ʲôE1 B ֮���
	string value;//��Ӧ���ŵ�ֵ
	string id;//��Ӧ���ڷ��ű��е����֣�������д���ű�
	int start;//��Ӧ�Ŀ�ʼ�����
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
//�������Ԫʽ
void Translation(int index)//index��ʾ����ʽ�����
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
			cout << "if���û���γɱհ�" << endl;
			exit(0);
		}
		temp_code = curr_code;//��GenCode�л�ı�curr_code������ҪԤ������
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
			cout << "if���û���γɱհ�" << endl;
			exit(0);
		}
		curr_code->des = IntToString(no_code);
		break;
	case 12://LOG->E1 LOG_OP E1 ��ת�����while�������ʱ��Ҫ��ȥ��д
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
			cout << "while���û���γɱհ�" << endl;
			exit(0);
		}
		temp_code = curr_code;//��GenCode�л�ı�curr_code������ҪԤ������
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
			cout << "δ����" + id << endl;
			exit(0);
		}		
		break;
	case 22://E1->id op E1
		val2_node = TranslationStack.top(); TranslationStack.pop();
		op_node = TranslationStack.top(); TranslationStack.pop();
		val1_node = TranslationStack.top();
		//��ȡid�Ŀ��
		val1_node.width = QuerySyntax_table(val1_node.id)->width;
		TranslationStack.top().width = val1_node.width;
		if (val1_node.width != val2_node.width)
		{
			cout << "���Ͳ�����" << endl;
			exit(0);
		}
		//�����µ���ʱ����
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
			cout << "δ����" + id << endl;
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
		TranslationStack.pop();//����48 ]
		num_groups = StringToInt(TranslationStack.top().name);
		TranslationStack.pop();//����35 ����
		TranslationStack.pop();//����47 [
		id = TranslationStack.top().name;
		TranslationStack.pop();//���� id
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
	//�������ʽ
	ifstream infile0("Grammar.in");
	infile0 >> N;
	for (int i = 0; i < N; i++)
	{
		statm[i] = Statm();
		//������
		infile0 >> statm[i].left;
		//�����Ҳ�
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
	//���������
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
	//���س���
	//������ܷ���
	table.Write(1, "$", "acc");
	offset = 0;
	stack<int> state;
	stack<syntax_pair> syntax_stack;
	//��0ѹ��ջ
	state.push(0);
	syntax_pair next_input = InputNextCharacter();
	string action;
	int go_to;
	while (next_input.kind != "end")
	{
		action = table.Read(state.top(), next_input.kind);
		if (action == "acc")
		{
			cout << "�﷨�������" << endl;
			next_input = InputNextCharacter();
			state.pop();
			continue;
		}
		if (action == "")
		{
			cout << "���ִ���" << endl;
			break;
		}
		if (action[0] == 's')//�ƽ�
		{
			string::iterator it = action.begin();
			action.erase(it);
			state.push(StringToInt(action));
			syntax_stack.push(next_input);
			Push_To_Translation_Stack(next_input);
			next_input = InputNextCharacter();
		}
		else//��Լ
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
			//�������
			Translation(index);
			syntax_pair temp_syntax_pair(statm[index].left, "");
			syntax_stack.push(temp_syntax_pair);
			//����goto
			go_to = StringToInt(table.Read(state.top(), syntax_stack.top().kind));//����״̬ѹ��
			state.push(go_to);
			//�������ʽ
			cout << GetOutputStatmFormForDisplay(statm[index]) << endl;
		}
	}
	infile2.close();
	//��������������Լ���Ԫʽ
	ofstream outfile("output");
	outfile << "���ű�" << endl;
	SyntaxTableNode* curr_syntax_table = syntax_table;
	while (curr_syntax_table != NULL)
	{
		outfile << curr_syntax_table->id + "\t" + IntToString(curr_syntax_table->addr) + "\t" + IntToString(curr_syntax_table->width) + "\t\n";
		curr_syntax_table = curr_syntax_table->next;
	}
	outfile << "��Ԫʽ" << endl;
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