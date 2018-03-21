#ifndef MULTICLASSGRID_H
#define MULTICLASSGRID_H

#include "grid.h"
#include <iostream>
#include "ioTools.h"

using namespace std;

template <typename Elem>
class MultiClassGrid : public D3D::Grid<Elem>
{
public:
    MultiClassGrid(unsigned int numClasses, unsigned int xDim, unsigned int yDim, unsigned int zDim, const Elem& value) :
        D3D::Grid<Elem>(numClasses*xDim, yDim, zDim, value)
    {
        _numClasses = numClasses;
        _xDimMC = xDim;
        _yDimMC = yDim;
        _zDimMC = zDim;
    }

    const Elem& operator ()(unsigned int i, unsigned int x, unsigned int y, unsigned int z) const
    {
        return this->_cells[z*this->_xyDim + y*this->_xDim + x*_numClasses + i];
    }

    Elem& operator ()(unsigned int i, unsigned int x, unsigned int y, unsigned int z)
    {
        return this->_cells[z*this->_xyDim + y*this->_xDim + x*_numClasses + i];
    }

    unsigned int whichLabel(unsigned int x, unsigned int y, unsigned int z);

    void saveAsDataFile(const std::string& fileName);
    void loadFromDataFile(const std::string& fileName);

    int getXDim()
    {
        return _xDimMC;
    }

    int getYDim()
    {
        return _yDimMC;
    }

    int getZDim()
    {
        return _zDimMC;
    }

protected:
    unsigned int _numClasses;
    unsigned int _xDimMC;
    unsigned int _yDimMC;
    unsigned int _zDimMC;
};

template <typename Elem>
void MultiClassGrid<Elem>::saveAsDataFile(const std::string& fileName)
{
    if (CHAR_BIT != 8)
    {
        D3D_THROW_EXCEPTION("Only platforms with 8 bit chars are supported.")
    }

    std::ofstream outStream;
    outStream.open(fileName.c_str(), std::ios::out | std::ios::binary);

    if (!outStream.is_open())
    {
        D3D_THROW_EXCEPTION("Could not open grid data output file for writing.")
    }

    // file format version, might be useful at some point
    unsigned char version = 1;
    outStream.write((char*)&version, 1);

    // endianness
    unsigned char endian = D3D::is_little_endian() ? 0 : 1;
    outStream.write((char*)&endian, 1);

    // store sizes of data types written
    // first unsigned int in an unsigned char because we know that char has always size 1
    unsigned char uintSize = sizeof(unsigned int);
    outStream.write((char*)&uintSize, 1);

    // for Elem we use unsigned int
    unsigned int elemSize = sizeof(Elem);
    outStream.write((char*)&elemSize, sizeof(unsigned int));

    // now we store the size of the grid
    outStream.write((char*)&_numClasses, sizeof(unsigned int));
    outStream.write((char*)&_xDimMC, sizeof(unsigned int));
    outStream.write((char*)&_yDimMC, sizeof(unsigned int));
    outStream.write((char*)&_zDimMC, sizeof(unsigned int));

    // now grid data is written
    outStream.write((char*)this->getDataPtr(), sizeof(Elem)*this->getNbVoxels());

    if (!outStream.good())
    {
        D3D_THROW_EXCEPTION("An error occured while writing the grid to a data file.")
    }

    // writing is done closing stream
    outStream.close();
}

template <typename Elem>
void MultiClassGrid<Elem>::loadFromDataFile(const std::string& fileName)
{
    if (CHAR_BIT != 8)
    {
        D3D_THROW_EXCEPTION("Only platforms with 8 bit chars are supported.")
    }

    std::ifstream inStream;
    inStream.open(fileName.c_str(), std::ios::in | std::ios::binary);

    if (!inStream.is_open())
    {
        D3D_THROW_EXCEPTION("Could not open grid data input file.")
    }

    // read in version
    unsigned char version;
    inStream.read((char*)&version, 1);
    if (version != 1)
    {
        D3D_THROW_EXCEPTION("Only version 1 is supported.")
    }

    // read in endian
    unsigned char endian;
    inStream.read((char*)&endian, 1);

    unsigned char currentEndian = D3D::is_little_endian() ? 0: 1;
    if (endian != currentEndian)
    {
        D3D_THROW_EXCEPTION("Current platform does not have the same endian as the depht map data file.")
    }

    // read in the size of an unsigned int from file
    unsigned char uintSize;
    inStream.read((char*)&uintSize, 1);

    // check if current plattform has the same unsigned int size
    if (uintSize != sizeof (unsigned int))
    {
        D3D_THROW_EXCEPTION("Current platform does not have the same unsigned int size as the one the file was written with.")
    }

    unsigned int elemSize;
    inStream.read((char*)&elemSize, sizeof(unsigned int));
    if (elemSize != sizeof(Elem))
    {
        D3D_THROW_EXCEPTION("Size of the datatype stored in the grid does not match with the one from the file.")
    }

    // read the grid size
    unsigned int numClasses, width, height, depth;
    inStream.read((char*)&numClasses, sizeof(unsigned int));
    inStream.read((char*)&width, sizeof(unsigned int));
    inStream.read((char*)&height, sizeof(unsigned int));
    inStream.read((char*)&depth, sizeof(unsigned int));

    // resize the grid
    this->resize(numClasses*width, height, depth);

    _numClasses = numClasses;
    _xDimMC = width;
    _yDimMC = height;
    _zDimMC = depth;

    // load the data stored in the grid
    inStream.read((char*)this->getDataPtr(), sizeof(Elem)*this->getNbVoxels());

    // check stream
    if (!inStream.good())
    {
        D3D_THROW_EXCEPTION("Error while loading the grid from the data file")
    }

    inStream.close();
}

template<typename Elem>
unsigned int MultiClassGrid<Elem>::whichLabel(unsigned int x, unsigned int y, unsigned int z){
    unsigned int label = 0;

    for(int c = 0 ; c < _numClasses ; c++){
        if(this->operator ()(c,x,y,z) > this->operator ()(label,x,y,z) )
            label = c;
    }

    return label;
}

#endif // MULTICLASSGRID_H





























