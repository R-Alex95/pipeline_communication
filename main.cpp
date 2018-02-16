/*
 * main.cpp
 *
 *  Created on: Apr 10, 2016
 *      Author: alex
 */

#include <iostream>
#include <fstream>
#include <sstream>  // istringstream

#include <stdio.h>
#include <stdlib.h> // strtol();
#include <unistd.h> // fork()/close();
#include <fcntl.h> // open();
#include <string.h> // memset();
#include <string>
#include <cerrno> // errno
#include <sys/types.h>
#include <sys/stat.h> // [mkfifo();]
#include <dirent.h>
#include <signal.h>

#include <cstddef>         // std::size_t
#include <sys/wait.h>

#include "board.h"
#include "channel.h"

#define MSGSIZE 64
#define RECEIV 2056

using namespace std;

void remove_all(string p) {
	DIR *d;
	struct dirent *dir;
	char const *cpath = p.c_str();
	d = opendir(cpath);
	int ret;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			string delfile(dir->d_name);
			string delpath = p + delfile;
			if (delfile == "." || delfile == "..")
				// Ignore current('.') and previous('..') directory
				continue;
			ret = remove(delpath.c_str());
			if (ret == -1) {
				char errorbuf[124];
				sprintf(errorbuf,"\tCannot remove %s ",delpath.c_str());
				perror(errorbuf);
			}
			else if (ret == 0)
				cout << "\tRemoved : " << delpath << endl;
		}
	}
	//rmdir(cpath);
}

void receiveNprint(int fd) {
	char tmpWrite[RECEIV];
	int bytes_r;
	if ((bytes_r = read(fd,tmpWrite,RECEIV)) == -1) {
		perror("createchannel-receive error");
	}
	else if (bytes_r == 0) {
		cout << "Nothing was received" << endl;
	}
	else {
		string str(tmpWrite);
		if (str == "EOF\n")
			return;
		cout << "Board_client Received : \n" << str;
	}
	memset(tmpWrite,0,RECEIV);
}

int get_serverPid(string path) {
	// add suffix _pid to path and open file.Return the file-integer
	string suffix = "_pid";
	string tempdir = path + suffix;
	const char *ctemp = tempdir.c_str();
	fstream pfile(ctemp);
	int server;
	pfile >> server;
	return server;
}

int main(int argc,char **argv) {
	cout << "***Board-server***" << endl;

	int fd_others,fd_self,fd_self2B,fd_other2B,bytes_r,nwrite;
	bool server_exist,exit_cin = false;
	char stringBuffer[MSGSIZE];
	char stringRead[MSGSIZE];
	char tmpWrite[MSGSIZE];
	memset(stringBuffer, 0, MSGSIZE); // Filling with zeros
	memset(stringRead, 0, MSGSIZE);

	// Ignore SIGINT (Ctrl + C) signal
	//signal(SIGINT, SIG_IGN);

	string path;
	if (argc != 2) {
		cout << "Wrong input for main.cpp(board)." << endl;
		cout << "Only path for named-pipe is needed!" << endl;
		return -1;
	}
	else {
		path = argv[1];
	}

	char last_char = path[path.length() - 1];
	if (last_char == '/')
		path.erase(path.end() -1); // erasing last '/' for extracting folder name from path

	// Getting the current folder & directory name
	size_t found = path.find_last_of("/\\");
	string folder = path.substr(found+1);
	string directory = path;
	int length = directory.size() - folder.size();
	directory.erase(length,directory.size());

	last_char = path[path.length() - 1];
	if (last_char != '/')
		path.push_back('/'); // appending the erased '/' at the end of path

	// Checking if DIR exists
	const char *DIRarg = path.c_str();
	DIR* dir = opendir(DIRarg);
	if (dir) {
	    /* Directory exists so does the server. */
		struct stat st = {0};
		string tmpdir = path + "_others";
		if (stat(tmpdir.c_str(), &st) == 0) {
		    server_exist = true;
		}
		else
			server_exist = false;
	    closedir(dir);
	}
	else if (errno == ENOENT) {
	    /* Directory does not exist nor server. */
		server_exist = false;
		struct stat st = {0};

		if (stat(DIRarg, &st) == -1) {
		    mkdir(DIRarg, 0700); // Full permission for owner
		}
		else {
			perror("Couldn't create a directory");
			return EXIT_FAILURE;
		}
		cout << "Given path was created because it did not exist." << endl;
	}
	/* Creating pathnames for named-pipes */
	string suffix = "_others";
	string others = path + suffix;
	suffix = "_self";
	string self = path + suffix;
	string self2B = path + "_selfS2B";
	string others2B = path + "_othersS2B";

	if (server_exist == false) {
		// If server doesn't exist make the named_pipes with correct permissions
		// user full perm and others only write.
		if (mkfifo(others.c_str(), 0622) == -1) {
			if ( errno != EEXIST) {
				perror("mkfifo_others");
				exit(6);
			}
		}
		//only user has full perm
		if (mkfifo(self.c_str(), 0600) == -1) {
			if ( errno != EEXIST) {
				perror("mkfifo_self");
				exit(6);
			}
		}
		if (mkfifo(self2B.c_str(), 0600) == -1) {
			if ( errno != EEXIST) {
				perror("mkfifo_self2B");
				exit(6);
			}
		}
		// only read permissions
		if (mkfifo(others2B.c_str(), 0644) == -1) {
			if ( errno != EEXIST) {
				perror("mkfifo_other2B");
				exit(6);
			}
		}
		if ((fd_others = open(others.c_str(), O_RDONLY | O_NONBLOCK)) < 0) {
			perror("fifo_others open problem");
			exit(3);
		}
		if ((fd_self = open(self.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
			perror("fifo_self open problem");
			exit(3);
		}
		if ((fd_self2B = open(self2B.c_str(), O_RDWR )) < 0) {
			perror("fifo_self2B open problem");
			exit(3);
		}
		if ((fd_other2B = open(others2B.c_str(), O_RDWR )) < 0) {
			perror("fifo_others2B open problem");
			exit(3);
		}
	}
	else {
		if ((fd_others = open(others.c_str(), O_RDONLY | O_NONBLOCK)) < 0) {
			perror("fifo open problem");
			exit(3);
		}
		if ((fd_self = open(self.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
			perror("fifo_self open problem");
			exit(3);
		}
		if ((fd_self2B = open(self2B.c_str(), O_RDWR )) < 0) {
			perror("fifo_self2B open problem");
			exit(3);
		}
		if ((fd_other2B = open(others2B.c_str(), O_RDWR )) < 0) {
			perror("fifo_others2B open problem");
			exit(3);
		}
	}
	pid_t server_pid;
	pid_t client_server = getpid();
	if (server_exist == false) { // create a server-process + pid file
		server_pid = fork();
		if (server_pid == 0) {
			board *serverInfo = new board();

			int server = getpid();
			// if child create a folder with pid
			cout << "Creating _pid file!" << endl;
			string suffix = "_pid";
			string tempdir = path + suffix;
			ofstream pfile(tempdir.c_str());
			pfile << server << "\n";
			pfile.close();
			chmod(tempdir.c_str(),0600); // only user rights
			while(1) {
				/****** Reading from board_Client *******/
				if ((bytes_r = read(fd_self,stringRead,MSGSIZE)) > 0) { //Read string characters
					stringRead[bytes_r] = '\0';		//Zero terminator
					cout << "1.Server_self received bytes |" << bytes_r << "| : " << stringRead << endl;
					fflush(stdout);
					string str(stringRead);
					serverInfo->operate(fd_self2B,str); // forward the message to server process
					if (str == "shutdown") {
						return EXIT_SUCCESS;
					}
					memset(stringRead,0,MSGSIZE);
				}

				/****** Reading from boardpost_Client *******/
				if ((bytes_r = read(fd_others, stringBuffer, MSGSIZE)) <= 0) {
					continue;
				}
				stringBuffer[bytes_r] = '\0';		//Zero terminator
				cout << "2.Server_others received bytes |" << bytes_r << "| : " << stringBuffer << endl;
				fflush(stdout);
				string str(stringBuffer);
				serverInfo->operate(fd_other2B,str);
				memset(stringBuffer, 0, MSGSIZE);

				usleep(100000);
			}
		}
	} // "connect" to server process
	else {
		cout << "Connecting to server!" << endl;
		server_pid = get_serverPid(path);
	}
	if (client_server != server_pid) {
		// Read from stdin if parent
		string line;
		do {
			getline(cin,line);
			istringstream iss(line);
			string word;
			iss >> word;
			memset(tmpWrite,0,MSGSIZE);
			if (word == "createchannel") {
				iss >> word;
				// checking if word is integer
				char* e;
				strtol(word.c_str(), &e, 10);
				if (*e) {
					cout << "Not integer.Try again" << endl;
					continue;
				}
				string tmp;
				iss >> tmp;
				if (tmp == "") {
					cout << "Please enter the name of the channel.(Name must be different from channel's id)" << endl;
					continue;
				}
				strncpy(tmpWrite,line.c_str(),line.length());
				if ((nwrite = write(fd_self,tmpWrite,line.length())) == -1) {
					perror("createchannel-write error");
					exit(2);
				}
				usleep(100000);
				receiveNprint(fd_self2B);

			}
			else if (word == "getmessages") {
				// Fetching the channel's id
				iss >> word;
				// Checking if its integer
				char* e;
				strtol(word.c_str(), &e, 10);
				if (*e) {
					cout << "Not integer.Try again" << endl;
					continue;
				}
				strncpy(tmpWrite,line.c_str(),line.length());
				if ((nwrite = write(fd_self,tmpWrite,line.length())) == -1) {
					perror("getmessages-write error:");
					exit(2);
				}
				usleep(100000);
				receiveNprint(fd_self2B);
			}
			else if (word == "exit") {
				// delete client
				close(fd_self);
				close(fd_others);
				return EXIT_SUCCESS;
			}
			else if (word == "shutdown") {
				// delete everything
				strncpy(tmpWrite,word.c_str(),word.length());
				if ((nwrite = write(fd_self,tmpWrite,word.length())) == -1) {
					perror("shutdown-write error:");
					exit(2);
				}
				remove_all(path);
				exit_cin = true;
			}
			else
				cout << "No such function found.Try again!" << endl;
		} while (!(cin.eof()));
	}
	cout << "\n\t\t*********REMINDER********* \n\nBoard_client is terminated with command \"exit\" and board_server with \"shutdown\"!" << endl << endl;
	cout << "Next time use the above commands instead of Ctrl + D" << endl;
	if (exit_cin == false) {
		cout << "Board_server process will be killed" << endl;
		memset(tmpWrite,0,MSGSIZE);
		// call self-destruction on server to prevent memory leaks
		string del = "shutdown";
		strncpy(tmpWrite,del.c_str(),del.length());
		if ((nwrite = write(fd_self,tmpWrite,del.length())) == -1) {
			perror("shutdown-write error:");
			exit(2);
		}
		int server = get_serverPid(path);
		kill(server,SIGKILL);
		remove_all(path);
	}
	close(fd_self);
	close(fd_others);
	cout << "Board_client Terminated" << endl;
	return EXIT_SUCCESS;
}
