
CMAKE_LIST_FILE=CMakeLists.$1.txt

	echo "**************************************"
	if [ -e $CMAKE_LIST_FILE  ]
	then
		echo "copy $CMAKE_LIST_FILE to CMakeLists.txt"
		cp -p $CMAKE_LIST_FILE CMakeLists.txt
	
		echo "call cmake ."
		cmake .
	
		#rm CMakeLists.txt
	
	else
		echo "**************************************"
		echo "Can't find: $CMAKE_LIST_FILE (CMakelists.\$1.txt)"
		echo "exiting . . ."	
	fi
	echo "**************************************"

