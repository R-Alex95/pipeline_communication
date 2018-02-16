#include <iostream>
#include <sstream>
#include "List.h"
#include "channel.h"

using namespace std;

List::List() : head(NULL), last(NULL), size(0) {
	//cout << "A list has been created!" << endl;
}

List::~List() {
	if (size > 0) {
		int x;
		do x = PopHead();
		while (x > 0);
	}
	//cout << "A list has been destroyed!" << endl;
}

int List::PopHead() {
	if (size == 1) {
		head->SetNext(NULL);
		head->SetPrev(NULL);
		last->SetPrev(NULL);
		last->SetNext(NULL);
		last = NULL;
		delete head;
		head = NULL;
	} else if (size > 1) {
		head = head->GetNext();
		delete head->GetPrev();
		head->SetPrev(NULL);
	}
	return --size;
}

channel* List::FindValue(int i) {
	int j = 0;
	Node *temp = head;
	do {
		//cout << "SIZE : " << size << " I : " << i << "  J : " << j << endl;
		if (temp->GetID() == i) {
			return temp->GetValue();
		}
		else {
			temp = temp -> GetNext();
		}
		j++;
	} while (j < size);
	//cout << "\tERROR : List::FindValue didn't found anything" << endl;
	return NULL;
}

void List::Push(channel *v) {
	if (size == 0) {
		head = new Node(v);
		last = head;
	}
	else {
		Node* tmp = new Node(v);
		tmp->SetPrev(last);
		last->SetNext(tmp);
		last = tmp;
	}
	size++;
}

std::string List::PrintList() {
	Node *cur = head;
	stringstream buf;
	if (size == 0) {
		buf << "ERROR : Empty!" << endl;
		return buf.str();
	}
	do {
		buf << "\tID : " <<  cur->GetID() << " & name : " << cur->getName() << endl;
		cur = cur->GetNext();
	} while (cur != NULL);
	buf << endl;
	return buf.str();
}

void List::deleteFiles() {
	if (size > 0) {
		Node *tmp = head;
		do {
			tmp->GetValue()->deleteFiles();
			tmp = tmp->GetNext();
		} while (tmp != NULL);
	}
}

