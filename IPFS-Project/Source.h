#pragma once


class machine;
int hashFunction(string s, int m);
class file
{
public:
	string name;
	int hash;
	file() { name = " ";hash = 0; }
	file& operator=(const file& other)
	{
		if (this != &other) {
			hash = other.hash;
			name = other.name;
		}
		return *this;
	}
};
//Btree Node
class BTreeNode
{
	bool leaf;
	file* keys;
	int t, n;
	BTreeNode** C;

public:
	void getFiles(file* f, int& ind)
	{

		int i;
		for (i = 0; i < n; i++)
		{
			if (leaf == false)
				C[i]->getFiles(f, ind);
			//cout << keys[i].hash << " " << keys[i].name << " | ";
			f[ind] = keys[i];
			ind++;
		}
		if (leaf == false)
			C[i]->getFiles(f, ind);
	}
	BTreeNode(int _t, bool _leaf);
	void traverse();                 // for traverse
	BTreeNode* search(int k); // returns NULL if k is not present.

	//insert helper
	int findKey(int k);
	void NOT_FULL_INSERT(file k);
	void CHILD_SPLIT(int i, BTreeNode* y);

	//remove and helper
	void remove(int k);
	void REMOVAL_FIX(int idx);
	void LEFT_BORROW(int idx);
	file PREDECESSOR(int idx);
	file SUCCESSOR(int idx);
	void MERGE(int idx);
	void RIGHT_BORROW(int idx);
	void NONLEAF_REMOVAL(int idx);
	void LEAF_REMOVAL(int idx);
	friend class Btree;
};


//Btree
class Btree
{
public:
	int t; // Minimum degree
	BTreeNode* root; // Pointer to root node
	int total_files;

	Btree()
	{
		root = NULL;
		t = -1;
		total_files = 0;
	}
	void traverse()
	{
		if (root != NULL)
			root->traverse();
	}
	void insert(file k);
	Btree(int _t)
	{
		t = _t;
		root = NULL;
		total_files = 0;
	}
	BTreeNode* search(int k)
	{
		if (root == NULL)
			return NULL;
		else
			return root->search(k);
	}
	void remove(int k);

	void getFiles(file* f, int ind)
	{
		if (root != nullptr)
			root->getFiles(f, ind);
	}

};

class machine
{
public:
	int ID;
	machine* next;
	int Successor_ID;
	machine** rtable;
	Btree b;
	bool status;
	int id_space;

	machine(int identifier_space, int id)
	{
		id_space = identifier_space;
		status = false;
		ID = id;
		next = NULL;
		Successor_ID = -1;
		rtable = new machine * [identifier_space];
	}
};
class ring_DHT
{
public:
	machine* head;
	int id_space;
	int total_nodes;
	// constructor
	ring_DHT(int identifier_space)
	{
		id_space = identifier_space;
		total_nodes = pow(2, id_space);
		head = new machine(id_space, 0);
		machine* temp = head;
		for (int i = 1;i < total_nodes;i++)
		{
			temp->next = new machine(id_space, i);
			temp = temp->next;
		}
		temp->next = head;
		initialize_routing_tables();
	}
	//fnc to initialize the rtable
	void initialize_routing_tables()
	{
		machine* temp2 = head;
		for (int i = 0;i < total_nodes;i++)
		{
			for (int j = 0;j < id_space;j++)
			{
				int a = pow(2, (j + 1) - 1);
				a += temp2->ID;
				machine* temp = extract_node(a);
				temp2->rtable[j] = temp;
				a = a;
			}
			temp2 = temp2->next;
		}
	}
	// fnc to update the routing table

	void update_routing_tables()
	{
		machine* temp2 = head;
		for (int i = 0;i < total_nodes;i++)
		{
			if (temp2->status == true) {
				for (int j = 0;j < id_space;j++)
				{
					int a = pow(2, (j + 1) - 1);
					a += temp2->ID;
					a = a % total_nodes;
					machine* temp = extract_node(a);
					if (temp->status == true)
					{
						temp2->rtable[j] = temp;
					}
					else {
						machine* temp3 = next_ON_node(temp);
						temp2->rtable[j] = temp3;
						//cout << "HELLO";
					}
				}
			}
			temp2 = temp2->next;
		}
	}
	void add_machine(int number, int m)//ON machine with given ID number
	{
		machine* temp = extract_node(number);
		temp->status = true;
		update_routing_tables();
		machine* temp2 = next_ON_node(temp);
		temp->b.t = m;
	}
	//fnc to transfer files to file* f from src
	void transfer_files(int size, string src, string dest, file* f)
	{
		for (int i = 0;i < size;i++)
		{
			string source = src;
			string dest2 = dest;

			source += f[i].name;
			dest2 += f[i].name;
			source += ".txt";
			dest2 += ".txt";
			std::ifstream sourceFile(source, std::ios::binary);
			std::ofstream destFile(dest2, std::ios::binary);
			if (!sourceFile) {
				std::cout << "Error opening source file: " << source << std::endl;
				return;
			}
			if (!destFile) {
				std::cout << "Error opening destination file: " << dest2 << std::endl;
				return;
			}
			destFile << sourceFile.rdbuf();   //cpy files
			sourceFile.close();
			destFile.close();
			if (std::remove(source.c_str()) == 0) {
				std::cout << "File moved successfully from " << source << " to " << dest << std::endl;
			}
			else {
				std::cout << "Error moving file." << std::endl;
			}
		}
	}
	//fnc to remove a machine
	void remove_machine(int number, string de_path)
	{
		machine* temp1 = extract_node(number);
		machine* temp2 = next_ON_node(temp1);
		int size = temp1->b.total_files;
		//	cout << "total files : " << temp1->b.total_files << endl;
		file* tempf = new file[temp1->b.total_files];
		temp1->b.getFiles(tempf, 0);

		///////////file handle
		string source = de_path;
		string dest = de_path;

		dest += to_string(temp2->ID);
		source += to_string(temp1->ID);
		dest += "/";
		source += "/";
		transfer_files(size, source, dest, tempf);

		///////////file handle
		// update Btree of next responsible machine
		for (int i = 0;i < size;i++)
		{
			temp1->b.remove(tempf[i].hash);
			temp2->b.insert(tempf[i]);
		}
		temp1->status = false;
		update_routing_tables();
	}
	//fnc to insert a file
	int insert_file(file f, int hash)
	{
		machine* temp = extract_node(hash);
		int count = 0;

		if (temp->status == true)
		{
			temp->b.insert(f);
			//cout << temp->ID << " machine " << endl;
			/*cout << "\n Traverse of Btree \n";
			temp->b.traverse();
			cout << endl;*/
			return temp->ID;
		}
		else
		{
			machine* temp2 = next_ON_node(temp);
			temp2->b.insert(f);
			//cout << temp2->ID << " machine2" << endl;
			/*cout << "traverse in next \n";
			temp2->b.traverse();*/
			return temp2->ID;
		}


	}
	//fnc to delete a file
	int delete_file(int hash)
	{
		machine* temp = extract_node(hash);
		if (temp->status == true)
		{
			temp->b.remove(hash);
			/*cout << "\n Traverse of Btree \n";
			temp->b.traverse();
			cout << "==================\n";*/
			return temp->ID;
		}
		else
		{
			machine* temp2 = next_ON_node(temp);
			temp2->b.remove(hash);
			/*cout << "traverse in next \n";
			temp2->b.traverse();
			cout << "==================\n";*/
			return temp2->ID;
		}
	}
	//fnc to find a file
	void  find_file(int hash)
	{
		machine* temp = head;
		int count = 0;
		while (count < total_nodes)
		{
			if (hash <= temp->ID)
			{
				temp->b.search(hash);
				return;
			}
			for (int i = 0;i < id_space;i++)
			{
				if (hash <= temp->rtable[i]->ID)
				{
					temp = temp->rtable[i];
					temp->b.search(hash);
					return;
				}
			}
			temp = temp->rtable[0];
			count++;
		}
	}
	//fnc to print rtable
	void Print_rtable(int ID)
	{
		machine* temp = extract_node(ID);
		if (temp->status == false)
		{
			cout << "MACHINE IS CLOSE of ID: "<<temp->ID << endl;
			return;
		}
		for (int i = 0;i < id_space;i++)
		{
			cout << temp->rtable[i]->ID << endl;
		}

	}
	// HELPER
	void print_helper(machine* n)
	{
		for (int i = 0;i < id_space;i++)
		{
			cout << n->rtable[i]->ID << endl;
		}
	}
	machine* extract_node(int value)
	{
		machine* temp = head;
		for (int i = 0;i < value;i++)
		{
			temp = temp->next;
		}
		return temp;
	}
	machine* next_ON_node(machine* n)
	{
		machine* temp = n->next;
		int count = 0;
		while (temp != NULL)
		{
			if (temp->status == true)
			{
				break;
			}
			if (count == (total_nodes - 1))
			{
				cout << "NO OPEN MACHINE" << endl;
				return NULL;
			}
			temp = temp->rtable[0];
			count++;
		}
		return temp;
	}
	// fnc to display Btree of machine
	void btree_display(int ID)
	{
		machine* temp = extract_node(ID);
		if (temp->status == false)
		{
			cout << "MACHINE IS OFF" << endl;
			return;
		}
		else
		{
			cout << " TRAVERSE FUNCTION OF MACHINE " << temp->ID << endl;
			temp->b.traverse();
		}
	}
	//traverse of Btree of responsible machine
	void btree_display2(int ID)
	{
		machine* temp = head;
		int count = 0;
		while (count < total_nodes)
		{
			if (ID <= temp->ID)
			{
				cout << " TRAVERSE FUNCTION OF MACHINE " << temp->ID << endl;
				temp->b.traverse();
				return;
			}
			for (int i = 0;i < id_space;i++)
			{
				if (ID <= temp->rtable[i]->ID)
				{
					temp = temp->rtable[i];
					cout << " TRAVERSE FUNCTION OF MACHINE " << temp->ID << endl;
					temp->b.traverse();
					return;
				}
			}
			temp = temp->rtable[0];
			count++;
		}
	}
};

///////////////////////////////////////////////////BTREE/////////////////////////////////////////////////////////////
BTreeNode::BTreeNode(int t1, bool leaf1)
{
	leaf = leaf1;
	t = t1;      // minimum no of keys
	int m = 2 * t;
	C = new BTreeNode * [m];    //childs
	n = 0;
	keys = new file[m - 1];   //keys
}

// finds key in a node
int BTreeNode::findKey(int k)
{
	int idx = 0;
	while (idx < n && keys[idx].hash < k)++idx;
	return idx;
}

// Remove the key k
void BTreeNode::remove(int k)
{
	int idx = 0;
	idx = findKey(k);            // returns index of key

	// The key to be removed is present in this node
	switch (idx < n && keys[idx].hash == k)
	{
	case 1:
		(leaf) ? LEAF_REMOVAL(idx) : NONLEAF_REMOVAL(idx);
		break;
	case 0:
		if (leaf)
		{
			cout << "The key " << k << " is does not exist in the tree\n";
			return;
		}
		bool flag = 0;

		if (idx == n)
			flag = true;
		else
			flag = false;

		(C[idx]->n < t) ? REMOVAL_FIX(idx) : void(0); // fixes the effect of removal of key

		(flag && idx > n) ? C[idx - 1]->remove(k) : C[idx]->remove(k);              // left : right of a key
		break;
	}
	return;
}
// removes from leaf
void BTreeNode::LEAF_REMOVAL(int idx)
{
	int i = idx + 1;
	while (i < n)
	{
		keys[i - 1] = keys[i];
		i++;
	}
	n--;
	return;
}
// removes internal key
void BTreeNode::NONLEAF_REMOVAL(int idx)
{
	int k = 0;
	k = keys[idx].hash;

	file pred = PREDECESSOR(idx);
	file succ = SUCCESSOR(idx);

	if (C[idx]->n >= t)				//if pred has extra key
	{
		keys[idx] = pred;
		C[idx]->remove(pred.hash);
	}
	else if (C[idx + 1]->n >= t)	// if succ has extra key
	{
		keys[idx] = succ;
		C[idx + 1]->remove(succ.hash);
	}
	else
	{								// otherwise merge
		MERGE(idx);
		C[idx]->remove(k);
	}
	return;
}

//find pred
file BTreeNode::PREDECESSOR(int idx)
{
	BTreeNode* cur = NULL;
	cur = C[idx];
	for (; !cur->leaf; cur = cur->C[cur->n]);                    // rightmost key
	return cur->keys[cur->n - 1];
}

//find succ
file BTreeNode::SUCCESSOR(int idx)
{
	BTreeNode* cur = NULL;
	cur = C[idx + 1];                                           // leftmost key
	for (; !cur->leaf; cur = cur->C[0]);
	return cur->keys[0];
}

//fnc to remove effect of removal key
void BTreeNode::REMOVAL_FIX(int idx)
{

	switch (idx != 0 && C[idx - 1]->n >= t)
	{
	case 1:
		LEFT_BORROW(idx);
		break;
	case 0:
		(idx != n && C[idx + 1]->n >= t) ?
			RIGHT_BORROW(idx)
			:
			(idx != n) ? MERGE(idx) : MERGE(idx - 1);
		break;
	}
	return;
}
// fnc to split child 
void BTreeNode::CHILD_SPLIT(int i, BTreeNode* y)
{

	BTreeNode* z = new BTreeNode(y->t, y->leaf);
	int j = 0;
	z->n = t - 1;

	while (j < t - 1)
	{
		z->keys[j] = y->keys[j + t];           //copy keys
		j++;
	}

	if (y->leaf == false)
	{
		int j = 0;
		while (j < t)       // cpy childs
		{
			z->C[j] = y->C[j + t];
			j++;
		}
	}
	y->n = t - 1;
	j++;
	j = n;
	while (j >= i + 1)   //shifting nodes internal
	{
		C[j + 1] = C[j];
		j--;
	}

	C[i + 1] = z;
	j--;
	j = n - 1;
	while (j >= i)   // shifting keys
	{
		keys[j + 1] = keys[j];
		j--;
	}
	keys[i] = y->keys[t - 1];     // shift key to up
	j++;
	n = n + 1;                     // add 1 in count of internal keys
}
//fnc to borrow from left
void BTreeNode::LEFT_BORROW(int idx)
{

	BTreeNode* child = C[idx], * sibling = C[idx - 1];

	int i = child->n - 1;
	while (i >= 0)                // finding last key
	{
		child->keys[i + 1] = child->keys[i];
		i--;
	}
	i++;
	if (!child->leaf)
	{
		int i = child->n;
		while (i >= 0)
		{
			child->C[i + 1] = child->C[i];
			i--;
		}
	}

	child->keys[0] = keys[idx - 1];
	if (!child->leaf)
		child->C[0] = sibling->C[sibling->n];
	// moving parent to right's child

	keys[idx - 1] = sibling->keys[sibling->n - 1];        // making key a parent
	child->n += 1;
	sibling->n -= 1;

	return;
}

// fnc to merge the nodes and delete the sibling
void BTreeNode::MERGE(int idx)
{
	BTreeNode* child = C[idx], *sibling = C[idx + 1];

	child->keys[t - 1] = keys[idx];

	int i = 0; 
	while (i < sibling->n)				// transfer keys
	{
		child->keys[i + t] = sibling->keys[i];
		i++;
	}

	if (!child->leaf)
	{
		int i = 0; 
		while (i <= sibling->n)			// transfer ptrs
		{	
			child->C[i + t] = sibling->C[i];
			i++;
		}
	}
	 i = idx + 1; 
	 while (i < n)				// shift keys
	 {
		 keys[i - 1] = keys[i];
		 i++;
	 }

	i = idx + 2; 
	while (i <= n)				// shift ptrss
	{
		C[i - 1] = C[i];
		i++;
	}
	i--;
	child->n += sibling->n + 1;
	n--;

	delete (sibling);
	return;
}

// fnc to borrow from right 
void BTreeNode::RIGHT_BORROW(int idx)
{

	BTreeNode* child = C[idx], * sibling = C[idx + 1];

	child->keys[(child->n)] = keys[idx];
	int ijk = 0;
	if (!(child->leaf))
		child->C[(child->n) + 1] = sibling->C[0];

	keys[idx] = sibling->keys[0];
	ijk++;
	int i = 1;
	while (i < sibling->n)
	{
		sibling->keys[i - 1] = sibling->keys[i];
		i++;
	}
	ijk++;
	if (!sibling->leaf)
	{
		int i = 1;
		while (i <= sibling->n)
		{
			sibling->C[i - 1] = sibling->C[i];
			i++;
		}
	}
	child->n += 1;
	sibling->n -= 1;

	return;
}
// fnc to search key
BTreeNode* BTreeNode::search(int k)
{
	int i = 0;
	bool f = 0;
	while (i < n && k > keys[i].hash)
		i++;

	if (keys[i].hash == k)
		return this;
	else if (leaf == true)
		return NULL;
	else
		return C[i]->search(k);
}

// fnc to insert a key
void Btree::insert(file k)
{
	total_files++;
	switch (root == NULL)
	{
	case 1:
		root = NULL;
		root=new BTreeNode(t, true);
		root->keys[0].hash = k.hash;
		root->keys[0].name = k.name;
		root->n = 1;
		break;
	case 0:
		if (root->n == 2 * t - 1)		//root is full
		{
			BTreeNode* s = NULL;
			s=new BTreeNode(t, false);
			s->C[0] = root;
			int i = 0;
			s->CHILD_SPLIT(0, root);                // making root
			(s->keys[0].hash < k.hash) ? i++ : i = i;                             // adding new key
			s->C[i]->NOT_FULL_INSERT(k);
			root = s;                            // change root
		}
		else
		{
			root->NOT_FULL_INSERT(k);			//not_full_insert
		}
		break;
	}
}
// fnc to insert if node is not full
void BTreeNode::NOT_FULL_INSERT(file k)
{
	int i = n - 1;

	// If this is a leaf node
	switch (leaf == true)
	{
	case 1:
		while (i >= 0 && keys[i].hash > k.hash)
		{
			keys[i + 1] = keys[i];
			i--;
		}
		keys[i + 1] = k;
		n = n + 1;
		break;
	case 0:
		while (i >= 0 && keys[i].hash > k.hash)
			i--;                        // child who is going to have the new key

		// found child is full
		if (C[i + 1]->n == 2 * t - 1)
		{
			int tmep = 0;
			CHILD_SPLIT(i + 1, C[i + 1]);
			(keys[i + 1].hash < k.hash) ? i++ : i = i;
		}
		C[i + 1]->NOT_FULL_INSERT(k);
		break;
	}
}


// fnc to traverse the Btree
void BTreeNode::traverse()
{
	int i;
	i = 0; 
	while(i < n)
	{
		(leaf == false) ? C[i]->traverse() : void(0);
		cout << keys[i].hash << " " << keys[i].name << " | ";
		i++;
	}
	(leaf == false) ? C[i]->traverse() : void(0);
}



// fnc to remove a key
void Btree::remove(int k)
{
	if (!root)		
	{
		cout << "The tree is empty\n";
		return;
	}
	total_files--;
	root->remove(k);

	if (root->n == 0)		// root node is empty
	{
		BTreeNode* tmp = NULL;
		tmp=root;
		(root->leaf) ? root = NULL : root = root->C[0];
		delete tmp;
	}
	return;
}

int hashFunction(string s, int m)
{
	return s.size() % m;
}