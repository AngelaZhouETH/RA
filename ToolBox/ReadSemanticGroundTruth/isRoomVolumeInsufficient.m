%% Checks whether the room volume is smaller than a threshold volume

function [ answer ] = isRoomVolumeInsufficient( roomvolume, threshold )

    answer = false;
    size = roomvolume(1) * roomvolume(2) * roomvolume(3);
    
    if size <= threshold
        answer = true;
    end


end

