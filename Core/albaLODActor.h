/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLODActor
 Authors: Paolo Quadrani & Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaLODActor_h
#define __albaLODActor_h

#include "vtkOpenGLActor.h"
class vtkPointSource;
class vtkPolyDataMapper;
class vtkActor;


class ALBA_EXPORT albaLODActor : public vtkOpenGLActor
{
protected:
  
public:
  static albaLODActor *New();
  vtkTypeRevisionMacro(albaLODActor,vtkOpenGLActor);

  // Description:
  void Render(vtkRenderer *ren, vtkMapper *mapper);

  /** Set the dimension in pixels of the FlagActor representing the actor when its dimensions are too small to be rendered.*/
  void SetFlagDimension(int flagDimension);

  /** Return the min value of FlagDimension */
  int GetFlagDimensionMinValue(){return 1;};

  /** Return the max value of FlagDimension */
  int GetFlagDimensionMaxValue(){return VTK_INT_MAX;};
  
  /** Get the dimension in pixels of the FlagActor.*/
  int GetFlagDimension(){return m_FlagDimension;};

  /** Set the threshold in pixels to switch the actor's shape to the FlagActor representation. Accepted values are > 1*/
  void SetPixelThreshold(int pixelThreshold);

  /** Return the min value of PixelThreshold */
  int GetPixelThresholdMinValue(){return 1;};

  /** Return the max value of PixelThreshold */
  int GetPixelThresholdMaxValue(){return VTK_INT_MAX;};
  
  /** Get the threshold in pixels*/
  int GetPixelThreshold(){return m_PixelThreshold;};

  /** Enable/Disable fade out and fade in for the actor when it switch between one LOD to another.*/
  void SetEnableFading(int enableFading);

  /** Return the min value of EnableFading */
  int GetEnableFadingMinValue(){return 0;};

  /** Return the max value of EnableFading */
  int GetEnableFadingMaxValue(){return 1;};

  /** Enable fade out and fade in for the actor when it switch between one LOD to another.*/
  void EnableFadingOn(){this->SetEnableFading(true);};

  /** Disable fade out and fade in for the actor when it switch between one LOD to another.*/
  void EnableFadingOff(){this->SetEnableFading(false);};

  /** Return the status of the fading flag.*/
  int GetEnableFading(){return m_EnableFading;};
  
  /** Enable/Disable high threshold LOD.*/
  void SetEnableHighThreshold(int enableHighThreshold);

  /** Return the min value of EnableHighThreshold */
  int GetEnableHighThresholdMinValue(){return 0;};

  /** Return the max value of EnableHighThreshold */
  int GetEnableHighThresholdMaxValue(){return 1;};

  /** Enable high threshold LOD.*/
  void EnableHighThresholdOn(){this->SetEnableHighThreshold(true);};

  /** Disable high threshold LOD.*/
  void EnableHighThresholdOff(){this->SetEnableHighThreshold(false);};

  /** Return the status of the high threshold LOD flag.*/
  int GetEnableHighThreshold(){return m_EnableHighThreshold;};

protected:
  albaLODActor();
  ~albaLODActor();

private:
  albaLODActor(const albaLODActor&);  // Not implemented.
  void operator=(const albaLODActor&);  // Not implemented.

  vtkPointSource    *m_FlagShape;
  vtkPolyDataMapper *m_FlagMapper;
  vtkActor          *m_FlagActor;

  int m_PixelThreshold;
  int m_FlagDimension;
  int m_EnableFading;
  int m_EnableHighThreshold;
};
#endif
