function [ gridPtsWorldX,gridPtsWorldY,gridPtsWorldZ,gridPtsWorld,gridPtsLabel, inRoom] = constructGrid( voxOriginWorld, voxUnit, voxSize )


        [gridPtsWorldX,gridPtsWorldY,gridPtsWorldZ] = ndgrid(voxOriginWorld(1):voxUnit:(voxOriginWorld(1)+(voxSize(1)-voxUnit)), ...
                                                             voxOriginWorld(2):voxUnit:(voxOriginWorld(2)+(voxSize(2)-voxUnit)), ...
                                                             voxOriginWorld(3):voxUnit:(voxOriginWorld(3)+(voxSize(3)-voxUnit)));


        % Voxel grid points and lables per voxel
        gridPtsWorld = [gridPtsWorldX(:),gridPtsWorldY(:),gridPtsWorldZ(:)]';
        gridPtsLabel = zeros(1,size(gridPtsWorld,2));


        % Structure to store polygons lie in grid
        inRoom = zeros(size(gridPtsWorldX));

        % Drop dimmention 2 in grid points (because its not used in next stages)
        gridPtsWorldX = gridPtsWorldX(:,1,:);
        gridPtsWorldY = gridPtsWorldY(:,1,:);
        gridPtsWorldZ = gridPtsWorldZ(:,1,:);


end

