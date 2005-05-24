/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMaterialButton.h,v $
  Language:  C++
  Date:      $Date: 2005-05-24 14:36:58 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgMaterialButton_H__
#define __mmgMaterialButton_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mmgPicButton;
class mmaMaterial;
class mmgGui;

//----------------------------------------------------------------------------
// mmgMaterialButton :
//----------------------------------------------------------------------------
/**
This component allow to manage surface materials.
*/
class mmgMaterialButton : public mafObserver
{
public:
	mmgMaterialButton(wxWindow* parent, mafVME *vme, mafObserver *listener = NULL);
	~mmgMaterialButton(); 
	void OnEvent(mafEventBase *event);
	void SetListener(mafObserver *listener) {m_Listener = listener;};

	/** Return mmgMaterialButton User Interface */
	mmgGui *GetGui() {return m_Gui;};

  /** update material icon. */
	void UpdateMaterialIcon();

  /** Enable-Disable material button. */
  void Enable(bool enable);

  mmaMaterial *GetMaterial() {return m_Material;};

protected:
	/** Create the GUI for the materials */
	void CreateGui();

  mmgPicButton *m_MaterialButton;  // material pic button
  wxStaticText *m_MaterialLabel;   // material name label
  mmaMaterial  *m_Material;

  mafObserver *m_Listener;
	mmgGui			*m_Gui;
  mafVME      *m_Vme;
};
#endif