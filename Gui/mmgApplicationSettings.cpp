/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationSettings.cpp,v $
Language:  C++
Date:      $Date: 2006-07-24 15:48:27 $
Version:   $Revision: 1.3 $
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

#include "mmgApplicationSettings.h"
#include <wx/config.h>
#include "mafDecl.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
mmgApplicationSettings::mmgApplicationSettings(mafObserver *Listener)
//----------------------------------------------------------------------------
{
	m_Listener    = Listener;
  
  // Default values for the application.
  m_LogToFile   = 0;
  m_VerboseLog  = 0;
  m_CacheFolder = wxGetCwd().c_str();
  m_LogFolder = wxGetCwd().c_str();
  
  InitializeApplicationSettings();

  m_Gui = new mmgGui(this);
  m_Gui->Label(_("Application general settings"));
  m_Gui->Bool(ID_LOG_TO_FILE,_("log to file"),&m_LogToFile,1);
  m_Gui->Bool(ID_LOG_VERBOSE,_("log verbose"),&m_VerboseLog,1);
  m_Gui->DirOpen(ID_LOD_DIR,_("log dir"),&m_LogFolder);
  m_Gui->Divider(2);
  m_Gui->DirOpen(ID_CACHE_FOLDER,"cache",&m_CacheFolder,_("set the local cache folder \nto put downloaded files"));
  m_Gui->Label(_("changes will take effect when \nthe application restart"),false,true);
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
mmgApplicationSettings::~mmgApplicationSettings() 
//----------------------------------------------------------------------------
{
  m_Gui = NULL; // gui is destroyed by the dialog.
}
//----------------------------------------------------------------------------
void mmgApplicationSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
  {
    case ID_LOG_TO_FILE:
    case ID_LOG_VERBOSE:
    case ID_LOD_DIR:
    case ID_CACHE_FOLDER:
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("LogToFile",m_LogToFile);
  config->Write("LogVerbose",m_VerboseLog);
  config->Write("CacheFolder",m_CacheFolder.GetCStr());
  config->Write("LogFolder",m_LogFolder.GetCStr());
  cppDEL(config);
}
//----------------------------------------------------------------------------
void mmgApplicationSettings::InitializeApplicationSettings()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  long log_flags;
  if(config->Read("LogToFile", &log_flags))
  {
    m_LogToFile = log_flags;
  }
  else
  {
    config->Write("LogToFile",m_LogToFile);
  }
  if(config->Read("LogVerbose", &log_flags))
  {
    m_VerboseLog = log_flags;
  }
  else
  {
    config->Write("LogVerbose",m_VerboseLog);
  }
  wxString cf;
  if(config->Read("LogFolder", &cf))
  {
    m_LogFolder = cf.c_str();
  }
  else
  {
    config->Write("LogFolder",m_LogFolder.GetCStr());
  }
  if(config->Read("CacheFolder", &cf))
  {
    m_CacheFolder = cf.c_str();
  }
  else
  {
    config->Write("CacheFolder",m_CacheFolder.GetCStr());
  }
  
  cppDEL(config);
}
