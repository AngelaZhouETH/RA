#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <Eigen/Core>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "d3d_base/depthMap.h"
#include "d3d_base/exception.h"
#include "d3d_base/grid.h" 

using namespace std;
namespace bpo = boost::program_options;

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}


string extractBaseName(string fullName)
{
    size_t pos = fullName.find_last_of('.');
    if(pos != string::npos)
        return fullName.substr(0,pos);
    else
        return fullName;
}


ifstream& goToLine(ifstream& file, int line)
{
    file.seekg(ios::beg);
    for(int i = 0; i < line - 1; ++i)
        file.ignore(numeric_limits<streamsize>::max(),'\n');

    return file;
}


template<typename T>
void readFromCamParam(vector<T> &param, stringstream &camParamStream)
{
    int numParam = param.size();

    for (int i = 0 ; i < numParam ; ++i){
        string param_i;
//        camParamStream >> param[i];
        getline(camParamStream, param_i, ',');
        param[i] = stod(param_i);
    }
}

template<typename T>
void readCamParam(vector<T> &camPos, vector<T> &towardsDir,
                  vector<T> &upDir , vector<T> &fieldOfView, string camParam)
{
    stringstream camParamStream(camParam);

    // Camera Position
    readFromCamParam(camPos, camParamStream);

    // View direction
    readFromCamParam(towardsDir, camParamStream);

    // Camera up direction
    readFromCamParam(upDir, camParamStream);

    // Camera field of view
    readFromCamParam(fieldOfView, camParamStream);
}


template<typename T>
void readRT(Eigen::Matrix<T, 3, 3> &rot, Eigen::Matrix<T, 3, 1> &trans, string rtParam)
{
    stringstream rtParamStream(rtParam);

    for(int i = 0 ; i < 3 ; ++i){
        for(int j = 0 ; j < 3 ; ++j){
            rtParamStream >> rot(i,j);
        }
        rtParamStream >> trans(i,0);
    }
}



int main(int argc, char* argv[])
{
    float maxFloatLim = numeric_limits<float>::max();
    string tempString;

    string imageFile;
    string cameraFile;

    int width;
    int height;

    float  min_thresh   = 0.0f;
    float  max_thresh   = 0.0f;
    string outputFolder;

    bpo::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Produce help message")
            ("depthMaps"   , bpo::value<string>(&imageFile   )->default_value("images.txt"   ), "List containing the poses")
            ("cameras"     , bpo::value<string>(&cameraFile  )->default_value("cameras.txt"  ), "Internal calibration matrix")
            ("width"       , bpo::value<int>   (&width       )->default_value(640            ), "Image width")
            ("height"      , bpo::value<int>   (&height      )->default_value(480            ), "Image height")
            ("min_thresh"  , bpo::value<float> (&min_thresh  )->default_value(0.35f          ), "Minimum valid depth")
            ("max_thresh"  , bpo::value<float> (&max_thresh  )->default_value(maxFloatLim    ), "Maximum valid depth")
            ("outputFolder", bpo::value<string>(&outputFolder)->default_value("out_depthMaps"), "Output folder")
            ;

    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << endl;
        return 1;
    }

    // Create Output Folder if necessary
    if(!boost::filesystem::exists(outputFolder.c_str()))
        if(!boost::filesystem::create_directories(outputFolder.c_str()))
            D3D_THROW_EXCEPTION("Error creating output directory.");

    // Read the depth files
    ifstream imageStream;
    imageStream.open(imageFile.c_str());

    if(!imageStream.is_open()){
        D3D_THROW_EXCEPTION("Could not open image files");
    }

    vector<string> imageList;
    int numDepth = 0;
    while (imageStream >> tempString){
        imageList.push_back(tempString);
        ++numDepth;
    }

    imageStream.close();

    // Read the camera files
    ifstream cameraStream;
    cameraStream.open(cameraFile);

    if(!cameraStream.is_open()){
        D3D_THROW_EXCEPTION("Could not open camera files");
    }

    vector<string> cameraList;
    int numCam = 0;
    while (getline(cameraStream, tempString)){
        cameraList.push_back(tempString);
        ++numCam;
    }

    cameraStream.close();

    // Sanity check
    if (numCam != numDepth){
        D3D_THROW_EXCEPTION("Not the same number of cameras and depths!");
    }

    // Go through all files and create the depth
    for (int view = 0 ; view < numCam ; ++view)
    {
        cout << "Processing camera #" << view << endl;

        // Read the camera parameters
        string camParam = cameraList[view];

        vector<double> camPos (3, 0.0f);
        vector<double> towardsDir(3, 0.0f);
        vector<double> upDir  (3, 0.0f);
        vector<double> fov    (2, 0.0f);

        readCamParam(camPos, towardsDir, upDir, fov, camParam);

        // Convert the parameters to camera matrix
        // Rotation matrix - Correspond aux calculs d'extrinsics de GAP
        Eigen::Vector3d towards(towardsDir.data());
        Eigen::Vector3d up(upDir.data());

        up *= -1.0;

        towards *= -1.0;
        towards.normalize();

        Eigen::Vector3d right = towards.cross(up);
        right.normalize();

        up = right.cross(towards);

        Eigen::Matrix<double, 3, 3> R = Eigen::Matrix<double, 3, 3>::Identity();
        for (int i = 0 ; i < 3 ; ++i){
            R(0,i) = right(i);
            R(1,i) = up(i);
            R(2,i) = -towards(i);
        }

        // Translation
        Eigen::Matrix<double, 3, 1> C;
        C(0,0) = camPos[0];
        C(1,0) = camPos[1];
        C(2,0) = camPos[2];

        Eigen::Matrix<double, 3, 1> T;
        T = -R*C;

        // Convert field of view
        double focalLength_x = width  / (2 * tan(fov[0]));
        double focalLength_y = height / (2 * tan(fov[1])); //width/height*fov[0]));

        // Internal calibration
        Eigen::Matrix3d K = Eigen::Matrix3d::Identity();
        K(0,0) = focalLength_y;
        K(1,1) = focalLength_x;
        K(0,2) = width/2;
        K(1,2) = height/2;

        // Create and store the camera
        D3D::CameraMatrix<double> camera;
        camera.setKRT(K, R, T);

        // Initialize the depth map
        D3D::DepthMap<float, double> depthMap(width, height, camera);

        // Get the name of the depth file
        string depthName = imageList[view];

        // Open image and read
        cv::Mat depthImg = cv::imread(depthName.c_str(), CV_LOAD_IMAGE_ANYDEPTH);

        for (int py = 0 ; py < height ; ++py){
            for (int px = 0 ; px < width ; ++px)
            {
                float depth = (float)depthImg.at<uint16_t>(py, px)/1000.0f;
                depthMap(px, py) = depth;
            }
        }

        // Save files
        string baseName = extractBaseName(depthName);

        string name_img = outputFolder + "/" + baseName + ".png";
        string name_dat = outputFolder + "/" + baseName + ".dat";

        depthMap.saveInvDepthAsColorImage(name_img.c_str(), min_thresh, max_thresh);
        depthMap.saveAsDataFile(name_dat.c_str());
    }


    return 0;
}





































