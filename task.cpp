#include <iostream>
#include <Windows.h>
#include <random>

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 20);

using namespace std;

struct Node{

	int key;
	Node* left_son = nullptr;
	Node* right_sibling = nullptr;
	Node* parent = nullptr;
	bool isVirtual;

	Node(int k, bool v) {
		key = k;
		isVirtual = v;
	}
};

class Tree {

public:
	Node* root = nullptr;

	Tree(){}

	~Tree() {
		delTree(root);
	}

	void insert(int key) {
		Node* node = new Node(key, false);
		if (root == nullptr) root = node;
		else {
			insertNode(root, node);				//создадим корень или добавим новый узел в дерево
		}
	}

	void insertNode(Node* current, Node* node) {			//при добавлении вершины стоит игнорировать повторяющиеся узлы

		if (node->key == current->key) return;				//исключаем повторения

		if (node->key < current->key) {
			if (getLkid(current) == NULL) {
				addLeft(current, node);
				node->parent = current;
				return;
			}												//если у узла нет левого потомка или он виртуальный
			if (current->left_son->isVirtual == true) {
				addLeft(current, node);
				node->parent = current;
				return;
			}
			else {
				insertNode(current->left_son, node);
			}
		}
		else {
			if (getRkid(current) == NULL) {			//если у узла нет правого потомка
				addRight(current, node);
				node->parent = current;
			}
			else {
				insertNode(current->left_son->right_sibling, node);			//в противном случае будем считать, что левый сын есть 
			}
		}	
	}

	void addLeft(Node* parent, Node* kid) {
		if (parent->left_son == NULL) parent->left_son = kid;
		if (parent->left_son->isVirtual == true) {
			kid->right_sibling = parent->left_son->right_sibling;		//переназначаем правого брата для нового узла
			delete parent->left_son;
			parent->left_son = kid;
		}
	}

	void addRight(Node* parent, Node* kid) {
		if (parent->left_son != NULL) {
			(parent->left_son)->right_sibling = kid;				//правый сын создается с виртуальным узлом если нет левого брата
		}
		else {
			Node* tmp = new Node(NULL, true);
			parent->left_son = tmp;				//создаем виртуальный узел для хранения правого сына
			tmp->parent = parent;
			addRight(parent, kid);
		}
	}

	Node* search(int key, Node* node) {			//функция возвращает nullptr если не был найден заданный узел
		if (node == nullptr) return nullptr;
		if (key == node->key) return node;

		if (key > node->key) {
			if (getRkid(node) != NULL) {
				search(key, node->left_son->right_sibling);
			}
			return nullptr;
		}
		else {
			if (getLkid(node) != NULL) {
				search(key, node->left_son);
			}
			return nullptr;
		}
	}

	void PrintTree(Node* node, int l) {
		if (root != NULL) {
			int i;
			if (getRkid(node) != NULL) PrintTree(node->left_son->right_sibling, l + 1);
			for (i = 1; i < l; i++) cout << "\t";
			if (node->isVirtual == false) {
				cout << node->key << endl;
			}
			else {
				cout << '-' << endl;
			}
			if (getLkid(node) != NULL) PrintTree(node->left_son, l + 1);
		}
		else {
			cout << "Tree is empty!" << endl;
		}
	}

	Node* delNode(Node* node) {

		if (node->left_son == NULL) {		//предположим, что у узла вообще нет детей. ни левого обычного, ли левого виртуального, ни правого

			if (node == root) {				//предположим, что узел является вершиной и у него нет родителя
				root = nullptr;
				delete node;
				return nullptr;				//это будет означать окончание работы алгоритма на дереве из-за удаления последнего узла
			}

			else {			//иначе считаем, что у узла есть ненулевой родитель и узел не корневой
				Node* tmp = node->parent;
				if (node->parent->left_son == node) {		//если удаляемая вершина является левым сыном своего родителя

															//тут тоже необходимо рассмотреть 2 случая, так как у узла может быть правый брат

					if (node->right_sibling == NULL) {		//правого брата нет, удаляем спокойно
						node->parent->left_son = nullptr;
						delete node;
						return tmp;
					}
					else {
						node->isVirtual = true;				//иначе узел становится левым виртуальным братом для правого брата
						node->key = 0;
						return tmp;
					}
				}
				else {			//иначе удаляемая вершина является правым сыном своего родителя

					if (node->parent->left_son->isVirtual == true) {		//необходимо проверить наобходимость удаления своего левого брата
						delete node->parent->left_son;
						node->parent->left_son = nullptr;		//удалили левого виртуального брата
						delete node;
						return tmp;			//удалили узел и вернули родителя
					}
					else {
						node->parent->left_son->right_sibling = nullptr;			//иначе ее левый брат лишается правого брата и узел удаляется
						delete node;
						return tmp;
					}
				}
			}

		}

		if (node->left_son != NULL and node->left_son->right_sibling == NULL) {			//у узла есть только левый сын
			Node* tmp = node->left_son;

			if (node == root) {				//предположим, что узел является вершиной и у него нет родителя
				root = node->left_son;
				node->left_son->parent = nullptr;
				delete node;
				return root;			//вернет новявленный корень дерева
			}

			else {			//родитель у узла имеется
				if (node->parent->left_son == node) {			//если является левым сыном родителя, нужно проверить, если ли у него правый брат!!
					node->parent->left_son = node->left_son;
					node->left_son->parent = node->parent;
					node->left_son->right_sibling = node->right_sibling;		//если вдруг у узла(левого сына) был правый брат, иначе nullptr
					delete node;
					return tmp;
				}
				else {			//иначе является правым сыном, значит есть реальный или виртуальный левый брат
					node->parent->left_son->right_sibling = node->left_son;
					node->left_son->parent = node->parent;
					delete node;
					return tmp;
				}
			}

		}

		if (node->left_son->isVirtual == true) {			//у узла есть только правый сын
			Node* tmp = node->left_son->right_sibling;

			if (node == root) {				//предположим, что узел является вершиной и у него нет родителя
				root = node->left_son->right_sibling;
				node->left_son->right_sibling->parent = nullptr;
				delete node->left_son;
				delete node;
				return tmp;
			}
			else {			//иначе у узла есть родитель и он либо левый либо правый сын

				if (node->parent->left_son == node) {		//если удаляемый узел является левым сыном своего родителя
					node->parent->left_son = node->left_son->right_sibling;
					node->left_son->right_sibling->parent = node->parent;
					node->left_son->right_sibling->right_sibling = node->right_sibling;			//если вдруг у узла(левого сына) был правый брат, иначе nullptr
					delete node->left_son;
					delete node;
					return tmp;
				}
				else {			//иначе узел является правым сыном и у родителя точно есть обычный или виртуальный левый сын
					node->parent->left_son->right_sibling = node->left_son->right_sibling;
					node->left_son->right_sibling->parent = node->parent;
					delete node->left_son;
					delete node;
					return tmp;
				}
			}
		}

		else {			//иначе считаем, что у узла и левый и правый сын

			//тут в целом неважно, является ли он корнем или не является. мы будет производить поиск подходящей замены и потом удалим ее рекурсивно
			
			//будем искать максимальное значение в левом поддереве
			Node* tmp = Max(node->left_son);
			node->key = tmp->key;			//заменяем на новый ключ
			delNode(tmp);		//функция возвратила бы родителя удаленной вершины, но вернет нам тот же самый узел с другим ключом.
			return node;
		}
		//по идее алгоритм сам разберется с вариантом удаления полученного максимума, так как прогонит его по той же самой функции
	}

	Node* getRkid(Node* parent) {
		if (parent->left_son != NULL) {
			return parent->left_son->right_sibling;
		}
		return NULL;
	}

	Node* getLkid(Node* parent) {
		if (parent->left_son != NULL) {
			return parent->left_son;
		}
		return NULL;
	}

	Node* Max(Node* node) {
		if (getRkid(node) != NULL) {
			return Max(node->left_son->right_sibling);
		}
		return node;
	}

	Node* Min(Node* node) {
		if (getLkid(node) != NULL) {
			return Min(node->left_son);
		}
		return node;
	}

	//алгоритмы прямого и обратного обхода в соответствии с тз

	void directWalk(Node* node) {
		if (node != NULL and node->isVirtual == false) {
			cout << node->key << endl;
			directWalk(getLkid(node));
			directWalk(getRkid(node));
		}
	}


	void symmetricWalk(Node* node) {
		if (node != NULL and node->isVirtual == false) {
			symmetricWalk(getLkid(node));
			cout << node->key << endl;
			symmetricWalk(getRkid(node));
		}
	}

	//для дерева B предписан симметричный обход. Он используетсся при поиске узла по ключу при прямом обходе A

	void symmetricSearch(Node* node, int key, bool &flag) {
		if (node != NULL and node->isVirtual == false and flag == false) {
			symmetricSearch(getLkid(node), key, flag);
			if (node->key == key) flag = true;
			symmetricSearch(getRkid(node), key, flag);
		}
	}

	//используем алгоритм обратного отхода для удаления дерева

	void delTree(Node* node) {
		if (node != NULL) {				//удалению подлежат и виртуальные и обычные узлы
			delTree(getRkid(node));				//сначала удалим правого брата, потом исходный узел
			delTree(getLkid(node));
			delete node;		//рекурсивно удаляем все узлы дерева 
		}
		root = nullptr;
	}

	//реализуем прямой обход дерева A с симметричным поиском встречаемых вершин в дереве B

	void doTask(Node* A, Node* B) {
		bool flag = false;
		if (A != NULL and A->isVirtual == false) {

			while (flag == false and A != NULL) {
				symmetricSearch(B, A->key, flag);			//если нам придется удалить узел, необходимо перепроверить, не стало ли его значение неподходящим

				if (flag == true) break;
				A = delNode(A);
			}

			if (A != NULL) {
				if (getLkid(A) != NULL) {
					doTask(getLkid(A), B);
				}
				if (getRkid(A) != NULL) {
					doTask(getRkid(A), B);
				}
			}


		}
	}

};


int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Tree A;
	Tree B;
	Tree C;
	
	for (int i = 0; i < 10; i++) {
		A.insert(dist6(rng));
	}


	for (int j = 0; j < 10; j++) {
		B.insert(dist6(rng));
	}

	A.PrintTree(A.root, 1);
	cout << "\n\n" << endl;
	B.PrintTree(B.root, 1);
	cout << "\n\n" << endl;
	
	A.doTask(A.root, B.root);
	A.PrintTree(A.root, 1);

	return 0;
}
