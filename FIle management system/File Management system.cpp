#include<iostream>
#include<string.h>
#include<vector>
#include<list>
#include<conio.h>
#include<fstream>
using namespace std;

class user
{
	int id;
	int operationtype;		//1 for write ,0 for read

	friend class file;

	template<typename v>
	friend class hashtable;

public:
	user()
	{
		id = 0;
		operationtype = 0;
	}
	user(int id, int OT)
	{
		this->id = id;
		operationtype = OT;
	}
	~user()
	{
	}
};

template<typename k>
class QItem
{
	k key;
	user info;

	template<typename k>
	friend class PQueue;

	friend class user;

	friend class file;

	template<typename v>
	friend class hashtable;
public:
	QItem()
	{
		key = 0;
	}
	QItem(k key, user value)
	{
		this->key = key;
		this->info = value;
	}
	user getinfo()
	{
		return info;
	}
	~QItem()
	{
		info.~user();
	}
};

template<typename k>
class PQueue
{
	vector<QItem<k>>arr;
	int total;
	int min;

	template<typename k>
	friend class QItem;

	friend class user;

	friend class file;

	template<typename v>
	friend class hashtable;

public:
	PQueue()
	{
		total = 0;
		min = 0;
	}
	void insert(k key, int id, int operation)
	{
	
		if (key == -1)
		{
			if (arr.size() != 0)
			{
				key = arr[0].key;
				key++;
			}
			else
				key = 100;
		}
		if (arr.size() == 0)
		{
			user val(id, operation);
			QItem<k> h(key, val);
			arr.push_back(h);
			total++;
		}
		else
		{
			user val(id, operation);
			QItem<k> h(key, val);
			arr.push_back(h);
			total++;
			int index = total - 1;
			while ((index - 1) >= 0)
			{
				if (arr[(index - 1) / 2].key < arr[index].key)
					swap(arr[(index - 1) / 2], arr[index]);

				index = (index - 1) / 2;
			}
			if (min < h.key)
				h.key = min;
		}


	}
	void getmax(user& val)
	{
		val = arr[0].info;
	}
	void ReheapDown(int cnode, int last)
	{
		int maxChild, rightChild, leftChild;
		leftChild = 2 * cnode + 1;
		rightChild = 2 * cnode + 2;
		if (leftChild <= last) { // left child is part of the heap

			if (leftChild == last) // only one child
				maxChild = leftChild;
			else {
				if (arr[leftChild].key <= arr[rightChild].key)

					maxChild = rightChild;

				else

					maxChild = leftChild;

			}
			if (arr[cnode].key < arr[maxChild].key)
			{
				
				swap(arr[cnode], arr[maxChild]);
				ReheapDown(maxChild, last);
			}
		}
	}
	void deletemax()
	{
		QItem<k> item = arr[0];
		arr[0] = arr[total - 1];
		total--;
		arr.resize(total);
		ReheapDown(0, total - 1);
	}
	bool isEmpty()
	{
		return !total;
	}
	int size()
	{
		return total;
	}
	~PQueue()
	{

		arr.~vector();
	}

};

class file
{
	int id;
	PQueue<int> Q;
	vector<int> activeusers;
	bool writer;

	template<typename k>
	friend class PQueue;

	friend class user;

	template<typename k>
	friend class QItem;

	template<typename v>
	friend class hashtable;

public:
	file()
	{
		id = 0;
		writer = false;
	}
	file(int id)
	{
		this->id = id;
		writer = false;
	}
	void giveaccess()
	{

		while (!Q.isEmpty() && Q.arr[0].info.operationtype != 1 && writer != true)
		{
			activeusers.push_back(Q.arr[0].info.id);
			Q.deletemax();
		}
		if (Q.arr.size())
		{
			if (activeusers.empty() && Q.arr[0].info.operationtype == 1)
			{
				activeusers.push_back(Q.arr[0].info.id);
				Q.deletemax();
				writer = true;
			}
		}

	}
	void print()
	{
		cout << "File " << id << endl;
		if (activeusers.size() != 0)
		{
			cout << "access given to user " << activeusers[0] << endl;
		}
		cout << "activeusers : " << activeusers.size() << endl;
		if (Q.size() != 0)
		{
			cout << "next user " << Q.arr[0].info.id << " ";
			if (Q.arr[0].info.operationtype == 1)
				cout << " to write" << endl;
			else
				cout << " to read" << endl;
		}
		cout << "users waiting : " << Q.size() << endl << endl << endl;
	}
	void removeuser(int i)
	{
		if (activeusers.size() > 1)
		{
			for (int j = i; j + 1 < activeusers.size(); j++)
			{
				swap(activeusers[j], activeusers[j + 1]);
			}
			activeusers.resize(activeusers.size() - 1);
		}
		else if (activeusers.size() == 1)
		{
			activeusers.resize(activeusers.size() - 1);
			writer = false;
		}
	}
	~file()
	{
		Q.~PQueue();
		activeusers.~vector();
	}
};

template<typename v>
class hashtable
{
	vector<list<file>>table;
	int tablesize;

public:
	hashtable()
	{
		tablesize = 0;
	}
	hashtable(int tablesize)
	{
		this->tablesize = tablesize;
		table.resize(tablesize);

	}
	int hashfunction(int id)
	{
		return id % tablesize;
	}
	void hashfile(const file& F)
	{
		bool flag = false;
		int h = hashfunction(F.id);
		for (auto i = table[h].begin(); i != table[h].end(); i++)
		{
			if ((*i).id == F.id)
			{
				cout << "file already present"<<endl;
				flag = true;
				break;
			}
		}
		if (!flag)
			table[h].push_back(F);
	}
	void requestfile(int fileid, int user, int priority, int operation)// priority 1 for urgent, 0 for normal
	{
		bool flag = false;
		int h = hashfunction(fileid);
		for (auto i = table[h].begin(); i != table[h].end(); i++)
		{
			if ((*i).id == fileid)
			{
				(*i).Q.insert(priority, user, operation);
				flag = true;
			}
		}
		if (!flag)
		{
			cout << "file does not exist" << endl;
		}
		grantaccess();
	}
	void releasefile(int fileid, int user)
	{
		bool flag = false;
		int h = hashfunction(fileid);
		for (auto i = table[h].begin(); i != table[h].end(); i++)
		{
			if ((*i).id == fileid)
			{
				for (int j = 0; j < (*i).activeusers.size(); j++)
				{
					if ((*i).activeusers[j] == user)
					{
						(*i).removeuser(j);
						flag = true;
						break;
					}
				}
			}
		}
		if (flag == false)
		{
			cout << "user is not using the file" << endl;
		}
		grantaccess();
	}
	void grantaccess()
	{
		for (int j = 0; j < table.size(); j++)
		{
			for (auto i = table[j].begin(); i != table[j].end(); i++)
			{
				(*i).giveaccess();
			}
		}
	}
	void print()
	{
		for (int j = 0; j < table.size(); j++)
		{
			for (auto i = table[j].begin(); i != table[j].end(); i++)
			{
				cout << "H" << j << "->";
				(*i).print();
			}
		}

	}
	void deletefile(int fileid)
	{

		bool flag = false;
		int h = hashfunction(fileid);
		for (auto i = table[h].begin(); i != table[h].end();i++)
		{
			if ((*i).id == fileid)
			{
				if ((*i).activeusers.size() == 0)
					table[h].erase(i);
				else
					cout << "people are still using this file bruh :(" << endl;

				flag == true;
				break;
			}
		}
		if (!flag)
			cout << "file does not exist" << endl;
	}
	~hashtable()
	{
		table.~vector();
	}
};

void createfile(hashtable<file>& Filehanadler)
{
	int id;
x:
	cout << "Enter file ID : " << endl;
	cin >> id;

	if (id < 5000 || id>99999)
	{
		cout << "File ID out of range.";
		goto x;
	}
	file F(id);
	Filehanadler.hashfile(F);

}
void reqfile(hashtable<file>& Filehanadler)
{
	int fileid, user, priority, operation;
x1:
	cout << "Enter file id(from above) : ";
	cin >> fileid;
	if (fileid < 5000 || fileid>99999)
	{
		cout << "File ID out of range!" << endl;
		goto x1;
	}

x2:
	cout << "Enter user id :";
	cin >> user;
	if (user < 0 || user>10000)
	{
		cout << "User ID out of range!" << endl;
		goto x2;
	}

x3:
	cout << "enter priority(-1 for urgent) :";
	cin >> priority;
	if (priority < -1 || priority>100)
	{
		cout << "Priority out of range!" << endl;
		goto x3;
	}

x4:
	cout << "enter 1 for write and 0 for read : ";
	cin >> operation;
	if (operation < 0 || operation > 1)
	{
		cout << "Incorrect Format!" << endl;
		goto x4;
	}

	Filehanadler.requestfile(fileid, user, priority, operation);


}
void relfile(hashtable<file>& Filehanadler)
{
	int fileid, userid;
x1:
	cout << "enter file id to be released : ";
	cin >> fileid;
	if (fileid < 5000 || fileid>99999)
	{
		cout << "File ID out of range!" << endl;
		goto x1;
	}

x2:
	cout << "enter user id to be released : ";
	cin >> userid;
	if (userid < 0 || userid>10000)
	{
		cout << "User ID out of range!" << endl;
		goto x2;
	}

	Filehanadler.releasefile(fileid, userid);
}
void deletefile(hashtable<file>& Filehanadler)
{
	int fileid;
	cout << "enter file id to be deleted :";
	cin >> fileid;
	Filehanadler.deletefile(fileid);
}
void loaddata(hashtable<file>& Filehanadler)
{
	int fileid = 0, user = 0, priority = 0, operation = 0, noofdatapoints = 0;
	string o;
	ifstream rdr("Text.txt");
	rdr >> noofdatapoints;
	for (int i = 0;i < noofdatapoints;i++)
	{
		rdr.ignore() >> fileid;
		rdr.ignore() >> user;
		rdr.ignore() >> priority;
		rdr.ignore() >> o;
		if (o == "R")
			operation = 0;
		else
			operation = 1;

		file F(fileid);
		Filehanadler.hashfile(F);
		Filehanadler.requestfile(fileid, user, priority, operation);
	}
}
int main()
{
	hashtable<file> Filehanadler(11);


	while (true)
	{
		cout << endl << endl;
		Filehanadler.print();
		cout << endl << endl;
		cout << "press :" << endl;
		cout << "1 to create file" << endl;
		cout << "2 to delete file" << endl;
		cout << "3 to request file" << endl;
		cout << "4 to release file" << endl;
		cout << "5 to load data" << endl;
		cout << "any other key to exit" << endl;
		bool exit = false;;
		int keycode = 0;
		keycode = _getch();

		switch (keycode)
		{
		case 49:
			createfile(Filehanadler);
			break;
		case 50:
			deletefile(Filehanadler);
			break;
		case 51:
			reqfile(Filehanadler);
			break;
		case 52:
			relfile(Filehanadler);
			break;

		case 53:
			loaddata(Filehanadler);
			break;
		default:
			exit = true;
			break;
		}
		if (exit)
			break;
	}

}

