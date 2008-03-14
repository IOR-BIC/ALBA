/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkXRayVolumeMapper.h,v $
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
// .NAME vtkXRayVolumeMapper - class for performing XRAY projections of volume

// .SECTION Description
// vtkXRayVolumeMapper creates XRAY images of a volume using 2D texture mapping


// .SECTION see also
// vtkVolumeMapper vtkVolumeTextureMapper2D

#ifndef __vtkXRayVolumeMapper_h
#define __vtkXRayVolumeMapper_h

#include "vtkVolumeMapper.h"
#include "vtkImageData.h"
#include "vtkCamera.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "vtkMAFConfigure.h"

#define MaxNumberOfMipmapLevels 3
#define MaxNumberOfRenderings   360

class VTK_vtkMAF_EXPORT vtkXRayVolumeMapper : public vtkVolumeMapper {
  public:
    static vtkXRayVolumeMapper *New();
    vtkTypeRevisionMacro(vtkXRayVolumeMapper, vtkVolumeMapper);
    
    // standard vtkVolumeMapper only accept vtkImageData. Overwrite input functions
    void  SetInput(vtkDataSet *input);
    vtkDataSet*  GetInput() { return vtkVolumeMapper::GetInput(); }

    // Render the volume
    virtual void Render(vtkRenderer *ren, vtkVolume *vol);

    // reduce resolution to 9 bit
    static bool GetReduceColorResolution() { return ReduceColorResolution; } 
    static void SetReduceColorResolution(bool val) { ReduceColorResolution = val;}
    static void ReduceColorResolutionOn() { ReduceColorResolution = true;}
    static void ReduceColorResolutionOff() { ReduceColorResolution = false;}
    
    static void GetExposureCorrection(double correction[2]) { correction[0] = ExposureCorrection[0]; correction[1] = ExposureCorrection[1];}
    static const double *GetExposureCorrection() { return ExposureCorrection; }
    static bool SetExposureCorrection(double *val) { if (val[1] <= 1.f && val[1] >= -1.f && val[0] <= 1.f && val[0] >= -1.f) { ExposureCorrection[0] = val[0]; ExposureCorrection[1] = val[1]; return true;} return false; }
    
    static double GetGamma() { return Gamma; }
    static bool SetGamma(double val) { if (val > 3.f && val < 0.1f) return false; Gamma = val; return true; }

    static bool GetPerspectiveCorrection() { return PerspectiveCorrection; } 
    static void SetPerspectiveCorrection(bool val) { PerspectiveCorrection = val;}
    static void PerspectiveCorrectionOn() { PerspectiveCorrection = true;}
    static void PerspectiveCorrectionOff() { PerspectiveCorrection = false;}

    static void GetColor(double color[3]) { color[0] = Color[0]; color[1] = Color[1]; color[2] = Color[2]; }
    static const float *GetColor() { return Color; }
    static void SetColor(double *val) { Color[0] = val[0]; Color[1] = val[1]; Color[2] = val[2];/* no Modified() */ }
    static void SetColor(double r, double g, double b) { Color[0] = r; Color[1] = g; Color[2] = b;/* no Modified() */ }

    static bool GetEnableAutoLOD() { return EnableAutoLOD; } 
    static void SetEnableAutoLOD(bool val) { EnableAutoLOD = val;}
    static void EnableAutoLODOn() { SetEnableAutoLOD(true);}
    static void EnableAutoLODOff() { SetEnableAutoLOD(false);}

    vtkGetMacro( AllocatedTextureMemory, int );    
    vtkGetMacro( PercentageOfResidentTextures, int );

    bool IsDataValid(bool warnings = false); // Update should be called prior to this function
    void Update();

  protected:
    vtkXRayVolumeMapper();
    ~vtkXRayVolumeMapper();

    // rendering methods
    void StartRendering(vtkRenderer *renderer, vtkVolume *volume);
    void FinishRendering(vtkRenderer *renderer);

    void InitializeRender(bool setup);
    void FindMajorAxis(vtkRenderer *renderer, vtkVolume *volume);
    bool PrepareTextures();
    bool CacheScaledVolume();
    void EnableClipPlanes(bool enable);
    int  FindColorResolution();
    unsigned long CalculateChecksum(vtkVolume *volume);
    bool RegisterTexture(int axis, int texture, unsigned char *data);
    void ResetTextures();

        
    void FindExposure(double minValue, double maxValue, double gamma = 1.f);

    int  GetDataType();
    vtkTimeStamp   BuildTime;

  private:
    vtkXRayVolumeMapper(const vtkXRayVolumeMapper&);  // Not implemented.
    void operator=(const vtkXRayVolumeMapper&);  // Not implemented.

    // parameters
    static bool  ReduceColorResolution;
    static bool  AutoExposure;
    static bool  EnableAutoLOD;
    static bool  PerspectiveCorrection;
    static double ExposureCorrection[2];
    static double Gamma;
    static double AttenuationCoefficient;
    static double ScalingCoefficient;
    static float Color[4];

    // internal data
    enum { majorAxisX, majorAxisY, majorAxisZ };
    GLuint         GlTextures[3][MaxNumberOfRenderings][MaxNumberOfMipmapLevels + 1];        // OpenGL identifiers of textures
    GLuint         TextureSize[3][2];
    int            NumberOfTextures[3];
    int            RealColorDepth;
    int            OutputColorDepth;
    int            ScalarRange[2];
    int            MaxRenderingsWithoutOverflow;
    int            ColorBitsPerTextureByte[4];
    GLint          InternalTextureFormat;
    double          SliceDistance[3];      // (number of original slices / number of output slices)
    double          DataOrigin[3];
    double          DataExtent[6];
    int            CurrentAxis;
    int            SliceIncrement;

    vtkMatrix4x4  *TransformMatrix;
    vtkMatrix4x4  *VolumeMatrix;

    // accumulation buffer
    unsigned char     *ColorMap;
    double              ColorMapMax, ColorMapMin;
    
    double             *AccumBuffer;
    int                AccumBufferSize;
    unsigned long      AccumBufferCacheCheckSum;
    double              AccumBufferMaxPossibleValue;
    
    bool               IgnoreRendering;
    int                RenderingViewport[4];
    bool               EnableOptimization;
    
    double              CurrentAxisDirection[3];

    // statistics
    int            AllocatedTextureMemory;
    int            AllocatedTextureMemoryPerView[3];
    int            PercentageOfResidentTextures;
    double          TimeToDrawNotOptimized[3];
    
  };


#endif


