%% Set paths
suncgToolboxPath = 'ToolBox/';

addpath(suncgToolboxPath);
addpath(strcat(suncgToolboxPath,'/utils'));
addpath(strcat(suncgToolboxPath,'/benchmark'));

load(fullfile(suncgToolboxPath, 'ClassMapping.mat'));
load(fullfile(suncgToolboxPath, 'suncgObjcategory.mat'));

%% Compile C++
% mex -I"../Eigen/eigen" -outdir ToolBox/MyMexFiles -c ToolBox/MyMexFiles/ioTools.cpp
% mex -I"../Eigen/eigen" -outdir ToolBox/MyMexFiles -c ToolBox/MyMexFiles/volumetricFusionTools.cpp
% mex -I"../Eigen/eigen" -lpthread -lX11 ToolBox/MyMexFiles/ioTools.o ToolBox/MyMexFiles/volumetricFusionTools.o

%% Intiallize working folders
% reconstructionFolder = '/home/spyrosfedora/Desktop/CVG/Data_Small_Version/SUNCG/Results_24-10-2017_13:54:08/';
% suncgFolder = '/home/spyrosfedora/Desktop/CVG/Data_Full_Version/house/';
% toolsFolder = '/home/spyrosfedora/Desktop/CVG/SUNCGtoolbox-master/gaps/bin/x86_64/';

%% Copy views that correspond to the reconstructed rooms
% load label dictionary
M_index = csv2cell('class_index.csv','fromfile');

recfiles   = dir(fullfile(reconstructionFolder));

imagesPerView = 4;

% For all reconstructed scenes
for i=3:size(recfiles)
    
    currentFolder = fullfile(suncgFolder,recfiles(i).name);
    camerafile    = fullfile(currentFolder, 'outputcamerafile');
    
    % If exist camera file in the suncg dataset scene folder
    if exist(camerafile)
        fprintf(sprintf('Scene: %s\n', recfiles(i).name));
        cameraAll= load(camerafile);
        
        % If there are generated views
        if isempty(cameraAll)
            continue;
        end

        camera = cameraAll(:,1:3);

        % load the scene json files
        house  = loadjson(strcat(currentFolder,'/','house.json'));
        nodes  = house.levels{1}.nodes;
        
        % take all the views of the current data scene folder
        images = dir(strcat(currentFolder,'/','0*_*'));
        images = string({images.name});
        images = reshape(images,imagesPerView,[]);
        images = images.transpose;

        % for every room in the data scene
        for node_idx = 1:length(nodes)

            if strcmp(nodes{node_idx}.type, 'Room')
                % room size
                room = nodes{node_idx};
                bbox = nodes{node_idx}.bbox;
                max = bbox.max;
                min = bbox.min;
                centerX = (bbox.max(1)- bbox.min(1))/2  + bbox.min(1);
                centerZ = (bbox.max(3)- bbox.min(3))/2  + bbox.min(3);

                fprintf(sprintf('Reading room %s\n', room.modelId));

                path = fullfile(reconstructionFolder, recfiles(i).name, strcat('room_', room.modelId));
                disp(path);

                % if this room has been reconstructed
                if exist(path)
		    fid=fopen(fullfile(path, 'cameras.txt'), 'w');
		    fclose(fid);


                    % copy all the views of the scene lie inside
                    % the current reconstructed room
                    for view=1:size(camera,1)

                        if ((camera(view,:)<max) & (camera(view,:)>min) & ...
                           ((camera(view,1) - centerX)*cameraAll(view,4) < 0) & ...
                           ((camera(view,3) - centerZ)*cameraAll(view,6) < 0 ))

                            for image=1:imagesPerView
%                                 file = strcat(currentFolder,'/',images(view,image));
                                file = sprintf('%s/%s', currentFolder,images(view,image));
                                if exist(file)
                                    %movefile(char(file),char(path));
                                    copyfile(char(file),char(path));

                                    %translate category images to 36 classes format 
                                    if(endsWith(file,'category.png'))
                                       file2 = strcat(path,'/', images(view,image));
                                       translateCategories(file2, M_index, objcategory);
                                    end
                                end
                            end
                            % copy view features into a local file
                            % in reconstruction folder
                            dlmwrite(char(strcat(path,'/cameras.txt')),cameraAll(view,:),'-append');

                        end
                    end
                end    
            end
        end

    else
        disp('(Igone this message if the file is the camera file!)Cannot load views from file bellow:')
        disp(camerafile);
    end
    
    
    
end

disp('End!')
