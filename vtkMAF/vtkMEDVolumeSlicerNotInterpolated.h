/*=========================================================================

 Program: MAF2Medical
 Module: vtkMEDVolumeSlicerNotInterpolated
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMEDVolumeSlicerNotInterpolated_H__
#define __vtkMEDVolumeSlicerNotInterpolated_H__

#include "vtkImageData.h"
#include "mafConfigure.h"
#include "vtkDataSetToImageFilter.h"
#include "vtkRectilinearGrid.h"

#define MAX_NUMBER_OF_PIECES 20

class vtkDoubleArray;

//----------------------------------------------------------------------------
class MAF_EXPORT vtkMEDVolumeSlicerNotInterpolated : public vtkDataSetToImageFilter
//----------------------------------------------------------------------------
{
public:

  /** Public enumeration for slice axis directions */
  enum SLICE_AXIS_ID
  {
    SLICE_X,
    SLICE_Y,
    SLICE_Z
  };

  /** */
  static vtkMEDVolumeSlicerNotInterpolated *New();

  /** RTTI Macro */
  vtkTypeRevisionMacro(vtkMEDVolumeSlicerNotInterpolated, vtkDataSetToImageFilter);

  /** Set the slicer output */
  inline void SetOutput(vtkImageData * data) {vtkImageSource::SetOutput(data);};

  /** Set the slice origin */
  vtkSetVector3Macro(Origin,double);

  /** Get the slice origin */
  vtkGetVector3Macro(Origin,double);

  /** Set the slice direction */
  vtkSetMacro(SliceAxis,int);

  /** Get the rectilinear grid output */
  vtkGetMacro(OutputRectilinearGrid,vtkRectilinearGrid*);

  /** Get output data type */
  vtkGetMacro(OutputDataType,int);

  /** Get number of pieces */
  vtkGetMacro(NumberOfPieces,int);

protected:

  /** ctor */
  vtkMEDVolumeSlicerNotInterpolated();

  /** dtor */
  ~vtkMEDVolumeSlicerNotInterpolated();

  /** By default, UpdateInformation calls this method to copy information
  unmodified from the input to the output.*/
  virtual void ExecuteInformation();

   /**
   This method is the one that should be used by subclasses, right now the 
   default implementation is to call the backwards compatibility method */
   /*virtual*/void ExecuteData(vtkDataObject *output);

   /** Create geometry for the slice. */
   virtual void ExecuteData(vtkImageData *output);

  double Origin[3];                           //< Origin of the cutting plane
  double SliceOrigin[3];                      //< Origin of the slice
  double Bounds[6];                           //< Bounds of the input volume
  int SliceAxis;                              //< Axis on which slicing is performed (only "perpendicular" slicing is allowed)
  int NumberOfComponents;                     //< Number of scalar components
  int BaseIndex;                              //< Starting point of the slice
  int SliceDimensions[2];                     //< Dimensions of the slice (in number of points)
  int InputDimensions[3];                     //< Input volume dimensions
  double InputSpacing[3];                     //< Input volume spacing
  double SliceSpacing[3];                     //< Output slice spacing
  int InputDataType;                          //< Input data type
  int OutputDataType;                         //< Output data type
  vtkDoubleArray *CoordsXY[2];                //< X and Y coordinates of the slice if input is a rectilinear grid
  vtkRectilinearGrid *OutputRectilinearGrid;  //< Rectilinear grid as output produced if the input is a vtkRectilinearGrid
  int AxisX;                                  //< X axis of the image
  int AxisY;                                  //< Y axis of the image

  // new draft implementation for rectilinear grid
  // in real images often there are pieces of rg that have regular spacing:
  // the rg slicing algorithm decompose rg in pieces that have the same spacing along axis (image data)
  int SlicePieceDimensions[MAX_NUMBER_OF_PIECES][2];         //< Slice Piece dimensions
  double SlicePieceSpacings[MAX_NUMBER_OF_PIECES][2];        //< Slice Piece spacing
  double SlicePieceOrigins[MAX_NUMBER_OF_PIECES][3];         //< Slice Piece spacing
  int NumberOfPieces;
private:

  void AddOutputsAttributes(int dimension, double spacing, int** dimensions, double** spacings, int size);

};
#endif //#ifndef __vtkMEDVolumeSlicerNotInterpolated_H__