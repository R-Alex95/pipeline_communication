/*
 * board.cpp
 *
 *  Created on: Apr 18, 2016
 *      Author: alex
 */
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "List.h"
#include "channel.h"

#define MSGSIZE 1024

using namespace std;

board::board() {
	chList = new List();
	pid = -1;
}

board::~board() {
	delete chList;
}

void board::operate(int fd,string line) {
	istringstream iss(line);
	string word;
	iss >> word;
	if (word ==  "list") {
		cout << "\t(List) has been called." << endl;
		sendStatus(fd,this->list());
	}
	else if (word == "write") {
		cout << "\t(Write) has been called." << endl;
		iss >> word;
		int id = atoi(word.c_str());
		channel *ch = this->searchCh(id);
		if ( ch == NULL) {
			stringstream buf;
			buf << "\tERROR : Channel with id : |" << id << "| was not found!" << endl;
			sendStatus(fd,buf.str());
			return;
		}
		line.erase(0, 5 + 1 + word.length() + 1); // write == 5 + (space) + integer + (space)
		this->addMsg(fd,id,line);
		stringstream buf;
		buf << "\tMessage |" << line << "| added on channel " << id << endl;
		sendStatus(fd,buf.str());
	}
	else if (word == "send") {
		cout << "\t(Send) has been called." << endl;
        iss >> word;
        int id = atoi(word.c_str());
        iss >> word;
		struct stat st = {0};
		if (stat(word.c_str(), &st) != 0) {
			stringstream buf;
			buf << "\tERROR : Path : |" << word << "| doesn't correspond to file!" << endl;
			sendStatus(fd,buf.str());
			return;
		}

		string path = word;
		// Getting the file name
		size_t found = path.find_last_of("/\\");
		string file = path.substr(found + 1);
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
			perror("getcwd() error");
			return;
		}
		string destination = cwd;
		destination.push_back('/');
		destination += file;

		int source, dest, n_chars;
		char buf[1024];

		// while file exists add suffix _other
		while ( access(destination.c_str(), F_OK) == 0) {
			size_t dotpos = destination.find_last_of(".");
			destination.insert(dotpos,"_other");
		}

		// open file for read.
		if ((source = open(path.c_str(), O_RDONLY)) == -1) {
			char errorbuf[124];
			sprintf(errorbuf,"1.Cannot open %s",path.c_str());
			perror(errorbuf);
			return;
		}
		// create file on current dir.
		if ((dest =  open(destination.c_str(), O_WRONLY|O_CREAT,0666)) == -1) {
			char errorbuf[124];
			sprintf(errorbuf,"2.Cannot open %s",destination.c_str());
			perror(errorbuf);
			return;
		}

		/* copy files */
		while ((n_chars = read(source, buf, 1024)) > 0) {
			if (write(dest, buf, n_chars) != n_chars) {
				char errorbuf[124];
				sprintf(errorbuf,"2.Cannot write %s",destination.c_str());
				perror(errorbuf);
				return;
			}

			if (n_chars == -1) {
				char errorbuf[124];
				sprintf(errorbuf,"1.Cannot read %s",path.c_str());
				perror(errorbuf);
				return;
			}
		}

		/* close files */
		if (close(source) == -1) {
			char errorbuf[124];
			sprintf(errorbuf,"Cannot close %s",path.c_str());
			perror(errorbuf);
			return;
		}
		if (close(dest) == -1) {
			char errorbuf[124];
			sprintf(errorbuf,"Cannot close %s",destination.c_str());
			perror(errorbuf);
			return;
		}
		string str = this->addFile(fd,id,destination);
		if ( str == "OK" ) {
			stringstream buf;
			buf << "File named : |" << file << "| sucessfully uploaded to server!" << endl;
			sendStatus(fd,buf.str());
		}
		else
			sendStatus(fd,str);

	}
	else if (word == "createchannel") {
		cout << "\t(Createchannel) has been called." << endl;
		iss >> word;
		int id = atoi(word.c_str());
		iss >> word;
		this->addCh(fd,id, word);
	}
	else if (word == "getmessages") {
		cout << "\t(Getmessages) has been called." << endl;
		// Fetching the channel's id
		iss >> word;
		int id = atoi(word.c_str());
		this->getMsgs(fd,id); // get message and first file message
		this->getRestMsgs(fd,id); // get rest of file messages
		usleep(100);
		this->sendStatus(fd,"EOF\n");
	}
	else if (word == "shutdown") {
		cout << "\t(Shutdown) has been called." << endl;
		this->selfDestruction();
	}
	else
		cout << "\tERROR : Function |" << word << "| not found on server" << endl;

}

channel* board::searchCh(int id) {
	if (chList->getSize() == 0) {
		return NULL;
	}
	return chList->FindValue(id);
}

void board::addMsg(int fd,int id, string word) {
	channel *ch = this->searchCh(id);
	if ( ch == NULL) {
		stringstream buffer;
		buffer << "\tERROR : Channel with id : |" << id << "| was not found!" << endl;
		sendStatus(fd,buffer.str());
		return;
	}
	else {
		ch->addMsg(word);
	}
}

string board::addFile(int fd,int id,string word) {
	channel *ch = this->searchCh(id);
	if ( ch == NULL) {
		stringstream buffer;
		buffer << "\tERROR : Channel with id : |" << id << "| was not found!" << endl;
		return buffer.str();
	}
	else {
		return ch->addFile(word);
	}

}

void board::addCh(int fd,int id,string word) {
	channel *ch = this->searchCh(id);
	if ( ch != NULL) {
		stringstream buffer;
		buffer << "\tERROR : Channel with id : |" << id << "| already exists!" << endl;
		sendStatus(fd,buffer.str());
		return;
	}
	channel *new_c = new channel(id,word);
	int size = chList->getSize();
	chList->Push(new_c);
	if (chList->getSize() == size + 1) {
		stringstream buffer;
		buffer << "\tChannel with id : |" << id << "| and name : |"
		<< word << "| has been successfully added to server!" << endl;
		sendStatus(fd,buffer.str());
	}
}

string board::list() {
	return chList->PrintList();
}

void board::selfDestruction() {
	chList->deleteFiles();
	cout << "\tServer deleted" << endl;
	delete this;
}

void board::getMsgs(int fd,int id) {
	channel *ch = this->searchCh(id);
	if ( ch == NULL) {
		stringstream buffer;
		buffer << "\tERROR : Channel with id : |" << id << "| was not found!" << endl;
		sendStatus(fd,buffer.str());
		return;
	}
	stringstream buffer;
	if (strlen(ch->getMsg().c_str()) > 0) {
		buffer << "\tPrinting message of channel " << id << " :\n\t\t" << ch->getMsg() << endl;
		if (ch->getFiles() > 0) {
			buffer << "\tChannel's " << id << " file-message(s) is the following :\n\t" << ch->getFileMsgs() << endl;
		}

	}
	sendStatus(fd,buffer.str());
}

void board::getRestMsgs(int fd,int id) {
	channel *ch = this->searchCh(id);
	while (ch->checkNextFile() == true) {
		sendStatus(fd,ch->getRestFileMsgs());
		ch->inc();
	}
	ch->reset();
}

int board::getPid() {
	return pid;
}

void board::setPid(int _pid) {
	pid = _pid;
}

void board::sendStatus(int fd,string path) {
	char tmpWrite[MSGSIZE];
	int nwrite;
	strncpy(tmpWrite,path.c_str(),path.length());
	if ((nwrite = write(fd,tmpWrite,path.length())) == -1) {
		perror("sendStatus - write error:");
		exit(2);
	}
}
