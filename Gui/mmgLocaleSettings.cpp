/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgLocaleSettings.cpp,v $
Language:  C++
Date:      $Date: 2006-03-30 10:47:13 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
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
#include <wx/config.h>
#include "mmgGui.h"
#include "mmgDialog.h"

//----------------------------------------------------------------------------
mmgLocaleSettings::mmgLocaleSettings(mafObserver *Listener)
//----------------------------------------------------------------------------
{
	m_Listener   = Listener;
  m_LanguageId = 0;
}
//----------------------------------------------------------------------------
mmgLocaleSettings::~mmgLocaleSettings() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgLocaleSettings::ChooseLocale()
//----------------------------------------------------------------------------
{
  wxString lang_array[5] = {"English","French","German","Italian","Spanish"};
  int current_lang_id = m_LanguageId;

  mmgGui *gui = new mmgGui(this);
  gui->Combo(LANGUAGE_ID,"language", &m_LanguageId,5,lang_array);

  mmgDialog dlg("Settings",mafOK | mafCANCEL);
  dlg.Add(gui,1,wxEXPAND);
  int answere = dlg.ShowModal();
  if (answere == wxID_CANCEL)
  {
    m_LanguageId = current_lang_id;
    return;
  }
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
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("Language",m_Language);
  config->Write("Dictionary",m_LanguageDictionary);
  cppDEL(config);

  wxString msg = _("Restart application to make new settings available!");
  wxString caption = _("Warning");
  wxMessageBox(msg, caption);
}
//----------------------------------------------------------------------------
void mmgLocaleSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
  {
    case LANGUAGE_ID:
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
}
//----------------------------------------------------------------------------
void mmgLocaleSettings::InitializeLanguage()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  long lang;
  wxString dict;
  if(config->Read("Language", &lang))
  {
    m_Language = (wxLanguage)lang;
    config->Read("Dictionary", &dict);
    m_LanguageDictionary = dict;
  }
  else
  {
    // no language set; use default language: English
    config->Write("Language",wxLANGUAGE_ENGLISH);
    config->Write("Dictionary","en");
    m_Language = wxLANGUAGE_ENGLISH;
    m_LanguageDictionary = "en";
  }
  cppDEL(config);

  wxString prefix;
  prefix = wxGetWorkingDirectory();
  prefix += "\\Language\\";
  m_Locale.Init(m_Language);
  m_Locale.AddCatalogLookupPathPrefix(prefix);
  m_Locale.AddCatalog(wxT(m_LanguageDictionary.GetCStr()));
#ifndef WIN32
  m_Locale.AddCatalog("fileutils");
#endif
}
