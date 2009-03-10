/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAttachCamera.h,v $
  Language:  C++
  Date:      $Date: 2009-03-10 14:55:25 $
  Version:   $Revision: 1.7.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafAttachCamera_H__
#define __mafAttachCamera_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include <ostream>

using namespace std;

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafRWI;
class mafVME;
class vtkMatrix4x4;

//----------------------------------------------------------------------------
// mafAttachCamera :
//----------------------------------------------------------------------------
/**
mafAttachCamera is tool to attach the camera present in mafRWI to the selected VME.
This tool has to be updated by calling UpdateCameraMatrix() during CameraUpdate into a view like mafViewVTK

@sa mafViewVTK for an example on how to use it, mafRWI

*/
class mafAttachCamera : public mafObserver
{
public:

  /** Attach camera is disabled by default: use EnableAttachCamera() to enable attaching */
	mafAttachCamera(wxWindow* parent, mafRWI *rwi, mafObserver *Listener = NULL);
	~mafAttachCamera(); 
  
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
  void SetVme(mafNode *node);
  mafNode *GetVme();

  /**Set an initial transform matrix before attaching camera. 
  this method must be called before SetVme*/
  void SetStartingMatrix(mafMatrix *matrix);
  mafMatrix GetStartingMatrix();

	void OnEvent(mafEventBase *maf_event);
  
	void SetListener(mafObserver *Listener);
  mafObserver *GetListener();

	/** 
  Returns the mafAttachCamera's GUI */
	mafGUI *GetGui() {return m_Gui;};

  void PrintSelf(ostream& os);

protected:
  
  /** 
  Create GUI for AttachCamera module.*/
  void CreateGui();

  int						 m_EnableAttachCamera; ///< Flag to turn On/Off the camera attaching on a particular VME
  mafVME				*m_AttachedVme; ///< VME on which the camera is attached when the attach-camera option is 'On'
  vtkMatrix4x4	*m_AttachedVmeMatrix; ///< Matrix given to the Camera to be moved together with m_AttachedVme
  vtkMatrix4x4	*m_StartingMatrix; ///< Matrix given to the Camera to be moved together with m_AttachedVme


  mafObserver	*m_Listener;
	mafGUI			*m_Gui;
	mafRWI      *m_RenderWindow;
	wxWindow	  *m_ParentPanel;
};
#endif
