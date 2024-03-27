/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAVolumeSlicer
 Authors: Alexander Savenko, Josef Kohout, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBAVolumeSlicer - a filter for creating slices from structured volume data (rectilinear grid or structured points)
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

#ifndef __vtkALBAVolumeSlicer_h
#define __vtkALBAVolumeSlicer_h

#include "albaConfigure.h"
#include "vtkDataSetAlgorithm.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"


//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkRectilinearGrid;
class vtkLinearTransform;
#ifdef _WIN32
//RELEASE NOTE: BES - 4.4.2008 - if you include <wx/string.h> (included by GPUOGL.h) here,
//VS 2008 C++ compiler will produce an incorrect code (compiler BUG), 
//when GetPlaneOrigin() is called from albaPipeVolumeSlice, function GetPlaneOrigin(double data[3])
//is called instead of it
class albaGPU3DTextureProviderHelper;
#endif


class ALBA_EXPORT vtkALBAVolumeSlicer : public vtkDataSetAlgorithm {
public:
  static vtkALBAVolumeSlicer *New();
  vtkTypeMacro(vtkALBAVolumeSlicer, vtkDataSetAlgorithm);


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
  (S - (L - W/2))/ W where S is the scalar value, L is the level and W is the window.
  BES: It is used nowhere, to be removed */
  vtkSetMacro( Window, double );
  vtkGetMacro( Window, double );

  /**
  Set / Get the Level to use -> modulation will be performed on the 
  color based on (S - (L - W/2))/W where S is the scalar value, L is
  the level and W is the window.
  BES: It is used nowhere, to be removed */
  vtkSetMacro( Level, double );
  vtkGetMacro( Level, double );

  /** Set/get auto-spacing feature. 
  In this mode the image spacing is selected automatically to fit the whole slice*/
  vtkSetMacro( AutoSpacing, int );
  vtkGetMacro( AutoSpacing, int );

	/** Set / Get Output Spacing*/
	vtkGetVectorMacro(OutputSpacing, double, 3);
	vtkSetVector3Macro(OutputSpacing, double);

  /** Set/get whether GPU should be used for slicing. 
  If GPU processing is enabled and it is available for the given input/output on the 
  current computer platform, it will be used instead of CPU. Currently, only gray-scale
  and RGB regular grids (vtkImageData) inputs/outputs are supported.
  GPU processing is enabled by the default. 
  NB. GPU processing is faster but may introduce some errors due to single precision */
  //vtkSetMacro( GPUEnabled, int );
  void SetGPUEnabled(int enable);
  vtkGetMacro( GPUEnabled, int );

  /** Set tri-linear interpolation to on */
  void SetTrilinearInterpolationOn(){m_TriLinearInterpolationOn = true;};
  
  /** Set tri-linear interpolation to off */
  void SetTrilinearInterpolationOff(){m_TriLinearInterpolationOn = false;};
  
  /** Set tri-linear interpolation */
  void SetTrilinearInterpolation(bool on){m_TriLinearInterpolationOn = on;};

	/**
  specify the image to be used for texturing output polydata object*/
  void SetTexture(vtkImageData *data) {this->SetInputData(1, data);};
	void SetTextureConnection(vtkAlgorithmOutput *connection) {this->SetInputConnection(1, connection);};
	 
  vtkImageData *GetTexture() { 
    return vtkImageData::SafeDownCast(this->GetInputDataObject(1,0));
  };

  /** 
  Transform slicer plane according to the given transformation before slicing.*/
  void SetSliceTransform(vtkLinearTransform *trans);

	void SetOutputType(char *vtkType);

	void SetOutputTypeToImageData();

	void SetOutputTypeToPolyData();

protected:
  vtkALBAVolumeSlicer();
  ~vtkALBAVolumeSlicer();

  /** Return this object's modified time. */  
  /*virtual*/ vtkMTimeType GetMTime();

  /** By default copy the output update extent to the input. */
	int RequestUpdateExtent( vtkInformation *request, vtkInformationVector **inputVector,	vtkInformationVector *outputVector);


  /** 
  By default, UpdateInformation calls this method to copy information
  unmodified from the input to the output.*/
  /*virtual*/int RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  /**
  This method is the one that should be used by subclasses, right now the 
  default implementation is to call the backwards compatibility method */
	/*virtual*/	int RequestData(vtkInformation *request,	vtkInformationVector **inputVector,	vtkInformationVector *outputVector);

  /** Create geometry for the slice. */
  virtual void RequestData(vtkInformation *outInfo,vtkPolyData *output);

  /** Create texture for the slice. */
  virtual void RequestData(vtkInformation *outInfo,vtkImageData *output);


  /** Prepares internal data structure for the given input data.
  This routine is called from ExecuteData to prepare data structures used in further computation.
  Calls appropriate PrepareVolume methods
  NB: if data structures prepared in previous call are still valid, they will be used. */
  virtual void PrepareVolume(vtkDataSet* input, vtkImageData* output);

  /** Prepares internal data structure for the regular grid data.
  This routine is called from ExecuteData to prepare data structures used in further computation.
  NB: if data structures prepared in previous call are still valid, they will be used. */
  virtual void PrepareVolume(vtkImageData* input, vtkImageData* output);

  /** Prepares internal data structure for the rectilinear grid data.
  This routine is called from ExecuteData to prepare data structures used in further computation.
  NB: if data structures prepared in previous call are still valid, they will be used. */
  virtual void PrepareVolume(vtkRectilinearGrid* input, vtkImageData* output);

protected:  
  /** BES: 15.12.2008 - when using mafOpCrop in mafViewOrthoSlice, the input
  dimensions change between ExecuteInformation and ExecuteData 
  This routine is supposed to be called from ExecuteData and it fixes this problem */
  void RequestDataHotFix(vtkInformation *request,	vtkInformationVector **inputVector,	vtkInformationVector *outputVector);

  /** Calculates the coordinates for the given point and texture denoted by its size and spacing.
  Texture is considered to have an origin at GlobalPlaneOrigin, to be oriented according to GlobalPlaneAxisX
  and GlobalPlaneAxisY and to cover area of size*spacing mm. Actually, this routine computes intersection of
  the line going through the given point and having vector GlobalPlaneAxisY with the line going through
  the origin of texture and having vector GlobalPlaneAxisY. The computed times are stored in ts.*/
  void CalculateTextureCoordinates(const float point[3], const int size[2], const double spacing[2], float ts[2]);

  /** Creates sampling table for the given coordinates 
  NB. coordinates must be zero aligned (i.e., the first coordinate must be 0,0,0)*/
  void CreateSamplingTable(float* voxelcoords[3]);


  /** Slices voxels from input producing image in output. */
  template<typename InputDataType> 
  void CreateImage(const InputDataType *input, vtkImageData *outputObject);

  /** Slices voxels from input producing image in output. */
  template<typename InputDataType, typename OutputDataType> 
  void CreateImage(const InputDataType *input, OutputDataType *output, vtkImageData *outputObject);

	/** specialize output information type */
	virtual int FillOutputPortInformation(int port, vtkInformation* info);

	char OutputVtkType[100];

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

  int AutoSpacing;
  int GPUEnabled;         //<Non-zero if GPU processing should be used whenever it is possible

  // look-up tables and caches
  vtkTimeStamp PreprocessingTime;
  
  double DataOrigin[3];
  double DataBounds[3][2];
  int    DataDimensions[3];
  double SamplingTableMultiplier[3];  
	
	//output generation
	int			OutputDimentions[3];
	double	OutputSpacing[3];
  
	//look-up table that maps fine samples to voxel indices - see CreateImage  
  int* StIndices[3];
  float* StOffsets[3];

  //last data set processed in PrepareVolume  
  vtkDataSet* LastPreprocessedInput;  
  bool BNoIntersection;       //<true, if there is no intersection of the data bounding box and cutting plane

  int LastGPUEnabled; //<to reflect GPU Enable/Disable change

#ifdef _WIN32
  bool m_bGPUProcessing;        //<true, if GPU processing will be used in ExecuteData
  albaGPU3DTextureProviderHelper *m_TextureHelper;
  
 
  bool m_TriLinearInterpolationOn; //<define if tri-linear interpolation is performed or not on slice's texture
#endif  

private:
  vtkALBAVolumeSlicer(const vtkALBAVolumeSlicer&);  // Not implemented.
  void operator=(const vtkALBAVolumeSlicer&);  // Not implemented.
};
#endif
