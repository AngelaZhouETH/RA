#ifndef MULTICLASSHALFTRANSITIONGRID_H
#define MULTICLASSHALFTRANSITIONGRID_H

#include <d3d_base/grid.h>


template <typename Elem>
class MultiClassHalfTransitionGrid : public D3D::Grid<Elem>
{
public:
    MultiClassHalfTransitionGrid(unsigned int numClasses, unsigned int xDim, unsigned int yDim, unsigned int zDim, const Elem& value);

    const Elem& operator()(unsigned int i, unsigned int j, unsigned int x, unsigned int y, unsigned int z) const;
    Elem& operator()(unsigned int i, unsigned int j, unsigned int x, unsigned int y, unsigned int z);

protected:
    unsigned int _numClasses;
    unsigned int _numElemsPerPos;
};

/*template <typename Elem>
MultiClassHalfTransitionGrid<Elem>::MultiClassHalfTransitionGrid(unsigned int numClasses, unsigned int xDim, unsigned int yDim, unsigned int zDim, const Elem& value) : D3D::Grid<Elem>(numClasses*numClasses*xDim, yDim, zDim, value)
{
    _numClasses = numClasses;
    _numElemsPerPos = (numClasses*(numClasses-1))/2;
}*/

template <typename Elem>
MultiClassHalfTransitionGrid<Elem>::MultiClassHalfTransitionGrid(unsigned int numClasses, unsigned int xDim, unsigned int yDim, unsigned int zDim, const Elem& value) : D3D::Grid<Elem>(xDim*numClasses*(numClasses-1)/2, yDim, zDim, value)
{
    _numClasses = numClasses;
    _numElemsPerPos = (numClasses*(numClasses-1))/2;
}

template <typename Elem>
const Elem& MultiClassHalfTransitionGrid<Elem>::operator ()(unsigned int i, unsigned int j, unsigned int x, unsigned int y, unsigned int z) const
{
//    if (!(i < j))
//    {
//        THROW_EXCEPTION("Only upper triangle can be accessed (i < j)")
//    }
    return this->_cells[z*this->_xyDim + y*this->_xDim + x*_numElemsPerPos + (i*(2*_numClasses - i - 1))/2 + j - (i+1)];
}

template <typename Elem>
Elem& MultiClassHalfTransitionGrid<Elem>::operator ()(unsigned int i, unsigned int j, unsigned int x, unsigned int y, unsigned int z)
{
//    if (!(i < j))
//    {
//        THROW_EXCEPTION("Only upper triangle can be accessed (i < j)")
//    }
    return this->_cells[z*this->_xyDim + y*this->_xDim + x*_numElemsPerPos + (i*(2*_numClasses - i - 1))/2 + j - (i+1)];
}

#endif // MULTICLASSHALFTRANSITIONGRID_H
