/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAVolumeResample
 Authors: Alexander Savenko
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBAVolumeResample- a filter for creating slices from structured volume data (rectilinear grid or structured points)
//
// .SECTION Description
// Inputs of the filter are:
//   volume (vtkRectilinearGrid or vtkImageData)
//   plane definition: origin and two vectors that define the axes of the image rectangle
//   color mapping: window and level of color mapping
//   
// Outputs of the filter are:
//   image (vtkImageData): The resolution and spacing of the output volume image should be specified by consumer. 
//                         The origin will be automatically set to zero.
//   polyline slice (vtkPolyData): This output includes both the polyline that define the cut of a plane and corresponded triangles. The texture coordinates are provided as well.
// .SECTION See Also
// vtkProbeFilter, vtkPlaneSource

#ifndef __vtkALBAVolumeResample_h
#define __vtkALBAVolumeResample_h

#include "albaConfigure.h"
#include "vtkDataSetAlgorithm.h"
#include "vtkImageData.h"

//class vtkImageData;
class vtkRectilinearGrid;


class ALBA_EXPORT vtkALBAVolumeResample: public vtkDataSetAlgorithm  {
public:
  static vtkALBAVolumeResample*New();
  vtkTypeMacro(vtkALBAVolumeResample, vtkDataSetAlgorithm );
  
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
  Specify a point defining the origin of the plane.*/
  vtkSetVector3Macro(VolumeOrigin, double);
  vtkGetVectorMacro(VolumeOrigin, double, 3);

  /**
  Specify x-axis of the plane*/
  void SetVolumeAxisX(double axis[3]);
  vtkGetVectorMacro(VolumeAxisX, double, 3);

  /**
  Specify x-axis of the plane*/
  void SetVolumeAxisY(double axis[3]);
  vtkGetVectorMacro(VolumeAxisY, double, 3);

  /**
  Set / Get the Window for color modulation. The formula for modulation is 
  (S - (L - W/2))/ W where S is the scalar value, L is the level and W is the window.*/
  vtkSetMacro( Window, double );
  vtkGetMacro( Window, double );
  
  /**
  Set / Get the Level to use -> modulation will be performed on the 
  color based on (S - (L - W/2))/W where S is the scalar value, L is
  the level and W is the window.*/
  vtkSetMacro( Level, double );
  vtkGetMacro( Level, double );

  /**
  Set/Get the value used for padding empty areas*/
  vtkSetMacro( ZeroValue, double );
  vtkGetMacro( ZeroValue, double );
  

  /**
  Set/get auto-spacing feature. In this mode the image spacing is selected automatically to fit the whole slice*/
  vtkSetMacro( AutoSpacing, int );
  vtkGetMacro( AutoSpacing, int );
  vtkBooleanMacro(AutoSpacing, int );

	/**
	Specify output Spacing.*/
	vtkSetVector3Macro(OutputSpacing, double);
	vtkGetVectorMacro(OutputSpacing, double, 3);

	/**
	Specify output Extent*/
	vtkSetVector6Macro(OutputExtent, int);
	vtkGetVectorMacro(OutputExtent, int, 6);

protected:
  vtkALBAVolumeResample();
  ~vtkALBAVolumeResample();

  int RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);
  int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector );
  
  void RequestData(vtkInformation* request, vtkImageData *output);

	int RequestUpdateExtent( vtkInformation *request, vtkInformationVector **inputVector,	vtkInformationVector *outputVector);


  void PrepareVolume();
  void CalculateTextureCoordinates(const double point[3], const int size[2], const double spacing[2], double ts[2]);

	/** specialize output information type */
	virtual int FillOutputPortInformation(int port, vtkInformation* info);

	template<typename InputDataType> void CreateImage(const InputDataType *input, vtkDataArray *outputScalars, vtkImageData *outputObject);

  template<typename InputDataType, typename OutputDataType> void CreateImage(const InputDataType *input, OutputDataType *output, vtkImageData *outputObject);

  // plane coordinates
  double VolumeOrigin[3];
  double VolumeAxisX[3];
  double VolumeAxisY[3];
  double VolumeAxisZ[3];

  double ZeroValue;

  // color mapping
  double Window;
  double Level;

  int   AutoSpacing;

  // look-up tables and caches
  vtkTimeStamp PreprocessingTime;

  double*       VoxelCoordinates[3];
  double        DataOrigin[3];
  double        DataBounds[3][2];
  int           DataDimensions[3];
  double        SamplingTableMultiplier[3];

	//Output settings
	double OutputSpacing[3];
	int OutputExtent[6];

private:
  vtkALBAVolumeResample(const vtkALBAVolumeResample&);  // Not implemented.
  void operator=(const vtkALBAVolumeResample&);  // Not implemented.
};

#endif

