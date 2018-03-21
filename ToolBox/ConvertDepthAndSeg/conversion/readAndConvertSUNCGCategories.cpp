#include <fstream>
#include <iostream>
#include <list>
#include <new>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <d3d_base/exception.h>
#include <d3d_base/grid.h>
#include <d3d_io/ioTools.h>

#include <sys/stat.h>

using namespace std;
namespace bpo = boost::program_options;

struct label
{
    string name;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

string myExtractBaseFileName(const string &fullFileName)
{
    size_t pos = fullFileName.find_last_of('/');
    string baseName;
    if (pos != string::npos)
    {
        baseName = fullFileName.substr(pos+1, fullFileName.size()-pos);
    }
    else
    {
        baseName = fullFileName;
    }
    // remove the ending
    pos = baseName.find_last_of('.');
    if (pos != string::npos)
    {
        return baseName.substr(0, pos);
    }
    else
    {
        return baseName;
    }
}

int main(int argc, char* argv[])
{
    string labelsFile;
    string outputFolder;
    string imagesList;

    bpo::options_description desc("Options");
    desc.add_options()
            ("help","Display allowed options")
            ("images"      , bpo::value<string>(&imagesList)->default_value("images.txt")  , "List of all scores files")
            ("labels"      , bpo::value<string>(&labelsFile)    ->default_value("labels.txt")  , "Description of existing labels")
            ("outputFolder", bpo::value<string>(&outputFolder)  ->default_value("segmentation"), "Folder for storing the output files")
            ;

    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(argc,argv).options(desc).run(), vm);
    bpo::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << endl;
        return 1;
    }

    // Read output directory
    if (!boost::filesystem::exists(outputFolder))
    {
        if (!boost::filesystem::create_directory(outputFolder))
        {
            D3D_THROW_EXCEPTION("Error creating output directory.")
        }
    }

    // Read the labels file
    ifstream labelStream;
    labelStream.open(labelsFile.c_str());

    if(!labelStream.is_open())
        D3D_THROW_EXCEPTION("Could not Open Label File")

    vector<label>labelsList;
    string labelName;
    while(labelStream >> labelName)
    {
        label tempLabel;
        tempLabel.name = labelName;

        int val;

        labelStream >> val;
        tempLabel.r = val;

        labelStream >> val;
        tempLabel.g = val;

        labelStream >> val;
        tempLabel.b = val;

        labelsList.push_back(tempLabel);
    }

    labelStream.close();

    int numClasses = labelsList.size();

    // Read images name
    ifstream imageStream;
    imageStream.open(imagesList.c_str());

    if(!imageStream.is_open()){
        D3D_THROW_EXCEPTION("ERROR READING IMAGES LIST");
    }

    vector<string> imageNamesList;
    string imageName;
    while(imageStream >> imageName){
        imageNamesList.push_back(imageName);
    }

    imageStream.close();

    int numImg = imageNamesList.size();
    float avgVal = 1.0f / (float)numImg;

    // Begin processing the scores files from Darwin Library
    for(int i = 0 ; i < numImg ; i++)
    {
        imageName = imageNamesList[i];

        cout << "Process " << imageName << "..." << endl;

        // Open the image
        cv::Mat img = cv::imread(imageName.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        int width  = img.cols;
        int height = img.rows;

        // Initialize segmentation grid (one hot encoding)
        D3D::Grid<float> segmentation(numClasses, width, height, 0.0f);

        // Initialize color image for visualization
        cv::Mat colorImgSeg(height, width, CV_8UC3, cv::Scalar(0,0,0));

        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++)
            {
                // Read label at pixel (y,x)
                int currLabel = img.at<unsigned char>(y,x);

                // If unlabeled region
                if(currLabel == numClasses){
                    for(int c = 0 ; c < numClasses ; ++c){
                        segmentation(c, x, y) = avgVal;
                    }
                    continue;
                }

                // Update segmentation
                segmentation(currLabel, x, y) = 1.0f;

                // Update color image
                colorImgSeg.at<unsigned char>(y,3*x  ) = labelsList[currLabel].b;
                colorImgSeg.at<unsigned char>(y,3*x+1) = labelsList[currLabel].g;
                colorImgSeg.at<unsigned char>(y,3*x+2) = labelsList[currLabel].r;
            }
        }

        // write out the image
        string baseName = myExtractBaseFileName(imageName);

        stringstream outputFileName;
        outputFileName << outputFolder << "/" << baseName << ".png";
        imwrite(outputFileName.str().c_str(), colorImgSeg);

        // write out scores grid
        stringstream scoresGridFileName;
        scoresGridFileName << outputFolder << "/" << baseName << ".dat";
        segmentation.saveAsDataFile(scoresGridFileName.str());
    }

    return 0;
}
