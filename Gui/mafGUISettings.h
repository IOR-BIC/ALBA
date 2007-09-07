/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettings.h,v $
Language:  C++
Date:      $Date: 2007-09-07 15:47:42 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUISettings_H__
#define __mafGUISettings_H__

#include "mafObserver.h"
#include <wx/config.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;

//----------------------------------------------------------------------------
// mafGUISettings :
//----------------------------------------------------------------------------
/**
*/
class mafGUISettings : public mafObserver
{
public:
	mafGUISettings(mafObserver *Listener);
	~mafGUISettings(); 

  /** Answer to the messages coming from interface. */
  virtual void OnEvent(mafEventBase *maf_event);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Return the GUI of the setting panel.*/
  mmgGui* GetGui();

protected:
  /** Create the GUI for the setting panel.*/
  virtual void CreateGui();

  /** Initialize the settings.*/
  virtual void InitializeSettings();

  mmgGui *m_Gui;

  wxConfig *m_Config;
  
  mafObserver *m_Listener;
};
#endif
