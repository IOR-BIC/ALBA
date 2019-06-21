/*=========================================================================

Program: ALBA
Module: vtkALBAVolumeOrthoSlicer
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBAVolumeOrthoSlicer - makes a voxel projection of structured points dataset along axis directions

// .SECTION Description
// vtkALBAVolumeOrthoSlicer makes a voxel projection of structured points dataset 
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

#ifndef __vtkALBAProjectRG_h
#define __vtkALBAProjectRG_h

#include "vtkDataSetToDataSetFilter.h"
#include "albaConfigure.h"

#define ORTHOSLICER_X_SLICE 0
#define ORTHOSLICER_Y_SLICE 1
#define ORTHOSLICER_Z_SLICE 2

class vtkDataArray;
class vtkImageData;

/**
Class Name: vtkALBAVolumeOrthoSlicer.
 vtkALBAVolumeOrthoSlicer makes a voxel projection of structured points dataset 
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
class ALBA_EXPORT vtkALBAVolumeOrthoSlicer : public vtkDataSetToDataSetFilter
{
public:
  /** RTTI Macro */
  vtkTypeRevisionMacro(vtkALBAVolumeOrthoSlicer, vtkDataSetToDataSetFilter);
  
  /** Static Function for object instantiation */
  static vtkALBAVolumeOrthoSlicer *New();

	void SetPlaneOrigin(double *origin);

  /** Set Projection Direction to X */
  void SetSlicingModeToX()
    {this->SetSclicingMode(ORTHOSLICER_X_SLICE);};
 
  /** Set Projection Direction to Y */
  void SetSlicingModeToY()
    {this->SetSclicingMode(ORTHOSLICER_Y_SLICE);};
 
  /** Set Projection Direction to Z */
  void SetSlicingModeToZ()
    {this->SetSclicingMode(ORTHOSLICER_Z_SLICE);};
 
  /** Retrieve the direction of projection as string */
  char *GetProjectionModeAsString(void)
  {
     switch (this->SclicingMode) {
        case ORTHOSLICER_X_SLICE: return "X";
        case ORTHOSLICER_Y_SLICE: return "Y";
     }
     return "Z";
  };

	/** Updated function to avoid extent propagation */
	virtual void PropagateUpdateExtent(vtkDataObject *output);

  /** Macro for Set Projection Mode */
  vtkSetMacro(SclicingMode,int);
  /** Macro for Get Projection Mode */
  vtkGetMacro(SclicingMode,int);

protected:
  /** constructor */
  vtkALBAVolumeOrthoSlicer();
  /** destructor */
	~vtkALBAVolumeOrthoSlicer() {};
 /** copy constructor not implemented*/
  vtkALBAVolumeOrthoSlicer(const vtkALBAVolumeOrthoSlicer&);
  /** assign operator not implemented*/
  void operator=(const vtkALBAVolumeOrthoSlicer&);

  /** Update dimensions and whole extents */
  void ExecuteInformation();
  /** Execute the projection and fill output scalars */
  void Execute();

	template<typename DataType>
	void SliceScalars(int * inputDims, DataType * inputScalars, DataType * slicedScalars);

	void GenerateOutputFromID(vtkImageData * inputSP, int * projectedDims, vtkDataArray * projScalars);

	void GenerateOutputFromRG(vtkRectilinearGrid * inputRG, int * projectedDims, vtkDataArray * projScalars);
		
	int  SclicingMode;
	double Origin[3];

private:
	void GetSlicingInfo(int* plane1, int* plane2, double* ratio1, double* ratio2);
};


#endif




