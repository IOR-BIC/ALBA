/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIApplicationSettings
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIApplicationSettings_H__
#define __albaGUIApplicationSettings_H__

#include "albaGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaGUIApplicationSettings :
//----------------------------------------------------------------------------
/**
   Class Name: albaGUIApplicationSettings.
   Class that contains several application options like: 
   - logging to file
   - logging verbosity
   - logging directory
   - Undo warning
   - passphrase for data encryption
   - default image type when saving a snapshot
*/
class ALBA_EXPORT albaGUIApplicationSettings : public albaGUISettings
{
public:
  /** constructor .*/
	albaGUIApplicationSettings(albaObserver *Listener, const albaString &label = _("Application"));
  /** destructor .*/
	~albaGUIApplicationSettings(); 

  /** GUI IDs */
  enum APP_SETTINGS_WIDGET_ID
  {
    ID_LOG_TO_FILE = MINID,
    ID_LOG_DIR,
    ID_WARN_UNDO,
  };
  
  /** Answer to the messages coming from interface. */
  void OnEvent(albaEventBase *alba_event);

  /** Return true if Log To File is enabled.*/
  bool GetLogToFileStatus() {return m_LogToFile != 0;};

  /** Set the log status flag.*/
  void SetLogFileStatus(int log_status);

  
  /** Return the folder in which store the log files.*/
  albaString &GetLogFolder() {return m_LogFolder;};

  /** Set a folder name for the log file.*/
  void SetLogFolder(albaString log_folder);

  /** Return the status of the WarnUser flag.*/
  bool GetWarnUserFlag() {return m_WarnUserFlag != 0;};

  /** Enable the possibility to choose log directory. */
  void EnableLogDirChoices(){m_EnableLogDirChoices = true;};
  /** Disable the possibility to choose log directory. */
  void DisableLogDirChoices(){m_EnableLogDirChoices = false;};

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize the application settings.*/
  void InitializeSettings();

  /** Used to enable/disable items according to the current widgets state.*/
  void EnableItems();

  // Log variables
  int     m_LogToFile; ///< Flag used to store the On-Off state of log to file mechanism.
  albaString m_LogFolder;///< Flag used to store the log folder. 

  int m_WarnUserFlag; ///< Warn user flag on not supported undo operations

  bool m_EnableLogDirChoices;
};
#endif
