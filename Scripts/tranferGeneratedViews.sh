# -rec_dir  	RECONSTRUCTION DIRECTORY, example: /home/spyrosfedora/Desktop/CVG/Data_Small_Version/SUNCG/Results_24-10-2017_13:54:08/
# -suncg_dir	SUNCG DATA DIRECTORY, example: /home/spyrosfedora/Desktop/CVG/Data_Full_Version/house/

#!/bin/bash
	clear
	echo -e "Transfering Camera Views to Reconstruction Folder..\n"

	# Default Value
	reconstruction_dir="'/home/iancher/Desktop/4Moyuan/Data_Test'"
	suncg_dir="'/home/iancher/Desktop/4Moyuan/Data_raw/house/'"
	toolbox="/home/iancher/Desktop/4Moyuan/ToolBox"
	

	# Parse args and assign values
	POSITIONAL=()
	while [[ $# -gt 0 ]]
	do
	key="$1"

	case $key in
	    -rec_dir|--rec)
	    reconstruction_dir="'$2/'"
	    shift # past argument
	    shift # past value
	    ;;
	    -suncg_dir|--suncg)
	    suncg_dir="'$2'/"
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
	echo "SUNCG DATA DIRECTORY		= ${suncg_dir}"
	echo -e "\n"

	cd ${toolbox}/ReadSemanticGroundTruth

	/bin/sleep 2

	# Run matlab in command line mode using the input args
	matlab -nodisplay -nodesktop -r "clear all;reconstructionFolder = ${reconstruction_dir};suncgFolder =${suncg_dir};generateViews;clear;exit();"
