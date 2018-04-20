There are 4 folders:

- Data_raw: contains the raw data as downloaded from SUNCG
- Parameters: contains a text file with the semantic labels description as used by us
- Scripts: contains bash scripts that do the conversion to a suitable format
- ToolBox: contains the codes used by the bash scripts

In order to succesfully reconstrcut the rooms and generate their associated views you should execute the scripts bellow with this specific order:

1. Script/generateSyntheticRooms.sh
2. Script/generateViews.sh
3. Script/convertDepthAndSegmentation.sh

-------------------------------------------------------------------------------------------------------------------------------------------------
Compile
-------------------------------------------------------------------------------------------------------------------------------------------------
1. Compile GAPS, the library that comes with SUNCG
* cd ToolBox/SUNCGtoolbox-spyridon/gaps
* make clean
* make

2. Compile ConvertDepthAndSeg
* cd ToolBox/ConvertDepthAndSeg/build
* cmake ..
* make

3. Compile Mex files
* The command for compiling mex files are in ToolBox/ReadSemanticGroundTruth/readAndConvertSUNCG.m
* You can uncomment them the first time you run readAndConvertSUNCG.m (called in Script/generateSyntheticRooms.sh), and once the compilation is successful, comment them again (if not, it will compile them every time, which is a little bit time consuming).

-------------------------------------------------------------------------------------------------------------------------------------------------
Execution instructions
-------------------------------------------------------------------------------------------------------------------------------------------------

- generateSyntheticRooms.sh

* Goes through the scenes in the Data_raw folder and generate rooms from these scenes. Each room is created in a separated folder so that they can be manipulated independantly
* An output directory is created, in which a different directory for each scene is created
* It constructs a specific number of rooms given by the command line
* Before you execute it you have to match the paths with your file destination in your system (absolute path).
* For more execution details and passing of parameters check the comments in generateSyntheticRooms.sh

- generateViews.sh

* It uses the SUNCG toolbox to generate the views and the corresponding images inside the full SUNCG dataset directory
* SUNCG toolbox has been modified, so you should use my version. Its inside the main folder
* Before you execute it you have to match the paths with your file destination in your system (absolute path).
* Everytime you want to generate the views you have to give the destination of the reconstructed rooms
* If for one scene you have generated already the views the script automatcally deleted everything and creates it from start
* It transfers the previously generated rooms to the reconstruction folder
* For more execution details and passing of parameters check the comments in generateViews.sh

- convertDepthAndSegmentation.sh

* Reads all the depth and semantic information provided by the generateViews.sh script and convert them to a format used by us
* Before you execute it you have to match the paths with your file destination in your system (absolute path).
* For more execution details and passing of parameters check the comments in convertDepthAndSegmentation.sh

-------------------------------------------------------------------------------------------------------------------------------------------------
Miscelleanous
-------------------------------------------------------------------------------------------------------------------------------------------------
* No need to read every piece of code. The main ones are called from the bash scripts in the Script folder.
* For more information on GAPS and SUNCG in general, visit 
https://github.com/shurans/SUNCGtoolbox
* More informations on some of the matlab scripts can be found in
https://github.com/shurans/sscnet
