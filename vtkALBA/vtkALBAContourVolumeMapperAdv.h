/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAContourVolumeMapperAdv
 Authors: Alexander Savenko, Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/* DESCRIPTION
vtkALBAContourVolumeMapperAdv - a mapper for direct rendering of isosurfaces &
a filter for extracting them.

vtkALBAContourVolumeMapperAdv performs a direct rendering of isosurfaces and 
extracts them as polydata objects if requested.
The input for the class can be either structured points (vtkImageData) or 
rectilinear grids (vtkRectilinearGrids). The optional output is vtkPolyData object.
This class can produce two kinds of surfaces: a standard isosurface and a 
simplified one that is obtained by skipping some of the voxels.
It is not guaranteed that the simplified model will have the same topology as the original one.
This mapper uses a special representation (min-max blocks) of the volume to speed up rendering. 
Additional acceleration is achieved by avoiding recalculation of vertices 
when the same edge is processed again during next iteration. 
This also solves the problem of locating identical points - the Achilles' heel 
of the standard VTK implementation of Marching Cubes algorithm.
*/

/* SEE ALSO
vtkVolumeMapper vtkContourFilter vtkMarchingCubes
*/

/* CONTENTS
namespace vtkALBAContourVolumeMapperAdvNamespace
class vtkALBAContourVolumeMapperAdv
class Polyline2DAdv
class ListOfPolyline2DAdv
*/

/* PROGRAM FLOW
First call EstimateRelevantVolume()
Then Render()
*/

#ifndef __vtkALBAContourVolumeMapperAdv_h
#define __vtkALBAContourVolumeMapperAdv_h

#include <vector>

#include "vtkALBAContourVolumeMapper.h"

#include "albaConfigure.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class Idepth ;
class Polyline2DAdv;
class ListOfPolyline2DAdv;

/**
class name: vtkALBAContourVolumeMapperAdv.
*/
class ALBA_EXPORT vtkALBAContourVolumeMapperAdv : public vtkALBAContourVolumeMapper 
{
public:
  /** create an instance of the object */
  static vtkALBAContourVolumeMapperAdv *New();
  /** RTTI Macro */
  vtkTypeMacro(vtkALBAContourVolumeMapperAdv, vtkALBAContourVolumeMapper);

protected:
  /** constructor */
  vtkALBAContourVolumeMapperAdv();
  /** destructor */
  ~vtkALBAContourVolumeMapperAdv();

  /** prepare contours */
  template <typename DataType> void PrepareContoursTemplate(const int slice, const DataType *imageData);
  /** call prepare contours template */
  void PrepareContours(const int slice, const void *imageData, ListOfPolyline2DAdv&);

private:
  /** Copy Constructor , not implemented.*/
  vtkALBAContourVolumeMapperAdv(const vtkALBAContourVolumeMapperAdv&);
  /** operator =, not implemented.*/
  void operator=(const vtkALBAContourVolumeMapperAdv&);

  // min-max block structure
  int            NumBlocks[3];
  void          *BlockMinMax; // min - 0, max - 1

  // vertices of voxel cube
  int           VoxelVertIndicesOffsets[NumberOfLods][8];   // voxel cube as index offsets in the data array

  // parameters of the mapper
  float          ContourValue;           ///< current contour value
  int            AutoLODRender;          ///< enable level of detail when rendering
  int            AutoLODCreate;          ///< enable level of detail when extracting polydata
  int            EnableContourAnalysis;  ///< shall we optimize the surface?

  // to set the alpha parameter
  double         Alpha;
  double         MAXScalar;

  // volume info
  double         DataOrigin[3];
  double         DataSpacing[3];
  int            DataDimensions[3];
  float*         VoxelCoordinates[3];

  // statistics
  int            VoxelsRendered;
  int            VoxelsSkipped;
  float          SkippedVoxelBlocks;                // 0 - 100 (%)
  float          TimeToDrawRMC[NumberOfLods] ;      // time to draw with RenderMCubes()
  float          TimeToDrawDC[NumberOfLods] ;       // time to draw with DrawCache()
  int            NumberOfTriangles[NumberOfLods];   // no. of triangles  
  int            CreatedTriangles;                  // valid when CreateMCubes is running
  float          TimePerTriangle ;                  // running mean of time per triangle

  // caching
  bool           CacheCreated ;                     // flag indicating that cache has been created
  float          PrevContourValue ;                 // last contour value
  float         *TriangleCache[NumberOfLods];       // Triangle caches for each level of detail
  unsigned int   TriangleCacheSize[NumberOfLods];   // in triangles (there are 6 floats per vertex, 18 floats per triangle)

  // sorting triangles
  unsigned int *OrderedVertices[NumberOfLods] ;    // indices of vertices in sort order

  // helping objects
  float          ViewportDimensions[2];
  vtkMatrix4x4  *TransformMatrix;
  vtkMatrix4x4  *VolumeMatrix;

  ListOfPolyline2DAdv *Polylines; // This should be a parameter of a method but VStudio have problems with template argument in template function.
};


/**
class name: Polyline2DAdv
these classes are used for optimizing the surface by analyzing 2D contours
*/
class Polyline2DAdv 
{
public:
  /**
  struct name:  Point
  2D point with operators for ==, [], const [] and ()
  */
  struct Point 
  {
    short xy[2];
    /** redefine operator== */
    bool operator ==(const Point& operand) const 
    {
#ifdef WIN32
      return *((int*)xy) == *((int*)operand.xy);
#else
      return xy[0] == operand.xy[0] && xy[1] == operand.xy[1];
#endif
    }
    /** redefine operator[] */
    short& operator[](int index) { return xy[index]; }
    /** redefine operator[] const*/
    const short& operator[](int index) const { return xy[index]; }
    /** redefine operator() */
    const short* operator()() const { return this->xy; }
  };

  // vertices
  int    start;
  int    end;
  Point *vertices;

  mutable short bbox[4]; // bounding box
  mutable bool  updateBoundingBox;

  float minDistance[2];
  int   closestPolyline[2];

public:
  /** constructor */
  Polyline2DAdv(const Point *line);
  /** destructor */
  ~Polyline2DAdv() { if (this->vertices != this->verticesBuffer) delete [] vertices; }

  /** get length*/
  int  Length() const   { return this->end -this->start + 1; }
  /** check if polyline is closed  */
  bool IsClosed() const { return (this->vertices[this->start] == this->vertices[this->end]); }

  /** Add point and create next line*/
  bool AddNextLine(const Point *line);
  /** merge two polylines */
  bool Merge(Polyline2DAdv &polyline);
  /** close polyline */
  void Close();

  /** update current bounding box */
  void UpdateBoundingBox() const;

  /** check if one polyline is inside another  */
  bool IsInsideOf(const Polyline2DAdv *polyline) const;
  /** find the closest polyline */
  void FindClosestPolyline(int index, int numOfPolylines, Polyline2DAdv* polylines);
  /** compare polylines and find the best match */
  int  FindSubPolyline(int numOfPolylines, Polyline2DAdv* polylines, float &minDistance);
  /** create two polylines splitting the original*/
  bool SplitPolyline(Polyline2DAdv& subpoly, Polyline2DAdv& newpoly);

  /** if vertices are different, copy all members from parameter polyline */
  void Move(Polyline2DAdv &polyline);

protected:
  int size;
  /** allocation of polyline vertices*/
  void Allocate(int newsize);
  /** reallocation of polyline vertices*/
  void Reallocate();

#define VERTICES_BUFFER_SIZE 64
  Point verticesBuffer[VERTICES_BUFFER_SIZE];
};


/**
Class Name:ListOfPolyline2DAdv.
This is a std::vector of polyline pointers, with 3 extra functions.
*/
class ListOfPolyline2DAdv : public std::vector<Polyline2DAdv*> 
{
public:
  /** Clear the list of polylines. */
  void clear();
  /** check if point is inside the contour */
  bool IsInside(int x, int y, int polylineLengthThreshold);
  /** retrieve contour polyline */
  Polyline2DAdv *FindContour(int x, int y, int polylineLengthThreshold, int distance = 1);
};

#endif


