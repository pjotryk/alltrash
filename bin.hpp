#include <iostream>

using namespace std;

template<typename T>
class binary_tree
{	
	struct Node
	{
		T data;
		Node* left, *right;
		
		Node(T i, Node* l=NULL, Node* r = NULL)
		{
			data = i;
			left = l;
			right = r;
		}
	};
	Node *root;
public:
	binary_tree() { root= NULL; }
	binary_tree(T* init, int size)
	{
		root = new Node[size];
	}
	~binary_tree() { delete root; }
	
	
	
};
