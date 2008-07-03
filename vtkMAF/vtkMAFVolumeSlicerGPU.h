/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: vtkMAFVolumeSlicerGPU.h,v $
Language:  C++
Date:      $Date: 2008-07-03 11:27:45 $
Version:   $Revision: 1.1 $
Authors:   Alexander Savenko, Josef Kohout
Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
Copyright (c) 2002 
CINECA - Interuniversity Consortium (www.cineca.it)
v. Magnanelli 6/3
40033 Casalecchio di Reno (BO)
Italy
ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

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

#include "vtkMAFConfigure.h"
#include "vtkDataSetToDataSetFilter.h"
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
//when GetPlaneOrigin() is called from mafPipeVolumeSlice, function GetPlaneOrigin(double data[3])
//is called instead of it
class mafGPUOGL;
#endif


class VTK_vtkMAF_EXPORT vtkMAFVolumeSlicer : public vtkDataSetToDataSetFilter {
public:
  static vtkMAFVolumeSlicer *New();
  vtkTypeRevisionMacro(vtkMAFVolumeSlicer, vtkDataSetToDataSetFilter);

#pragma region Attributes
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

#pragma deprecated(GetWindow, SetWindow)
#pragma deprecated(GetLevel, SetLevel)
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

  /** Set/get whether GPU should be used for slicing. 
  If GPU processing is enabled and it is available for the given input/output on the 
  current computer platform, it will be used instead of CPU. Currently, only gray-scale
  and RGB regular grids (vtkImageData) inputs where data type is signed or unsigned char
  are supported. Only outputs with signed or unsigned char data type are supported.
  GPU processing is enabled by the default. 
  NB. GPU processing is faster but may introduce some errors due to single precision */
  vtkSetMacro( GPUEnabled, int );
  vtkGetMacro( GPUEnabled, int );
#pragma endregion Attributes


  void SetOutput(vtkImageData *data) { 
    vtkDataSetSource::SetOutput(data); 
  }
  
  void SetOutput(vtkPolyData  *data) { 
    vtkDataSetSource::SetOutput(data); 
  }

  /**
  specify the image to be used for texturing output polydata object*/
  void SetTexture(vtkImageData *data) {
    this->SetNthInput(1, (vtkDataObject*)data);
  };
  vtkImageData *GetTexture() { 
    return vtkImageData::SafeDownCast(this->Inputs[1]);
  };

  /** 
  Transform slicer plane according to the given transformation before slicing.*/
  void SetSliceTransform(vtkLinearTransform *trans);

protected:
  vtkMAFVolumeSlicer();
  ~vtkMAFVolumeSlicer();

  /** Return this object's modified time. */  
  /*virtual*/ unsigned long int GetMTime();

  /** By default copy the output update extent to the input. */
  /*virtual*/ void ComputeInputUpdateExtents(vtkDataObject *output);

  /** 
  By default, UpdateInformation calls this method to copy information
  unmodified from the input to the output.*/
  /*virtual*/void ExecuteInformation();

  /**
  This method is the one that should be used by subclasses, right now the 
  default implementation is to call the backwards compatibility method */
  /*virtual*/void ExecuteData(vtkDataObject *output);

  /** Create geometry for the slice. */
  virtual void ExecuteData(vtkPolyData *output);

  /** Create texture for the slice. */
  virtual void ExecuteData(vtkImageData *output);


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
  template<typename InputDataType, typename OutputDataType> 
  void CreateImage(const InputDataType *input, OutputDataType *output, vtkImageData *outputObject);

#ifdef _WIN32
  /** Slices voxels from input producing image in output using GPU. */  
  void CreateImageGPU(const void* input, void* output, vtkImageData *outputObject);

  /** Creates GPU provider, shaders, etc. 
  Returns false, if GPU provider could not be created */
  bool CreateGPUProvider();
#endif

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

  //look-up table that maps fine samples to voxel indices - see CreateImage  
  int* m_StIndices[3];
  float* m_StOffsets[3];

  //last data set processed in PrepareVolume  
  vtkDataSet* m_LastPreprocessedInput;  
  bool m_bNoIntersection;       //<true, if there is no intersection of the data bounding box and cutting plane

#ifdef _WIN32
  bool m_bGPUProcessing;        //<true, if GPU processing will be used in ExecuteData
  mafGPUOGL* m_pGPUProvider;    //<GPU provider for GPU computation
  int m_TextureId;           //<Texture representing the input data

  float m_GPUDataDimensions[3]; //<area covered by input data (in mm)
#endif  

private:
  vtkMAFVolumeSlicer(const vtkMAFVolumeSlicer&);  // Not implemented.
  void operator=(const vtkMAFVolumeSlicer&);  // Not implemented.
};
#endif
