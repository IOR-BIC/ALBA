/*=========================================================================

 Program: MAF2
 Module: mafGUIApplicationSettings
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIApplicationSettings_H__
#define __mafGUIApplicationSettings_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUIApplicationSettings :
//----------------------------------------------------------------------------
/**
   Class Name: mafGUIApplicationSettings.
   Class that contains several application options like: 
   - logging to file
   - logging verbosity
   - logging directory
   - Undo warning
   - passphrase for data encryption
   - default image type when saving a snapshot
*/
class MAF_EXPORT mafGUIApplicationSettings : public mafGUISettings
{
public:
  /** constructor .*/
	mafGUIApplicationSettings(mafObserver *Listener, const mafString &label = _("Application"));
  /** destructor .*/
	~mafGUIApplicationSettings(); 

  /** GUI IDs */
  enum APP_SETTINGS_WIDGET_ID
  {
    ID_LOG_TO_FILE = MINID,
    ID_LOG_DIR,
    ID_WARN_UNDO,
  };
  
  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Return true if Log To File is enabled.*/
  bool GetLogToFileStatus() {return m_LogToFile != 0;};

  /** Set the log status flag.*/
  void SetLogFileStatus(int log_status);

  
  /** Return the folder in which store the log files.*/
  mafString &GetLogFolder() {return m_LogFolder;};

  /** Set a folder name for the log file.*/
  void SetLogFolder(mafString log_folder);

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
  mafString m_LogFolder;///< Flag used to store the log folder. 

  int m_WarnUserFlag; ///< Warn user flag on not supported undo operations

  bool m_EnableLogDirChoices;
};
#endif
