/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMaterialButton
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIMaterialButton_H__
#define __albaGUIMaterialButton_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaEvent.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class albaGUIPicButton;
class mmaMaterial;
class albaGUI;

//----------------------------------------------------------------------------
// albaGUIMaterialButton :
//----------------------------------------------------------------------------
/**
This component allow to manage surface materials.
*/
class ALBA_EXPORT albaGUIMaterialButton : public albaObserver, public albaServiceClient
{
public:
	albaGUIMaterialButton(albaVME *vme, albaObserver *listener = NULL);
	~albaGUIMaterialButton(); 
	void OnEvent(albaEventBase *alba_event);
	void SetListener(albaObserver *listener) {m_Listener = listener;};

	/** Return albaGUIMaterialButton User Interface */
	albaGUI *GetGui() {return m_Gui;};

  /** update material icon. */
	void UpdateMaterialIcon();

  /** Enable-Disable material button. */
  void Enable(bool enable);

  mmaMaterial *GetMaterial() {return m_Material;};

	void SetVME(albaVME *vme);

protected:
	/** Create the GUI for the materials */
	void CreateGui();

  albaGUIPicButton *m_MaterialButton;  // material pic button
  wxStaticText *m_MaterialLabel;   // material name label
  mmaMaterial  *m_Material;

  albaObserver *m_Listener;
	albaGUI			*m_Gui;
  albaVME      *m_Vme;
};
#endif
