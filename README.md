###Proccess communication & synchronization via pipeline.

####This project is about making a server-client simulation.

## Compile

To compile run `make`.
	(There are many compile commands in Makefile)

## Run

Invocation:

	./board     path-for-pipeline-folder
	./boardpost path-for-pipeline-folder

**Path-for-pipeline-folder is created automatically if not exist.Must be the same to both invocations.**

Board and boardpost are communicating and sending bits via pipelines.
First run board.cpp and then run boardpost.cpp
Server - client communication must begin by creating a channel on server-side via **createchannel channel_id(integer) channel_name**.Then you can send messages or files to the server  **write channel_id your_message** or **send channel_id path_to_file** accordingly.File is saved on the server_path.Lastly client can see what messages has been written to what channel via the **list** command.

Other commands that board-server can run : 
-getmessages (prints all the messages/files of all channels)
-exit (exit server and can reconnect later if given the correct path-for-pipeline-folder path)
-shutdown (exit server and deletes all data automatically)

