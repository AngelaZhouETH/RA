%% Reads all objects used in all scenes and calculates their frequency

clear;
close all;

%% Read the path

suncgDataPath    = 'SUNCG_Data_All/';
suncgToolboxPath = 'ToolBox/';
outputdir        = './';

addpath(suncgToolboxPath);
addpath(strcat(suncgToolboxPath,'/utils'));
addpath(strcat(suncgToolboxPath,'/benchmark'));


%% List of scenes

fId = fopen('listAllHouse.txt');
listScenes = textscan(fId, '%s');
fclose(fId);

listScenes = listScenes{1};
% random Shuffle
listScenes = listScenes(randperm(length(listScenes)));

numScenes = size(listScenes,1);


%% Create Hashtable
map = containers.Map;

%% Read files
for sceneIdx = 1:numScenes
    %% House
    
    
    sceneID = listScenes{sceneIdx};
    fprintf('Reading Scene:%s \n\n',sceneID);
    fprintf('Loading house.json\n');
    
    house  = loadjson(fullfile(suncgDataPath,'house', sceneID,'house.json'));

    levels = house.levels{1};
    nodes  = levels.nodes;
   
    
    for node_idx = 1:length(nodes)
        if strcmp(nodes{node_idx}.type, 'Room')

        room    = nodes{node_idx};

        %% Objectstoc;
            if isfield(room, 'nodeIndices')
                
                obj_in_room = room.nodeIndices;

                for objId = obj_in_room
                    object_struct = nodes{objId+1};

                    if isfield(object_struct, 'modelId')

                        % Load segmentation of object in object coordinates
                        filename= fullfile(suncgDataPath,'object_vox/object_vox_data/',...
                                           strrep(object_struct.modelId,'/','__')  ,...
                                           [strrep(object_struct.modelId,'/','__') , '.binvox']);
                          
                        % count              
                        if(isKey(map,filename))
                            map(filename) = map(filename) + 1;
                        else
                            map(filename) = 0;
                        end
                            

                       

                    end
                end
            end
        end
    end
end
    
    
    