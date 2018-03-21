% Checks if each of the two input rooms/bboxes lie inside the other
function [ result ] = roomIncluded( old , new )

    result = 0;
    % tolerance in equality
    margin = 0.1;
    if((old.min - margin <= new.min) & (old.max >= new.max -margin)  | (new.min - margin<= old.min) & (new.max >= old.max -margin))
        
        result= 1;
    end

end

