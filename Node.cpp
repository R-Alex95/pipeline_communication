#include <iostream>
#include "Node.h"

using namespace std;

Node::Node(channel *val) : next(NULL),prev(NULL) {
	value = val;
	//cout << "A node has been created" << endl;
}

Node::~Node() {
	delete value; // Delete the class created from main.
	// cout << "A Node has been destroyed" << endl;
}

channel* Node::GetValue() {
	return this->value;
}

int Node::GetID() {
	return value->getID();
}

Node *Node::GetNext() {
	return next;
}

Node *Node::GetPrev() {
	return prev;
}


void Node::SetValue(channel *val) {
	value = val;
}

void Node::SetNext(Node *n) {
	next = n;
}

void Node::SetPrev(Node *n) {
	prev = n;
}
