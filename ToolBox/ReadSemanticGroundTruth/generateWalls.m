% Reads and reconstructs the walls of a room

function [ gridPtsLabel ] = generateWalls( path, id, sizeR, voxUnit, voxOriginWorld, gridPtsWorld, gridPtsWorldX, gridPtsWorldZ, gridPtsLabel)

   if(exist (path)) 

        wallObj  = read_wobj(path);

        inRoom = zeros(sizeR);

        % for all wall objects (walls include many objects like windows etc)
        for oi = 1:length(wallObj.objects)
            if wallObj.objects(oi).type == 'f'

                for i = 1:length(wallObj.objects(oi).data.vertices)
                    faceId = wallObj.objects(oi).data.vertices(i,:);
                    floorP = wallObj.vertices(faceId,[1,3])';
                    inRoom = inRoom|inpolygon(gridPtsWorldX,gridPtsWorldZ,floorP(1,:),floorP(2,:));
                end
            end
        end

        gridPtsObjWorldInd = inRoom(:)' & ...
                            (gridPtsWorld(2,:)<voxOriginWorld(2)+size(gridPtsLabel, 2)-2*voxUnit/2) & ...
                            (gridPtsWorld(2,:)>voxOriginWorld(2)+0-2*voxUnit/2);

        gridPtsLabel(gridPtsObjWorldInd) = id;
        
   end

end

