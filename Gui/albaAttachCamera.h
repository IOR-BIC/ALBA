/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttachCamera
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAttachCamera_H__
#define __albaAttachCamera_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include <ostream>
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

using namespace std;

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaRWI;
class albaVME;
class vtkMatrix4x4;

//----------------------------------------------------------------------------
// albaAttachCamera :
//----------------------------------------------------------------------------
/**
albaAttachCamera is tool to attach the camera present in albaRWI to the selected VME.
This tool has to be updated by calling UpdateCameraMatrix() during CameraUpdate into a view like albaViewVTK

@sa albaViewVTK for an example on how to use it, albaRWI

*/
class ALBA_EXPORT albaAttachCamera : public albaObserver, public albaServiceClient
{
public:

  /** Attach camera is disabled by default: use EnableAttachCamera() to enable attaching */
	albaAttachCamera(wxWindow* parent, albaRWI *rwi, albaObserver *Listener = NULL);
	~albaAttachCamera(); 
  
  /** Enable/Disable attach camera to vme */
  void EnableAttachCamera();
  void DisableAttachCamera();
  void SetEnableAttachCamera(int enable);
  int GetEnableAttachCamera();

  /** 
  Update the camera according to the absolute position of the attached VME.
  To be used in client view overridden CameraUpdate() method*/
  void UpdateCameraMatrix();

  /** Set the vme to follow with camera. 
  This method is optional; in alternative the vme to follow is set on the attach camera activation keeping the selected vme.*/
  void SetVme(albaVME *node);
  albaVME *GetVme();

  /**Set an initial transform matrix before attaching camera. 
  this method must be called before SetVme*/
  void SetStartingMatrix(albaMatrix *matrix);
  albaMatrix GetStartingMatrix();

	void OnEvent(albaEventBase *alba_event);
  
	void SetListener(albaObserver *Listener);
  albaObserver *GetListener();

	/** 
  Returns the albaAttachCamera's GUI */
	albaGUI *GetGui();

  void PrintSelf(ostream& os);

protected:
  
  /** 
  Create GUI for AttachCamera module.*/
  void CreateGui();

  int						 m_EnableAttachCamera; ///< Flag to turn On/Off the camera attaching on a particular VME
  albaVME				*m_AttachedVme; ///< VME on which the camera is attached when the attach-camera option is 'On'
  vtkMatrix4x4	*m_AttachedVmeMatrix; ///< Matrix given to the Camera to be moved together with m_AttachedVme
  vtkMatrix4x4	*m_StartingMatrix; ///< Matrix given to the Camera to be moved together with m_AttachedVme


  albaObserver	*m_Listener;
	albaGUI			*m_Gui;
	albaRWI      *m_RenderWindow;
	wxWindow	  *m_ParentPanel;
};
#endif
