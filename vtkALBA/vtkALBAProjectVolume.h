/*=========================================================================

Program: ALBA
Module: vtkALBAVolumeSlicer
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBAProjectVolume - makes a voxel projection of structured points dataset along axis directions

// .SECTION Description
// vtkALBAProjectVolume makes a voxel projection of structured points dataset 
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

#define VTK_PROJECT_FROM_X 1
#define VTK_PROJECT_FROM_Y 2
#define VTK_PROJECT_FROM_Z 3

class vtkDataArray;
class vtkImageData;

/**
Class Name: vtkALBAProjectVolume.
 vtkALBAProjectVolume makes a voxel projection of structured points dataset 
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
class ALBA_EXPORT vtkALBAProjectVolume : public vtkDataSetToDataSetFilter
{
public:
  /** RTTI Macro */
  vtkTypeRevisionMacro(vtkALBAProjectVolume, vtkDataSetToDataSetFilter);
  
  /** Static Function for object instantiation */
  static vtkALBAProjectVolume *New();

  /** Set Projection Direction to X */
  void SetProjectionModeToX()
    {this->SetProjectionMode(VTK_PROJECT_FROM_X);};
 
  /** Set Projection Direction to Y */
  void SetProjectionModeToY()
    {this->SetProjectionMode(VTK_PROJECT_FROM_Y);};
 
  /** Set Projection Direction to Z */
  void SetProjectionModeToZ()
    {this->SetProjectionMode(VTK_PROJECT_FROM_Z);};
 
  /** Retrieve the direction of projection as string */
  char *GetProjectionModeAsString(void)
  {
     switch (this->ProjectionMode) {
        case VTK_PROJECT_FROM_X: return "X";
        case VTK_PROJECT_FROM_Y: return "Y";
     }
     return "Z";
  };

	/** Updated function to avoid extent propagation */
	virtual void PropagateUpdateExtent(vtkDataObject *output);

  /** Macro for Set Projection Mode */
  vtkSetMacro(ProjectionMode,int);
  /** Macro for Get Projection Mode */
  vtkGetMacro(ProjectionMode,int);

	/** Macro for Set Project Sub Range status */
	vtkSetMacro(ProjectSubRange, bool);
	/** Macro for Get Project Sub Range status*/
	vtkGetMacro(ProjectSubRange, bool);
	/** On/Off Macro for Project Sub Range status*/
	vtkBooleanMacro(ProjectSubRange, bool);

	/** Macro for Set Projection Range*/
	vtkSetVector2Macro(ProjectionRange, int);
	/** Macro for Get Projection Range*/
	vtkGetVector2Macro(ProjectionRange, int);

protected:
  /** constructor */
  vtkALBAProjectVolume();
  /** destructor */
	~vtkALBAProjectVolume() {};
 /** copy constructor not implemented*/
  vtkALBAProjectVolume(const vtkALBAProjectVolume&);
  /** assign operator not implemented*/
  void operator=(const vtkALBAProjectVolume&);

  /** Update dimensions and whole extents */
  void ExecuteInformation();
  /** Execute the projection and fill output scalars */
  void Execute();

	template<typename DataType>
	void ProjectScalars(int * inputDims, DataType * inputScalars, DataType * projScalars);

	void GenerateOutputFromID(vtkImageData * inputSP, int * projectedDims, vtkDataArray * projScalars);

	void GenerateOutputFromRG(vtkRectilinearGrid * inputRG, int * projectedDims, vtkDataArray * projScalars);

	int  ProjectionMode;
	bool ProjectSubRange;
	int  ProjectionRange[2];
};


#endif




