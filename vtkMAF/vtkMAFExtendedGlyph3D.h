/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFExtendedGlyph3D.h,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:27:45 $
  Version:   $Revision: 1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFExtendedGlyph3D - copy oriented and scaled glyph geometry to every input point
// .SECTION Description
// vtkMAFExtendedGlyph3D is an extended version of vtkGlyph3D supporting visibility by scalar, 
// i.e. the glyph is visualized only if scalar is != 0. Beware, if visibility is ON, scaling and 
// color by scalar are ignored.

// .SECTION See Also
// vtkTensorGlyph vtkGlyph3D

#ifndef __vtkMAFExtendedGlyph3D_h
#define __vtkMAFExtendedGlyph3D_h

#include "vtkDataSetToPolyDataFilter.h"
#include "vtkMAFConfigure.h"

#define VTK_SCALE_BY_SCALAR 0
#define VTK_SCALE_BY_VECTOR 1
#define VTK_SCALE_BY_VECTORCOMPONENTS 2
#define VTK_DATA_SCALING_OFF 3

#define VTK_COLOR_BY_SCALE  0
#define VTK_COLOR_BY_SCALAR 1
#define VTK_COLOR_BY_VECTOR 2

#define VTK_USE_VECTOR 0
#define VTK_USE_NORMAL 1
#define VTK_VECTOR_ROTATION_OFF 2

#define VTK_INDEXING_OFF 0
#define VTK_INDEXING_BY_SCALAR 1
#define VTK_INDEXING_BY_VECTOR 2

class VTK_vtkMAF_EXPORT vtkMAFExtendedGlyph3D : public vtkDataSetToPolyDataFilter
{
public:
  vtkTypeRevisionMacro(vtkMAFExtendedGlyph3D,vtkDataSetToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
  Construct object with scaling on, scaling mode is by scalar value, 
  scale factor = 1.0, the range is (0,1), orient geometry is on, and
  orientation is by vector. Clamping and indexing are turned off. No
  initial sources are defined.*/
  static vtkMAFExtendedGlyph3D *New();

  /**
  Get the number of source objects used to define the glyph
  table. Specify the number of sources before defining a table of glyphs.*/
  void SetNumberOfSources(int num);
  int GetNumberOfSources();

  /**
  Set the source to use for he glyph.*/
  void SetSource(vtkPolyData *pd) {this->SetSource(0,pd);};

  /**
  Specify a source object at a specified table location.*/
  void SetSource(int id, vtkPolyData *pd);

  /**
  Get a pointer to a source object at a specified table location.*/
  vtkPolyData *GetSource(int id=0);

  /**
  Turn on/off scaling of source geometry.*/
  vtkSetMacro(Scaling,int);
  vtkBooleanMacro(Scaling,int);
  vtkGetMacro(Scaling,int);

  /**
  Either scale by scalar or by vector/normal magnitude.*/
  vtkSetMacro(ScaleMode,int);
  vtkGetMacro(ScaleMode,int);
  void SetScaleModeToScaleByScalar() 
    {this->SetScaleMode(VTK_SCALE_BY_SCALAR);};
  void SetScaleModeToScaleByVector() 
    {this->SetScaleMode(VTK_SCALE_BY_VECTOR);};
  void SetScaleModeToScaleByVectorComponents()
    {this->SetScaleMode(VTK_SCALE_BY_VECTORCOMPONENTS);};
  void SetScaleModeToDataScalingOff()
    {this->SetScaleMode(VTK_DATA_SCALING_OFF);};
  const char *GetScaleModeAsString();

  /**
  Either color by scale, scalar or by vector/normal magnitude.*/
  vtkSetMacro(ColorMode,int);
  vtkGetMacro(ColorMode,int);
  void SetColorModeToColorByScale() 
    {this->SetColorMode(VTK_COLOR_BY_SCALE);};
  void SetColorModeToColorByScalar() 
    {this->SetColorMode(VTK_COLOR_BY_SCALAR);};
  void SetColorModeToColorByVector() 
    {this->SetColorMode(VTK_COLOR_BY_VECTOR);};
  const char *GetColorModeAsString();

  /**
  Specify scale factor to scale object by.*/
  vtkSetMacro(ScaleFactor,double);
  vtkGetMacro(ScaleFactor,double);

  /**
  Specify range to map scalar values into.*/
  vtkSetVector2Macro(Range,double);
  vtkGetVectorMacro(Range,double,2);

  /**
  Turn on/off orienting of input geometry along vector/normal.*/
  vtkSetMacro(Orient,int);
  vtkBooleanMacro(Orient,int);
  vtkGetMacro(Orient,int);

  /**
  Turn on/off clamping of "scalar" values to range. (Scalar value may be 
   vector magnitude if ScaleByVector() is enabled.)*/
  vtkSetMacro(Clamping,int);
  vtkBooleanMacro(Clamping,int);
  vtkGetMacro(Clamping,int);

  /**
  Specify whether to use vector or normal to perform vector operations.*/
  vtkSetMacro(VectorMode,int);
  vtkGetMacro(VectorMode,int);
  void SetVectorModeToUseVector() {this->SetVectorMode(VTK_USE_VECTOR);};
  void SetVectorModeToUseNormal() {this->SetVectorMode(VTK_USE_NORMAL);};
  void SetVectorModeToVectorRotationOff() 
    {this->SetVectorMode(VTK_VECTOR_ROTATION_OFF);};
  const char *GetVectorModeAsString();

  /**
  Index into table of sources by scalar, by vector/normal magnitude, or
  no indexing. If indexing is turned off, then the first source glyph in
  the table of glyphs is used.*/
  vtkSetMacro(IndexMode,int);
  vtkGetMacro(IndexMode,int);
  void SetIndexModeToScalar() {this->SetIndexMode(VTK_INDEXING_BY_SCALAR);};
  void SetIndexModeToVector() {this->SetIndexMode(VTK_INDEXING_BY_VECTOR);};
  void SetIndexModeToOff() {this->SetIndexMode(VTK_INDEXING_OFF);};
  const char *GetIndexModeAsString();

  /**
  Enable/disable the generation of point ids as part of the output. The
  point ids are the id of the input generating point. The point ids are
  stored in the output point field data and named "InputPointIds". Point
  generation is useful for debugging and pick operations.*/
  vtkSetMacro(GeneratePointIds,int);
  vtkGetMacro(GeneratePointIds,int);
  vtkBooleanMacro(GeneratePointIds,int);

  /**
  Set/Get the name of the PointIds array if generated. By default the Ids
  are named "InputPointIds", but this can be changed with this function.*/
  vtkSetStringMacro(PointIdsName);
  vtkGetStringMacro(PointIdsName);

  /**
  Turn on/off scaling of source geometry.*/
  vtkSetMacro(ScalarVisibility,int);
  vtkBooleanMacro(ScalarVisibility,int);
  vtkGetMacro(ScalarVisibility,int);

protected:
  vtkMAFExtendedGlyph3D();
  ~vtkMAFExtendedGlyph3D();

  void Execute();
  void ExecuteInformation();
  void ComputeInputUpdateExtents(vtkDataObject *output);

  int NumberOfSources; // Number of source objects
  vtkPolyData **Source; // Geometry to copy to each point
  int Scaling; // Determine whether scaling of geometry is performed
  int ScaleMode; // Scale by scalar value or vector magnitude
  int ColorMode; // new scalars based on scale, scalar or vector
  double ScaleFactor; // Scale factor to use to scale geometry
  double Range[2]; // Range to use to perform scalar scaling
  int Orient; // boolean controls whether to "orient" data
  int VectorMode; // Orient/scale via normal or via vector data
  int Clamping; // whether to clamp scale factor
  int IndexMode; // what to use to index into glyph table
  int GeneratePointIds; // produce input points ids for each output point
  char *PointIdsName;
  int ScalarVisibility;

  char *InputScalarsSelection;
  char *InputVectorsSelection;
  char *InputNormalsSelection;
  vtkSetStringMacro(InputScalarsSelection);
  vtkSetStringMacro(InputVectorsSelection);
  vtkSetStringMacro(InputNormalsSelection);

private:
  vtkMAFExtendedGlyph3D(const vtkMAFExtendedGlyph3D&);  // Not implemented.
  void operator=(const vtkMAFExtendedGlyph3D&);  // Not implemented.
};

/**
Return the method of scaling as a descriptive character string.*/
inline const char *vtkMAFExtendedGlyph3D::GetScaleModeAsString(void)
{
  if ( this->ScaleMode == VTK_SCALE_BY_SCALAR )
    {
    return "ScaleByScalar";
    }
  else if ( this->ScaleMode == VTK_SCALE_BY_VECTOR ) 
    {
    return "ScaleByVector";
    }
  else 
    {
    return "DataScalingOff";
    }
}

/**
Return the method of coloring as a descriptive character string.*/
inline const char *vtkMAFExtendedGlyph3D::GetColorModeAsString(void)
{
  if ( this->ColorMode == VTK_COLOR_BY_SCALAR )
    {
    return "ColorByScalar";
    }
  else if ( this->ColorMode == VTK_COLOR_BY_VECTOR ) 
    {
    return "ColorByVector";
    }
  else 
    {
    return "ColorByScale";
    }
}

/**
Return the vector mode as a character string.*/
inline const char *vtkMAFExtendedGlyph3D::GetVectorModeAsString(void)
{
  if ( this->VectorMode == VTK_USE_VECTOR) 
    {
    return "UseVector";
    }
  else if ( this->VectorMode == VTK_USE_NORMAL) 
    {
    return "UseNormal";
    }
  else 
    {
    return "VectorRotationOff";
    }
}

/**
Return the index mode as a character string.*/
inline const char *vtkMAFExtendedGlyph3D::GetIndexModeAsString(void)
{
  if ( this->IndexMode == VTK_INDEXING_OFF) 
    {
    return "IndexingOff";
    }
  else if ( this->IndexMode == VTK_INDEXING_BY_SCALAR) 
    {
    return "IndexingByScalar";
    }
  else 
    {
    return "IndexingByVector";
    }
}

#endif
