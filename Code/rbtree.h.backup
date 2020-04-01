#define STRLEN 40
#define RefValue -1e8
#define red 0
#define black 1
#include <string.h>
#include <memory.h>
#include <stdio.h>

typedef struct RBNode {
	char color;
	char key[40];
 	int data;
	struct RBNode *left, *right;
} RBNode;

void initRBNode1(RBNode *node) {
	node->left = NULL;
	node->right = NULL;
	node->color = red;
}
void initRBNode2(RBNode *node, char d[], int k, char c, struct RBNode *l, struct RBNode *r) {
	strcpy(node->key, d);
	node->left = l;
	node->right = r;
	node->key = k;
	node->color = c;
}
typedef struct RBTree{
	RBNode *root;
	size_t _size;
} RBTree;
	void RotateL(RBTree* tree, RBNode *&ptr) {
		RBNode<K, E> *subL = ptr;
		ptr = ptr->right;
		subL->right = ptr->left;
		ptr->left = subL;
	}
	void RotateR(RBNode<K, E> *&ptr) {
		RBNode<K, E> *subR = ptr;
		ptr = ptr->left;
		subR->left = ptr->right;
		ptr->right = subR;
	}
	E & Search(RBNode<K, E> *&ptr, K k, bool& found) {
		E tmp();
		RBNode<K, E> *cur = ptr;
		while (cur != NULL) {
			if (k == cur->key) {
				found = true;
				return cur->data;
			}
			else if (k < cur->key) cur = cur->left;
			else cur = cur->right;
		}
		found = false;
		return tmp;
	}
	bool Insert(RBNode<K, E> *&ptr, E d, K k) {
		RBNode<K, E> *cur = NULL, *p = NULL, *pp = NULL;
		if (ptr == NULL) {
			ptr = new RBNode<K, E>(d, k, black);
			++_size;
			return true;
		}
		stack<RBNode<K, E> *> st;
		cur = ptr;
		while (cur != NULL) {
			st.push(cur);
			pp = p;
			p = cur;
			if (k == cur->key) return false;
			else if (k < cur->key) cur = cur->left;
			else cur = cur->right;
		}
		cur = new RBNode<K, E>(d, k, red);
		++_size;
		if (k < p->key) {
			p->left = cur;
		}
		else p->right = cur;
		if (p->color == black) return true;
		while(st.top() != pp) {
			st.pop();
		}
		st.pop();
		while (true) {
			RBNode<K, E> *uncle = (pp->left == p) ? pp->right : pp->left;
			if (p->color == black) break; 
	  		else if (uncle != NULL && uncle->color == red) {
				pp->left->color = pp->right->color = black;
				if (pp == root) break;
				pp->color = red;
				cur = pp;
			} 
			else if (uncle == NULL || uncle->color == black) {
				RBNode<K, E> *orgpp = pp; //原先的pp 
				if (pp->left == p) {
					if (cur == p->right) {
						RotateL(p);
						pp->left = p;
						cur = p->left;
					}
					pp->color = red;
					p->color = black;
					RotateR(pp); 
				}
				else if (pp->right == p) {
					if (cur == p->left) {
						RotateR(p);
						pp->right = p;
						cur = p->right;	
					}
					pp->color = red;
					p->color = black;
					RotateL(pp); 
				}
				if (st.empty()) {
					root = pp;
				}
				else {
					RBNode<K, E> *ppp = st.top();
					if (ppp->left == orgpp) ppp->left = pp;
					else ppp->right = pp;
				}
				break;
			}
			p = st.top();//注意此时栈一定不空，如果空的话，前面一定break掉了，所以可以果断弹出 
			st.pop();
			if (!st.empty()) { //这时候栈空不空就要看命了，如果这时候栈空，那么p就是root，下一次循环直接break，用不到pp 
				pp = st.top();
				st.pop();
			}
		}
		return true;
	}
	void delete_tree(RBNode<K, E> *ptr) {
		if (ptr != NULL) {
			delete_tree(ptr->left);
			delete_tree(ptr->right);
			delete ptr;
		}
	}
	void preOrder(RBNode<K, E> *ptr) {
		if (ptr != NULL) {
			cout << ptr->data << " color" << ptr->color << endl;
			preOrder(ptr->left);
			preOrder(ptr->right);
		}
	}
	void inOrder(RBNode<K, E> *ptr) {
		if (ptr != NULL) {
			inOrder(ptr->left);
			cout << ptr->data << " color" << ptr->color << endl;
			inOrder(ptr->right);
		}
	}
	void testRBTree(RBNode<K, E> *ptr, int &deep) {
		if (ptr != NULL) {
			if (ptr->color == black) {
				++deep;
			}
			testRBTree(ptr->left, deep);
			testRBTree(ptr->right, deep);
			if (ptr->color == black) --deep;
		}
		else {
			cout << deep << endl;
		}
	}
	
public:
	RBTree(): root(NULL), _size(0) {}
	~RBTree() {
		delete_tree(root);
		root = NULL;
	}
	size_t size() {
		return _size;
	}
	E & Search(K k) {//这个Search实际上是为了Map写的，如果查不到，就插入该节点，这是STL map使用[]访问时的行为 ！！ 
		bool found = false;
		E &result = Search(root, k, found);
		if (found == false) {
			Insert(E(), k);
			return Search(root, k, found);
		}
		return result;
	}
	bool Contains(K k) {
		bool found = false;
		Search(root, k, found);
		return found;
	}
	bool Insert(E d, K k) {
		return Insert(root, d, k);
	}
	bool Remove(K k) {
		return Remove(root, k);
	}
	void showInOrder() {
		inOrder(root);
	}
	void showPreOrder(){
		preOrder(root);
	}
	void testRB() {
		int deep = 0;
		testRBTree(root, deep);
	}
};
