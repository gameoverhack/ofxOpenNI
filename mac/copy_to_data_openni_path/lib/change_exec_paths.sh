in="install_name_tool -change "

cd ../OpenNI_openFrameworks/openni/lib

libs=(*.dylib)
num_libs=${#libs[*]}

for lib in *.dylib ; do 
	i=0
	echo ""
	echo "${lib}"
	echo "-------------------------------------------------"
	lines=`otool -L  ${lib}`
	echo ${lines}
	for linked_name in ${lines} ; do 
		for local_lib in ${libs[@]}; do
			in=`echo ${linked_name} | grep -i ${local_lib}`
			if [ "$in" != "" ] ; then
				check_id=`echo "${linked_name}" | grep -i "${lib}"`
				
				#install_name_tool -change "/opt/local/lib/libusb-1.0.0.dylib" "@executable_path/./../../../data/openni/lib/libusb-1.0.0.dylib" ${lib}
				#echo install_name_tool -change "/opt/local/lib/libusb-1.0.0.dylib" "@executable_path/./../../../data/openni/lib/libusb-1.0.0.dylib" ${lib}
				
				if [ "$check_id" != "" ] ; then
				 	install_name_tool -id  "@executable_path/./../../../data/openni/lib/${local_lib}" ${lib}
					echo install_name_tool -id  "@executable_path/./../../../data/openni/lib/${local_lib}" ${lib}
				else 
					install_name_tool -change "${linked_name}" "@executable_path/./../../../data/openni/lib/${local_lib}" ${lib}
					echo  install_name_tool -change "${linked_name}" "@executable_path/./../../../data/openni/lib/${local_lib}" ${lib}
				fi 
			fi	
		done
	done
done
