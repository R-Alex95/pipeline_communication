#boardstatus script for checking status

active=0
inactive=0
#for every file in path-argument
for f in $( ls $1 ); do
	path="$1/$f"
	# check if $path is a directory
	if [ -d $path ] 
	then
		server_pid="$path/_pid"
		# check if server_pid file exists
		if [ -f $server_pid ]
		then
			echo "Active board : $path"
			active=$((active+1))
		else
			echo "Inactive board : $path"
			inactive=$((inactive+1))
		fi
	fi
done
echo "Overall active boards : $active"
echo "Overall inactive boards : $inactive"
