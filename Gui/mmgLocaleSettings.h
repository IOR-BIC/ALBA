/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgLocaleSettings.h,v $
Language:  C++
Date:      $Date: 2007-09-28 15:04:50 $
Version:   $Revision: 1.6 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgLocaleSettings_H__
#define __mmgLocaleSettings_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mmgLocaleSettings :
//----------------------------------------------------------------------------
/**
*/
class mmgLocaleSettings : public mafGUISettings
{
public:
	mmgLocaleSettings(mafObserver *Listener, const mafString &label = _("Interface language"));
	~mmgLocaleSettings(); 

  enum LOCALE_WIDGET_ID
  {
    LANGUAGE_ID = MINID,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize language used into the application.*/
  void InitializeSettings();

  int          m_LanguageId;
  wxLocale     m_Locale;
  wxLanguage   m_Language;
  mafString    m_LanguageDictionary;
};
#endif
