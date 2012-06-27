/*=========================================================================

 Program: MAF2
 Module: mafLODActor
 Authors: Paolo Quadrani & Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafLODActor_h
#define __mafLODActor_h

#include "vtkOpenGLActor.h"
class vtkPointSource;
class vtkPolyDataMapper;
class vtkActor;


class MAF_EXPORT mafLODActor : public vtkOpenGLActor
{
protected:
  
public:
  static mafLODActor *New();
  vtkTypeRevisionMacro(mafLODActor,vtkOpenGLActor);

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
  void EnableFadingOn(){this->SetEnableFading(TRUE);};

  /** Disable fade out and fade in for the actor when it switch between one LOD to another.*/
  void EnableFadingOff(){this->SetEnableFading(FALSE);};

  /** Return the status of the fading flag.*/
  int GetEnableFading(){return m_EnableFading;};
  
  /** Enable/Disable high threshold LOD.*/
  void SetEnableHighThreshold(int enableHighThreshold);

  /** Return the min value of EnableHighThreshold */
  int GetEnableHighThresholdMinValue(){return 0;};

  /** Return the max value of EnableHighThreshold */
  int GetEnableHighThresholdMaxValue(){return 1;};

  /** Enable high threshold LOD.*/
  void EnableHighThresholdOn(){this->SetEnableHighThreshold(TRUE);};

  /** Disable high threshold LOD.*/
  void EnableHighThresholdOff(){this->SetEnableHighThreshold(FALSE);};

  /** Return the status of the high threshold LOD flag.*/
  int GetEnableHighThreshold(){return m_EnableHighThreshold;};

protected:
  mafLODActor();
  ~mafLODActor();

private:
  mafLODActor(const mafLODActor&);  // Not implemented.
  void operator=(const mafLODActor&);  // Not implemented.

  vtkPointSource    *m_FlagShape;
  vtkPolyDataMapper *m_FlagMapper;
  vtkActor          *m_FlagActor;

  int m_PixelThreshold;
  int m_FlagDimension;
  int m_EnableFading;
  int m_EnableHighThreshold;
};
#endif
