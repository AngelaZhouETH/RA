#!/bin/bash
	clear
	reconstruction_dir="/home/moyuan/Documents/RA/Data_Test"
	suncg_dir="/home/moyuan/Documents/RA/Data_raw/house/"

	scenes=`ls $reconstruction_dir`
	empty_num=0
	scene_num=0

	for eachfile in $scenes
	do

		if  [[ $eachfile != "timelog.txt" ]] ;
		then
			cd $suncg_dir$eachfile
			scene_num=$((scene_num+1))
			if [ -f "outputcamerafile" ]; then
				if [ ! -s "outputcamerafile" ] ;
				then
					empty_num=$((empty_num+1))
					echo "empty outputcamerafile in scene: $eachfile"
				fi
			else
				echo "error: no outputcamera file!!"
			fi

		fi
	done

	echo "empty_num: $empty_num"
	echo "scene_num: $scene_num"