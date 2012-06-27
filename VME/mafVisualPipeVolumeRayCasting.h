/*=========================================================================

 Program: MAF2
 Module: mafVisualPipeVolumeRayCasting
 Authors: Alexander Savenko - Paolo Quadrani (porting MAF2.2)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVisualPipeVolumeRayCasting_H_
#define __mafVisualPipeVolumeRayCasting_H_

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
class vtkMAFAdaptiveVolumeMapper;

//----------------------------------------------------------------------------
// mafVisualPipeVolumeRayCasting :
//----------------------------------------------------------------------------
class MAF_EXPORT mafVisualPipeVolumeRayCasting : public mafPipe 
{
public:
  mafTypeMacro(mafVisualPipeVolumeRayCasting, mafPipe);

  mafVisualPipeVolumeRayCasting();
  virtual ~mafVisualPipeVolumeRayCasting();

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI  *CreateGui();

  vtkMAFAdaptiveVolumeMapper  *m_Mapper; 
  vtkVolume *m_Volume;
  vtkActor  *m_Box;
};  
#endif // __mafVisualPipeVolumeRayCasting_H_
