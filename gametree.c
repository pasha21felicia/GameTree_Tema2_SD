#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct node {
	char **table;		//matricea de la taskul 1
	int nr_children;	//nr de copii al nodului curent
	int visited;		//pentru taskul 3 are valoare 1/0
	int value;			//pentru taskul 2-3 valoarea nodului
	struct node *firstChild;	//pointer la lista de copii
	struct node *parent;		//pointer la parintele nodului curent
	struct node *nextSibling;	//pointer la urmatorul element din lista
} Node;

//structura auxiliara folosita la taskul 2 pentru a creea arborele
typedef struct queue {
	Node *tree_node;
	struct queue *next;
} queue;

//**************************taskul 1*******************************

//Functia citeste din fisierul de intrare datele initiale si
//matricea initiala pe care o adauga in root
void read_table(Node **root, int *rows, int *cols, char *player, FILE *input) {
	char *aux = calloc(10, sizeof(char));
	char *str = calloc(10000, sizeof(char));

	fgets(aux, 9, input);
	*rows = aux[0] - '0';
	*cols = aux[2] - '0';
	*player = aux[4];

	(*root)->table = calloc(*rows + 1, sizeof(char *));

	for (int i = 0; i < *rows; i++) {
		fgets(str, 9999, input);
		int k = 0;
		(*root)->table[i] = calloc(*cols + 1, sizeof(char));

		if (str[strlen(str) - 1] == '\n')
			str[strlen(str) - 1] = '\0';
		//adaug in root->table doar caracterele diferite de ' '
		for (int j = 0; j < strlen(str); j++) {
			if (str[j] != ' ') {
				(*root)->table[i][k] = str[j];
				k++;
			}
		}
	}
	free(str);
	free(aux);
}
//Functia de printare a tabelului cu numarul de \t corespunzator level
void print_table(Node *nod, FILE *output, int level, int rows, int cols) {

	for (int i = 0; i < rows; i++) {
		//printez taburile
		for (int k = 0; k < level; k++)
			fprintf(output, "\t");
		//printez deja caracterele
		for (int j = 0; j < cols-1; j++)
			fprintf(output, "%c ", nod->table[i][j]);

		fprintf(output, "%c\n", nod->table[i][cols-1]);
	}
	fprintf(output, "\n");
}

//Functii de verificare daca jocul s-a sfarsit
//prin 4 elemente consecutive in cele 4 directii
int checkVerti(Node *nod, int i, int j, int rows, int cols) {
	if (rows - i < 4) return 0;		//conditie de validare a directiei
	char cell = nod->table[i][j];

	if (cell == 'R' || cell == 'B') {
		if (cell == nod->table[i+1][j]
			&& cell == nod->table[i+2][j]
			&& cell == nod->table[i+3][j])
			return 1;
	}
	return 0;
}
int checkHori(Node *nod, int i, int j, int rows, int cols) {

	if (cols - j < 4) return 0;
	char cell = nod->table[i][j];

	if (cell == 'R' || cell == 'B') {
		if (cell == nod->table[i][j+1]
			&& cell == nod->table[i][j+2]
			&& cell == nod->table[i][j+3])
			return 1;
	}
	return 0;
}
int checkDiagLeft(Node *nod, int i, int j, int rows, int cols) {

	if (j < 3 || rows - i < 4) return 0;
	char cell = nod->table[i][j];

	if (cell == 'R' || cell == 'B') {
		if (cell == nod->table[i+1][j-1]
			&& cell == nod->table[i+2][j-2]
			&& cell == nod->table[i+3][j-3])
			return 1;
	}
	return 0;
}
int checkDiagRight(Node *nod, int i, int j, int rows, int cols) {

	if (rows - i < 4 || cols - j < 4) return 0;
	char cell = nod->table[i][j];

	if (cell == 'R' || cell == 'B') {
		if (cell == nod->table[i+1][j+1]
			&& cell == nod->table[i+2][j+2]
			&& cell == nod->table[i+3][j+3])
			return 1;
	}
	return 0;
}
//Functia de verificare daca s-a gasit o combinatie castigatoare
int check(Node *nod, int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (checkVerti(nod, i, j, rows, cols)
				|| checkHori(nod, i, j, rows, cols)
				|| checkDiagRight(nod, i, j, rows, cols)
				|| checkDiagLeft(nod, i, j, rows, cols))
				return 1; //jocul s-a sfarsit
		}
	}
	return 0; //jocul continua
}
//Functie de a afla consecutivitatea de mutari a jucatorilor
char whose_turn_is(char current_player) {
	if (current_player == 'B') return 'R';
	else return 'B';
}
//Functie utilitara de a copia datele dintr-un tabel in alt tabel
void copyTable(Node **dest, Node *src, int rows, int cols) {

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) 
			(*dest)->table[i][j] = src->table[i][j];
	}
}
//Functia de a crea un nod de lista
Node *alloc_Node(Node *current_node, int rows, int cols) {

	Node *newNode= calloc(1, sizeof(Node));
	//aloc memorie si tabelului
	newNode->table = calloc(rows, sizeof(char *));
	for (int i = 0; i < rows; i++)
		newNode->table[i] = calloc(cols, sizeof(char));

	newNode->parent = current_node;
	newNode->firstChild = NULL;
	newNode->nr_children = 0;
	newNode->nextSibling = NULL;
	return newNode;
}
//Functia de a adauga un nod in lista de copii a nodului curent
void add_Child (Node **current_node, Node *newChild) {
	Node *temp;
	//daca lista de copii e goala, capul listei devine noul nod
	if ((*current_node)->firstChild == NULL)
		(*current_node)->firstChild = newChild;
	else {
		temp = (*current_node)->firstChild;
		//parcurg lista de copii si adaug nodul la final
		while (temp->nextSibling != NULL )
			temp = temp->nextSibling;
		temp->nextSibling = newChild;
	}
}
//Functia principala a taskului 1 de creare a arborelui de joc
void create_Tree(Node *current_node, int rows, int cols, char player, int level, FILE *output) {
	char next_player;
	Node *newNode = NULL;
	//parcurgerea unui tabel are loc mai intii pe coloane apoi de randuri
	//pe coloane de la coloana 0 -> cols-1
	//pe randuri de la randul rows-1 -> 0

	for(int i = 0; i < cols; i++) {
		//cazul particular daca am '-' pe ultimul rand si pozitionez
		//urmatorul caracter exact pe acel rand rows-1
		if (current_node->table[rows - 1][i] == '-') {
			//aloc un nod nou
			newNode  = alloc_Node(current_node, rows, cols);
			//copiez tabela nodului curent/parinte in tabela noului nod
			copyTable(&newNode, current_node, rows, cols);
			//pozitionarea caracterului pe locul gasit cu '-'
			newNode->table[rows - 1][i] = player;
			//printarea tabelui
			print_table(newNode, output, level, rows, cols);
			//adaugarea noului nod in lista de copii a nodului curent
			add_Child(&current_node, newNode);
			//validarea conditiei daca jocul nu s-a terminat
			//si recursiv apelam functia in care nodul curent devine noul fostul nod
			if(check(newNode, rows, cols) == 0) {
				next_player = whose_turn_is(player);
				create_Tree(newNode, rows, cols, next_player, level+1, output);
			}
		}
		//cazul general cand parcurg de la rows-2 -> 0
		for(int j = rows - 2; j >= 0; j--) {
			//conditie pentru a gasi ultima '-' de pe coloana
			if(current_node->table[j][i] == '-' && current_node->table[j+1][i] != '-') {
				newNode  = alloc_Node(current_node, rows, cols);
				copyTable(&newNode, current_node, rows, cols);
				newNode->table[j][i] = player;
				print_table(newNode, output, level, rows, cols);
				add_Child(&current_node, newNode);

				if(check(newNode, rows, cols) == 0) {
					next_player = whose_turn_is(player);
					create_Tree(newNode, rows, cols, next_player, level+1, output);
				}
			}
		}
	}
}
//Functia de eliberare a arborelui pt taskul 1
void free_tree(Node *tree, int rows) {
	if(tree) {
		free_tree(tree->nextSibling, rows);
		free_tree(tree->firstChild, rows);
		for (int i = 0; i < rows; i++)
			free(tree->table[i]);
		free(tree->table);
		free(tree);
	}
}

//************************** taskul 2******************************

//Functia de adaugare a nodului curent intr-o coada auxiliara
//folosita pentru citirea arborelui
void push(queue **last, Node *current_node) {
	queue *temp = calloc(1, sizeof(queue));
	temp->tree_node = current_node;
	temp->next = NULL;
	if (*last != NULL) {
		(*last)->next = temp;
		*last = (*last)->next;
	} else
		*last = temp;
}
//Functia de sterge primul element din coada auxiliara
queue *pull(queue *current_node) {
	if (current_node->next != NULL) {
		queue *temp = current_node;
		current_node = current_node->next;
		free(temp);
		return current_node;
	} else {
		free(current_node);
		return NULL;
	}
}
//Functia de creare a unui nod pentru taskul 2
Node *alloc_Node2(Node *current_node, int nr) {
	Node *newNode= calloc(1, sizeof(Node));

	newNode->parent = current_node;
	newNode->firstChild = NULL;
	newNode->value = nr;
	newNode->nr_children = 0;
	newNode->nextSibling = NULL;
	return newNode;
}
//Functia de citirea si creare a arborelui la taskul 2-3
void read_minimax_tree(FILE *input, Node *current_node) {
	//Initializez valorile
	Node *newNode = NULL;
	queue *first = NULL;
	queue *last = NULL;
	//adaug nodul root in coada
	push(&first, current_node);
	last = first;

	char character;
	int nr;
	//citesc caracter cu caracter 
	while (fscanf(input, "%c", &character) == 1 ) {
		//string auxiliar in care se pastreaza valorile nodurilor ca caractere
		char *aux = calloc(10, sizeof(char));

		if( character == '(' ) {
			fscanf(input, "%c", &character);
			while (character != ')'){
				strncat(aux, &character, 1); //adaug caracterul la finalul aux
				fscanf(input, "%c", &character);
			}
			//transform stringul in int
			nr = atoi(aux);
			//creeaz un nod nou
			newNode = alloc_Node2(current_node, nr);
			//incrementez nr de copii a nodului curent
			(current_node->nr_children)++;
			//adaug noul nod in lista de copii a nodului curent
			add_Child(&current_node, newNode);
			//adaug acel nod in coada
			push(&last, newNode);
			//daca numarul de copii a nodului curent == cu valoare lui
			//inseamna ca nu mai trebuie sa-i adaugam copii si i; stergem din coada
			//iar nodul curent devine urmatorul nod din coada
			if (current_node->nr_children == current_node->value){
				first = pull(first);
				current_node = first->tree_node;
			}
		}
		if (character == '[') {
			//adaug in aux valoare nodului ca caractere
			fscanf(input, "%c", &character);
			int nr;
			while (character != ']') {
				strncat(aux, &character, 1);
				fscanf(input, "%c", &character);
			}
			//transform aux in int
			nr = atoi(aux);
			//aloc memorie noului nod
			newNode = alloc_Node2(current_node, nr);
			//incrementez nr de copii a nodului curent
			(current_node->nr_children)++;
			//adaug noul nod in lista de copii a nodului curent
			add_Child(&current_node, newNode);
			//daca nr de copii a nodului curent egal ca valoare lui
			//stergem nodul curent din coada si curent node devine
			//urmatorul nod din coada
			if (current_node->nr_children == current_node->value) {
				if (first->next != NULL)
					first = pull(first);
				current_node = first->tree_node;
			}
		}
		free(aux);
	}
	first = pull(first);
}
//Functia in care citesc valoare nodului root
int read_root_value(FILE *input) {
	int nr = 0;
	char character;
	while (fscanf(input, "%c", &character) == 1 ) {
		if (character == '('){
			fscanf(input, "%c", &character);
			nr = 0;
			while (character != ')') {
				nr *= 10;
				nr += character - '0';
				fscanf(input, "%c", &character);
			}
		return nr;
		}
	}
}
//Functia de implementare a algoritmului minimax
int minimax(Node *current_node, int depth) {
	int i = 1;
	int max, min, value;

	//daca nodul curent este frunza se returneaza valoare sa
	if (current_node->nr_children == 0)
		return current_node->value;

	//daca nodul nu e frunza atunci completez copii lui si apoi nodul curent
	if (current_node->nr_children != 0 ) {
		//initializez min, max cu valoare primului nod copil
		value = minimax(current_node->firstChild, depth + 1);
		max = value;
		min = value;
		Node *temp = current_node->firstChild;
		//parcurg lista de copii si initializez min/max
		while (temp != NULL){
			value = minimax(temp, depth + 1);
			if(max < value) max = value;
			if(min > value) min = value;
			temp = temp->nextSibling;
		}
		//verific adancimea nodului curent si decid daca returnez min sau max
		if (depth % 2 != 1){
			current_node->value = min;
			return min;	
		} else {
			current_node->value = max;
			return max;
		}
	} 
}
//Functia de printare a arboreului minimax
void printminimax(Node *current_node, int level, FILE *output) {
	//parcurg lista
	while (current_node != NULL) {
		//printez taburile dupa ce nr de nivel al nodului
		for (int i = 0; i < level; i++)
			fprintf(output, "\t");
		//printez valorile nodului curent
		fprintf(output, "%d\n", current_node->value);
		//recursiv apelez functia pentru o parcurgere in adancime
		printminimax(current_node->firstChild, level + 1 , output);
		//daca nu mai am copii se va reintoarce sa parcurga urmatorul frate
		current_node = current_node->nextSibling;
	} 
}
//Functia de eliminare a arborelui
void free_tree2(Node *tree) {
	if (tree) {
		free_tree2(tree->nextSibling);
		free_tree2(tree->firstChild);
		free(tree);
	}
}
//***************************task 3*********************************
int minim(int min, int value) {
	min = (min < value ? min : value);
	return min;
}
int maxim(int max, int value) {
	max = (max > value ? max : value);
	return max;
}

//Functie de implementare algoritmului alpha-beta prunning
int pruning(Node *current_node, int alfa, int beta, int depth) {
	int bestVal, value;
	//conditie de terminare daca nodul curent este frunza
	if (current_node->nr_children == 0) {
		current_node->visited = 1;  //marcam nodul ca vizitat
		return current_node->value; //returnam valoae nodului
	}	
	//Parcurgem toate nodurile in adancime
	if (current_node->nr_children != 0 ) {
		//verific daca nodul este minimizer
		if (depth % 2 == 0) {
			bestVal = INT_MAX;	
			Node *temp = current_node->firstChild;
			//parcurg lista de copii a nodului curent
			while (temp != NULL) {
				temp->visited = 1; //marchez nodul ca vizitat
				value = pruning(temp, alfa, beta, depth + 1);
				bestVal = minim(bestVal, value);
				beta = minim(beta, bestVal);
				//daca beta mai mic ca alfa am gasit valoare optima
				if (beta <= alfa)
					break;
				temp = temp->nextSibling;
			}
			current_node->value = bestVal;
			return bestVal;

		} else { //daca nodul este maximizer
			bestVal = INT_MIN;
			Node *temp  = current_node->firstChild;
			//parcurg lista de copii a nodului curent
			while (temp != NULL) {
				temp->visited = 1; //marchez nodul ca vizitat
				value = pruning(temp, alfa, beta, depth + 1);
				bestVal = maxim(bestVal, value);
				alfa = maxim(alfa, bestVal);
				//am gasit valoare optima
				if (beta <= alfa)
					break;
				temp = temp->nextSibling;
			}
			current_node->value = bestVal;
			return bestVal;
		}
	}
}
//Functie de eliminare a listei de copii a nodului curent
void free_2(Node *current_node) {
	while (current_node->firstChild != NULL) {
		Node *copy = current_node->firstChild;
		free_2(current_node->firstChild);
		current_node->firstChild = current_node->firstChild->nextSibling;
		free(copy);
	}
}
//Functia verifica nodurile nevizitate si ii elimina copii
//astfel noduri nevizitate voe ramane numai ca frunze
void free_subtree(Node *current_node) {
	Node *temp = current_node->firstChild;
	//parcurg lista si gasesc nodurile cu visited = 0 -nevizitate
	while (temp != NULL) {
		if (temp->visited == 0)
			free_2(temp);
		free_subtree(temp);
		temp = temp->nextSibling;
	}
}
//Functie utilitara de stergere a tuturor apariilor lui 0 intr-o lista
void delete_zero_from_list(Node **current_node) {
	Node* temp = *current_node;
	Node *last;

	while (temp != NULL && temp->visited == 0) {
		*current_node = temp->nextSibling;
		free(temp);
		temp = *current_node;
	}
	while (temp != NULL) {
		while (temp != NULL && temp->visited != 0) {
			last = temp;
			temp = temp->nextSibling;
		}
		if (temp == NULL) return;

		last->nextSibling = temp->nextSibling;
		free(temp);
		temp = last->nextSibling;
	}
}
//Functia de stergere a tuturor nodurilor nevizitate din frunze
void free_zero_leafs(Node **current_node) {
	Node *temp = (*current_node)->firstChild;

	while (temp != NULL) {
		delete_zero_from_list(&temp);
		free_zero_leafs(&temp);
		temp = temp->nextSibling;
	}
}
int main (int argc, char **argv) {
	FILE *input = fopen(argv[2], "r");
	FILE *output = fopen(argv[3], "w");
	int rows = 0;
	int cols = 0;
	char player;

	Node *root = calloc(1, sizeof(Node));

	root->parent = NULL;
	root->nr_children = 0;
	root->firstChild = NULL;
	root->nextSibling = NULL;
	root->value = 0;
	//apel cerinta 1
	if (strcmp(argv[1], "-c1") == 0){
		read_table(&root, &rows, &cols, &player, input);
		print_table(root, output, 0, rows, cols);
		create_Tree(root, rows, cols, player, 1, output);
		free_tree(root, rows);
	}
	//apel cerinta 2
	else if (strcmp(argv[1], "-c2") == 0){
		char depth[3];

		fgets(depth, 3, input);
		root->value = read_root_value(input);
		read_minimax_tree(input, root);
		root->value = minimax(root, 1);
		printminimax(root, 0, output);
		free_tree2(root);
	}
	//apel cerinta 3
	else if (strcmp(argv[1], "-c3") == 0){
		char depth[3];

		fgets(depth, 3, input);
		root->value = read_root_value(input);
		read_minimax_tree(input, root);
		root->value = pruning(root, INT_MIN, INT_MAX, 1);
		free_subtree(root);
		free_zero_leafs(&root);
		printminimax(root, 0, output);
		free_2(root);
		free(root);
	}

	fclose(input);
	fclose(output);
	return 0;

}