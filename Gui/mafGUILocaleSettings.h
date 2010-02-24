/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUILocaleSettings.h,v $
Language:  C++
Date:      $Date: 2010-02-24 09:36:52 $
Version:   $Revision: 1.1.2.1 $
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

/**
  class name :mafGUILocaleSettings 
  class that handle gui panel for locale settings inside application settings.
*/
class mafGUILocaleSettings : public mafGUISettings
{
public:
  /** constructor */
	mafGUILocaleSettings(mafObserver *Listener, const mafString &label = _("Interface language"));
  /** destructor */
	~mafGUILocaleSettings(); 

  enum LOCALE_WIDGET_ID
  {
    LANGUAGE_ID = MINID,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Setter for Language Flag  */
  void SetEnableLanguage(bool flag){m_EnableLanguage = flag;};
  /** Getter for Language Flag  */
  bool GetEnableLanguage(){return m_EnableLanguage;};
  /** Enable Language Flag */
  void EnableLanguageOn(){m_EnableLanguage = true;};
  /** Disable Language Flag */
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
