/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipeVolumeRayCasting
 Authors: Alexander Savenko - Paolo Quadrani (porting ALBA.2)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVisualPipeVolumeRayCasting_H_
#define __albaVisualPipeVolumeRayCasting_H_

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
class vtkALBAAdaptiveVolumeMapper;

//----------------------------------------------------------------------------
// albaVisualPipeVolumeRayCasting :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaVisualPipeVolumeRayCasting : public albaPipe 
{
public:
  albaTypeMacro(albaVisualPipeVolumeRayCasting, albaPipe);

  albaVisualPipeVolumeRayCasting();
  virtual ~albaVisualPipeVolumeRayCasting();

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual albaGUI  *CreateGui();

  vtkALBAAdaptiveVolumeMapper  *m_Mapper; 
  vtkVolume *m_Volume;
  vtkActor  *m_Box;
};  
#endif // __albaVisualPipeVolumeRayCasting_H_
