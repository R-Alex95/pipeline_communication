/*
 * board.h
 *
 *  Created on: Apr 18, 2016
 *      Author: alex
 */

#include "List.h"

#ifndef BOARD_H_
#define BOARD_H_

class board {
	List *chList;
	int pid;
	std::string status;
public:
	board();
	~board();

	void operate(int,std::string);
	void addCh(int,int,std::string);
	void addMsg(int,int,std::string);
	std::string addFile(int,int,std::string);
	channel* searchCh(int );
	std::string list();
	std::string statusMsg();
	void statusClear() { status.clear(); };
	void sendStatus(int,std::string);
	void selfDestruction();


	void getMsgs(int,int);
	void getRestMsgs(int,int);
	int getPid();
	void setPid(int);
};

#endif /* BOARD_H_ */
