
for scene_path in $(ls -d /Users/zhoumoyuan/Documents/sem4/RA/Data_Test/*/)
do
    scene_name=$(basename $scene_path)

    for room_path in $(ls -d ${scene_path}/*/)
    do
        room_name=$(basename $room_path)
        echo "${scene_name} - ${room_name}" 

	cp labels_new_format.txt  ${room_path}/labels.txt

        python3 /Users/zhoumoyuan/Documents/sem4/RA/Training_On_Synthetic/TSDF/tsdf_fusion.py \
            --input_path $room_path/ \
            --output_path $room_path/datacost \
            --frame_rate 1 \
            --resolution 0.02

    done

done
