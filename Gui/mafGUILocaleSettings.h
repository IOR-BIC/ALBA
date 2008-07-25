/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUILocaleSettings.h,v $
Language:  C++
Date:      $Date: 2008-07-25 06:53:38 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani - Daniele Giunchi
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUILocaleSettings_H__
#define __mafGUILocaleSettings_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUILocaleSettings :
//----------------------------------------------------------------------------
/**
*/
class mafGUILocaleSettings : public mafGUISettings
{
public:
	mafGUILocaleSettings(mafObserver *Listener, const mafString &label = _("Interface language"));
	~mafGUILocaleSettings(); 

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

  /*Function for set language directory*/
  void SetLanguageDirectory(const char* prefix, const char* languageDirectory);

  /* Function for set a different language*/
  void ChangeLanguage(wxLanguage languageEnum = wxLANGUAGE_ENGLISH , const char *languageAcronym = "en");

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
