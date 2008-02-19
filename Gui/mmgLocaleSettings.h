/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgLocaleSettings.h,v $
Language:  C++
Date:      $Date: 2008-02-19 09:49:14 $
Version:   $Revision: 1.7 $
Authors:   Paolo Quadrani - Daniele Giunchi
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

  /** Functions that handle if in Gui, the language is Enable or Disable */
  void SetEnableLanguage(bool flag){m_EnableLanguage = flag;};
  bool GetEnableLanguage(){return m_EnableLanguage;};
  void EnableLanguageOn(){m_EnableLanguage = true;};
  void EnableLanguageOff(){m_EnableLanguage = false;};

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize language used into the application.*/
  void InitializeSettings();

  bool         m_EnableLanguage;
  int          m_LanguageId;
  wxLocale     m_Locale;
  wxLanguage   m_Language;
  mafString    m_LanguageDictionary;
};
#endif
