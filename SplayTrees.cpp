#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

ifstream in("splaytree.in");
ofstream out("splaytree.out");

//Faster access to recently used keys
//search, insert, remove, succesor, predecesor, sorted
// 1: Node is root -> we simply return the root
// 2: Node has only parents (no grandparents)
// -> node is a left child of root (we do a right rotation) = ZIG
// -> node is a right child of its parent (we do a left rotation) = ZAG
// 3: Node has both parent and grandparent
// -> node is left child of parent and parent is also left child of grand parent (two right rotations) = ZIG-ZIG
// -> node is right child of parent and parent is also right child of grand parent (two left Rotations) = ZAG-ZAG
// -> node is left child of parent and parent is right child of grand parent (left rotation followed by right rotation) = ZAG-ZIG
// -> node is right child of parent and parent is left child of grand parent (right rotation followed by left rotation) = ZIG-ZAG

//node
struct node
{
	int data;
	node* left;
	node* right;
	node* parent;
};

class Splay_Tree
{
	node* root = NULL;

public:
	node* new_node(int data, node* n) 
	{
		node* a = new node;
		a->data = data;
		a->parent = n;
		a->right = NULL;
		a->left = NULL;
		return a;
	}

	node* getRoot()
	{
		return root;
	}

	void right_rotate(node* x)
	{
		node* y = x->left;
		if (x->left != NULL)
			x->left = y->right;
		if (y->right != NULL)
			y->right->parent = x;
		y->parent = x->parent;
		if (x->parent == NULL) //x = root
			root = y;
		else if (x == x->parent->right) //x = right child
			x->parent->right = y;
		else							//x = left child
			x->parent->left = y;
		y->right = x;
		x->parent = y;
	}

	void left_rotate(node* x)
	{
		node* y = x->right;
		if (x->right != NULL)
			x->right = y->left;
		if (y->left != NULL)
			y->left->parent = x;
		y->parent = x->parent;
		if (x->parent == NULL) //x = root
			root = y;
		else if (x == x->parent->left) //x = left child
			x->parent->left = y;
		else						   //x = right child
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}

	void splay(node* x)
	{
		while (x->parent != NULL) //node != root
		{ 
			if (x->parent == root) //node has only parents (no grandparents)
				if (x == x->parent->left) 
					right_rotate(x->parent); //ZIG
				else
					left_rotate(x->parent); //ZAG

			else  //node has both parents and grandparents
			{
				node* p = x->parent;
				node* g = p->parent; //grandparent

				if (x->parent->left == x && p->parent->left == p)  
					//both are left children = ZIG-ZIG
				{
					right_rotate(g);
					right_rotate(p);
				}
				else if (x->parent->right == x && p->parent->right == p) 
					//both are right children = ZAG-ZAG
				{
					left_rotate(g);
					left_rotate(p);
				}
				else if (x->parent->right == x && p->parent->left == p) 
					//node is left child of parent and parent is right child of grand parent = ZAG-ZIG
				{
					left_rotate(p);
					right_rotate(g);
				}
				else if (x->parent->left == x && p->parent->right == p) 
					//node is right child of parent and parent is left child of grand parent = ZIG-ZAG
				{
					right_rotate(p);
					left_rotate(g);
				}
			}
		}
	}

	//insert
	void insert(int data) 
	{
		if (root == NULL) //if tree is empty
		{
			root = new_node(data, NULL);
			return;
		}
		node* r = root;
		node* parent = NULL;
		while (r != NULL) 
		{
			parent = r;
			if (r->data > data)
				r = r->left; //if the value we want to insert is lower than our current node we continue to the left subtree
			else
				r = r->right; //if the value we want to insert is higher than our current node we continue to the right subtree
		}
		node* auxnode = new_node(data, parent);
		//auxnode now has left = null , right = null
		if (data < parent->data) 
			parent->left = auxnode; //if the value we want to insert is lower than our current node we insert it to the left
		else
			parent->right = auxnode; // if the value we want to insert is higher than our current node we insert it to the right
		splay(auxnode); //making the new node the root
	}

	//search
	node* search(int data) 
	{
		node* r = root; //we start from the root of the tree
		while (r != NULL && r->data != data) //searching the tree
		{
			if (data < r->data) //if the data given is less than our current node
				r = r->left; //we go to the left subtree
			else
				r = r->right;//else we go to the right subtree
		}
		if (r == NULL)
			return NULL; //the value searched is not in the tree
		else
			return r; //we found the value searched
	}

	node* val_minim(node* n)//search the minimum value of the splaytree
	{
		while (n->left != NULL)
			n = n->left;
		return n;
	}

	//remove
	void remove(int data) 
	{
		node* n = search(data); //in n we have the node we want to remove
		if (n == NULL) //we don't have the node in the tree
			return;
		splay(n);
		node* parent = n->parent;
		if (n->left == NULL && n->right == NULL) //node has no children
		{
			if (n != root) //if n is not the root
			{
				if (parent->left == n)
					parent->left = NULL;
				else
					parent->right = NULL;
			}
			else
				root = NULL;//if n is the root, the splaytree becomes empty
			delete n;
		}
		else if (n->left != NULL && n->right != NULL)//node has 2 children
		{
			node* s = val_minim(n->right);//we take the most left node from the right subtree
			int data = s->data;
			remove(s->data);
			n->data = data;
		}
		else // node has 1 children
		{
			node* c;
			if (n->left != NULL)
				c = n->left;
			else
				c = n->right;
			if (n != root) //if n is not the root, its parents takes their child
				if (n == parent->left)
					parent->left = c;
				else
					parent->right = c;
			else//if n is the root, set the child to be the root
			{
				root = c;
				c->parent = NULL;
			}
			delete n;
		}

	}

	int maxi_data(int data)
	{
		node* r = root;
		node* parent = NULL;
		while (r != NULL)
		{
			if (data == r->data)
			{
				splay(r);
				return r->data;
			}
			else if (data < r->data)
				r = r->left;
			else
			{
				parent = r;
				r = r->right;
			}
		}
		splay(parent);
		return parent->data;
	}

	int mini_data(int data)
	{
		node* r = root;
		node* parent = NULL;
		while (r != NULL)
		{
			if (data == r->data)
			{
				splay(r);
				return r->data;
			}
			else if (data > r->data)
				r = r->right;
			else
			{
				parent = r;
				r = r->left;
			}
		}
		splay(parent);
		return parent->data;
	}

	void order(node* root, int x, int y)
	{
		if (root == NULL)
			return;
		if (x < root->data)
			order(root->left, x, y);
		if (x <= root->data && y >= root->data)
			out << root->data << " ";
		if (y > root->data)
			order(root->right, x, y);
	}
};

int main()
{
	//1 : inserati numarul X in multime
	//2 : stergeti numarul X din multime(daca acesta exista)
	//3 : afisati 1 daca numarul X este in multime, alftel afisati 0
	//4 : afisati cel mai mare numar Y, mai mic sau egal cu X
	//5 : afisati cel mai mic numar Y, mai mare sau egal cu X
	//6 : afisati in ordine sortata, toate numerele Z, unde X ≤ Z ≤ Y

	int Q, task, x, y;
	Splay_Tree splaytree;
	in >> Q;
	for (int i = 0; i < Q; i++)
	{
		in >> task >> x;
		if (task == 1)
			splaytree.insert(x);
		else if (task == 2)
			splaytree.remove(x);
		else if (task == 3)
		{
			if (splaytree.search(x) == NULL)
				out << "0" << "\n";
			else
				out << "1" << "\n";
		}
		else if (task == 4)
			out << splaytree.maxi_data(x) << "\n";
		else if (task == 5)
			out << splaytree.mini_data(x) << "\n";
		else if (task == 6)
		{
			in >> y;
			splaytree.order(splaytree.getRoot(), x, y);
			out << "\n";
		}
	}
}
