 # -i input folder
 # -o output folder
 # -r number of rooms to be generated
 # -v voxel size (best 0.01 - 0.08)
 # -m object cache activation
 # -s shuffle scene order
 # -d discard small rooms
 # -g GPU mode
 # -t generate instance labeling

 #!/bin/bash
	clear
	echo -e "Generating Synthetic Rooms Using the Bellow Configuration ...\n"

	# Default Values
	ROOMS="5"
	GRANULARITY="0.02"
	INPATH="'/Users/zhoumoyuan/Documents/sem4/RA/Data_raw'"
	OUTPATH="'/Users/zhoumoyuan/Documents/sem4/RA/Data_Test'"
	TOOLBOXPATH="/Users/zhoumoyuan/Documents/sem4/RA/ToolBox"
	HASH="false"
	SHUFFLE="false"
	DISCARD="false"
	GPU="false"
	INSTANCE="false"

	# Parse args and assign values
	POSITIONAL=()
	while [[ $# -gt 0 ]]
	do
	key="$1"

	case $key in
	    -r|--rooms)
	    ROOMS="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -v|--granularity)
	    GRANULARITY="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -i|--inpath)
	    INPATH="'$2/'"
	    shift # past argument
	    shift # past value
	    ;;
        -o|--outpath)
	    OUTPATH="'$2/'"
	    shift # past argument
	    shift # past value
	    ;;
	    -d|--discard)
	    DISCARD="true"
	    shift # past argument
	    ;;
	    -m|--map)
	    HASH="true"
	    shift # past argument
	    ;;
	    -s|--shuffle)
	    SHUFFLE="true"
	    shift # past argument
	    ;;
	    -g|--gpu)
	    GPU="true"
	    shift # past argument
	    ;;
        -t|--instance)
        INSTANCE="true"
        shift # past argument
        ;;
        --default)
	    DEFAULT=YES
	    shift # past argument
	    ;;
	    *)    # unknown option
	    POSITIONAL+=("$1") # save it in an array for later
	    shift # past argument
	    ;;
	esac
	done
	set -- "${POSITIONAL[@]}" # restore positional parameters
    
	# Print configuration
	echo "INPUT PATH		= ${INPATH}"
	echo "OUTPUT PATH		= ${OUTPATH}"
	echo "NUMBER OF ROOMS		= ${ROOMS}"
	echo "VOXEL SIZE		= ${GRANULARITY}"
	echo "SHUFFLE SCENES		= ${SHUFFLE}"
	echo "OBJECT CACHE		= ${HASH}"
	echo "DISCARD SMALL ROOMS	= ${DISCARD}"
	echo "GPU MODE		= ${GPU}"
    echo "INSTANCE LABELING       = ${INSTANCE}"
	echo -e "\n"

	cd ${TOOLBOXPATH}/ReadSemanticGroundTruth
	/bin/sleep 3

	# Run matlab in command line mode using the input args
	matlab -nodisplay -nodesktop -r "clear;totalRooms = ${ROOMS};suffle = ${SHUFFLE};rawDataPath = ${INPATH};outputdir = ${OUTPATH};readMap = ${HASH}; discard = ${DISCARD};voxUnit = ${GRANULARITY};enabledGPU =  ${GPU};withInstance =  ${INSTANCE};readAndConvertSUNCG;clear;exit();"


