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

#ifndef __vtkALBAProjectVolume_h
#define __vtkALBAProjectVolume_h

#include "vtkDataSetAlgorithm.h"
#include "albaConfigure.h"

#define VTK_PROJECT_FROM_X 1
#define VTK_PROJECT_FROM_Y 2
#define VTK_PROJECT_FROM_Z 3

#define VTK_PROJECT_MEAN 1
#define VTK_PROJECT_MAX 2

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
class ALBA_EXPORT vtkALBAProjectVolume : public vtkDataSetAlgorithm
{
public:
	/** RTTI Macro */
  vtkTypeMacro(vtkALBAProjectVolume, vtkDataSetAlgorithm);
  
  /** Static Function for object instantiation */
  static vtkALBAProjectVolume *New();

  /** Set Projection Direction to X */
	void SetProjectionSideToX()
	{
		this->SetProjectionSide(VTK_PROJECT_FROM_X);
	};

	/** Set Projection Direction to Y */
	void SetProjectionSideToY()
	{
		this->SetProjectionSide(VTK_PROJECT_FROM_Y);
	};

	/** Set Projection Direction to Z */
	void SetProjectionSideToZ()
	{
		this->SetProjectionSide(VTK_PROJECT_FROM_Z);
	};

	/** Set Projection Modality to Mean */
	void SetProjectionModalityToMean()
	{
		this->SetProjectionModality(VTK_PROJECT_MEAN);
	};
 
	/** Set Projection Modality to Max */
	void SetProjectionModalityToMax()
	{
		this->SetProjectionModality(VTK_PROJECT_MAX);
	};

  /** Retrieve the direction of projection as string */
  char *GetProjectionSideAsString(void)
  {
     switch (this->ProjectionSide) {
        case VTK_PROJECT_FROM_X: return "X";
        case VTK_PROJECT_FROM_Y: return "Y";
     }
     return "Z";
  };

	/** Updated function to avoid extent propagation */
	virtual void PropagateUpdateExtent(vtkDataObject *output);

  /** Macro for Set Projection Side */
  vtkSetMacro(ProjectionSide,int);
  /** Macro for Get Projection Side */
  vtkGetMacro(ProjectionSide,int);

	/** Macro for Set Projection Modality */
	vtkSetMacro(ProjectionModality, int);
	/** Macro for Get Projection Modality */
	vtkGetMacro(ProjectionModality, int);


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

	/** specialize output information type */
	int FillOutputPortInformation(int port, vtkInformation* info);
	
  /** Update dimensions and whole extents */
	int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  /** Execute the projection and fill output scalars */
	int RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

	template<typename DataType>
	void ProjectScalars(int * inputDims, DataType * inputScalars, DataType * projScalars);

	void GenerateOutputFromID(vtkInformation *request, vtkImageData * inputSP, int * projectedDims, vtkDataArray * projScalars);

	void GenerateOutputFromRG(vtkInformation *request, vtkRectilinearGrid * inputRG, int * projectedDims, vtkDataArray * projScalars);

	int  ProjectionSide;
	int	 ProjectionModality;
	bool ProjectSubRange;
	int  ProjectionRange[2];
};


#endif




