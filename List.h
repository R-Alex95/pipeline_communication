#ifndef LIST_H_
#define LIST_H_

#include "Node.h"

class List {
	Node *head,*last;
	int size;
public:
    List();
    ~List();


    channel *FindValue(int);   // find int-th_id channel
    int PopHead(); 	          // deletes head
    void Push(channel *);    // inserting a channel to the end of the list
    std::string PrintList(); 	    // Ektipwnei ola ta value tou List
    void deleteFiles();

    int getSize() { return size;};
};

#endif 
