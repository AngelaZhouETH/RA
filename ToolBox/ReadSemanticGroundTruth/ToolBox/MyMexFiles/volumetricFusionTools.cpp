#include "volumetricFusionTools.h"
#include "CImg.h"
#include <fstream>
#include "my_exception.h"
#include <vector>

namespace D3D
{
template <typename NumericType>
void saveVolumeAsVRMLMesh(const Grid<NumericType>& volume, NumericType isoValue,
                          const Eigen::Vector3f& minCorner, const Eigen::Vector3f& size,
                          const Eigen::Matrix4f& boxToGlobal, const Eigen::Vector3f& color,
                          std::string fileName, bool displayGlobalAxis, bool append, float transparency)
{
    // extract iso-surface with CImg marching cubes

    // create CImg image
    cimg_library::CImg<NumericType> uCImg(volume.getWidth(), volume.getHeight(), volume.getDepth(), 1);

    for (unsigned int x=0; x < volume.getWidth(); x++) {
        for (unsigned int y=0; y < volume.getHeight(); y++) {
            for (unsigned int z=0; z < volume.getDepth(); z++) {
                uCImg(x,y,z) = volume(x,y,z);
            }
        }
    }

    // run isosurface
    cimg_library::CImgList<unsigned int> faces3d;
    const cimg_library::CImg<float> points3d = uCImg.get_isosurface3d(faces3d,isoValue);

    // write vrml file

    // try to open file
    std::ofstream stream;
    if(append)
        stream.open(fileName.c_str(),std::ios::app);
    else
        stream.open(fileName.c_str(),std::ios::trunc);

    if (!stream.is_open())
    {
        D3D_THROW_EXCEPTION("Error opening vrml file for writing.")
    }

    const float deltaX = size(0)/volume.getWidth();
    const float deltaY = size(1)/volume.getHeight();
    const float deltaZ = size(2)/volume.getDepth();

    // header
    stream << "#VRML V2.0 utf8" << std::endl << std::endl;

    stream << " Shape {" << std::endl;
    stream << " appearance Appearance {" << std::endl;
    stream << " material Material {" << std::endl;
    stream << " diffuseColor " << color(0) << " " << color(1) << " " << color(2) << std::endl;
    stream << " ambientIntensity 0.5" << std::endl;
    stream << " emissiveColor 0.0 0.0 0.0" << std::endl;
    stream << " specularColor 0.0 0.0 0.0" << std::endl;
    stream << " shininess 0.5" << std::endl;
    stream << " transparency "<< transparency << std::endl;
    stream << "}" << std::endl;
    stream << "}" << std::endl;
    stream << "geometry IndexedFaceSet {" << std::endl;

    stream << "coord Coordinate {" << std::endl;
    stream << "point [" << std::endl;

    for (int i=0; i < points3d.width(); i++) {

        // calculate point
        Eigen::Vector4f point;
        point(0) = minCorner(0) + deltaX*points3d(i,0);
        point(1) = minCorner(1) + deltaY*points3d(i,1);
        point(2) = minCorner(2) + deltaZ*points3d(i,2);
        point(3) = 1;

        // rotate point
        point = boxToGlobal*point;

        // write point to file
        stream << point(0) << " " << point(1) << " " << point(2) << "," << std::endl;
    }

    // coordinates
    stream << "]" << std::endl;
    stream << "}" << std::endl;

    stream << "coordIndex" << std::endl;
    stream << "[" << std::endl;

    for (unsigned int i=0; i < faces3d.size(); i++) {
        for (int j=faces3d(i).height()-1; j >=0 ; j--) {
            stream << faces3d(i)(0,j) << " ";
        }
        stream << -1 << std::endl;
    }

    stream << "]" << std::endl;

    stream << "}" << std::endl;
    stream << "}" << std::endl;

    if (displayGlobalAxis)
    {
        stream << "Shape {" << std::endl;
        stream << "geometry IndexedLineSet {" << std::endl;
        stream << "coord Coordinate {" << std::endl;
        stream << "point [" << std::endl;

        float length = std::max(size(0), std::max(size(1), size(2)));
        length /= 4;

        Eigen::Vector3f origin(0,0,0);
        stream << origin(0) << " " << origin(1) << " " << origin(2) << "," << std::endl;

        Eigen::Vector3f pointX(length, 0, 0);
        stream << pointX(0) << " " << pointX(1) << " " << pointX(2) << "," << std::endl;


        Eigen::Vector3f pointY(0, length, 0);
        stream << pointY(0) << " " << pointY(1) << " " << pointY(2) << "," << std::endl;

        Eigen::Vector3f pointZ(0, 0, length);
        stream << pointZ(0) << " " << pointZ(1) << " " << pointZ(2) << "," << std::endl;

        stream << "]" << std::endl;
        stream << "}" << std::endl;

        stream << "color Color {" << std::endl;
        stream << "color [" << std::endl;
        stream << "1.0 0.0 0.0," << std::endl;
        stream << "0.0 1.0 0.0," << std::endl;
        stream << "0.0 0.0 1.0," << std::endl;
        stream << "]" << std::endl;
        stream << "}" << std::endl;

        stream << "colorPerVertex FALSE" << std::endl;

        stream << "coordIndex [" << std::endl;
        stream << "0, 1, -1," << std::endl;
        stream << "0, 2, -1," << std::endl;
        stream << "0, 3, -1," << std::endl;
        stream << "]" << std::endl;

        stream << "colorIndex [" << std::endl;
        stream << "0,1,2" << std::endl;
        stream << "]" << std::endl;

        stream << "}" << std::endl;
        stream << "}" << std::endl;

    }
}

template void saveVolumeAsVRMLMesh<float>(const Grid<float>& volume, float isoValue,
                                          const Eigen::Vector3f& minCorner, const Eigen::Vector3f& size,
                                          const Eigen::Matrix4f& transform, const Eigen::Vector3f& color,
                                          std::string fileName, bool displayAxis, bool append = false,float transparency=0.0);
template void saveVolumeAsVRMLMesh<double>(const Grid<double>& volume, double isoValue,
                                           const Eigen::Vector3f& minCorner, const Eigen::Vector3f& size,
                                           const Eigen::Matrix4f& transform, const Eigen::Vector3f& color,
                                           std::string fileName, bool displayAxis, bool append = false,float transparency=0.0);
}
