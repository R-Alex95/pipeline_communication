/*
 * main.cpp
 *
 *  Created on: Apr 10, 2016
 *      Author: alex
 */

#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h> // mkfifo();
#include <fcntl.h>
#include <string.h> // strlen

#define MSGSIZE 256
#define RECEIV 2056

using namespace std;

void receiveNprint(int fd) {
	char tmpWrite[RECEIV];
	int bytes_r;
	while ((bytes_r = read(fd,tmpWrite,RECEIV)) > 0) {
		string str(tmpWrite);
		if (str == "OK")
			return;
		cout << "Boardpost_client Received : \n" << str;

	}
	memset(tmpWrite,0,RECEIV);
}

int main(int argc,char **argv) {

	cout << "***Boardpost***" << endl;
	char tmp[MSGSIZE];
	string path;

	if (argc != 2) {
		cout << "Wrong input for main.cpp(board)." << endl;
		cout << "Only path for named-pipe is needed!" << endl;
		return -1;
	}
	else
		path = argv[1];

	char last_char = path[path.length() - 1];
	if (last_char != '/')
		path.push_back('/');

	string fifo_path = path + "_others";
	string fifo_path2B = path + "_othersS2B";
	int fd,fd2B,nwrite;

	if ((fd = open(fifo_path.c_str(), O_WRONLY | O_NONBLOCK)) < 0) {
		perror("|BOARDPOST| Named-pipe(B2S) open error");
		exit(1);
	}

	if ((fd2B = open(fifo_path2B.c_str(), O_RDONLY | O_NONBLOCK)) < 0) {
		perror("|BOARDPOST| Named-pipe(S2B) open error");
		exit(1);
	}

	// Read from stdin
	string line;
	cout << "Awaiting user input ..." << endl;
	do {
		getline(cin,line);
		istringstream iss(line);
		string word;
		iss >> word;
		memset(tmp, 0, MSGSIZE);
		if (word ==  "list") {
			strncpy(tmp,word.c_str(),word.length());
			if ((nwrite = write(fd,tmp,4)) == -1 ) {
				perror("|BOARDPOST| Named-pipe write(1) error");
				exit(2);
			}
			usleep(1000000);
			receiveNprint(fd2B);
		}
		else if (word == "write") {
			iss >> word;
			char* e;
			strtol(word.c_str(), &e, 10);
			if (*e) {
				cout << "Not integer.Try again" << endl;
				continue;
			}
            string tmp1;
			iss >> tmp1;
			if (tmp1 == "") {
				cout << "Please enter a message , as well." << endl;
				continue;
			}
			strncpy(tmp,line.c_str() , line.length());
			if ((nwrite = write(fd, tmp, line.length())) == -1) {
				exit(2);
			}
			usleep(1000000);
			receiveNprint(fd2B);
		}
		else if (word == "send") {
			// Fetching the channel's id
            iss >> word;
            // Checking if its integer
            char* e;
            strtol(word.c_str(), &e, 10);
            if (*e) {
                cout << "Not integer.Try again." << endl;
                continue;
            }
            string tmp1;
			iss >> tmp1;
			if (word == tmp1) {
				cout << "Please enter the path of the file." << endl;
				continue;
			}
			strncpy(tmp,line.c_str() , line.length());
			if ((nwrite = write(fd, tmp, line.length())) == -1) {
				exit(2);
			}
			usleep(1000000);
			receiveNprint(fd2B);
		}
		else cout << "No such function found.Try again!" << endl;
	} while (!cin.eof());

	close(fd);
	cout << "Program Terminated" << endl;
	return EXIT_SUCCESS;
}
