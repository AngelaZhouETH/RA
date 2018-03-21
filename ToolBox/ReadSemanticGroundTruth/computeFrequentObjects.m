%% Computes or loads from files the most freequent objects(.bivox files) and stores them into a map structure.

function [ objmap, scalemap, maxobj, minobj ] = computeFrequentObjects(readMap)


    maxobj = load('max100keys.mat');
    minobj = load('min100keys.mat');

    if(readMap)
        fprintf('\nLoading most frequent objects...');


        %% Create Hashtable
        objmap = containers.Map;
        scalemap = containers.Map;

        for i=1:500

                % Get filename
                filename= cell2mat(maxobj.maxk(i));

                % Load Object file
                [voxels, scale, translate] = read_binvox(filename);
                [x, y, z] = ind2sub(size(voxels),find(voxels(:)>0));
                objSegPts = bsxfun(@plus,[x,y,z]*scale,translate([1,2,3])');
                objSegPts = [objSegPts(:,[1,3,2])';ones(1,size(x,1))];


                % Save to map
                objmap(filename) = objSegPts;
                scalemap(filename) = scale;

        end

        fprintf('Objects loaded.\n\n');
    else
        objmap = containers.Map;
        scalemap = containers.Map;
        %objmap = load('frequentObjects.mat');
    end




end

