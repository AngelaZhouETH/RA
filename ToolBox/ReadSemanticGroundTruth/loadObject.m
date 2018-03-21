%% Checks whether an object is in cache or it has to be loaded. In case of full cache removes a bunch of least important objects

function [ objSegPts, scale] = loadObject(filename, map, scales, minobjs, capacity )
    
    % If object is in cache fetch it
    if(isKey(map, filename))
        objSegPts = map(filename);
        scale = scales(filename);
    else
        % If the object is not in cache and there is still available space
        if (size(keys(map)) < capacity)
            
            % load and apply transorms
            [voxels, scale, translate] = read_binvox(filename);
            [x, y, z] = ind2sub(size(voxels),find(voxels(:)>0));
            objSegPts = bsxfun(@plus,[x,y,z]*scale,translate([1,2,3])');
            objSegPts = [objSegPts(:,[1,3,2])';ones(1,size(x,1))];
            

            % Save to map
            map(filename) =objSegPts;
            scales(filename) = scale;
            
        else
            % If no space in cache
            index = 1;
            while(size(keys(map))~= capacity - 100)
                % remove 100 least important keys
                remove(map, cell2mat(minobjs.mink(index)));
                remove(scales, cell2mat(minobjs.mink(index)));
                index = index + 1;
                
                if(index == size(minobjs.mink) - 1)
                    break;
                end
                
                
            end
            
            % load and apply transorms
            [voxels, scale, translate] = read_binvox(filename);
            [x, y, z] = ind2sub(size(voxels),find(voxels(:)>0));
            objSegPts = bsxfun(@plus,[x,y,z]*scale,translate([1,2,3])');
            objSegPts = [objSegPts(:,[1,3,2])';ones(1,size(x,1))];

            % Save to map
            map(filename) = objSegPts;
            scales(filename) = scale;

        end

    %else
       % if (size(keys(filename)) >= capacity)
            
    end
end

