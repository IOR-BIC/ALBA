/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAVolumeToClosedSmoothSurface
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAVolumeToClosedSmoothSurface_H__
#define __vtkALBAVolumeToClosedSmoothSurface_H__

#define _WINSOCKAPI_ 

#include "albaConfigure.h"
#include "vtkALBAContourVolumeMapper.h"
#include "vtkMatrix4x4.h"
class vtkImageData;
class vtkRectilinearGrid;
class vtkDataArray;


/** vtkALBAVolumeToClosedSmoothSurface: This filter is an extension of vtkALBAContourVolumeMapper
The original mapper generates an contour surface from a volume, this extension add the possibility
of getting an closed or smoothed surface.
This method does NOT use a fill holes procedure (witch is really slow) and creates a new volume 
whit an 1-voxel border of the input volume to obtain the same effect of fill holes in a faster way
*/
//---------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAVolumeToClosedSmoothSurface : public vtkALBAContourVolumeMapper
//---------------------------------------------------------------------------
{
public:
  
  /** Add collect revision method */
  vtkTypeMacro(vtkALBAVolumeToClosedSmoothSurface,vtkALBAContourVolumeMapper);

  
  /**
  This class can function both as a mapper and as polydata source. 
  This function extracts the isosurface as polydata.
  The level parameter controls the resolution of the extracted surface,
  where level=0 is full resolution, 1 is 1/2, 2 is 1/4 and 3 is 1/8
  Allocates polydata if input polydata is NULL */
  vtkPolyData *GetOutput(int level = 0, vtkPolyData *data = NULL);

  /** Update Mapper */
  void Update();

  /** create an instance of the object */
  static vtkALBAVolumeToClosedSmoothSurface *New();

  /** Return true if fill holes procedure is enabled. */
  vtkGetMacro(FillHoles, int); 
  /** Enable or disables fill holes procedure. */  
  void SetFillHoles(int value){FillHoles=value;};
  /** bool macro, Enable or disables fill holes procedure. */
  vtkBooleanMacro(FillHoles, int);

  /** Return true if output smoothing is enabled. */
  vtkGetMacro(SmoothSurface, int);  
  /** Enable or disables output smoothing. */  
  void SetSmoothSurface(int value){SmoothSurface=value;};
  /** bool macro, Enable or disables output smoothing. */
  vtkBooleanMacro(SmoothSurface, int);

private:

  int FillHoles;
  int SmoothSurface;
  vtkImageData *BorderVolumeID;
  vtkRectilinearGrid *BorderVolumeRG;
  double InputBounds[6];
  double VoxelShift[6];

  /** Default constructor */
  vtkALBAVolumeToClosedSmoothSurface();

  /** Default destructor */
  ~vtkALBAVolumeToClosedSmoothSurface();

  /** Caluclates the scale/traslation to obtain a cube in [-1,1], 
     If toUnity is set to false returns the inverse factors*/
  void GetTransformFactor( int toUnity,double *bounds, double *scale, double *traslation );
};

#endif