/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkContourVolumeMapper.h,v $
  Language:  C++
  Date:      $Date: 
  Version:   $Revision: 


Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names
   of any contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
// .NAME vtkContourVolumeMapper - a mapper for direct rendering of isosurfaces & a filter for extracting them

// .SECTION Description
// vtkContourVolumeMapper performs a direct rendering of isosurfaces and extracts them as polydata objects if requested.
// The input for the class can be either structured points (vtkImageData) or rectilinear grids (vtkRectilinearGrids). The optional output is vtkPolyData object.
// This class can produce two kinds of surfaces: a standard isosurface and a simplified one that is obtained by skipping some of the voxels.
// It is not guaranteed that the simplified model will have the same topology as the original one.
// This mapper uses a special representation (min-max blocks) of the volume to speed up rendering. 
// Additional acceleration is achieved by avoiding recalculation of vertices when the same edge is processed again during next iteration. 
// This also solves the problem of locating identical points - the Achilles' heel of the standard VTK implementation of Marching Cubes algorithm.

// .SECTION see also
// vtkVolumeMapper vtkContourFilter vtkMarchingCubes

#ifndef __vtkContourVolumeMapper_h
#define __vtkContourVolumeMapper_h

#include <vector>

#include "vtkVolumeMapper.h"
#include "vtkPolyData.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "vtkMAFConfigure.h"

class Polyline2D;
class ListOfPolyline2D;

namespace vtkContourVolumeMapperNamespace {
  static const int VoxelBlockSizeLog = 3; 
  static const int VoxelBlockSize = 1 << VoxelBlockSizeLog; 
  static const int VoxelsInBlock = VoxelBlockSize * VoxelBlockSize * VoxelBlockSize; 
  };

class VTK_vtkMAF_EXPORT vtkContourVolumeMapper : public vtkVolumeMapper {
  public:
    static vtkContourVolumeMapper *New();
    vtkTypeRevisionMacro(vtkContourVolumeMapper, vtkVolumeMapper);

    void PrintSelf( ostream& os, vtkIndent index );
    
    /**
    the input should be either vtkImageData or vtkRectilinearGrid*/
    void  SetInput(vtkDataSet *input);
    vtkDataSet*  GetInput() { return (vtkDataSet*)vtkVolumeMapper::GetInput(); }

    // Render the volume
    void Render(vtkRenderer *ren, vtkVolume *vol);
    
    /**
    Enable or disable multiresolution feature. By default it is enabled*/
    vtkGetMacro(EnableAutoLOD, int);    
    void SetEnableAutoLOD(int val) { this->EnableAutoLOD = val; }
    vtkBooleanMacro(EnableAutoLOD, int);

    /**
    Enable or disables optimization of produced polydata by eliminating "non-visible" enclosed surfaces from the output.*/
    vtkGetMacro(EnableContourAnalysis, int);    
    void SetEnableContourAnalysis(int val) { this->EnableContourAnalysis = val; }
    vtkBooleanMacro(EnableContourAnalysis, int);

    /**
    Set/get the threshold for Marching cubes algorithm*/
    vtkGetMacro(ContourValue, float);    
    void SetContourValue(float val) { if (this->ContourValue != val) { this->ContourValue = val; this->CacheCreated = false; } }

    void Update();

    /**
    Checks if the input data is supported*/
    bool IsDataValid(bool warnings);

    /**
    This class can function both as a mapper and as polydata source. The level parameter controls the 
		resolution of the extracted surface: 0 - original surface, 1- simplified surface.*/
    vtkPolyData *GetOutput(int level = 0, vtkPolyData *data = NULL);

    /**
    Get number of triangles in the extracted surface. This method can be used during extraction too!*/
    unsigned int GetCurrentNumberOfTriangles() const { return this->CreatedTriangles; }
    
    /**
    This method is useful for initial selection of contour value. If the volume is close to 1 than the surface will contain too much noise.*/
    float        EstimateRelevantVolume(const double value);
		
		/**
		To set the value of trasparecy*/
		void SetAlpha(double alpha){m_Alpha=alpha;};

		void SetMaxScalar(double scalar){m_MAXScalar=scalar;};

  protected:
    vtkContourVolumeMapper();
    ~vtkContourVolumeMapper();

    // rendering methods
    template <typename DataType> void  RenderMCubes(vtkRenderer *renderer, vtkVolume *volume, const DataType *dataPointer);
    template <typename DataType> void  CreateMCubes(int level, vtkPolyData *polydata, const DataType *dataPointer);
    template <typename DataType> float EstimateRelevantVolumeTemplate(const DataType ContourValue);
    //template <typename DataType> void RenderMSquares(vtkRenderer *renderer, vtkVolume *volume, const DataType *dataPointer);
    template <typename DataType> bool PrepareAccelerationDataTemplate(const DataType *dataPointer);
    template <typename DataType> void PrepareContoursTemplate(const int slice, const DataType *imageData);
    void PrepareContours(const int slice, const void *imageData, ListOfPolyline2D&);

    void DrawCache(vtkRenderer *renderer, vtkVolume *volume);

    void InitializeRender(bool setup, vtkRenderer *renderer = NULL, vtkVolume *volume = NULL);
    void EnableClipPlanes(bool enable);
  
    int  GetDataType();

    void ReleaseData();

    vtkTimeStamp   BuildTime;

    // statistics
    int            GetPercentageOfSkippedVoxels() const { return 100.f * this->VoxelsSkipped / (this->VoxelsRendered + this->VoxelsSkipped); }
    float          GetPercentageOfSkippedBlocks() const { return this->SkippedVoxelBlocks; }

  private:
    vtkContourVolumeMapper(const vtkContourVolumeMapper&);  // Not implemented.
    void operator=(const vtkContourVolumeMapper&);  // Not implemented.

    // min-max block structure
    int            NumBlocks[3];
    void          *BlockMinMax; // min - 0, max - 1
    int            VoxelVertIndicesOffsets[8];

    // parameters of the mapper
    float          ContourValue;
    int            EnableAutoLOD;          // shall we use multiresolution?
    int            EnableContourAnalysis;  // shall we optimize the surface?
		
		//to set the alpha parameter
		double				 m_Alpha;

		double				 m_MAXScalar;

    // volume info
    double         DataOrigin[3];
    double         DataSpacing[3];
    int            DataDimensions[3];
    float*         VoxelCoordinates[3];

    // statistics
    int            VoxelsRendered;
    int            VoxelsSkipped;
    float          SkippedVoxelBlocks;     // 0 - 100 (%)
    float          TimeToDrawNotOptimized;
    float          TimeToDrawNotOptimizedCache;
    int            CreatedTriangles; // valid when CreateMCubes is running

    // caching
    bool           CacheCreated;
    float          PrevContourValue[2];   // 0 - for normal and 1 - for LOD mode
    unsigned  int  NumberOfTriangles[2];  
    float         *TriangleCache[2];
    int            TriangleCacheSize[2]; // in triangles (i.e. 6 floats per triangle)

    // helping objects
    float          ViewportDimensions[2];
    vtkMatrix4x4  *TransformMatrix;
    vtkMatrix4x4  *VolumeMatrix;

    ListOfPolyline2D *Polylines; // This should be a parameter of a method but VStudio have problems with template argument in template function.
  };


// these classes are used for optimizing the surface by analysing 2D contours
class Polyline2D {
  public:
    
    struct Point {
      short xy[2];
      bool operator ==(const Point& operand) const {
#ifdef WIN32
         return *((int*)xy) == *((int*)operand.xy);
#else
         return xy[0] == operand.xy[0] && xy[1] == operand.xy[1];
#endif
        }
      short& operator[](int index) { return xy[index]; }
      const short& operator[](int index) const { return xy[index]; }

      operator const short*() const { return this->xy; }
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
    
    Polyline2D(const Point *line);
    ~Polyline2D() { if (this->vertices != this->verticesBuffer) delete [] vertices; }
    
    int  Length() const   { return this->end -this->start + 1; }
    bool IsClosed() const { return (this->vertices[this->start] == this->vertices[this->end]); }

    // modifying polyline
    bool AddNextLine(const Point *line);
    bool Merge(Polyline2D &polyline);
    void Close();

    void UpdateBoundingBox() const;

    bool IsInsideOf(const Polyline2D *polyline) const;
    void FindClosestPolyline(int index, int numOfPolylines, Polyline2D* polylines);
    int  FindSubPolyline(int numOfPolylines, Polyline2D* polylines, float &minDistance);
    bool SplitPolyline(Polyline2D& subpoly, Polyline2D& newpoly);

    void Move(Polyline2D &polyline);

  protected:
    int size;

    void Allocate(int newsize);
    void Reallocate();

#define VERTICES_BUFFER_SIZE 64
    Point verticesBuffer[VERTICES_BUFFER_SIZE];
  }; // class


class ListOfPolyline2D : public std::vector<Polyline2D*> {
  public:
    void clear();
    bool IsInside(int x, int y, int polylineLengthThreshold);
    Polyline2D *FindContour(int x, int y, int polylineLengthThreshold, int distance = 1);
  };

#endif


