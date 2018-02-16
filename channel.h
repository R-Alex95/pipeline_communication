/*
 * channel.h
 *
 *  Created on: Apr 15, 2016
 *      Author: alex
 */
#include <iostream>

#ifndef CHANNEL_H_
#define CHANNEL_H_

// Maximum saved file names of each server-channel
#define MAXFILES 20


class channel {
	char fdArray[MAXFILES][64]; // array of pathnames to files
	int id; // id of channel
	int files; // how many files exists on chanel
	int readfiles; // how many files we read
	bool cont; // continue to read files or not
	std::string name;
	std::string message;
public:
	channel(int,std::string);
	~channel();

	void inc() { readfiles++;};
	void reset() { readfiles = 0;};
	std::string addFile(std::string);
	void addMsg(std::string);
	void deleteFiles();
	bool checkNextFile(); // checking if there is another file in channel

	// Getter Functions
	std::string getFileMsgs();
	std::string getRestFileMsgs();
	int getID();
	int getFiles() { return files;};
	std::string getName();
	std::string getMsg();
};

#endif /* CHANNEL_H_ */
