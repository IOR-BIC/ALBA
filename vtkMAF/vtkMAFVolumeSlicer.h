/*=========================================================================

 Program: MAF2
 Module: vtkMAFVolumeSlicer
 Authors: Alexander Savenko
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFVolumeSlicer - a filter for creating slices from structured volume data (rectilinear grid or structured points)
//
// .SECTION Description
// Inputs of the filter are:
//   volume (vtkRectilinearGrid or vtkImageData)
//   plane definition: origin and two vectors that define the axes of the image rectangle
//   color mapping: window and level of color mapping
//   
// Outputs of the filter are:
//   image (vtkImageData): The resolution and spacing of the image should be specified by consumer. 
//                         The origin will be automatically set to zero.
//   polyline slice (vtkPolyData): This output includes both the polyline that define the cut and corresponded triangles. The texture coordinates are provided as well.
// .SECTION See Also
// vtkProbeFilter, vtkPlaneSource

#ifndef __vtkMAFVolumeSlicer_h
#define __vtkMAFVolumeSlicer_h

#include "mafConfigure.h"
#include "vtkDataSetToDataSetFilter.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkRectilinearGrid;
class vtkLinearTransform;

class MAF_EXPORT vtkMAFVolumeSlicer : public vtkDataSetToDataSetFilter {
public:
  static vtkMAFVolumeSlicer *New();
  vtkTypeRevisionMacro(vtkMAFVolumeSlicer, vtkDataSetToDataSetFilter);

  /**
  Specify a point defining the origin of the plane.*/
//  vtkSetVector3Macro(PlaneOrigin, double);
  void SetPlaneOrigin(double origin[3]);
  void SetPlaneOrigin(double x, double y, double z);
  vtkGetVectorMacro(PlaneOrigin, double, 3);

  /** Specify x-axis of the plane*/
  void SetPlaneAxisX(float axis[3]);
  vtkGetVectorMacro(PlaneAxisX, float, 3);

  /** Specify x-axis of the plane*/
  void SetPlaneAxisY(float axis[3]);
  vtkGetVectorMacro(PlaneAxisY, float, 3);

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
  Set/get auto-spacing feature. In this mode the image spacing is selected automatically to fit the whole slice*/
  vtkSetMacro( AutoSpacing, int );
  vtkGetMacro( AutoSpacing, int );

  void SetOutput(vtkImageData *data) { vtkDataSetSource::SetOutput(data); }
  void SetOutput(vtkPolyData  *data) { vtkDataSetSource::SetOutput(data); }

  /**
  specify the image to be used for texturing output polydata object*/
  void SetTexture(vtkImageData *data) {this->SetNthInput(1, (vtkDataObject*)data);};
  vtkImageData *GetTexture() { return vtkImageData::SafeDownCast(this->Inputs[1]);};

  /** 
  Transform slicer plane according to the given transformation before slicing.*/
  void SetSliceTransform(vtkLinearTransform *trans);

  /** Set tri-linear interpolation to on */
  void SetTrilinearInterpolationOn(){TriLinearInterpolationOn = true;};

  /** Set tri-linear interpolation to off */
  void SetTrilinearInterpolationOff(){TriLinearInterpolationOn = false;};

  /** Set tri-linear interpolation */
  void SetTrilinearInterpolation(bool on){TriLinearInterpolationOn = on;this->ExecuteData(this->GetOutput(0));};

protected:
  vtkMAFVolumeSlicer();
  ~vtkMAFVolumeSlicer();

  unsigned long int GetMTime();

  void ExecuteInformation();
  void ExecuteData(vtkDataObject *output);
  
  // different implementations for polydata and imagedata
  void ExecuteData(vtkPolyData *output);
  void ExecuteData(vtkImageData *output);

  void ComputeInputUpdateExtents(vtkDataObject *output);

  void PrepareVolume();
  void CalculateTextureCoordinates(const float point[3], const int size[2], const double spacing[2], float ts[2]);

  template<typename InputDataType, typename OutputDataType> void CreateImage(const InputDataType *input, OutputDataType *output, vtkImageData *outputObject);

	int   NumComponents;
  // plane coordinates
  double PlaneOrigin[3];
  float PlaneAxisX[3];
  float PlaneAxisY[3];
  float PlaneAxisZ[3];
  
  double GlobalPlaneOrigin[3];
  float GlobalPlaneAxisX[3];
  float GlobalPlaneAxisY[3];
  float GlobalPlaneAxisZ[3];

  vtkLinearTransform *TransformSlice;

  // color mapping
  double Window;
  double Level;

  int   AutoSpacing;

  // look-up tables and caches
  vtkTimeStamp PreprocessingTime;

  float*  VoxelCoordinates[3];
  double  DataOrigin[3];
  double  DataBounds[3][2];
  int     DataDimensions[3];
  double  SamplingTableMultiplier[3];
  bool    TriLinearInterpolationOn;
private:
  vtkMAFVolumeSlicer(const vtkMAFVolumeSlicer&);  // Not implemented.
  void operator=(const vtkMAFVolumeSlicer&);  // Not implemented.
};
#endif
