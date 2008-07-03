/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFDistanceFilter.h,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:27:45 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Gori (Cineca), Alexander Savenko (Luton University)
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
// .NAME vtkMAFDistanceFilter - computes distances from isosurfaces. It can also be used to compute densities.
// .SECTION Description
// vtkMAFDistanceFilter is a filter that computes distance vectors
//  at specified point positions from isosurfaces. The filter has two inputs:
// the Input and Source. The Input geometric structure is passed through the
// filter. The vector distances are computed along normals at the Input point positions
// from isosurfaces of defined value obtained by interpolating into the source data using a search step. 
// For example, we can compute distances on a geometry
// (specified as Input) from an isosurface in a volume (Source).

#ifndef __vtkMAFDistanceFilter_h
#define __vtkMAFDistanceFilter_h

#include "vtkDataSetToDataSetFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

#include "vtkMAFConfigure.h"

class VTK_vtkMAF_EXPORT vtkMAFDistanceFilter : public vtkDataSetToDataSetFilter {
  public:
    static vtkMAFDistanceFilter *New();
    vtkTypeRevisionMacro(vtkMAFDistanceFilter, vtkDataSetToDataSetFilter);
    
    /**
    Specify the point locations used to Distance input. Any geometry
    can be used.*/
    void SetSource(vtkDataSet *data);
    vtkDataSet *GetSource();
    
    /**
    Specify the transformation matrix that should be applied to input points prior to rendering*/
    vtkSetObjectMacro(InputMatrix, vtkMatrix4x4);
    vtkGetObjectMacro(InputMatrix, vtkMatrix4x4);

    /**
    Specify the transformation that should be applied to input points prior to rendering*/
    vtkSetObjectMacro(InputTransform, vtkTransform);
    vtkGetObjectMacro(InputTransform, vtkTransform);

    /**
    Set/Get an isosurface value  */
    vtkSetMacro(Threshold,float);
    vtkGetMacro(Threshold,float);
    
    /**
    Set/Get maximum traverse distance. If it is set to VTK_FLOAT_MAX each ray will be traversed unitl it leaves the bounding box of the volume */
    vtkSetMacro(MaxDistance,float);
    vtkGetMacro(MaxDistance,float);
    
    /**
    Set/Get the density for outside points. The default is VTK_FLOAT_MIN*/
    vtkSetMacro(OutOfBoundsDensity,float);
    vtkGetMacro(OutOfBoundsDensity,float);
    
#define VTK_SCALAR 1
#define VTK_VECTOR 2
    
    /**
    Control the distance mode. If the filter is in density mode than this option is ignored.*/
    vtkSetClampMacro(DistanceMode, int, VTK_SCALAR, VTK_VECTOR);
    vtkGetMacro(DistanceMode, int);
    void SetDistanceModeToScalar() { this->SetDistanceMode(VTK_SCALAR); };
    void SetDistanceModeToVector() { this->SetDistanceMode(VTK_VECTOR); };
    char *GetDistanceModeAsString() const { if (this->DistanceMode == VTK_SCALAR) return "Scalar"; else return "Vector"; }

#define VTK_DISTANCE_MODE 1
#define VTK_DENSITY_MODE  2

    /**
    Switch between density and distance modes (Distance is the default one)*/
    vtkSetClampMacro(FilterMode, int, VTK_DISTANCE_MODE, VTK_DENSITY_MODE);
    vtkGetMacro(FilterMode, int);
    void SetFilterModeToDistance() { this->SetFilterMode(VTK_DISTANCE_MODE); };
    void SetFilterModeToDensity()  { this->SetFilterMode(VTK_DENSITY_MODE); };
    char *GetFilterModeAsString() const { if (this->FilterMode == VTK_DISTANCE_MODE) return "Distance"; else return "Density"; }

  protected:
    vtkMAFDistanceFilter();
    ~vtkMAFDistanceFilter();

    unsigned long int GetMTime();

    void ExecuteInformation();
    void ExecuteData(vtkDataObject *output);
    void ComputeInputUpdateExtents(vtkDataObject *output);

    /**
    Prepare special data fast traversing in the volume*/
    void PrepareVolume();

    /**
    This is the main function. It traces a ray from a given point until the ray hits the voxel that exceeds Threshold 
    or the traversed distance exceeds MaxDistance values.*/
    template<typename DataType> double TraceRay(const double origin[3], const double ray[3], const DataType *dataPointer);
    template<typename DataType> double FindDensity(const double point[3], const DataType *dataPointer);

    // parameters
    double Threshold; 
    double MaxDistance;
    double OutOfBoundsDensity;
    int   DistanceMode;
    int   FilterMode;

    vtkMatrix4x4 *InputMatrix;
    vtkTransform *InputTransform;

    // caches
    // data-related variables
    vtkTimeStamp   BuildTime;

    float          DataBounds[6];
    int            DataDimensions[3];
    int            InterpolationOffsets[8];
    float         *VoxelSizes[3];
    float         *UniformToRectGridIndex[3];            // used to find indeces in rectalinear grid
    int            UniformToRectGridMaxIndex[3];
    float          UniformToRectGridMultiplier[3];
  
  private:
    vtkMAFDistanceFilter(const vtkMAFDistanceFilter&); // not implemented
    void operator=(const vtkMAFDistanceFilter&);    // not implemented
  };

#endif



