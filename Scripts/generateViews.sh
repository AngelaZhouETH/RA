# -rec_dir  		RECONSTRUCTION DIRECTORY, example: /home/spyrosfedora/Desktop/CVG/Data_Small_Version/SUNCG/Results_25-10-2017_16:46:36/
# -tools_dir		SUNCG TOOLBOX DIRECTORY, example: /home/spyrosfedora/Desktop/CVG/SUNCGtoolbox-master/gaps/bin/x86_64
# -suncg_dir		SUNCG DATA DIRECTORY, example: /home/spyrosfedora/Desktop/CVG/Data_Full_Version/house/
# -modelmapping_file	MODEL MAPPING FILE, example: /home/spyrosfedora/Desktop/CVG/SUNCGtoolbox-master/metadata

#!/bin/bash
	clear
	echo -e "Generating Camera Views for Reconstructed Rooms...\n"

	readonly CAT_ONLY="1"
	readonly INST_ONLY="2"
	readonly CAT_and_INST="3"

	# Default Values
	reconstruction_dir="/Users/zhoumoyuan/Documents/sem4/RA/Data_Test"
	tools_dir="/Users/zhoumoyuan/Documents/sem4/RA/ToolBox/SUNCGtoolbox-spyridon/gaps/bin/x86_64"
	suncg_dir="/Users/zhoumoyuan/Documents/sem4/RA/Data_raw/house/"
	toolbox="/Users/zhoumoyuan/Documents/sem4/RA/ToolBox"
	modelmapping=${toolbox}/SUNCGtoolbox-spyridon/metadata
	segment_type=$CAT_and_INST
    if [ $segment_type -eq $INST_ONLY ]; then
        echo "GENERATE instance only."
        echo "***"
        imagesPerView="4"
    elif [ $segment_type -eq $CAT_ONLY ]; then
        echo "GENARATE category only."
        echo "***"
        imagesPerView="4"
    else
        echo "GENARATE instance and category."
        echo "***"
        imagesPerView="5"
    fi



	# Parse args and assign values
	POSITIONAL=()
	while [[ $# -gt 0 ]]
	do
	key="$1"

	case $key in
	    -rec_dir|--rec)
	    reconstruction_dir="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -tools_dir|--tools)
	    suncg_dir="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -suncg_dir|--suncg)
	    suncg_dir="$2"
	    shift # past argument
	    shift # past value
	    ;;
    	    -modelmapping_file|--mod)
	    modelmapping="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    *)    # unknown option
	    POSITIONAL+=("$1") # save it in an array for later
	    shift # past argument
	    ;;
	esac
	done
	set -- "${POSITIONAL[@]}" # restore positional parameters
    
	# Print configuration
	echo "RECONSTRUCTION DIRECTORY	= ${reconstruction_dir}"
	echo "SUNCG TOOLS DIRECTORY	= ${tools_dir}"
	echo "SUNCG DATA DIRECTORY	= ${suncg_dir}"
	echo "MODEL MAPPING FILE	= ${modelmapping}"
	echo -e "\n"

	/bin/sleep 2

	


	# take all scenes of the reconstruction scenes
	scenes=`ls $reconstruction_dir`

	# for each scene	
	for eachfile in $scenes
	do
	   echo $eachfile
		
	   if  [[ $eachfile != "timelog.txt" ]] ;
	   then
			# remove previous views
			cd $suncg_dir$eachfile
			find . -name "000*.png" -type f -delete
			find . -name "000*.jpg" -type f -delete
			# find . -name "outputcam*" -type f -delete	

			# execute SUNCG tools in order to generate views
			cd $suncg_dir$eachfile && ls
			pwd
			$tools_dir/scn2scn house.json house.obj
			$tools_dir/scn2cam house.json outputcamerafile -categories $modelmapping/ModelCategoryMapping.csv -v
			if [ $segment_type -eq $INST_ONLY ]; then
				$tools_dir/scn2img house.json outputcamerafile ./ -capture_instance_images -capture_color_images -capture_depth_images -capture_kinect_images -v
			elif [ $segment_type -eq $CAT_ONLY ]; then
				$tools_dir/scn2img house.json outputcamerafile ./ -categories $modelmapping/ModelCategoryMapping.csv -v
			else
				$tools_dir/scn2img house.json outputcamerafile ./ -capture_instance_images -capture_color_images -capture_depth_images -capture_kinect_images -categories $modelmapping/ModelCategoryMapping.csv -v
			fi
	   fi
	done

	# Transfer views in the proper folder
	cd ${toolbox}/ReadSemanticGroundTruth

	# Run matlab in command line mode using the input args
	matlab -nodisplay -nodesktop -r "clear all;reconstructionFolder = '${reconstruction_dir}';suncgFolder='${suncg_dir}';imagesPerView=${imagesPerView};generateViews;clear;exit();"

	# Delete duplicate images in Data_raw
	# find ${suncg_dir} ! -name 'house.*' -type f -exec rm -f {} +


