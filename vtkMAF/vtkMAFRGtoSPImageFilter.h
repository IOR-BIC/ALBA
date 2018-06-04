/*=========================================================================

Program: MAF2
Module: vtkMAFRGtoSPImageFilter
Authors: Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFRGtoSPImageFilter - makes a voxel projection of structured points dataset along axis directions

// .SECTION Description
// vtkMAFRGtoSPImageFilter makes a voxel projection of structured points dataset 
// along axis directions considering opacity.  The output of
// this filter is a structured points dataset. The filter treats input data
// of any topological dimension (i.e., point, line, image, or volume) and 
// generates image output data.
//
// To use this filter set the projection mode  ivar. 
// specify a rectangular region in the data. (Note that these are 0-offset.)
// You can also specify a sampling rate to subsample the data.
//
// Typical applications of this filter are to produce an image from a volume
// for image processing or visualization.


// .SECTION See Also
// vtkGeometryFilter vtkExtractGeometry vtkExtractGrid

#ifndef __vtkMAFRGtoSPImageFilter_h
#define __vtkMAFRGtoSPImageFilter_h

#include "vtkDataSetToDataSetFilter.h"
#include "mafConfigure.h"

#define ORTHOSLICER_X_SLICE 0
#define ORTHOSLICER_Y_SLICE 1
#define ORTHOSLICER_Z_SLICE 2

class vtkDataArray;
class vtkImageData;

/**
Class Name: vtkMAFRGtoSPImageFilter.
 vtkMAFRGtoSPImageFilter makes a voxel projection of structured points dataset 
 along axis directions considering opacity.  The output of
 this filter is a structured points dataset. The filter treats input data
 of any topological dimension (i.e., point, line, image, or volume) and 
 generates image output data.

 To use this filter set the projection mode  ivar. 
 specify a rectangular region in the data. (Note that these are 0-offset.)
 You can also specify a sampling rate to subsample the data.

 Typical applications of this filter are to produce an image from a volume
 for image processing or visualization.
*/
class MAF_EXPORT vtkMAFRGtoSPImageFilter : public vtkDataSetToDataSetFilter
{
public:
  /** RTTI Macro */
  vtkTypeRevisionMacro(vtkMAFRGtoSPImageFilter, vtkDataSetToDataSetFilter);
  
  /** Static Function for object instantiation */
  static vtkMAFRGtoSPImageFilter *New();

	/** Updated function to avoid extent propagation */
	virtual void PropagateUpdateExtent(vtkDataObject *output);

protected:
  /** constructor */
  vtkMAFRGtoSPImageFilter();
  /** destructor */
	~vtkMAFRGtoSPImageFilter() {};
 /** copy constructor not implemented*/
  vtkMAFRGtoSPImageFilter(const vtkMAFRGtoSPImageFilter&);
  /** assign operator not implemented*/
  void operator=(const vtkMAFRGtoSPImageFilter&);

  /** Update dimensions and whole extents */
  void ExecuteInformation();
  /** Execute the projection and fill output scalars */
  void Execute();

	template<typename DataType>
	void FillSP(vtkRectilinearGrid * input, vtkImageData * output, DataType *inputScalars, DataType *outScalars);

	template<typename DataType, typename CoordDataType>
	void FillSP(vtkRectilinearGrid * input, vtkImageData * output, DataType *inputScalars, DataType *outScalars, CoordDataType *xCoord, CoordDataType *yCoord);


	/** Gets the best spacing for Rectilinear Grid probing*/
	void GetBestSpacing(vtkRectilinearGrid* rGrid, double * bestSpacing);

private:
};


#endif




