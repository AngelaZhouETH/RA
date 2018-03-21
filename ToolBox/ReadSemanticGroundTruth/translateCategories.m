% translates a category image to 36 classes format 
function [ output_args ] = translateCategories(file, indexM , objcategory)

    % read image
    category =imread(char(file));
    category_new = zeros(size(category));
    % get its labels
    un = unique(category);

    % for all labels
    for i=1:size(un)
        % translate
        [~,label, ~, ~, ~] = getobjclassSUNCG(strrep(string(indexM(un(i)+1,2)),'/','__'),objcategory);
        % replace
        category_new(category==un(i)) = label;
    end
    
    % overwrite
    imwrite(uint8(category_new),char(file));
    
    output_args = true;
end

