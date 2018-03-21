#!/bin/bash
	set -e
	echo "Running multiple iterations ...\n"

	# Paths
	MAIN_PATH="/Users/zhoumoyuan/Documents/sem4/RA/"

	TOOLBOX_PATH=${MAIN_PATH}/ToolBox
	CONVERSION_PATH=${TOOLBOX_PATH}/ConvertDepthAndSeg/build/conversion
	
	SCENES_PATH=${MAIN_PATH}/Data_Test
	
	LABEL_PATH=${MAIN_PATH}/Parameters/labels.txt
	
	# take all scenes of the reconstruction scenes
	scenes=`ls $SCENES_PATH`

	# for each scene	
	for eachfile in $scenes
	do
		echo $eachfile

		cd ${SCENES_PATH}/${eachfile}
		
		rooms=`ls`

		for eachroom in $rooms
		do
			echo $eachroom
			if  [[ $eachroom != "config.mat" ]] ;
	   		then
				cd ${eachroom}
				if ls *_depth.png >/dev/null 2>&1; then
					ls *_depth.png>images.txt
					${CONVERSION_PATH}/readAndConvert16bitDM

					ls *_category.png>images.txt
					${CONVERSION_PATH}/readAndConvertSUNCGCategories --labels $LABEL_PATH	
				fi
				cd ..
			fi
		done
	done
