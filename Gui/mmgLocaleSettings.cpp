/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgLocaleSettings.cpp,v $
Language:  C++
Date:      $Date: 2008-02-19 09:49:14 $
Version:   $Revision: 1.11 $
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

#include "mmgLocaleSettings.h"
#include <wx/intl.h>
#include "mmgGui.h"
#include "mmgDialog.h"

//----------------------------------------------------------------------------
mmgLocaleSettings::mmgLocaleSettings(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  m_EnableLanguage = false; //29-03-2007 disabling for bug #218
  m_LanguageId = 0;
  InitializeSettings();
}
//----------------------------------------------------------------------------
mmgLocaleSettings::~mmgLocaleSettings() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgLocaleSettings::CreateGui()
//----------------------------------------------------------------------------
{
  wxString lang_array[5] = {"English","French","German","Italian","Spanish"};

  m_Gui = new mmgGui(this);   
  m_Gui->Label(_("User Interface Language"));
  m_Gui->Radio(LANGUAGE_ID,"", &m_LanguageId,5,lang_array);
	m_Gui->Enable(LANGUAGE_ID,m_EnableLanguage); 
  m_Gui->Label(_("changes will take effect when \nthe application restart"),false,true);
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
void mmgLocaleSettings::OnEvent(mafEventBase *maf_event)
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
void mmgLocaleSettings::InitializeSettings()
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
  default: //wxLANGUAGE_ENGLISH;
    m_LanguageId =0; 
  }
}
