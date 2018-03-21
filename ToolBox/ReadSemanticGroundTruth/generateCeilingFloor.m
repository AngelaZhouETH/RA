% Reads and reconstructs the floor ot the ceiling of a room

function [ gridPtsLabel ] = generateCeilingFloor( path, id, sizeR, voxUnit, voxOriginWorld, gridPtsWorld, gridPtsWorldX, gridPtsWorldZ, gridPtsLabel)


    if(exist(path))

        % load ceiling object
        obj  = read_wobj(path);
        % set z as the average of all points
        posZ  = mean(obj.vertices(:,2));

        inRoom = zeros(sizeR);

        % for all vertices of ceiling objects
        for i = 1:length(obj.objects(3).data.vertices)
            % take obj vertice
            faceId1 = obj.objects(3).data.vertices(i,:);
            % store obj vertices potitions in axis x and z
            pos  = obj.vertices(faceId1,[1,3])';
            % store all points in the voxel grid which lie inside
            % the cieling polygons polygon
            [in,on] = inpolygon(gridPtsWorldX,gridPtsWorldZ,pos(1,:),pos(2,:));
            % gradually update inRoom
            inRoom = inRoom | in | on;
        end
        % keep all the points which lie on the level of the ceiling height
        gridPtsObjWorldInd = inRoom(:)' & (abs(gridPtsWorld(2,:)-voxOriginWorld(2)-posZ) <= 2*voxUnit/2);
        % label all the ceiling points in the scene
        gridPtsLabel(gridPtsObjWorldInd) = id;
        
    end


end

