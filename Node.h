#pragma once

#include "channel.h"

class Node
{
	channel *value;
	Node *next, *prev;
public:
	Node(channel* );
	~Node();

	// Getter funtions
	channel* GetValue();
	int GetID();
	std::string getName() { return value->getName(); };
	bool GetVisited();
	Node *GetNext();
	Node *GetPrev();

	// Setter functions
	void SetValue(channel *);
	void SetNext(Node *);
	void SetPrev(Node *);
};
