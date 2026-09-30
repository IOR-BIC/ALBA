/*=========================================================================

Program: ALBA
Module: vtkALBARGtoSPImageFilter
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBARGtoSPImageFilter - makes a voxel projection of structured points dataset along axis directions

// .SECTION Description
// vtkALBARGtoSPImageFilter makes a voxel projection of structured points dataset 
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

#ifndef __vtkALBARGtoSPImageFilter_h
#define __vtkALBARGtoSPImageFilter_h

#include "vtkDataSetAlgorithm.h"
#include "albaConfigure.h"

#define ORTHOSLICER_X_SLICE 0
#define ORTHOSLICER_Y_SLICE 1
#define ORTHOSLICER_Z_SLICE 2

class vtkDataArray;
class vtkImageData;

/**
Class Name: vtkALBARGtoSPImageFilter.
 vtkALBARGtoSPImageFilter makes a voxel projection of structured points dataset 
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
class ALBA_EXPORT vtkALBARGtoSPImageFilter : public vtkDataSetAlgorithm
{
public:
  /** RTTI Macro */
  vtkTypeMacro(vtkALBARGtoSPImageFilter, vtkDataSetAlgorithm);
  
  /** Static Function for object instantiation */
  static vtkALBARGtoSPImageFilter *New();

protected:
  /** constructor */
  vtkALBARGtoSPImageFilter();
  /** destructor */
	~vtkALBARGtoSPImageFilter() {};
		
	/** specialize output information type */
	virtual int FillOutputPortInformation(int port, vtkInformation* info);
		
 /** copy constructor not implemented*/
  vtkALBARGtoSPImageFilter(const vtkALBARGtoSPImageFilter&);
  /** assign operator not implemented*/
  void operator=(const vtkALBARGtoSPImageFilter&);

  /** Update dimensions and whole extents */
	int RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  /** Execute the projection and fill output scalars */
	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	template<typename DataType>
	void FillSP(vtkRectilinearGrid * input, vtkImageData * output, DataType *inputScalars, DataType *outScalars);

	template<typename DataType, typename CoordDataType>
	void FillSP(vtkRectilinearGrid * input, vtkImageData * output, DataType *inputScalars, DataType *outScalars, CoordDataType *xCoord, CoordDataType *yCoord);


	/** Gets the best spacing for Rectilinear Grid probing*/
	void GetBestSpacing(vtkRectilinearGrid* rGrid, double * bestSpacing);

private:

	int OutputExtent[6];
};


#endif




