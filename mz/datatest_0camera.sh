#!/bin/bash
	clear
	reconstruction_dir="/home/moyuan/Documents/RA/Data_Test/"

	scenes=`ls $reconstruction_dir`
	no_room_in_scene=0
	no_view_in_room=0
	no_view_in_scene=0
	no_camera_in_room=0
	no_camera_in_scene=0
	empty_camera_in_room=0
	empty_cam_in_scene=0
	all_room_hasviews=0
	at_least_room_view=0
	room_hasview=0
	scene_num=0
	room_num=0

	for eachfile in $scenes
	do
		if  [[ $eachfile != "timelog.txt" ]] ;
		then
			cd $reconstruction_dir$eachfile
			scene_num=$((scene_num+1))
			rooms=`ls`
			flag="false"
			camtxt="false"
			scene_views=0
			all_room_views="true"
			all_room_empty_cam="true"
			at_least_view="false"
			for eachroom in $rooms
			do
				if [[ $eachroom != "config.mat" ]];
				then
					flag="true"
					room_num=$((room_num+1))
					cd $eachroom
					views=`find . -maxdepth 2 -name "000*.jpg" -type f  | wc -l`
					scene_views=$((scene_views+views))
					if [[ $views == 0 ]] ; then
						no_view_in_room=$((no_view_in_room+1))
						all_room_views="false"
					else
						at_least_view="true"
						room_hasview=$((room_hasview+1))
					fi

					if [ -f "cameras.txt" ]; then
						camtxt="true"
						if [ -s "cameras.txt" ]; then
							all_room_empty_cam="false"
						else
							empty_camera_in_room=$((empty_camera_in_room+1))
						fi
					else
						no_camera_in_room=$((no_camera_in_room+1))
					fi

					cd ..
				fi
			done

			if [[ "$flag" == "false" ]] ; then
				no_room_in_scene=$((no_room_in_scene+1))
				echo "no room in scene: $eachfile"
			elif [[ "$camtxt" == "false" ]] ; then
				no_camera_in_scene=$((no_camera_in_scene+1))
				echo "no camera in scene: $eachfile"
			elif [[ "$all_room_empty_cam" == "true" ]]; then
				empty_cam_in_scene=$((empty_cam_in_scene+1))
				echo "all rooms empty camera: $eachfile"
			elif [[ $scene_views == 0 ]]; then
				echo "no view in allrooms for other reason: $eachfile"
			else
				if [[ "$all_room_views" == "true" ]];
				then
					all_room_hasviews=$((all_room_hasviews+1))
				fi
				if [[ "$at_least_view" == "true" ]] ; then
					at_least_room_view=$((at_least_room_view+1))
				fi
			fi
			if [[ $scene_views == 0 ]] ; then
				no_view_in_scene=$((no_view_in_scene+1))
			fi

		fi
	done

	echo "all_room_hasviews: $all_room_hasviews"
	echo "at_least_room_view: $at_least_room_view"
	echo "no_room_in_scene: $no_room_in_scene"
	echo "no_camera_in_scene: $no_camera_in_scene"
	echo "empty_cam_in_scene: $empty_cam_in_scene"
	echo "no_view_in_allrooms: $no_view_in_scene"
	echo "scene_num: $scene_num"
	echo

	echo "room_hasview: $room_hasview"
	echo "no_camera_in_room: $no_camera_in_room"
	echo "empty_camera_in_room: $empty_camera_in_room"
	echo "no_view_in_room: $no_view_in_room"
	echo "room_num: $room_num"
