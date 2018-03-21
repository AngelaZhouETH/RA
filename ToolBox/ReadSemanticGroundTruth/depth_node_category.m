
suncgToolboxPath = 'ToolBox/';
addpath(suncgToolboxPath);
addpath(strcat(suncgToolboxPath,'/utils'));
addpath(strcat(suncgToolboxPath,'/benchmark'));

load(fullfile(suncgToolboxPath, 'ClassMapping.mat'));
load(fullfile(suncgToolboxPath, 'suncgObjcategory.mat'));

% Compile C++
mex -I"/home/spyrosfedora/Desktop/CVG/Data_Small_Version/Eigen/eigen" -outdir ToolBox/MyMexFiles -c ToolBox/MyMexFiles/ioTools.cpp
mex -I"/home/spyrosfedora/Desktop/CVG/Data_Small_Version/Eigen/eigen" -outdir ToolBox/MyMexFiles -c ToolBox/MyMexFiles/volumetricFusionTools.cpp
mex -I"/home/spyrosfedora/Desktop/CVG/Data_Small_Version/Eigen/eigen" -lpthread -lX11 ToolBox/MyMexFiles/ioTools.o ToolBox/MyMexFiles/volumetricFusionTools.o



depth = imread('/home/spyrosfedora/Desktop/CVG/Data_Small_Version/SUNCG/Results_01-11-2017_11:54:01/0d573534b541b85cced6f3b560dd263d/level1/room_fr_0rm_1/000042_kinect.png');
node =imread('/home/spyrosfedora/Desktop/CVG/Data_Small_Version/SUNCG/Results_01-11-2017_11:54:01/0d573534b541b85cced6f3b560dd263d/level1/room_fr_0rm_1/000042_node.png');
category =imread('/home/spyrosfedora/Desktop/CVG/Data_Small_Version/SUNCG/Results_01-11-2017_11:54:01/0d573534b541b85cced6f3b560dd263d/level1/room_fr_0rm_1/000042_category.png');

%imshow(depth*12);

un = unique(node)
c = distinguishable_colors(size(un,1));
categoryRGB1 = zeros(480,640);
categoryRGB2 = zeros(480,640);
categoryRGB3 = zeros(480,640);

for i=1:size(un)
    
    categoryRGB1(node==un(i)) = c(i,1);
    categoryRGB2(node==un(i)) = c(i,2);
    categoryRGB3(node==un(i)) = c(i,3);
    
end

rgb1 = cat(3, categoryRGB1, categoryRGB2, categoryRGB3);


un = unique(category)
c = distinguishable_colors(size(un,1));
categoryRGB1 = zeros(480,640);
categoryRGB2 = zeros(480,640);
categoryRGB3 = zeros(480,640);

for i=1:size(un)
    
    categoryRGB1(category==un(i)) = c(i,1);
    categoryRGB2(category==un(i)) = c(i,2);
    categoryRGB3(category==un(i)) = c(i,3);
    
end

rgb2 = cat(3, categoryRGB1, categoryRGB2, categoryRGB3);
figure, imshow(depth*12);
figure, imshow(rgb2);

% from mapnyu894to40 get string on the index. In nyu40class find the index
% of the string found before. In mapnyu40to36 go to the index found before
% and get the string. Finally from p5d36class find the index of the
% previous string

% M = csv2cell('class_index.csv','fromfile');
% string(122)
% [ena,dyo, tria, tessera, pente] = getobjclassSUNCG(string("floor"),objcategory)