/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADistanceFilter
 Authors: Roberto Gori (Cineca), Alexander Savenko (Luton University)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBADistanceFilter - computes distances from isosurfaces. It can also be used to compute densities.
// .SECTION Description
// vtkALBADistanceFilter is a filter that computes distance vectors
//  at specified point positions from isosurfaces. The filter has two inputs:
// the Input and Source. The Input geometric structure is passed through the
// filter. The vector distances are computed along normals at the Input point positions
// from isosurfaces of defined value obtained by interpolating into the source data using a search step. 
// For example, we can compute distances on a geometry
// (specified as Input) from an isosurface in a volume (Source).

#ifndef __vtkALBADistanceFilter_h
#define __vtkALBADistanceFilter_h

#include "vtkDataSetToDataSetFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

#include "albaConfigure.h"

#define DISTANCE_FILTER_SCALARS_NAME "DistanceFilterScalars"

/**
Class Name:vtkALBADistanceFilter.
vtkALBADistanceFilter is a filter that computes distance vectors
at specified point positions from isosurfaces. The filter has two inputs:
the Input and Source. The Input geometric structure is passed through the
filter. The vector distances are computed along normals at the Input point positions
from isosurfaces of defined value obtained by interpolating into the source data using a search step. 
For example, we can compute distances on a geometry
(specified as Input) from an isosurface in a volume (Source).
*/
class ALBA_EXPORT vtkALBADistanceFilter : public vtkDataSetToDataSetFilter {
  public:
    /** create an instance of the object */
    static vtkALBADistanceFilter *New();
    /** RTTI Macro */
    vtkTypeRevisionMacro(vtkALBADistanceFilter, vtkDataSetToDataSetFilter);
    
    /**   Specify the point locations used to Distance input. Any geometry  can be used.*/
    void SetSource(vtkDataSet *data);
    /** Retireve the Source pointer */
    vtkDataSet *GetSource();
    
    /**   Set Macro, Specify the transformation matrix that should be applied to input points prior to rendering*/
    vtkSetObjectMacro(InputMatrix, vtkMatrix4x4);
    /**  Get Macro,Specify the transformation matrix that should be applied to input points prior to rendering*/
    vtkGetObjectMacro(InputMatrix, vtkMatrix4x4);

    /**  Set Macro, Specify the transformation matrix that should be applied to input points prior to rendering*/
    vtkSetObjectMacro(InputTransform, vtkTransform);
    /**  Get Macro,Specify the transformation matrix that should be applied to input points prior to rendering*/
    vtkGetObjectMacro(InputTransform, vtkTransform);

    /**  Set an isosurface value  */
    vtkSetMacro(Threshold,float);
    /**  Get an isosurface value  */
    vtkGetMacro(Threshold,float);
    
    /**  Set maximum traverse distance. If it is set to VTK_FLOAT_MAX each ray will be traversed unitl it leaves the bounding box of the volume */
    vtkSetMacro(MaxDistance,float);
    /**  Set maximum traverse distance. If it is set to VTK_FLOAT_MAX each ray will be traversed unitl it leaves the bounding box of the volume */
    vtkGetMacro(MaxDistance,float);
    
    /** Set the density for outside points. The default is VTK_FLOAT_MIN*/
    vtkSetMacro(OutOfBoundsDensity,float);
    /** Get the density for outside points. The default is VTK_FLOAT_MIN*/
    vtkGetMacro(OutOfBoundsDensity,float);
    
#define VTK_SCALAR 1
#define VTK_VECTOR 2
    
    /**SetClamp macro, Control the distance mode. If the filter is in density mode than this option is ignored.*/
    vtkSetClampMacro(DistanceMode, int, VTK_SCALAR, VTK_VECTOR);
    /**Get macro, Control the distance mode. If the filter is in density mode than this option is ignored.*/
    vtkGetMacro(DistanceMode, int);
    /** set distance mode to scalar type */
    void SetDistanceModeToScalar() { this->SetDistanceMode(VTK_SCALAR); };
    /** set distance mode to vector type */
    void SetDistanceModeToVector() { this->SetDistanceMode(VTK_VECTOR); };
    /** get distance mode as string */
    char *GetDistanceModeAsString() const { if (this->DistanceMode == VTK_SCALAR) return "Scalar"; else return "Vector"; }

#define VTK_DISTANCE_MODE 1
#define VTK_DENSITY_MODE  2

    /**SetClamp macro,   Switch between density and distance modes (Distance is the default one)*/
    vtkSetClampMacro(FilterMode, int, VTK_DISTANCE_MODE, VTK_DENSITY_MODE);
    /**Getmacro,   Switch between density and distance modes (Distance is the default one)*/
    vtkGetMacro(FilterMode, int);
    /** set filter mode to distance */
    void SetFilterModeToDistance() { this->SetFilterMode(VTK_DISTANCE_MODE); };
    /** set filter mode to density */
    void SetFilterModeToDensity()  { this->SetFilterMode(VTK_DENSITY_MODE); };
    /** get filter mode as string*/
    char *GetFilterModeAsString() const { if (this->FilterMode == VTK_DISTANCE_MODE) return "Distance"; else return "Density"; }

  protected:
    /** constructor */
    vtkALBADistanceFilter();
    /** destructor */
    ~vtkALBADistanceFilter();

    /** get modified time*/
    unsigned long int GetMTime();

    /** execute information*/
    void ExecuteInformation();
    /** execute data*/
    void ExecuteData(vtkDataObject *output);
    /** compute and update extents */
    void ComputeInputUpdateExtents(vtkDataObject *output);

    /**  Prepare special data fast traversing in the volume*/
    void PrepareVolume();

    /**  This is the main function. It traces a ray from a given point until the ray hits the voxel that exceeds Threshold 
    or the traversed distance exceeds MaxDistance values.*/
    template<typename DataType> double TraceRay(const double origin[3], const double ray[3], const DataType *dataPointer);
    /** find density in specific point */
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
    /** Copy Constructor , not implemented */
    vtkALBADistanceFilter(const vtkALBADistanceFilter&);
    /** operator =, not implemented */
    void operator=(const vtkALBADistanceFilter&);
  };

#endif



