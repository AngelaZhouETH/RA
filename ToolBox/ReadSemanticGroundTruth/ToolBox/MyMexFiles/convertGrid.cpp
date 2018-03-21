/*
 * convertGrid.cpp - example in MATLAB External Interfaces
 *
 * Convert a grid in matlab into Christian's D3D multiClassGrid format
 *
 * The calling syntax is:
 *
 *		convertGrid(inGrid)
 *
 * This is a MEX file for MATLAB.
*/

#include <fstream>
#include <string>
#include <vector>

#include "grid.h"
#include "multiClassGrid.h"
#include "volumetricFusionTools.h"

#include "mex.h"

using namespace std;

// Labels structure
struct label
{
    std::string name;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /** CHECK INPUT VALIDITY **/
    /* Check for proper number of arguments */
    if(nrhs != 5) {
        mexErrMsgIdAndTxt("MyMexFiles:convertGrid:nrhs", "5 input required.");
    }
    
    if(nlhs > 0) {
        mexErrMsgIdAndTxt("MyMexFiles:convertGrid:nlhs", "No output required.");
    }
        
    /* Check if the input is of proper type */
    if(!mxIsDouble(prhs[0]) || mxIsComplex(prhs[0])) {
        mexErrMsgIdAndTxt("MyMexFiles:convertGrid:notDouble", 
                            "Input grid values must be type double.");
    }
    
    if(!mxIsDouble(prhs[1]) || mxIsComplex(prhs[1])) {
        mexErrMsgIdAndTxt("MyMexFiles:convertGrid:notDouble", 
                            "Input grid dimension must be type double.");
    }
        
    if(!mxIsDouble(prhs[2]) || mxIsComplex(prhs[2])) {
        mexErrMsgIdAndTxt("MyMexFiles:convertGrid:notDouble", 
                            "Input grid origin must be type double.");
    }
    
    if ( mxIsChar(prhs[3]) != 1) {
        mexErrMsgIdAndTxt( "MATLAB:convertGrid:inputNotString",
                            "Input labels file must be a string.");
    }

    if ( mxIsChar(prhs[4]) != 1) {
        mexErrMsgIdAndTxt( "MATLAB:convertGrid:inputNotString",
                            "OutputFolder nane must be a string.");
    }

    
    /* Check if input are row vectors */
    if(mxGetM(prhs[0]) != 1) {
        mexErrMsgIdAndTxt("MyMexFiles:convertGrid:notRowVector", 
                            "Input grid values must be row vector.");
    }
    
    if(mxGetM(prhs[1]) != 1 || mxGetN(prhs[1]) != 4) {
        mexErrMsgIdAndTxt("MyMexFiles:convertGrid:notRowVector", 
                            "Input grid dimension must be row vector of size 4.");
    }
    
    if(mxGetM(prhs[2]) != 1 || mxGetN(prhs[2]) != 3) {
        mexErrMsgIdAndTxt("MyMexFiles:convertGrid:notRowVector", 
                            "Input grid origin must be row vector of size 3.");
    }
    
    /** CORE FUNCTION **/
    /* Read the input data */
    double *gridValues  = mxGetPr(prhs[0]);
    double *gridSize    = mxGetPr(prhs[1]);
    double *gridOrig    = mxGetPr(prhs[2]);
    
    int numClasses = gridSize[0];
    int xRes = gridSize[1];
    int yRes = gridSize[2];
    int zRes = gridSize[3];
    
    char *labelFile;
    labelFile = mxArrayToString(prhs[3]);
    
    char *outputFolder;
    outputFolder = mxArrayToString(prhs[4]);
    
    /* Create a MultiClassGrid */
    D3D::Grid<int> gtGrid(xRes, yRes, zRes, 0.0f);
    
    for(int z = 0 ; z < zRes ; ++z){
        int idx_z = z*xRes*yRes;
        
        for(int y = 0 ; y < yRes ; ++y){
            int idx_yz = y*xRes + idx_z;
            
            for(int x = 0 ; x < xRes ; ++x){
                int idx_xyz = x + idx_yz;
                
                int c = gridValues[idx_xyz];
                
                gtGrid(x, y, z) = c;
                
            }
        }
    }
    
    /* Save the multiclassgrid */
    std::stringstream gtDatFile; // Name of the complete 3D model
    gtDatFile << outputFolder << "/GroundTruth.dat";
    
    gtGrid.saveAsDataFile(gtDatFile.str().c_str());
    
    /* Create a 3d mesh for visualization */
    label empty_label = {"sky", 0, 0, 0};
    
    vector<label> labels(37);
    labels[0] = empty_label;
    
    ifstream labelStream;
    labelStream.open(labelFile);

    if (!labelStream.is_open())
        D3D_THROW_EXCEPTION("Could not open label file.");

    string labelName;
    int labelIdx = 1;
    while (labelStream >> labelName)
    {
        label newLabel;
        newLabel.name = labelName;

        int val;
        labelStream >> val;
        newLabel.r = val;
        labelStream >> val;
        newLabel.g = val;
        labelStream >> val;
        newLabel.b = val;

        labels[labelIdx] = newLabel;
        ++labelIdx;
    }

    labelStream.close();
    
    Eigen::Vector3f minCorner;
    minCorner(0) = gridOrig[0];
    minCorner(1) = gridOrig[1];
    minCorner(2) = gridOrig[2];
    
    Eigen::Vector3f size;
    size(0) = 0.02*xRes;
    size(1) = 0.02*yRes;
    size(2) = 0.02*zRes;
    
    int write_idx = 0; // Useful for creating the complete model
    for (int i = 0; i < numClasses; i++)
    {
        if (!strcmp(labels[i].name.c_str(), "sky"))
            continue;

        // Create the 3D grid
        D3D::Grid<float> tempGrid(xRes, yRes, zRes, 0.0f); // Grid which will be saved as a 3D model

        int totalVoxelsAssigned = 0;
        
        for (int z = 0; z < zRes; z++)
            for (int y = 0; y < yRes; y++)
                for (int x = 0; x < xRes; x++)
                {
                    if(gtGrid(x, y, z) == i)
                    {
                        float voxelValue = 1.0f;
                        tempGrid(x, y, z) += voxelValue; //groundTruth_sub(i,x+1,y+1,z+1);
                        totalVoxelsAssigned += voxelValue;
                    }
                }
        
        if(totalVoxelsAssigned == 0)
            continue;
                    
        // Get the color
        Eigen::Vector3f color;
        color(0) = (labels[i].r)/255.0f;
        color(1) = (labels[i].g)/255.0f;
        color(2) = (labels[i].b)/255.0f;

        // Get the file names
        std::stringstream fileName_c; // Name of the 3D model class i composing the scene
        fileName_c << outputFolder << "/optModel_" << labels[i].name << ".wrl";

        std::stringstream fileNameComplete_c; // Name of the complete 3D model
        fileNameComplete_c << outputFolder << "/fullModel.wrl";

        // Save the 3D model of the class i composing the scene
        D3D::saveVolumeAsVRMLMesh(tempGrid, 0.5f, minCorner, size,
                                  Eigen::Matrix4f::Identity(), color,
                                  fileName_c.str(), true);

        // Add the 3D model of class i to the complete model
        if(write_idx == 0)
        {
            // First class to be added to the complete 3D model
            D3D::saveVolumeAsVRMLMesh(tempGrid, 0.5f, minCorner, size,
                                      Eigen::Matrix4f::Identity(), color,
                                      fileNameComplete_c.str(), false, false);
        }
        else if(write_idx == numClasses-2)
        {
            // Last class to be added to the complete 3D model, display axis
            D3D::saveVolumeAsVRMLMesh(tempGrid, 0.5f, minCorner, size,
                                      Eigen::Matrix4f::Identity(), color,
                                      fileNameComplete_c.str(), true, true);
        }
        else
        {
            // Add class to the complete 3D model
            D3D::saveVolumeAsVRMLMesh(tempGrid, 0.5f, minCorner, size,
                                      Eigen::Matrix4f::Identity(), color,
                                      fileNameComplete_c.str(), false, true);
        }

        write_idx++;
    }
}








































