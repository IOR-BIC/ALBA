/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMaterialButton.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUIMaterialButton_H__
#define __mafGUIMaterialButton_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafGUIPicButton;
class mmaMaterial;
class mafGUI;

//----------------------------------------------------------------------------
// mafGUIMaterialButton :
//----------------------------------------------------------------------------
/**
This component allow to manage surface materials.
*/
class mafGUIMaterialButton : public mafObserver
{
public:
	mafGUIMaterialButton(mafVME *vme, mafObserver *listener = NULL);
	~mafGUIMaterialButton(); 
	void OnEvent(mafEventBase *maf_event);
	void SetListener(mafObserver *listener) {m_Listener = listener;};

	/** Return mafGUIMaterialButton User Interface */
	mafGUI *GetGui() {return m_Gui;};

  /** update material icon. */
	void UpdateMaterialIcon();

  /** Enable-Disable material button. */
  void Enable(bool enable);

  mmaMaterial *GetMaterial() {return m_Material;};

protected:
	/** Create the GUI for the materials */
	void CreateGui();

  mafGUIPicButton *m_MaterialButton;  // material pic button
  wxStaticText *m_MaterialLabel;   // material name label
  mmaMaterial  *m_Material;

  mafObserver *m_Listener;
	mafGUI			*m_Gui;
  mafVME      *m_Vme;
};
#endif
