#ifndef MULTICLASSFULLTRANSTIONGRID_H
#define MULTICLASSFULLTRANSTIONGRID_H

#include <d3d_base/grid.h>


template <typename Elem>
class MultiClassFullTransitionGrid : public D3D::Grid<Elem>
{
public:
    MultiClassFullTransitionGrid(unsigned int numClasses, unsigned int xDim, unsigned int yDim, unsigned int zDim, const Elem& value);

    const Elem& operator()(unsigned int i, unsigned int j, unsigned int x, unsigned int y, unsigned int z) const;
    Elem& operator()(unsigned int i, unsigned int j, unsigned int x, unsigned int y, unsigned int z);

protected:
    unsigned int _numClasses;
    unsigned int _numClasses2;
};

template <typename Elem>
MultiClassFullTransitionGrid<Elem>::MultiClassFullTransitionGrid(unsigned int numClasses, unsigned int xDim, unsigned int yDim, unsigned int zDim, const Elem& value) : D3D::Grid<Elem>(numClasses*numClasses*xDim, yDim, zDim, value)
{
    _numClasses = numClasses;
    _numClasses2 = numClasses*numClasses;
}

template <typename Elem>
const Elem& MultiClassFullTransitionGrid<Elem>::operator ()(unsigned int i, unsigned int j, unsigned int x, unsigned int y, unsigned int z) const
{
    return this->_cells[z*this->_xyDim + y*this->_xDim + x*_numClasses2 + i*_numClasses + j];
}

template <typename Elem>
Elem& MultiClassFullTransitionGrid<Elem>::operator ()(unsigned int i, unsigned int j, unsigned int x, unsigned int y, unsigned int z)
{
    return this->_cells[z*this->_xyDim + y*this->_xDim + x*_numClasses2 + i*_numClasses + j];
}


#endif // MULTICLASSFULLTRANSTIONGRID_H

