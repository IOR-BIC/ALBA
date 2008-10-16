/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUILocaleSettings.cpp,v $
Language:  C++
Date:      $Date: 2008-10-16 17:37:31 $
Version:   $Revision: 1.1.2.1 $
Authors:   Paolo Quadrani - Daniele Giunchi
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUILocaleSettings.h"
#include <wx/intl.h>
#include "mafGUI.h"
#include "mafGUIDialog.h"

//----------------------------------------------------------------------------
mafGUILocaleSettings::mafGUILocaleSettings(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  m_EnableLanguage = false; //29-03-2007 disabling for bug #218
  m_LanguageId = 0;
  InitializeSettings();
}
//----------------------------------------------------------------------------
mafGUILocaleSettings::~mafGUILocaleSettings() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::CreateGui()
//----------------------------------------------------------------------------
{
  wxString lang_array[8] = {"English","French","German","Italian","Spanish","Russian","Polish","Czech"};

  m_Gui = new mafGUI(this);   
  m_Gui->Label(_("User Interface Language"));
  m_Gui->Radio(LANGUAGE_ID,"", &m_LanguageId, 8,lang_array);
	m_Gui->Enable(LANGUAGE_ID,m_EnableLanguage); 
  m_Gui->Label(_("changes will take effect when \nthe application restart"),false,true);
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case LANGUAGE_ID:
    {
      switch(m_LanguageId) 
      {
      case 1:
        m_Language = wxLANGUAGE_FRENCH;
        m_LanguageDictionary = "fr";
        break;
      case 2:
        m_Language = wxLANGUAGE_GERMAN;
        m_LanguageDictionary = "de";
        break;
      case 3:
        m_Language = wxLANGUAGE_ITALIAN;
        m_LanguageDictionary = "it";
        break;
      case 4:
        m_Language = wxLANGUAGE_SPANISH;
        m_LanguageDictionary = "es";
        break;
	  case 5:
		m_Language = wxLANGUAGE_RUSSIAN;
        m_LanguageDictionary = "ru";
        break;
	  case 6:
		m_Language = wxLANGUAGE_POLISH;
        m_LanguageDictionary = "pl";
        break;
	  case 7:
		m_Language = wxLANGUAGE_CZECH;
		m_LanguageDictionary = "cs";
		break;
      default:
        m_Language = wxLANGUAGE_ENGLISH;
        m_LanguageDictionary = "en";
      }
      m_Config->Write("Language",m_Language);
      m_Config->Write("Dictionary",m_LanguageDictionary);
      m_Config->Flush();
    }
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
  long lang;
  wxString dict;
  if(m_Config->Read("Language", &lang))
  {
    m_Language = (wxLanguage)lang;
    m_Config->Read("Dictionary", &dict);
    m_LanguageDictionary = dict;
  }
  else
  {
    // no language set; use default language: English
    m_Config->Write("Language",wxLANGUAGE_ENGLISH);
    m_Config->Write("Dictionary","en");
    m_Language = wxLANGUAGE_ENGLISH;
    m_LanguageDictionary = "en";
  }

  m_Config->Flush();

  wxString prefix;
  char *tmp = wxGetWorkingDirectory();
  prefix = tmp;
  delete[] tmp;
  prefix += "\\Language\\";
  m_Locale.Init(m_Language);
  m_Locale.AddCatalogLookupPathPrefix(prefix);
  m_Locale.AddCatalog(wxT(m_LanguageDictionary.GetCStr()));
#ifndef WIN32
  m_Locale.AddCatalog("fileutils");
#endif

  switch(m_Language) 
  {
  case wxLANGUAGE_FRENCH:
    m_LanguageId =1;
    break;
  case wxLANGUAGE_GERMAN:
    m_LanguageId =2;
    break;
  case wxLANGUAGE_ITALIAN:
    m_LanguageId =3;
    break;
  case wxLANGUAGE_SPANISH:
    m_LanguageId =4;
    break;
  case wxLANGUAGE_RUSSIAN:
    m_LanguageId =5;
    break;
  case wxLANGUAGE_POLISH:
    m_LanguageId =6;
    break;
  case wxLANGUAGE_CZECH:
	m_LanguageId =7;
	break;
  default: //wxLANGUAGE_ENGLISH;
    m_LanguageId =0; 
  }
}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::SetLanguageDirectory(const char* prefix, const char* languageDirectory)
//----------------------------------------------------------------------------
{
  m_LanguageDictionary = wxT(languageDirectory);
  m_Locale.AddCatalogLookupPathPrefix(prefix);

  m_Locale.AddCatalog(wxT(m_LanguageDictionary.GetCStr()));
  if (strcmp(wxT(m_LanguageDictionary.GetCStr()), "fr") == 0)
  {
  	m_Language		= wxLANGUAGE_FRENCH;
	m_LanguageId	= 1;
  }
  else if (strcmp(wxT(m_LanguageDictionary.GetCStr()), "de") == 0)
  {
    m_Language		= wxLANGUAGE_GERMAN;
	m_LanguageId	= 2;
  }
  else if (strcmp(wxT(m_LanguageDictionary.GetCStr()), "it") == 0)
  {
	  m_Language	= wxLANGUAGE_ITALIAN;
	m_LanguageId	= 3; 
  }
  else if (strcmp(wxT(m_LanguageDictionary.GetCStr()), "es") == 0)
  {
    m_Language		= wxLANGUAGE_SPANISH;
	m_LanguageId	= 4;
  }
  else if (strcmp(wxT(m_LanguageDictionary.GetCStr()), "ru") == 0)
  {
    m_Language		= wxLANGUAGE_RUSSIAN;
	m_LanguageId	= 5;
  }
  else if (strcmp(wxT(m_LanguageDictionary.GetCStr()), "pl") == 0)
  {
    m_Language		= wxLANGUAGE_POLISH;
	m_LanguageId	= 6;
  }
  else if (strcmp(wxT(m_LanguageDictionary.GetCStr()), "cs") == 0)
  {
	m_Language		= wxLANGUAGE_CZECH;
	m_LanguageId	= 7;
  }
  else if (strcmp(wxT(m_LanguageDictionary.GetCStr()), "en") == 0)
  {
    m_Language		= wxLANGUAGE_ENGLISH;
	m_LanguageId	= 0;
  }

}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::ChangeLanguage(wxLanguage languageEnum, const char *languageAcronym)
//----------------------------------------------------------------------------
{
  m_Config->Write("Language",languageEnum);
  m_Config->Write("Dictionary",languageAcronym);
  m_Language = languageEnum;
  m_LanguageDictionary = languageAcronym;
}
