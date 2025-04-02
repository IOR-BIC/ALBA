/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAContourVolumeMapperGPU
 Authors: Alexander Savenko, Nigel McFarlane, Baoquan Liu (GPU)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/* DESCRIPTION
vtkALBAContourVolumeMapperGPU - a mapper for direct rendering of isosurfaces &
a filter for extracting them.

vtkALBAContourVolumeMapperGPU performs a direct rendering of isosurfaces and 
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
namespace vtkALBAContourVolumeMapperNamespace
class vtkALBAContourVolumeMapperGPU
class Polyline2DGPU
class ListOfPolyline2DGPU
*/

/* PROGRAM FLOW
First call EstimateRelevantVolume()
Then Render()
*/

#ifndef __vtkALBAContourVolumeMapper_h
#define __vtkALBAContourVolumeMapper_h

#include <vector>

#include "vtkVolumeMapper.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <gl/GL.h>
#else
    #include <GL/gl.h>
#endif

#include <vtkOpenGLHelper.h>

#include "albaConfigure.h"
/**
namespace name: Baoquan
*/
// namespace Baoquan
//{

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class Idepth ;
class Polyline2DGPU;
class ListOfPolyline2DGPU;


/**
      namespace name: vtkALBAContourVolumeMapperGPU
*/
//------------------------------------------------------------------------------
namespace vtkALBAContourVolumeMapperNamespace
{
  // Defines block as 8x8x8 cube.  
  // VoxelBlockSizeLog = 3, VoxelBlockSize = 8, VoxelsInBlock = 8^3
  static const int VoxelBlockSizeLog = 3;
  static const int VoxelBlockSize = 1 << VoxelBlockSizeLog;
  static const int VoxelsInBlock = VoxelBlockSize * VoxelBlockSize * VoxelBlockSize;

  // define vertices and edges of a cube
  static const int unitCubeVertsXYZ[8][3] = { {0, 0, 0}, {1, 0, 0}, { 1, 1, 0}, { 0, 1, 0}, {0, 0, 1}, {1, 0, 1}, { 1, 1, 1}, { 0, 1, 1} };
  static const int edgeOffsets[12][2] = { {0, 1}, {1, 2}, {3, 2}, {0, 3}, {4, 5}, {5, 6}, {7, 6}, {4, 7}, {0,4}, {1, 5}, {3, 7}, {2, 6} };
  static const int edgeAxis[12] = { 0, 1, 0, 1, 0, 1, 0, 1, 2, 2, 2, 2 };

  // max no. of triangles before LOD required
  static const int MaxTrianglesNotOptimized = 2000000;

  // approx. ratio of time to draw between DrawCache() and RenderMCubes()
  static const float TimeCacheToMCubesRatio = 0.2;

  // no. of levels of detail allowed (1, 2, 3 or 4)
  static const int NumberOfLods = 4;

  // Empirical constant: approx no. of triangles for every voxel in a block containing contour
  // This is used in RenderMCubes() to calculate the default lod.
  // Too low and RenderMCubes will underestimate the render time, 
  // too high and the rendering will flicker down to low resolution too much.
  // Actual value is about 0.5, but the contour slider is very sticky at this value.
  const float triangles_per_voxel = 2.0;

  // Transparency constant: defines fraction of triangles which are actually sorted on each render.
  // eg if SortFraction = 10, then 1/10 of triangles sorted per render, takes 2*10-1 = 19 renders to complete
  // Should be as large as possible without creating artefacts.
  static const int SortFraction = 10;

};


/*
Class Name: Idepth.
Container type for sorting depth values
*/
class Idepth
{
public:
  float depth ;
  int index ;
  /** redefine < operator in order to check depth variable. */
  bool operator < (const Idepth &b) const {return this->depth < b.depth ;}
};



using namespace vtkALBAContourVolumeMapperNamespace ;

#define EDGE_LIST int

/**
class name: vtkALBAContourVolumeMapperGPU.
*/
  class ALBA_EXPORT vtkALBAContourVolumeMapperGPU : public vtkVolumeMapper 
  {
  public:

    HDC hOldDC;
    HGLRC hOldRC;
    HGLRC hRC;


    //for isosurfaces: 
    DWORD mountVMEM;	
    int  InitOpenGL();
    GLfloat mvMatrix[16];
    GLfloat ProjMatrix[16];
    float m_viewport[4];


    unsigned int				m_tex_function;
    unsigned int	m_hp_tex;
    float m_isovalue_scale;
    int m_hp_size_log2;
    int m_slice_level;
    int m_function_tsize_log2;
    int m_hp_size;

    GLuint m_function_tsize;
    GLuint m_function_slices;

    float spacing[3];
    float ms_center[3];



    std::vector<GLuint>	m_hp_framebuffers;

    GLuint				m_enumerate_vbo;
    unsigned int		m_enumerate_vbo_size;
    GLuint N;


    GLuint m_gpgpu_passthru_v;
    GLuint m_baselevel_f;
    GLuint m_baselevel_p;

    GLuint m_reduce_f;
    GLuint m_reduce_p;
    GLuint m_onscreen_v;
    GLuint m_onscreen_f;
    GLuint m_onscreen_p;

    GLuint m_onscreen_ms_eye;
    GLuint m_onscreen_ms_spacing;
    GLuint m_onscreen_ms_center;


    GLuint				m_reduce_uniform_delta;
    GLuint				m_uniform_onscreen_func_delta;
    GLuint				m_uniform_onscreen_key_off;
    GLuint				m_uniform_onscreen_threshold;
    GLuint				m_uniform_baselevel_threshold;

    //std::string	m_defines;

    GLfloat memN[4*64+16];
    GLfloat memN_Offset[4*64+16];
    GLfloat memN_LOD[8][4*64+16];

    GLint m_old_drawbuffer;

    /** create GPU context */
    bool CreateGPU_Context();
    /** Delete Texture */
    void DeleteTexture();
    /** Create Textures for Isosurface */
    int CreateTexturesForIsosurface(const void *dataPointer);
    /** build  Histopyramid and slice sorting */
    int BuildHistopyramidSliceSorting();
    /** Render isosurface on GPU */
    template <typename DataType> void  RenderIsoSurfaceOnGPU(vtkRenderer *renderer, vtkVolume *volume, const DataType *dataPointer);
    /** pad memory */
    template <typename DataType> void PaddingMem( DataType *dataPointer);
    void * pPading;

    /** initialize isosurface render */
    void InitializeIsosurfaceRender(bool setup, vtkRenderer *renderer = NULL, vtkVolume *volume = NULL);



    /** Set whether GPU should be used for visualization. 
    If GPU processing is enabled and it is available for the given input/output on the 
    current computer platform, it will be used instead of CPU. GPU processing is enabled by the default.  */
    //vtkSetMacro( GPUEnabled, int );
    virtual void SetGPUEnabled(int newMode);
/** Get whether GPU should be used for visualization. 
    If GPU processing is enabled and it is available for the given input/output on the 
    current computer platform, it will be used instead of CPU. GPU processing is enabled by the default.  */
    vtkGetMacro( GPUEnabled, int );

    int m_bGPU_isosurfaceSupported;//test to see if GPU_isosurface is supported or not on current hardware
    int bBuildHp;
    //int bCreated_texture;

    //end lbq
    /** create an instance of the object */
    static vtkALBAContourVolumeMapperGPU *New();
    /** RTTI Macro */
    vtkTypeMacro(vtkALBAContourVolumeMapperGPU, vtkVolumeMapper);
    

    /** The input should be either vtkImageData or vtkRectilinearGrid */
    void  SetInput(vtkDataSet *input);
    /** Get current input*/
		vtkDataSet*  GetInput();

    /** 
    Render the isosurface.
    If data has been cached for this contour value, calls DrawCache()
    Else calls PrepareAccelerationDataTemplate() and RenderMCubes() */
    void Render(vtkRenderer *ren, vtkVolume *vol);

    /** get multi-resolution feature. By default it is enabled  */
    vtkGetMacro(AutoLODRender, int);
    /** set multi-resolution feature. By default it is enabled  */
    void SetAutoLODRender(int val) { this->AutoLODRender = val; }
    /** bool macro,Enable or disable multi-resolution feature. By default it is enabled  */
    vtkBooleanMacro(AutoLODRender, int);

    /** get multi-resolution feature. By default it is enabled  */
    vtkGetMacro(AutoLODCreate, int);
    /** set multi-resolution feature. By default it is enabled  */ 
    void SetAutoLODCreate(int val) { this->AutoLODCreate = val; }
    /** bool macro, Enable or disable multi-resolution feature. By default it is enabled  */
    vtkBooleanMacro(AutoLODCreate, int);

    /** get optimization of produced polydata by eliminating 
    "non-visible" enclosed surfaces from the output.*/
    vtkGetMacro(EnableContourAnalysis, int);
    /** set optimization of produced polydata by eliminating 
    "non-visible" enclosed surfaces from the output.*/   
    void SetEnableContourAnalysis(int val) { this->EnableContourAnalysis = val; }
    /** bool macro, Enable or disables optimization of produced polydata by eliminating 
    "non-visible" enclosed surfaces from the output.*/
    vtkBooleanMacro(EnableContourAnalysis, int);

    /** get the threshold for Marching cubes algorithm */
    vtkGetMacro(ContourValue, float);   
    /** set the threshold for Marching cubes algorithm */ 
    void SetContourValue(float val) { if (this->ContourValue != val) { this->ContourValue = val; this->CacheCreated = false; } }
    /** Update Mapper*/
    void Update();

    /** Checks if the input data is supported */
    bool IsDataValid(bool warnings);

    /**
    This class can function both as a mapper and as polydata source. 
    This function extracts the isosurface as polydata.
    The level parameter controls the resolution of the extracted surface,
    where level=0 is full resolution, 1 is 1/2, 2 is 1/4 and 3 is 1/8
    Allocates polydata if input polydata is NULL */
    vtkPolyData *GetOutput(int level = 0, vtkPolyData *data = NULL);

    /**
    Get number of triangles in the extracted surface. This method can be used during extraction too! */
    unsigned int GetCurrentNumberOfTriangles() const { return this->CreatedTriangles; }

    /**
    This is the first function to be called before Render()
    It is used by albaPipeIsosurface::Create() to set the initial contour to an acceptable value.
    If the value is close to 1 than the surface will contain too much noise.
    Calls EstimateRelevantVolumeTemplate() with correct scalar datatype
    Returns the fraction of blocks which contain the contour. */
    float EstimateRelevantVolume(const double value);

    /** To set the value of transparency */
    void SetAlpha(double alpha){m_Alpha=alpha;};
    /** set maximum value of sclar*/
    void SetMaxScalar(double scalar){m_MAXScalar=scalar;};

    /** Return the index increments in xy and z given the lod index
    For lod = 0,1,2,3... lodxy = 2^n = 1,2,4,8...
    However, the resolution in z, between slice planes, may already be poor, so
    lodz <= lodx such that z resolution is not worse than x resolution. */
    void CalculateLodIncrements(int lod, int *lodxy, int *lodz) const ;

  protected:
    /** constructor */
    vtkALBAContourVolumeMapperGPU();
    /** destructor */
    ~vtkALBAContourVolumeMapperGPU();

    /** Marching cubes algorithm - calculate triangles, cache and render */
    template <typename DataType> void  RenderMCubes(vtkRenderer *renderer, vtkVolume *volume, const DataType *dataPointer);

    /** marching cubes algorithm - calculate triangles and return vtkPolyData */
    template <typename DataType> void  CreateMCubes(int lod, vtkPolyData *polydata, const DataType *dataPointer);

    /** marching cubes algorithm - calculate triangles and return vtkPolyData */
    template <typename DataType> void  CreateMCubes_new(int lod, vtkPolyData *polydata, const DataType *dataPointer);

    /** marching cubes algorithm - calculate triangles and return vtkPolyData */
    template <typename DataType> void  CreateMCubes_old(int lod, vtkPolyData *polydata, const DataType *dataPointer);

    /** template corresponding to EstimateRelevantVolume() */
    template <typename DataType> float EstimateRelevantVolumeTemplate(const DataType ContourValue);

    //template <typename DataType> void RenderMSquares(vtkRenderer *renderer, vtkVolume *volume, const DataType *dataPointer);

    /**
    Set up the acceleration structures.
    Gets data extents.
    Calculates coords of every voxel, and offsets between neighbouring voxels.
    Divides volume into 8x8x8 blocks and calculates the min and max of each block. */
    template <typename DataType> bool PrepareAccelerationDataTemplate(const DataType *dataPointer);

    /** prepare contours */
    template <typename DataType> void PrepareContoursTemplate(const int slice, const DataType *imageData);
    /** call prepare contours template */
    void PrepareContours(const int slice, const void *imageData, ListOfPolyline2DGPU&);

    /** Initialize OpenGL rendering */
    void InitializeRender(bool setup, vtkRenderer *renderer = NULL, vtkVolume *volume = NULL);

    /** OpenGL code to render the triangles */
    void DrawCache(vtkRenderer *renderer, vtkVolume *volume, int lod);
    /** enable/disable clipping planes */
    void EnableClipPlanes(bool enable);
    /** Return datatype of input scalars */
    int  GetDataType();

    /** Delete allocated data */
    void ReleaseData();


    /** Return vertices of voxel cube as offsets to indices in data array */
    void CalculateVoxelVertIndicesOffsets(int lod, int *offset) const ;

    /** Calculate volume of voxel given lod
    useful for estimating times or no. of triangles from one lod to another */
    int VoxelVolume(int lod) const ;

    /** Estimate the number of triangles for the given LOD using fraction of relevant volume.
    EstimateRelevantVolume() is only called the first time the contour value changes,
    after which there is no further time cost. */
    int EstimateTrianglesFromRelevantVolume(int lod) ;

    /** Return an estimate of the expected number of triangles for the given LOD.
    Only use this to help decide whether to render at this LOD or not. */
    int EstimateNumberOfTriangles(int lod) ;

    /** Return an estimate of the time to draw using DrawCache()
    Only use this to help decide whether to render at this LOD or not. */
    float EstimateTimeToDrawDC(int lod) const ;

    /** Return an estimate of the time to draw using RenderMCubes()
    Only use this to help decide whether to render at this LOD or not. */
    float EstimateTimeToDrawRMC(int lod) ;

    /** Return highest resolution (LOD) which DrawCache() can draw
    This only checks time and triangles - not whether cache exists yet
    Used to decide whether to call DrawCache() or RenderMCubes() */
    int BestLODForDrawCache(vtkRenderer *renderer) ;

    /** Return highest resolution (LOD) which RenderMCubes() can draw
    Used to decide which LOD to render */
    int BestLODForRenderMCubes(vtkRenderer *renderer) ;

    /** Return highest resolution (LOD) which CreateMCubes() can extract
    Used to decide which LOD to create */
    int BestLODForCreateMCubes() ;

    /** Free caches and set stats to undefined, eg after contour value changes */
    void ClearCachesAndStats() ;

    /** Calculate matrix required for depth transform
    Return product MP of modelview and projection matrices */
    void CalculateDepthMatrix(double *PM) const ;

    /** calculate depth of vertex from screen */
    float DepthOfVertex(double *PM, float *vertex) const ;

    /** Sort triangles into back-to-front order */
    void SortTriangles(int lod, bool sortall) ;

    /** Useful debugging function to convert data pointer to xyz coords */
    template<typename DataType> const DataType* PointerFromIndices(const DataType *dataPointer, int x, int y, int z) ;

    /** Useful debugging function to convert xyz coords to data pointer */
    template<typename DataType> void IndicesFromPointer(const DataType* dataPointer, const DataType* p, int *x, int *y, int *z) ;


    vtkTimeStamp   BuildTime;

    /** Get Statistic: Percentage of Skipped Voxels */
    int            GetPercentageOfSkippedVoxels() const { return 100.f * this->VoxelsSkipped / (this->VoxelsRendered + this->VoxelsSkipped); }
    /** Get Statistic: Percentage of Skipped Blocks */
    float          GetPercentageOfSkippedBlocks() const { return this->SkippedVoxelBlocks; }

  private:
    /** Copy Constructor , not implemented.*/
    vtkALBAContourVolumeMapperGPU(const vtkALBAContourVolumeMapperGPU&);
    /** operator =, not implemented. */
    void operator=(const vtkALBAContourVolumeMapperGPU&);          // Not implemented.

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
    double         m_Alpha;
    double         m_MAXScalar;

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
    unsigned int *ordered_vertices[NumberOfLods] ;    // indices of vertices in sort order

    // helping objects
    float          ViewportDimensions[2];
    vtkMatrix4x4  *TransformMatrix;
    vtkMatrix4x4  *VolumeMatrix;

    ListOfPolyline2DGPU *Polylines; // This should be a parameter of a method but VStudio have problems with template argument in template function.

    int GPUEnabled;         //<Non-zero if GPU processing should be used whenever it is possible
  };


/**
class name: Polyline2DGPU
these classes are used for optimizing the surface by analyzing 2D contours
*/
  class Polyline2DGPU 
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
    Polyline2DGPU(const Point *line);
    /** destructor */
    ~Polyline2DGPU() { if (this->vertices != this->verticesBuffer) delete [] vertices; }

    /** get length*/
    int  Length() const   { return this->end -this->start + 1; }
    /** check if polyline is closed  */
    bool IsClosed() const { return (this->vertices[this->start] == this->vertices[this->end]); }

    /** Add point and create next line*/
    bool AddNextLine(const Point *line);
    /** merge two polylines */
    bool Merge(Polyline2DGPU &polyline);
    /** close polyline */
    void Close();

    /** update current bounding box */
    void UpdateBoundingBox() const;
    
    /** check if one polyline is inside another  */
    bool IsInsideOf(const Polyline2DGPU *polyline) const;
    /** find the closest polyline */
    void FindClosestPolyline(int index, int numOfPolylines, Polyline2DGPU* polylines);
    /** compare polylines and find the best match */
    int  FindSubPolyline(int numOfPolylines, Polyline2DGPU* polylines, float &minDistance);
    /** create two polylines splitting the original*/
    bool SplitPolyline(Polyline2DGPU& subpoly, Polyline2DGPU& newpoly);

    /** if vertices are different, copy all members from parameter polyline */
    void Move(Polyline2DGPU &polyline);

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
  Class Name:ListOfPolyline2DGPU.
  This is a std::vector of polyline pointers, with 3 extra functions.
*/
  class ListOfPolyline2DGPU : public std::vector<Polyline2DGPU*>
  {
  public:
    /** Clear the list of polylines. */
    void clear();
    /** check if point is inside the contour */
    bool IsInside(int x, int y, int polylineLengthThreshold);
    /** retrieve contour polyline */
    Polyline2DGPU* FindContour(int x, int y, int polylineLengthThreshold, int distance = 1);
  };
//}//end baoquan space
#endif
