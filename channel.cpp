/*
 * channel.cpp
 *
 *  Created on: Apr 15, 2016
 *      Author: alex
 */

#include <stdio.h>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "channel.h"

#define READBUF 8192

using namespace std;

channel::channel(int _id, string _name) : id(_id),files(0),readfiles(0),name(_name) {
	cont = false;
}

channel::~channel() {}

void channel::addMsg(string word) {
	message.clear();
	message = word;
	/*
	 * Alla3a tin ylopoihsh
	 * message = message + "$" + word;
	 */
}
string channel::addFile(string path) {
	stringstream buf;
	if (files == MAXFILES) {
		buf << "\tERROR : Channel reached full capacity of files" << endl;
		return buf.str();
	}
	strncpy(fdArray[files],path.c_str(),path.length());
	files++;
	return "OK";
}

void channel::deleteFiles() {
	for (int i = 0 ; i < files ; i++) {
		cout << i << " == " << files << endl;
		int ret = remove(fdArray[i]);
		if (ret == -1) {
			char errorbuf[124];
			sprintf(errorbuf,"Cannot remove %s",fdArray[i]);
			perror(errorbuf);
		}
		else if (ret == 0)
			cout << "\tRemoved : " << fdArray[i] << endl;
	}
}


std::string channel::getFileMsgs() {
	if (files == 0) {
		string str = "There is no file in this channel";
		return str;
	}
	for (int i = 0 ; i < files ; i++) {
		int fd,n_chars;
		char buf[READBUF];
		if (i + 1 < files) {
			readfiles = i;
			cont = true;
		}
		if ((fd = open(fdArray[i], O_RDONLY)) == -1 ) {
			char errorbuf[256];
			sprintf(errorbuf,"Cannot open %s from channel %d",fdArray[i],id);
			perror(errorbuf);
			string str = "Cannot open path to read file";
			return str;
		}
		string wholemsg;
		while ((n_chars = read(fd, buf, READBUF)) > 0) {
			wholemsg.append(buf);
			if (n_chars == -1) {
				char errorbuf[256];
				sprintf(errorbuf,"1.Cannot read %s",fdArray[i]);
				perror(errorbuf);
				string str = "Cannot open path to read file";
				return str;
			}
		}
		return wholemsg;
		fflush(stdout);
		memset(buf,0,READBUF);
	}
	return "OK";
}

bool channel::checkNextFile() {
	if (readfiles < files) {
		cont = true;
	}
	else
		cont = false;
	return cont;

}

std::string channel::getRestFileMsgs() {
	int fd,n_chars;
	char buf[READBUF];
	if ((fd = open(fdArray[readfiles], O_RDONLY)) == -1 ) {
		char errorbuf[256];
		sprintf(errorbuf,"Cannot open %s from channel %d",fdArray[readfiles],id);
		perror(errorbuf);
		string str = "Cannot open path to read file";
		return str;
	}
	string wholemsg;
	while ((n_chars = read(fd, buf, READBUF)) > 0) {
		wholemsg.append(buf);
		if (n_chars == -1) {
			char errorbuf[256];
			sprintf(errorbuf,"1.Cannot read %s",fdArray[readfiles]);
			perror(errorbuf);
			string str = "Cannot open path to read file";
			return str;
		}
	}
	return wholemsg;
}

int channel::getID() {
	return id;
}

string channel::getName() {
	return name;
}

string channel::getMsg() {
	/* Alla3a tin ylopoihsh,prin ta ekana append me eidiko
	 * xaraktira to $ kai ta diavaza ola apla sto piazza itan
	 * dekto na ananewnetai to message poy ine pio aplo gia ta pipes.
	 * string path = message;
	size_t fo = path.find_first_of("$");
	string part = path.substr(0,fo);
	fflush(stdout);
	cout << "\tPrinting messages of channel " << this->id << " :" << endl;
	while (fo != string::npos) {
		fo = path.find("$");
		part = path.substr(0,fo);
		cout << "\t\t" << part << endl;
		path.erase(0,fo + 1);
	}*/
	return message;
}


