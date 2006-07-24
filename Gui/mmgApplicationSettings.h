/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationSettings.h,v $
Language:  C++
Date:      $Date: 2006-07-24 15:48:27 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgApplicationSettings_H__
#define __mmgApplicationSettings_H__

#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;

//----------------------------------------------------------------------------
// mmgApplicationSettings :
//----------------------------------------------------------------------------
/**
*/
class mmgApplicationSettings : public mafObserver
{
public:
	mmgApplicationSettings(mafObserver *Listener);
	~mmgApplicationSettings(); 

  enum APP_SETTINGS_WIDGET_ID
  {
    ID_LOG_TO_FILE = MINID,
    ID_LOG_VERBOSE,
    ID_LOD_DIR,
    ID_CACHE_FOLDER
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  mmgGui* GetGui() {return m_Gui;};

  /** Return true if Log To File is enabled.*/
  bool GetLogToFileStatus() {return m_LogToFile != 0;};

  /** Return true if Verbose Log is enabled.*/
  bool GetLogVerboseStatus() {return m_VerboseLog != 0;};

  /** Return the folder in which store the log files.*/
  mafString &GetLogFolder() {return m_LogFolder;};

  /** Return the path for the local cache in which download remote VMEs.*/
  mafString &GetCacheFolder() {return m_CacheFolder;};

protected:
  /** Initialize the application settings.*/
  void InitializeApplicationSettings();
  mmgGui *m_Gui;
  int     m_LogToFile;
  int     m_VerboseLog;
  mafString m_LogFolder;
  mafString m_CacheFolder;

  mafObserver *m_Listener;
};
#endif
