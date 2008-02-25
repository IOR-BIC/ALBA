/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkXRayVolumeMapper.cxx,v $
  Language:  C++
  Date:      $Date: 2008-02-25 14:26:20 $
  Version:   $Revision: 1.3 $


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

#ifndef _DEBUG		
//BES: 25.2.2008 - I really don't think it is a good idea to disable
//runtime checks because it may hide some bugs from their occurrence in 
//DEBUG mode and it is quite difficult to track these nasty bugs in RELEASE mode
//Thus I changed this code to disable runtime checks only for RELEASE.
#pragma runtime_checks( "s", off )
#endif


#include "vtkObjectFactory.h"

#include "vtkMatrix4x4.h"
#include "vtkMath.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRectilinearGrid.h"
#include "vtkCullerCollection.h"
#include "vtkCuller.h"
#include "vtkPlaneCollection.h"
#include "vtkPlane.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkTransform.h"
#include "vtkTimerLog.h"


#include <assert.h>

#include "vtkXRayVolumeMapper.h"

static const unsigned int MaxTextureResolution    = 512;
static const unsigned int MinTextureResolution    = 2;
static const unsigned int TargetImageColorDepth   = 12;

static const unsigned int ColorMapLength          = 1 << TargetImageColorDepth;

static const unsigned int HistogramSize = 2048;

// parameters
bool  vtkXRayVolumeMapper::ReduceColorResolution = true;
bool  vtkXRayVolumeMapper::AutoExposure = true;
bool  vtkXRayVolumeMapper::EnableAutoLOD = true;
bool  vtkXRayVolumeMapper::PerspectiveCorrection = false;
double vtkXRayVolumeMapper::ExposureCorrection[2] = { 0, 0};
double vtkXRayVolumeMapper::Gamma = 1.6f;
double vtkXRayVolumeMapper::AttenuationCoefficient = 0.001f;
double vtkXRayVolumeMapper::ScalingCoefficient = 1.f;
float vtkXRayVolumeMapper::Color[4] = { 0.5f, 0.5f, 0.5f, 1.f };


void ProjectBoxToViewport(const vtkMatrix4x4 *matrixObject, const double (&boundingBox)[6], double (&minMaxViewportCoordinates)[4]);
  
vtkCxxRevisionMacro(vtkXRayVolumeMapper, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkXRayVolumeMapper);

//------------------------------------------------------------------------------
vtkXRayVolumeMapper::vtkXRayVolumeMapper() {
  // create textures
  memset(this->GlTextures[0][0], 0, sizeof(this->GlTextures));

  this->SliceIncrement = 1;

  CurrentAxis = -1;
  this->NumberOfTextures[0] = this->NumberOfTextures[1] = this->NumberOfTextures[2] = 0;
  this->RealColorDepth = -1;
  this->OutputColorDepth = -1;
  this->AllocatedTextureMemory = 0;
  this->AllocatedTextureMemoryPerView[0] = this->AllocatedTextureMemoryPerView[1] = this->AllocatedTextureMemoryPerView[2] = 0;
  
  this->TimeToDrawNotOptimized[0] = this->TimeToDrawNotOptimized[1] = this->TimeToDrawNotOptimized[2]= 0;

  this->TransformMatrix = vtkMatrix4x4::New();
  this->VolumeMatrix    = vtkMatrix4x4::New();

  this->AccumBuffer = NULL;
  this->AccumBufferCacheCheckSum = 0;
  this->ColorMap = new unsigned char[1 << TargetImageColorDepth];
  }


//------------------------------------------------------------------------------
vtkXRayVolumeMapper::~vtkXRayVolumeMapper() {
  // delete textures if any
  ResetTextures();
  
  delete [] ColorMap;
  ColorMap = NULL;
    
  delete [] AccumBuffer;
  AccumBuffer = NULL;

  this->TransformMatrix->Delete();
  this->VolumeMatrix->Delete();
  }



//------------------------------------------------------------------------------
void vtkXRayVolumeMapper::StartRendering(vtkRenderer *renderer, vtkVolume *volume) {
  // calculate number of DDR volumes and their checksum (does not take into account changes in the hierarchy)
  const unsigned long prevCheckSum = AccumBufferCacheCheckSum;
  AccumBufferCacheCheckSum = 0;

  AccumBufferCacheCheckSum += PerspectiveCorrection * 7 + 1719 * renderer->GetActiveCamera()->GetMTime();
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  AccumBufferCacheCheckSum += unsigned long(viewport[0] * 17 + viewport[1] * 131 + viewport[2] * 37 +  viewport[3] * 1711);

  AccumBufferCacheCheckSum += this->CalculateChecksum(volume);

  // try to reuse accum buffer
  if (prevCheckSum == AccumBufferCacheCheckSum && AccumBuffer != NULL && AccumBufferCacheCheckSum != 0) {
    IgnoreRendering = true;
    EnableOptimization = false;
    assert(RenderingViewport[2] == viewport[2] && RenderingViewport[3] == viewport[3]);	
    }
  else {	  
    IgnoreRendering = false;
    EnableOptimization = EnableAutoLOD && renderer->GetAllocatedRenderTime() < 0.3f;
    if (EnableOptimization) {		
      AccumBufferCacheCheckSum = 0; // reset the cache
      RenderingViewport[2] = viewport[2] > 800 ? 512 : 256;
      RenderingViewport[3] = viewport[3] > 800 ? 512 : 256;
      if ((viewport[2] < 256) || (viewport[3] < 256))
        RenderingViewport[2] = viewport[2], RenderingViewport[3] = viewport[3];
      RenderingViewport[0] = ((viewport[2] - RenderingViewport[2]) >> 1) + viewport[0];
      RenderingViewport[1] = ((viewport[3] - RenderingViewport[3]) >> 1) + viewport[1];
      }
    else {
      memcpy(RenderingViewport, viewport, sizeof(RenderingViewport));
      }
    
    // init accumulation buffer
    if (AccumBuffer == NULL || (RenderingViewport[2] * RenderingViewport[3]) > AccumBufferSize) {		
      delete [] AccumBuffer;
      AccumBufferSize = RenderingViewport[2] * RenderingViewport[3];
      AccumBuffer = new double[AccumBufferSize + 16];
      }

    memset(AccumBuffer, 0, sizeof(AccumBuffer[0]) * RenderingViewport[2] * RenderingViewport[3]);
    AccumBufferMaxPossibleValue = 0;
    }
  }

void vtkXRayVolumeMapper::FinishRendering(vtkRenderer *renderer) {
  const int width  = RenderingViewport[2];
  const int height = RenderingViewport[3];

  // process the accumulation buffer
  unsigned int *Histogram = new unsigned int [HistogramSize + 1];

  memset(Histogram, 0, HistogramSize * sizeof(Histogram[0]));
  const double AccumValueToHistogramIndex = (double)HistogramSize / AccumBufferMaxPossibleValue;
  
  const int autoX0 = width / 10, autoY0 = height / 10;
  const int autoX1 = width - width / 10, autoY1 = height - height / 10;
  
  for (int y = autoY0; y < autoY1; y++) {
    const double *pAccumBuffer = AccumBuffer + y * width + autoX0;
    for (int x = autoX0; x < autoX1; x++, pAccumBuffer++) {
      int index = int(double(*pAccumBuffer) * AccumValueToHistogramIndex);
      if (index >= 0 && index < HistogramSize)
        Histogram[index]++;
      }
    }


  //---------------------------------------------- find exposure
  int histogramSum = 0;
  for (int i = 1; i < HistogramSize; i++)
    histogramSum += Histogram[i];
  
  // find min intensity value
  int minSum = int(histogramSum * 0.02f); int minI;
  for (minI = 1; minI < HistogramSize && minSum > 0; minI++)
    minSum -= Histogram[minI];
  const double minVal = minI / AccumValueToHistogramIndex;
  
  // find max intensity value
  int maxSum = int(histogramSum * 0.05f);int maxI;
  for (maxI = HistogramSize - 1; maxI > 1 && maxSum > 0; maxI--)
    maxSum -= Histogram[maxI];
  const double maxVal = maxI / AccumValueToHistogramIndex;
  
  delete [] Histogram;


  if ((1.5f * minVal + 1.f) < maxVal) {
    double minValCorrected = (ExposureCorrection[0] > 0) ? (minVal + (maxVal - minVal) * ExposureCorrection[0]) : minVal * (1.f + ExposureCorrection[0]);
    double maxValCorrected = (ExposureCorrection[1] < 0) ? (maxVal + (maxVal - minVal) * ExposureCorrection[1]) : maxVal * (1.f + ExposureCorrection[1]);

    FindExposure(minValCorrected, maxValCorrected, Gamma);
    }
  else {
    FindExposure(maxVal * 0.01f, maxVal * 0.8f, Gamma);
    }
  
  // create the final image
  const double colorMappingMultiplier = double(ColorMapLength) / (ColorMapMax - ColorMapMin + 0.001f);

  unsigned char  * const imageBuffer = new unsigned char[3 * width * height];
  unsigned char  *dst = imageBuffer;
  const double    *pAccumBuffer = AccumBuffer;
  double * const pAccumBufferEnd = AccumBuffer + width * height;

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  if (viewport[2] == width && viewport[3] == height) {
    for ( ; pAccumBuffer < pAccumBufferEnd; pAccumBuffer++) {
      int index = int((*pAccumBuffer - ColorMapMin) * colorMappingMultiplier);
      unsigned char val = (index >= (int)ColorMapLength) ? 255 : ((index > 0) ? ColorMap[index] : 0);
      *(dst++) = unsigned char(val * Color[0]);
      *(dst++) = unsigned char(val * Color[1]);
      *(dst++) = unsigned char(val * Color[2]);
      }
    }
  else {
    for ( ; pAccumBuffer < pAccumBufferEnd; dst++, pAccumBuffer++) {
      int index = int((*pAccumBuffer - ColorMapMin) * colorMappingMultiplier);
      *dst = (index >= (int)ColorMapLength) ? 255 : ((index > 0) ? ColorMap[index] : 0);
      }
    }

 
  //--------------------------------------------------------------- draw the result
  // prepare for drawing the image
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDisable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  glDisable(GL_POLYGON_SMOOTH);

  glPixelTransferf(GL_RED_SCALE, 1.f);
  glPixelTransferf(GL_GREEN_SCALE, 1.f);
  glPixelTransferf(GL_BLUE_SCALE, 1.f);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glRasterPos3i(-1, -1, 0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);

  if (viewport[2] == width && viewport[3] == height) {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    //glPixelTransferf(GL_RED_SCALE, Color[0]);
    //glPixelTransferf(GL_GREEN_SCALE, Color[1]);
    //glPixelTransferf(GL_BLUE_SCALE, Color[2]);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, imageBuffer);
    }
  else {
    assert(width % 256 == 0 && height % 256 == 0);
    
    glEnable(GL_LIGHTING);
    // enable lighting but disable lights
    for (int l = GL_LIGHT0; l < (GL_LIGHT0 + GL_MAX_LIGHTS) && (glGetError() == GL_NO_ERROR); l++)
      glDisable(l);
    GLfloat ambient[4] = { 1, 1, 1, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    GLuint GlOutputTexture;
    glGenTextures(1, &GlOutputTexture);
    glBindTexture(GL_TEXTURE_2D, GlOutputTexture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, imageBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Color);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-1.f, -1.f, 0);
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-1.f, 1.f, 0);
    glTexCoord2f(1.f, 1.0f);   glVertex3f(1.f, 1.f, 0);
    glTexCoord2f(1.f, 0.0f);   glVertex3f(1.f, -1.f, 0);
    glEnd();
    glDeleteTextures(1, &GlOutputTexture);
    }

  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  delete [] imageBuffer;
  }

//------------------------------------------------------------------------------
unsigned long vtkXRayVolumeMapper::CalculateChecksum(vtkVolume *volume) {
  // calculate check sum
  if (this->GetInput() == NULL)
    return 0;
  unsigned long checksum = unsigned long(this->GetInput());
  checksum += unsigned long(this->GetInput()->GetMTime());

  unsigned char *mPtr = (unsigned char *)volume->GetMatrix()->Element[0];
  for (int ii = 0; ii < (16 * sizeof(double)); ii++)
    checksum += int(mPtr[ii] + 1) * (151 + ii) + int(mPtr[ii] + 7) * (31 * ii);
  return checksum;
  }



//------------------------------------------------------------------------------
void vtkXRayVolumeMapper::Render(vtkRenderer *renderer, vtkVolume *volume) {
  StartRendering(renderer, volume);
  if (IgnoreRendering) {
    FinishRendering(renderer);
    return;
    }
  
  //--------------------------- prepare for rendering
  if (!PrepareTextures())
    return;

  InitializeRender(true); // this should be called after the color depth is known
  EnableClipPlanes(true);
  
  this->Timer->StartTimer();

  // select the optimal direction
  this->FindMajorAxis(renderer, volume);
  if (this->NumberOfTextures[this->CurrentAxis] == 0) {
    assert(false);
    return;
    }

  // get viewport parameters
  GLint prevBuffer;
  glGetIntegerv(GL_DRAW_BUFFER, &prevBuffer);
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  // prepare the transformation
  volume->GetMatrix(this->VolumeMatrix);
  this->TransformMatrix->DeepCopy(renderer->GetActiveCamera()->GetCompositePerspectiveTransformMatrix((double)viewport[2] / viewport[3], 0, 1));
  vtkMatrix4x4::Multiply4x4(this->TransformMatrix, this->VolumeMatrix, this->TransformMatrix);
  this->VolumeMatrix->Transpose();
  glMatrixMode(GL_MODELVIEW);
  glMultMatrixd(this->VolumeMatrix->Element[0]);

  // set viewport parameters
  glViewport(RenderingViewport[0], RenderingViewport[1], RenderingViewport[2], RenderingViewport[3]);

  // make the viewport smaller if possible
  double volumeProjection[4];
  ProjectBoxToViewport(this->TransformMatrix, this->DataExtent, volumeProjection);
  
  int updateBox[4];
  updateBox[0] = int(0.5f * (volumeProjection[0] + 1.f) * RenderingViewport[2]) - 1;
  updateBox[0] = (updateBox[0] > 0 && updateBox[0] <= RenderingViewport[2]) ? updateBox[0] : 0;
  
  updateBox[1] = int(0.5f * (volumeProjection[1] + 1.f) * RenderingViewport[2]) + 1;
  updateBox[1] = (updateBox[1] > 0 && updateBox[1] <= RenderingViewport[2]) ? updateBox[1] : (RenderingViewport[2] - 1);
  
  updateBox[2] = int(0.5f * (volumeProjection[2] + 1.f) * RenderingViewport[3]) - 1;
  updateBox[2] = (updateBox[2] > 0 && updateBox[2] <= RenderingViewport[3]) ? updateBox[2] : 0;
  
  updateBox[3] = int(0.5f * (volumeProjection[3] + 1.f) * RenderingViewport[3]) + 1;
  updateBox[3] = (updateBox[3] > 0 && updateBox[3] <= RenderingViewport[3]) ? updateBox[3] : (RenderingViewport[3] - 1);
  
  const int width  = updateBox[1] - updateBox[0] + 1;
  const int height = updateBox[3] - updateBox[2] + 1;
  
  glReadBuffer(GL_BACK);
  glDrawBuffer(GL_BACK);

  unsigned char *prevImage = new unsigned char [3 * width * height];
  glReadPixels(RenderingViewport[0] + updateBox[0], RenderingViewport[1] + updateBox[2], width, height, GL_RGB, GL_UNSIGNED_BYTE, prevImage);
  
  this->PercentageOfResidentTextures = 0;

  // find mipmap level
  unsigned int mipMapLevel = 0, maxSize = unsigned(width > height ? width : height);
  while (mipMapLevel <= MaxNumberOfMipmapLevels && maxSize < (this->TextureSize[this->CurrentAxis][0] >> mipMapLevel) && maxSize < (this->TextureSize[this->CurrentAxis][1] >> mipMapLevel))
    mipMapLevel++;
  if (mipMapLevel > MaxNumberOfMipmapLevels)
    mipMapLevel = MaxNumberOfMipmapLevels;

  const bool  enablePerspectiveCorrection = PerspectiveCorrection && !renderer->GetActiveCamera()->GetParallelProjection();
  // perspective correction
  vtkCamera *camera = renderer->GetActiveCamera();
  double cameraX[3], cameraY[3], cameraZ[3];
  if (enablePerspectiveCorrection) {
    const double windowUnit = 2 * tan(vtkMath::DegreesToRadians() * 0.5f * camera->GetViewAngle()) * camera->GetDistance() / RenderingViewport[3];
    camera->GetViewUp(cameraY);
    camera->GetFocalPoint(cameraZ);
    camera->GetPosition(cameraX);
    cameraZ[0] -= cameraX[0];
    cameraZ[1] -= cameraX[1];
    cameraZ[2] -= cameraX[2];
    vtkMath::Cross(cameraY, CurrentAxisDirection, cameraX);
    vtkMath::Normalize(cameraX);
    vtkMath::Normalize(cameraY);
    cameraX[0] *= windowUnit;
    cameraX[1] *= windowUnit;
    cameraX[2] *= windowUnit;
    cameraY[0] *= windowUnit;
    cameraY[1] *= windowUnit;
    cameraY[2] *= windowUnit;
    }

  
  //------------------------------------------------------------- render textures
  unsigned char * const imageBuffer  = new unsigned char [3 * width * height];
  unsigned int  * const iAccumBuffer = new unsigned int  [3 * width * height];
  bool iAccumBufferInitialized = false;

  this->SliceIncrement *= -1; // optimization for texture caching

  // estimate number of renderings
  int TotalNumberOfRenderings = 0, z = 0;
  for (; z < this->NumberOfTextures[this->CurrentAxis]; z++) {
    if (this->GlTextures[this->CurrentAxis][z][0] != 0)
      TotalNumberOfRenderings++;
    }
  int numberOfRenderings = 0, prevAccumNumberOfRenderings = -1;

  //------------------------------ render textures ----------------------------------
  for (z = (this->SliceIncrement > 0) ? 0 : this->NumberOfTextures[this->CurrentAxis] - 1; (z >= 0) && (z < this->NumberOfTextures[this->CurrentAxis]); z += this->SliceIncrement) {
    if ((numberOfRenderings % this->MaxRenderingsWithoutOverflow) == 0) {
      glEnable(GL_SCISSOR_TEST);
      glScissor(RenderingViewport[0] + updateBox[0], RenderingViewport[1] + updateBox[2], width, height);
      glClear(GL_COLOR_BUFFER_BIT);
      glDisable(GL_SCISSOR_TEST);
      }
    
    // ignore empty slices
    if (this->GlTextures[this->CurrentAxis][z][0] != 0) {
      numberOfRenderings++;

	  // prepare the texture
      GLuint textureId = this->GlTextures[this->CurrentAxis][z][mipMapLevel] != 0 ? this->GlTextures[this->CurrentAxis][z][mipMapLevel] :
                                                                                    this->GlTextures[this->CurrentAxis][z][0];
      GLboolean textureResident;
      if (glAreTexturesResident(1, &textureId, &textureResident))
        this->PercentageOfResidentTextures++;
      
      glBindTexture(GL_TEXTURE_2D, textureId);
     
      // draw a slice
      double slice = double(z) / (this->NumberOfTextures[this->CurrentAxis] - 1);
      glMatrixMode(GL_MODELVIEW);
      glBegin(GL_QUADS);
      if (this->CurrentAxis == majorAxisZ) {
        slice = this->DataExtent[4] + (this->DataExtent[5] - this->DataExtent[4]) * slice;
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(this->DataExtent[0], this->DataExtent[2], slice);
        glTexCoord2f(0.0f, 1.0f);  glVertex3f(this->DataExtent[0], this->DataExtent[3], slice);
        glTexCoord2f(1.0f, 1.0f);  glVertex3f(this->DataExtent[1], this->DataExtent[3], slice);
        glTexCoord2f(1.0f, 0.0f);  glVertex3f(this->DataExtent[1], this->DataExtent[2], slice);
        }
      else if (this->CurrentAxis == majorAxisY) {
        slice = this->DataExtent[2] + (this->DataExtent[3] - this->DataExtent[2]) * slice;
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(this->DataExtent[0], slice, this->DataExtent[4]);
        glTexCoord2f(0.0f, 1.0f);  glVertex3f(this->DataExtent[1], slice, this->DataExtent[4]);
        glTexCoord2f(1.0f, 1.0f);  glVertex3f(this->DataExtent[1], slice, this->DataExtent[5]);
        glTexCoord2f(1.0f, 0.0f);  glVertex3f(this->DataExtent[0], slice, this->DataExtent[5]);
        }
      else { // (this->CurrentAxis == majorAxisX)
        slice = this->DataExtent[0] + (this->DataExtent[1] - this->DataExtent[0]) * slice;
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(slice, this->DataExtent[2], this->DataExtent[4]);
        glTexCoord2f(0.0f, 1.0f);  glVertex3f(slice, this->DataExtent[2], this->DataExtent[5]);
        glTexCoord2f(1.0f, 1.0f);  glVertex3f(slice, this->DataExtent[3], this->DataExtent[5]);
        glTexCoord2f(1.0f, 0.0f);  glVertex3f(slice, this->DataExtent[3], this->DataExtent[4]);
        }
      glEnd();
      glFlush();
      assert(glGetError() == GL_NO_ERROR);
      }

    // is this the time to pass the values to the accumulation buffer?
    if (((numberOfRenderings % this->MaxRenderingsWithoutOverflow) != 0 && numberOfRenderings != TotalNumberOfRenderings) || 
        numberOfRenderings == 0 || prevAccumNumberOfRenderings == numberOfRenderings)
      continue;
    
    prevAccumNumberOfRenderings = numberOfRenderings;
    
    // copy the image from the framebuffer to accumulation buffer
    unsigned char *src = imageBuffer, *srcEnd = imageBuffer + 3 * width * height;
    unsigned int  *dst = iAccumBuffer;

    glReadPixels(RenderingViewport[0] + updateBox[0], RenderingViewport[1] + updateBox[2], width, height, GL_RGB, GL_UNSIGNED_BYTE, src);

    // accumulate the colors
    const unsigned int shiftG = this->ColorBitsPerTextureByte[0], shiftB = this->ColorBitsPerTextureByte[0] + this->ColorBitsPerTextureByte[1];

    if (iAccumBufferInitialized) {
      for ( ; src < srcEnd; src += 3, dst++)
        *dst += (unsigned int(src[0]) + (unsigned int(src[1]) << shiftG) + (unsigned int(src[2]) << shiftB));
      }
    else {
      for ( ; src < srcEnd; src += 3, dst++)
        *dst = (unsigned int(src[0]) + (unsigned int(src[1]) << shiftG) + (unsigned int(src[2]) << shiftB));
      iAccumBufferInitialized = true;
      }
    } // for (slice)
  assert(glGetError() == GL_NO_ERROR);

  // copy int buffer to double buffer
  const unsigned int *iBuffer = iAccumBuffer;

  const double accumValueMultiplier = this->SliceDistance[this->CurrentAxis] * double(1 << (this->RealColorDepth - this->OutputColorDepth)) /
                                     (enablePerspectiveCorrection ? 1.f : fabs(CurrentAxisDirection[this->CurrentAxis]));
  AccumBufferMaxPossibleValue += accumValueMultiplier * (TotalNumberOfRenderings * ((1 << this->OutputColorDepth) - 1));

  if (!enablePerspectiveCorrection) {
    for (int y = updateBox[2]; y <= updateBox[3]; y++) {
      double *fBuffer = AccumBuffer + y * RenderingViewport[2] + updateBox[0];
      for (int x = updateBox[0]; x <= updateBox[1]; x++, iBuffer++, fBuffer++)
        *fBuffer += double(accumValueMultiplier * double(*iBuffer));
      }
    }
  else {
    for (int y = updateBox[2]; y <= updateBox[3]; y++) {
      const double cameraYZ[3] = { cameraY[0] * ((y + (RenderingViewport[3] >> 1)) - RenderingViewport[3]) + cameraZ[0],
                                  cameraY[1] * ((y + (RenderingViewport[3] >> 1)) - RenderingViewport[3]) + cameraZ[1],
                                  cameraY[2] * ((y + (RenderingViewport[3] >> 1)) - RenderingViewport[3]) + cameraZ[2]};
      double  *fBuffer = AccumBuffer + y * RenderingViewport[2] + updateBox[0];
      for (int x = updateBox[0]; x <= updateBox[1]; x++, iBuffer++, fBuffer++) {
        const double fx = (x + (RenderingViewport[2] >> 1)) - RenderingViewport[2];
        const double ray[3] = {cameraYZ[0] + cameraX[0] * fx,  cameraYZ[1] + cameraX[1] * fx, cameraYZ[2] + cameraX[2] * fx };
        
        *fBuffer += double(accumValueMultiplier * double(*iBuffer) * 
          sqrt(ray[0] * ray[0] + ray[1] * ray[1] + ray[2] * ray[2]) / fabs(ray[this->CurrentAxis]));
        }
      }
    }


  // restore the image
  glViewport(RenderingViewport[0] + updateBox[0], RenderingViewport[1] + updateBox[2], width, height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glRasterPos3i(-1, -1, 0);

  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, prevImage);

  // test
  /*GLdouble mColor[4] = { 1, 1, 1, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mColor);
  glBegin(GL_LINE_STRIP);
  glVertex2i(-1, -1);
  glVertex2i(1, -1);
  glVertex2i(1, 1);
  glVertex2i(-1, 1);
  glEnd();*/

  // restore viewport
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  delete [] prevImage;
  delete [] imageBuffer;
  delete [] iAccumBuffer;

  if (prevBuffer != GL_BACK)
    glDrawBuffer(prevBuffer);

  InitializeRender(false);

  this->PercentageOfResidentTextures = int(100.f * double(this->PercentageOfResidentTextures) / numberOfRenderings);
  FinishRendering(renderer);

  this->Timer->StopTimer();
  this->TimeToDraw = (double)this->Timer->GetElapsedTime();
  if (!EnableOptimization)
    this->TimeToDrawNotOptimized[CurrentAxis] = this->TimeToDraw;
  
  if ( this->TimeToDraw < 0.0001f)
    this->TimeToDraw = 0.0001f;
  }



void vtkXRayVolumeMapper::SetInput(vtkDataSet *input) {
  this->vtkProcessObject::SetNthInput(0, input);
  ResetTextures();
  }

//----------------------------------------------------------------------
void vtkXRayVolumeMapper::InitializeRender(bool setup) {
  glGetError(); // reset error flag
  if (setup) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // enable texturing
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // enable lighting but disable lights
    glEnable(GL_LIGHTING);
    for (int l = GL_LIGHT0; l < (GL_LIGHT0 + GL_MAX_LIGHTS) && (glGetError() == GL_NO_ERROR); l++)
      glDisable(l);
    GLfloat ambient[4] = { 1, 1, 1, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    GLfloat mColor[4] = { 1.f / (1 << (8 - this->ColorBitsPerTextureByte[0])), 1.f / (1 << (8 - this->ColorBitsPerTextureByte[1])), 1.f / (1 << (8 - this->ColorBitsPerTextureByte[2])), 1.f / (1 << (8 - this->ColorBitsPerTextureByte[3]))};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mColor);

    glShadeModel(GL_FLAT);
    glClearColor(0, 0, 0, 0);
    
    // disable any antialiasing
    bool multisamplingExt = (strstr((const char *)glGetString(GL_EXTENSIONS), "GL_EXT_multisample") != NULL ||
                             strstr((const char *)glGetString(GL_EXTENSIONS), "GL_SGIS_multisample") != NULL);
    if (multisamplingExt)
      glDisable(0x809D);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    glDisable(GL_POLYGON_SMOOTH);

    glPixelTransferf(GL_RED_SCALE, 1.f);
    glPixelTransferf(GL_GREEN_SCALE, 1.f);
    glPixelTransferf(GL_BLUE_SCALE, 1.f);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
  else { // restore the settings
    glPopAttrib();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }
  assert(glGetError() == 0);
  }


//----------------------------------------------------------------------
void vtkXRayVolumeMapper::EnableClipPlanes(bool enable) {
  if (enable) {
    if (this->ClippingPlanes) {
      int numClipPlanes = this->ClippingPlanes->GetNumberOfItems();
      if (numClipPlanes > 6)
        vtkErrorMacro(<< "OpenGL guarantees only 6 additional clipping planes");
      
      for (int i = 0; i < this->ClippingPlanes->GetNumberOfItems(); i++) {
        glEnable((GLenum)(GL_CLIP_PLANE0+i));
        
        vtkPlane *plane = (vtkPlane *)this->ClippingPlanes->GetItemAsObject(i);
   
        double planeEquation[4];
        planeEquation[0] = plane->GetNormal()[0]; 
        planeEquation[1] = plane->GetNormal()[1]; 
        planeEquation[2] = plane->GetNormal()[2];
        planeEquation[3] = -(planeEquation[0] * plane->GetOrigin()[0] + planeEquation[1] * plane->GetOrigin()[1]+
                            planeEquation[2] * plane->GetOrigin()[2]);
        glClipPlane((GLenum)(GL_CLIP_PLANE0+i),planeEquation);
        }
      }
    }
  else {
    if (this->ClippingPlanes) {
      for (int i = 0; i < this->ClippingPlanes->GetNumberOfItems(); i++)
        glDisable((GLenum)(GL_CLIP_PLANE0+i));
      }
    }
  }


/////////////////////////////////////////////////////////////////////////////////////
// generate opengl textures from input data set
bool vtkXRayVolumeMapper::PrepareTextures() {

  // Is data the same?
  if (this->GetInput() != NULL && this->GetInput()->GetMTime() > this->BuildTime)
    ResetTextures();
  else if (this->NumberOfTextures[0] > 0 && this->NumberOfTextures[1] > 0 && this->NumberOfTextures[2] > 0)
    return true; // nothing to do

  // check the data
  if (this->GetInput() && this->GetInput()->GetDataReleased())
    this->GetInput()->Update(); // ensure that the data is loaded
  if (!this->IsDataValid(true))
    return false;

  vtkImageData       *imageData = vtkImageData::SafeDownCast(this->GetInput());
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(this->GetInput());

  // deal with color depth
  if (this->RealColorDepth <= 0) {
    this->RealColorDepth = FindColorResolution();
    if (this->RealColorDepth < 8)
      this->RealColorDepth = 8;
    }

  this->OutputColorDepth = this->RealColorDepth;
  if (ReduceColorResolution && this->RealColorDepth > 9)
    this->OutputColorDepth = 9;
  else if (this->OutputColorDepth > 12)
    this->OutputColorDepth = 12;

  this->ColorBitsPerTextureByte[0] = (this->OutputColorDepth <= 9) ? 3 : 4;
  this->ColorBitsPerTextureByte[1] = (this->OutputColorDepth <= 9) ? 3 : 4; 
  this->ColorBitsPerTextureByte[2] = (this->OutputColorDepth <= 9) ? 3 : 4; 
  this->ColorBitsPerTextureByte[3] = 0; // no alpha
  this->MaxRenderingsWithoutOverflow = (this->OutputColorDepth <= 9) ? (255 / 7) : (255 / 15);
  this->InternalTextureFormat = GL_RGB4; // GL_R3_G3_B2 can be also useful but I cannot test it.

  // find extent
  const bool regularSpacing = imageData != NULL;
  double* VoxelCoordinates[3];
  int dims[3], extent[6];
  if (imageData) {
    double dataSpacing[3];
    imageData->GetWholeExtent(extent);
    imageData->GetDimensions(dims);
    imageData->GetOrigin(this->DataOrigin);
    imageData->GetSpacing(dataSpacing);
    for (int axis = 0; axis < 3; axis++) {
      VoxelCoordinates[axis] = new double [dims[axis]];
      double f = this->DataOrigin[axis] + extent[2 * axis] * dataSpacing[axis];
      for (int i = 0; i < dims[axis]; i++, f += dataSpacing[axis])
        VoxelCoordinates[axis][i] = f;
      }
    }
  else {
    gridData->GetWholeExtent(extent);
    gridData->GetDimensions(dims);
    this->DataOrigin[0] = gridData->GetXCoordinates()->GetTuple(0)[0];
    this->DataOrigin[1] = gridData->GetYCoordinates()->GetTuple(0)[0];
    this->DataOrigin[2] = gridData->GetZCoordinates()->GetTuple(0)[0];
    
    for (int axis = 0; axis < 3; axis++) {
      VoxelCoordinates[axis] = new double [dims[axis]];
      vtkDataArray *coordinates = (axis == 2) ? gridData->GetZCoordinates() : (axis == 1 ? gridData->GetYCoordinates() : gridData->GetXCoordinates());
      for (int i = 0; i < dims[axis]; i++)
        VoxelCoordinates[axis][i] = *(coordinates->GetTuple(i));
      }
    }

  this->DataExtent[0] = VoxelCoordinates[0][0];
  this->DataExtent[1] = VoxelCoordinates[0][dims[0] - 1];
  this->DataExtent[2] = VoxelCoordinates[1][0];
  this->DataExtent[3] = VoxelCoordinates[1][dims[1] - 1];
  this->DataExtent[4] = VoxelCoordinates[2][0];
  this->DataExtent[5] = VoxelCoordinates[2][dims[2] - 1];

  ////////////////////////////// create new textures
  for (int CurrentAxis = 0; CurrentAxis < 3; CurrentAxis++) {
    // clear error flag
    glGetError();
    
    // modify the dimensions (texture size should be equal to 2^N)
    int newDims[3];
	int d = 0;
    for (; d < 3; d++) {
      // find log2
      newDims[d] = dims[d];
	  int p = 0;
      for (; newDims[d] > 0; p++)
        newDims[d] >>= 1;
      if (d == CurrentAxis) {
        newDims[d] = dims[d];
        if (newDims[d] > MaxNumberOfRenderings)
          newDims[d] = MaxNumberOfRenderings;
        }
      else {
        newDims[d] = (dims[d] >= MaxTextureResolution) ? MaxTextureResolution : ( ( (double(1 << p) / dims[d]) < 1.5f) ? (1 << p) : (1 << (p - 1)));
        }
      }
    
    int indexMap[3] = {0, 1, 2}; // map texture indeces to the volume indeces
    if (CurrentAxis == majorAxisX)
      indexMap[0] = 1, indexMap[1] = 2, indexMap[2] = 0;
    else if (CurrentAxis == majorAxisY)
      indexMap[0] = 2, indexMap[1] = 0, indexMap[2] = 1;
    int indexDimension[3] = { 1, dims[0], dims[0] * dims[1]};
    if (CurrentAxis == majorAxisX)
      indexDimension[0] = dims[0], indexDimension[1] = dims[0] * dims[1], indexDimension[2] = 1;
    else if (CurrentAxis == majorAxisY)
      indexDimension[0] = dims[0] * dims[1], indexDimension[1] = 1, indexDimension[2] = dims[0];
    
    const int &width  = this->TextureSize[CurrentAxis][0] = newDims[indexMap[0]];
    const int &height = this->TextureSize[CurrentAxis][1] = newDims[indexMap[1]];
    const int &numOfSlices  = this->NumberOfTextures[CurrentAxis] = newDims[indexMap[2]];
    const int &origWidth  = dims[indexMap[0]];
    const int &origHeight = dims[indexMap[1]];
    const int &origNumOfSlices  = dims[indexMap[2]];
    
    // average
    this->SliceDistance[CurrentAxis] = (this->DataExtent[2 * CurrentAxis + 1] - this->DataExtent[2 * CurrentAxis]) / numOfSlices;
    
    // allocate memory for the texture
    const unsigned int textureSize = 3 * this->TextureSize[CurrentAxis][0] * this->TextureSize[CurrentAxis][1];
    unsigned char  * const texture = new unsigned char [textureSize + 256];
    
    /////////////////////////////////////////////// prepare textures (scale and pack)
    
    // calculate scaling coefs
    const int maxL = (newDims[0] > newDims[1]) ? (newDims[0] > newDims[2] ? newDims[0] : newDims[2]) :
                     (newDims[1] > newDims[2] ? newDims[1] : newDims[2]);
    double (*scaleK)[3] = (double (*)[3])new double[3 * (maxL + 1)];
    int   (*scaleI)[3] = (int (*)[3])new int[3 * (maxL + 1)];
    for (d = 0; d < 3; d++) {
      const int axis = indexMap[d];
      const double interval = (this->DataExtent[2 * axis + 1] - this->DataExtent[2 * axis]) / (newDims[axis] - 1);
      double fi = VoxelCoordinates[axis][0];
      for (int i = 0, vi = 0; i < newDims[axis]; i++, fi += interval) {
        // find 
        while (vi < dims[axis] && VoxelCoordinates[axis][vi] <= fi)
          vi++;

        // special cases
        if (vi >= (dims[axis] - 1)) {
          scaleI[i][d] = vi - 2;
          scaleK[i][d] = 0;
          continue;
          }
        if (vi == 0) {
          scaleI[i][d] = 0;
          scaleK[i][d] = 1.f;
          continue;
          }

        double fi1 = VoxelCoordinates[axis][vi - 1];
        double fi2 = VoxelCoordinates[axis][vi];

        scaleI[i][d] = vi - 1;
        scaleK[i][d] = 1.f - (fi - fi1) / (fi2 - fi);
        if (scaleK[i][d] < 0)
          scaleK[i][d] = 0;
        else if (scaleK[i][d] > 1.f)
          scaleK[i][d] = 1.f;
        }
      }
    bool xyScaleNeeded = width != origWidth || height != origHeight || !regularSpacing;
    bool zScaleNeeded  = numOfSlices != origNumOfSlices || !regularSpacing;
    const unsigned int colorShiftBits = this->RealColorDepth - this->OutputColorDepth;
    const unsigned int packingShift[3] = {8 - this->ColorBitsPerTextureByte[0], 8 - this->ColorBitsPerTextureByte[1], 8 - this->ColorBitsPerTextureByte[2]};
    
    const int  dataType = this->GetDataType();
    const void *dataPointer = this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);

    for (int z = 0; z < numOfSlices; z++) {
      unsigned int  notZeroMask = 0;
      unsigned char *pTexture = texture;
      unsigned char * const pTextureEnd = texture + 3 * width * height;
      
      this->UpdateProgress(CurrentAxis / 3.f + (double)z / (3.f * numOfSlices));
///////////////////////////////////////////////////////////// scaling and copying code
#define CREATE_TEXTURE(Type) \
        {\
        const Type *origData = (Type *)dataPointer;\
        \
        /* basic case: no reordering */\
        if (CurrentAxis == majorAxisZ) {\
          if (!xyScaleNeeded && !zScaleNeeded) {\
            const Type *pOrigData = origData + z * origWidth * origHeight;\
            for ( ; pTexture < pTextureEnd; pTexture += 3, pOrigData++) {\
              int svalue = ((int)*pOrigData - this->ScalarRange[0]);\
              unsigned int value = svalue > 0 ? (unsigned int(svalue) >> colorShiftBits) : 0;\
              notZeroMask |= value; /* test for zero texture*/ \
              pTexture[0] = unsigned char(value << packingShift[0]);\
              value >>= this->ColorBitsPerTextureByte[0];\
              pTexture[1] = unsigned char(value << packingShift[1]);\
              value >>= this->ColorBitsPerTextureByte[1];\
              pTexture[2] = unsigned char(value << packingShift[2]);\
              }\
            }\
          else {\
            /* scaling is required*/\
            const Type *origSlice1 = origData + dims[0] * dims[1] * scaleI[z][2];\
            const Type *origSlice2 = origData + dims[0] * dims[1] * (scaleI[z][2] + 1);\
            const double k1z = scaleK[z][2], k2z = 1.f - scaleK[z][2];\
            \
            for (int y = 0; y < height; y++) {\
              const Type *origRow11 = origSlice1 + dims[0] * scaleI[y][1];\
              const Type *origRow12 = origSlice1 + dims[0] * (scaleI[y][1] + 1);\
              const Type *origRow21 = origSlice2 + dims[0] * scaleI[y][1];\
              const Type *origRow22 = origSlice2 + dims[0] * (scaleI[y][1] + 1);\
              const double k1y = scaleK[y][1], k2y = 1.f - scaleK[y][1];\
              const double k11yz = k1y * k1z, k12yz = k1y * k2z, k21yz = k2y * k1z, k22yz = k2y * k2z;\
              \
              for (int x = 0; x < width; x++) {\
                const double k1x = scaleK[x][0], k2x = 1.f - scaleK[x][0];\
                const int   xi = scaleI[x][0], xi1 = scaleI[x][0] + 1;\
                int svalue = int(- this->ScalarRange[0] +\
                  origRow11[xi] * k1x * k11yz + origRow11[xi1] * k2x * k11yz +\
                  origRow12[xi] * k1x * k12yz + origRow12[xi1] * k2x * k12yz +\
                  origRow21[xi] * k1x * k21yz + origRow21[xi1] * k2x * k21yz +\
                  origRow22[xi] * k1x * k22yz + origRow22[xi1] * k2x * k22yz);\
                unsigned int value = svalue > 0 ? (unsigned int(svalue) >> colorShiftBits) : 0;\
                \
                notZeroMask |= value; /* test for zero texture */\
                pTexture[0] = unsigned char(value << packingShift[0]);\
                value >>= this->ColorBitsPerTextureByte[0];\
                pTexture[1] = unsigned char(value << packingShift[1]);\
                value >>= this->ColorBitsPerTextureByte[1];\
                pTexture[2] = unsigned char(value << packingShift[2]);\
                pTexture += 3;\
                } /* for (x) */\
              } /* for (y) */\
            }\
          } /* no reordering */\
        else {\
          /* reorder and scale */\
          const int z1    = scaleI[z][2] * indexDimension[2], z2 = (scaleI[z][2] + 1) * indexDimension[2];\
          const double k1z = scaleK[z][2], k2z = 1.f - scaleK[z][2];\
          \
          for (int y = 0; y < height; y++) {\
            const int y1 = scaleI[y][1] * indexDimension[1], y2 = (scaleI[y][1] + 1) * indexDimension[1];\
            const int yz11 = y1 + z1, yz12 = y1 + z2, yz21 = y2 + z1, yz22 = y2 + z2;\
            const double k1y = scaleK[y][1], k2y = 1.f - scaleK[y][1];\
            const double k11yz = k1y * k1z, k12yz = k1y * k2z, k21yz = k2y * k1z, k22yz = k2y * k2z;\
            \
            for (int x = 0; x < width; x++, pTexture) {\
              const int   x1 = scaleI[x][0] * indexDimension[0], x2 = (scaleI[x][0] + 1) * indexDimension[0];\
              const double k1x = scaleK[x][0], k2x = 1.f - scaleK[x][0];\
              \
              int svalue = int(- this->ScalarRange[0] +\
                origData[x1 + yz11] * k1x * k11yz + origData[x2 + yz11] * k2x * k11yz +\
                origData[x1 + yz21] * k1x * k21yz + origData[x2 + yz21] * k2x * k21yz +\
                origData[x1 + yz12] * k1x * k12yz + origData[x2 + yz12] * k2x * k12yz +\
                origData[x1 + yz22] * k1x * k22yz + origData[x2 + yz22] * k2x * k22yz);\
              unsigned int value = svalue > 0 ? (unsigned int(svalue) >> colorShiftBits) : 0;\
              notZeroMask |= value;\
              \
              pTexture[0] = unsigned char(value << packingShift[0]);\
              value >>= this->ColorBitsPerTextureByte[0];\
              pTexture[1] = unsigned char(value << packingShift[1]);\
              value >>= this->ColorBitsPerTextureByte[1];\
              pTexture[2] = unsigned char(value << packingShift[2]);\
              pTexture += 3;\
              } /* for (x) */\
            } /* for (y) */\
          } /* if (reorder) */ \
        }
      
//////////////////////////////////////////////// end of macro definition

      if (dataType == VTK_UNSIGNED_SHORT)
        CREATE_TEXTURE(unsigned short)
      else if (dataType == VTK_SHORT)
        CREATE_TEXTURE(short)
      else if (dataType == VTK_UNSIGNED_CHAR)
        CREATE_TEXTURE(unsigned char)
      else if (dataType == VTK_CHAR)
        CREATE_TEXTURE(char)
      else if (dataType == VTK_UNSIGNED_INT)
        CREATE_TEXTURE(unsigned int)
      else if (dataType == VTK_INT)
        CREATE_TEXTURE(int)
      else if (dataType == VTK_UNSIGNED_LONG)
        CREATE_TEXTURE(unsigned long)
      else if (dataType == VTK_LONG)
        CREATE_TEXTURE(long)
      else if (dataType == VTK_FLOAT)
        CREATE_TEXTURE(double)
      else if (dataType == VTK_DOUBLE)
        CREATE_TEXTURE(double)
     
      // pass the texture to OpenGL
      assert(this->GlTextures[CurrentAxis][z][0] == 0);
      if (notZeroMask)
        this->RegisterTexture(CurrentAxis, z, texture);
    }
    
    delete [] texture;
    delete [] (int*)scaleI;
    delete [] (double*)scaleK;
    }
  
  for (int axis = 0; axis < 3; axis++)
    delete [] VoxelCoordinates[axis];

  this->UpdateProgress(1.f);

  this->BuildTime.Modified();
  return true;
  }



//--------------------------------------------------------------------
void vtkXRayVolumeMapper::FindMajorAxis(vtkRenderer *renderer, vtkVolume *volume) {
  vtkMatrix4x4 *volMatrix = vtkMatrix4x4::New();
  volMatrix->DeepCopy(volume->GetMatrix());
  vtkTransform *worldToVolumeTransform = vtkTransform::New();
  worldToVolumeTransform->SetMatrix(volMatrix);
  
  // Create a transform that will account for the translation of the scalar data.
  vtkTransform *volumeTransform = vtkTransform::New();
  
  volumeTransform->Identity();
  volumeTransform->Translate(this->DataOrigin);
  
  // Now concatenate the volume's matrix with this scalar data matrix
  worldToVolumeTransform->PreMultiply();
  worldToVolumeTransform->Concatenate( volumeTransform->GetMatrix() );
  worldToVolumeTransform->Inverse();

  renderer->GetActiveCamera()->GetViewPlaneNormal(CurrentAxisDirection);
  worldToVolumeTransform->TransformVector(CurrentAxisDirection, CurrentAxisDirection);
  
  volMatrix->Delete();
  volumeTransform->Delete();
  worldToVolumeTransform->Delete();
  
  //if (CurrentAxis >= 0)
  //  CurrentAxisDirection[CurrentAxis] *= 1.05f; // a trick to avoid frequent switching of the axis
  if (fabs(CurrentAxisDirection[0]) >= fabs(CurrentAxisDirection[1]) && fabs(CurrentAxisDirection[0]) >= fabs(CurrentAxisDirection[2]))
    CurrentAxis = majorAxisX;
  else if ( fabs(CurrentAxisDirection[1]) >= fabs(CurrentAxisDirection[0]) && fabs(CurrentAxisDirection[1]) >= fabs(CurrentAxisDirection[2]) )
    CurrentAxis = majorAxisY;
  else
    CurrentAxis = majorAxisZ;
  }


//--------------------------------------------------------------------
int vtkXRayVolumeMapper::FindColorResolution() {
  if (this->GetDataType() == VTK_UNSIGNED_CHAR) {
    this->ScalarRange[0] = VTK_UNSIGNED_CHAR_MIN;
    this->ScalarRange[1] = VTK_UNSIGNED_CHAR_MAX;
    return 8;
    }
  else if (this->GetDataType() == VTK_CHAR) {
    this->ScalarRange[0] = VTK_CHAR_MIN;
    this->ScalarRange[1] = VTK_CHAR_MAX;
    return 8;
    }

  double range[2];
  this->GetInput()->GetScalarRange(range);
  this->ScalarRange[0] = int(range[0]);
  this->ScalarRange[1] = int(range[1]);

#ifndef DO_NOT_CHECK_FOR_FRAME
  if (this->GetInput() && this->GetDataType() == VTK_SHORT) {
    short *data = (short *)this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);
    const short *dataEnd = data + this->GetInput()->GetPointData()->GetScalars()->GetNumberOfTuples();
    char *histogramArray = new char [VTK_UNSIGNED_SHORT_MAX + 1];
    memset(histogramArray, 0, VTK_UNSIGNED_SHORT_MAX + 1);
    char * const histogram = histogramArray - VTK_SHORT_MIN;

    for ( ; data < dataEnd; data += 2)
      histogram[*data] = 1;

    // find the first 1
	int firstOne, start, end;
    for (firstOne = VTK_SHORT_MIN; histogram[firstOne] == 0; firstOne++)
      { ; }
    // find next one
    for (start = firstOne + 1; start < VTK_UNSIGNED_SHORT_MAX && histogram[start] == 0; start++)
      { ; }
    for (end = VTK_SHORT_MAX - 1; histogram[end] == 0; end--)
      { ; }
    this->ScalarRange[0] = start;
    this->ScalarRange[1] = end;

    delete [] histogramArray;
    }
#endif

  unsigned int MaxNum = (unsigned int)(this->ScalarRange[1] - this->ScalarRange[0]);
  int i;
  for (i = 1; i < 16; i++) {
    MaxNum >>= 1;
    if (MaxNum == 0)
      break;
    }
  return i;
  }

//-------------------------------------------------------------------
void vtkXRayVolumeMapper::Update() {
  if (vtkImageData::SafeDownCast(this->GetInput()) != NULL || 
      vtkRectilinearGrid::SafeDownCast(this->GetInput()) != NULL) {
    this->GetInput()->UpdateInformation();
    this->GetInput()->SetUpdateExtentToWholeExtent();
    this->GetInput()->Update();
    }
  }


//-------------------------------------------------------------------
bool vtkXRayVolumeMapper::RegisterTexture(int axis, int textureIndex, unsigned char *data) {
  glGetError(); // clear error 

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if (this->GlTextures[axis][textureIndex][0] == 0)
    glGenTextures(1, this->GlTextures[axis][textureIndex]);
  glBindTexture(GL_TEXTURE_2D, this->GlTextures[axis][textureIndex][0]);
  glTexImage2D(GL_TEXTURE_2D, 0, this->InternalTextureFormat, this->TextureSize[axis][0], this->TextureSize[axis][1], 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  const int size = this->TextureSize[axis][0] * this->TextureSize[axis][1]; // 2 = 1 + 0.5 + 0.25 + ...
  const unsigned int packingShift[3]   = {8 - this->ColorBitsPerTextureByte[0], 8 - this->ColorBitsPerTextureByte[1], 8 - this->ColorBitsPerTextureByte[2]};

  // create pseudo-mipmaps
  int level = 1;
  unsigned int width  = this->TextureSize[axis][0];
  unsigned int height = this->TextureSize[axis][1];
  while ((width >= 32 || height >= 32) && width > 4 && height > 4 && level <= MaxNumberOfMipmapLevels) {
    
    // sample texture using box filter
    unsigned char *dst = data;
    for (unsigned int y = 0; y < height; y += 2) {
      unsigned char *row0 = data + 3 * width * y;
      unsigned char *row1 = row0 + 3 * width;
      for (unsigned int x = 0; x < width; x += 2, row0 += 6, row1 += 6) {
        unsigned int value = ((unsigned int(row0[0]) + row0[3] + row1[0] + row1[3]) >> packingShift[0]) +
          (((unsigned int(row0[1]) + row0[4] + row1[1] + row1[4]) >> packingShift[1]) << this->ColorBitsPerTextureByte[0]) +
          (((unsigned int(row0[2]) + row0[5] + row1[2] + row1[5]) >> packingShift[2]) << (this->ColorBitsPerTextureByte[0] + this->ColorBitsPerTextureByte[1]));
        value >>= 2; // averaging
        
        // pack back
        dst[0] = unsigned char(value << packingShift[0]);
        value >>= this->ColorBitsPerTextureByte[0];
        dst[1] = unsigned char(value << packingShift[1]);
        value >>= this->ColorBitsPerTextureByte[1];
        dst[2] = unsigned char(value << packingShift[2]);
        dst += 3;
        }
      }

    width  >>= 1;
    height >>= 1;
    if (this->GlTextures[axis][textureIndex][level] == 0)
      glGenTextures(1, this->GlTextures[axis][textureIndex] + level);
    glBindTexture(GL_TEXTURE_2D, this->GlTextures[axis][textureIndex][level]);
    glTexImage2D(GL_TEXTURE_2D, 0, this->InternalTextureFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    level++;
    }


  if (glGetError() != GL_NO_ERROR) {
    assert(false);
    vtkErrorMacro(<< "XRAY mapper: cannot create textures.");
    return false;
    }

  // statistics
  GLint param;
  double fsize = 0.f;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &param);
  fsize += param / 8.f;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &param);
  fsize += param / 8.f;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &param);
  fsize += param / 8.f;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &param);
  fsize += param / 8.f;

  this->AllocatedTextureMemory += int(size * fsize);
  this->AllocatedTextureMemoryPerView[axis] += int(size * fsize);

  return true;
  }



//--------------------------------------------------------------------------
void vtkXRayVolumeMapper::FindExposure(double minVal, double maxVal, double gamma) {
  if (maxVal < 1.f || minVal < 0 || (maxVal - minVal) < 0.1f || gamma < 0.05f || gamma > 10.f)
    return;

  AttenuationCoefficient = double(log(2.) / ((maxVal - minVal) * (1.f - 0.5f / gamma)));
  ScalingCoefficient = double(255 / exp(AttenuationCoefficient * maxVal));
  
  ColorMapMax = maxVal;
  ColorMapMin = minVal;
  
  ColorMap[0] = 0;
  for (int i = 1; i < ColorMapLength; i++) {
    int result = int(255 * pow(double(i) / ColorMapLength, 1.f / gamma) + 0.5f);
    ColorMap[i] = (unsigned char)(result > 255 ? 255 : result);
    }
  }


//--------------------------------------------------------------------------
void vtkXRayVolumeMapper::ResetTextures() {
  glDeleteTextures(sizeof(this->GlTextures) / sizeof(this->GlTextures[0][0][0]), this->GlTextures[0][0]);
  memset(this->GlTextures[0][0], 0, sizeof(this->GlTextures));
  this->AllocatedTextureMemory = 0;
  this->AllocatedTextureMemoryPerView[0] = this->AllocatedTextureMemoryPerView[1] = this->AllocatedTextureMemoryPerView[2] = 0;
  this->TimeToDrawNotOptimized[0] = this->TimeToDrawNotOptimized[1] = this->TimeToDrawNotOptimized[2]= 0;
  this->NumberOfTextures[0] = this->NumberOfTextures[1] = this->NumberOfTextures[2] = 0;
  this->RealColorDepth = -1;
  }



bool vtkXRayVolumeMapper::IsDataValid(bool warnings) {
  vtkDataSet         *inputData = this->GetInput();
  vtkImageData       *imageData = vtkImageData::SafeDownCast(inputData);
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(inputData);
  
  // check the data
  if (inputData == NULL) {
    if (warnings)
      vtkErrorMacro(<< "XRAY mapper: No data to render.");
    return false;
    }
  if (imageData == NULL && gridData == NULL) {
    if (warnings)
      vtkErrorMacro(<< "XRAY mapper: this data format is not supported");
    return false;
    }

  int dataType = this->GetDataType();
  if (dataType == VTK_VOID || dataType == VTK_BIT) {
    if (warnings)
      vtkErrorMacro(<< "XRAY mapper: the data type is not supported");
    return false;
    }

  // check image data
  if (imageData) {
    int extent[6];		//BES: 25.2.2008 - bug fix
    imageData->GetExtent(extent);
    if (extent[4] >= extent[5]) {
      if (warnings)
        vtkErrorMacro(<< "XRAY mapper: 2D datasets are not supported");
      return false;
      }

    if (imageData->GetNumberOfScalarComponents() > 1) {
      if (warnings)
        vtkErrorMacro(<< "XRAY mapper: only monochrome images are currently supported.");
      return false;
      }

    return true;
    }


  if (gridData) {
    if (gridData->GetPointData() == NULL || gridData->GetPointData()->GetScalars() == NULL ||
      gridData->GetXCoordinates()->GetNumberOfTuples() < 2 ||
      gridData->GetYCoordinates()->GetNumberOfTuples() < 2 ||
      gridData->GetZCoordinates()->GetNumberOfTuples() < 2) {
      if (warnings)
        vtkErrorMacro(<< "XRAY mapper: the dataset is empty.");
      return false;
      }
    }

  return true;
  }


int vtkXRayVolumeMapper::GetDataType() {
  if (this->GetInput() && this->GetInput()->GetPointData())
    return this->GetInput()->GetPointData()->GetScalars()->GetDataType();

  return VTK_VOID;
  }


void ProjectBoxToViewport(const vtkMatrix4x4 *matrixObject, const double (&boundingBox)[6], double (&minMaxViewportCoordinates)[4]) {
  static const double pointIndexToOffset[8][3] = {{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, { 1.f, 1.f, 0.f}, { 0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}, {1.f, 0.f, 1.f}, { 1.f, 1.f, 1.f}, { 0.f, 1.f, 1.f}};
  const double (&matrix0)[4] = matrixObject->Element[0];
  const double (&matrix1)[4] = matrixObject->Element[1];
  const double (&matrix2)[4] = matrixObject->Element[2];
  const double (&matrix3)[4] = matrixObject->Element[3];

  // init coordinates
  minMaxViewportCoordinates[0] = minMaxViewportCoordinates[2] = VTK_FLOAT_MAX;
  minMaxViewportCoordinates[1] = minMaxViewportCoordinates[3] = VTK_FLOAT_MIN;

  double boxSize[3] = {boundingBox[1] - boundingBox[0], boundingBox[3] - boundingBox[2], boundingBox[5] - boundingBox[4]};
  for (int pi = 0; pi < 8; pi++) {
    double x = boundingBox[0] + boxSize[0] * pointIndexToOffset[pi][0];
    double y = boundingBox[2] + boxSize[1] * pointIndexToOffset[pi][1];
    double z = boundingBox[4] + boxSize[2] * pointIndexToOffset[pi][2];
    
    double normComponent = 1.f / (x * matrix3[0] + y * matrix3[1] + z * matrix3[2] + matrix3[3]);
    if (normComponent > VTK_FLOAT_MAX || normComponent < VTK_FLOAT_MIN)
      normComponent = 1.f;
    double xv = (x * matrix0[0] + y * matrix0[1] + z * matrix0[2] + matrix0[3]) * normComponent;
    double yv = (x * matrix1[0] + y * matrix1[1] + z * matrix1[2] + matrix1[3]) * normComponent;

    if (xv < minMaxViewportCoordinates[0])
      minMaxViewportCoordinates[0] = xv;
    if (xv > minMaxViewportCoordinates[1])
      minMaxViewportCoordinates[1] = xv;
    
    if (yv < minMaxViewportCoordinates[2])
      minMaxViewportCoordinates[2] = yv;
    if (yv > minMaxViewportCoordinates[3])
      minMaxViewportCoordinates[3] = yv;
    }
  }
