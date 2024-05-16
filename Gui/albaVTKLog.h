/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVTKLog
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVTKLog_h
#define __albaVTKLog_h

#include "vtkOutputWindow.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
class ALBA_EXPORT albaVTKLog : public vtkOutputWindow
//----------------------------------------------------------------------------
{
public:

  static albaVTKLog *New();
  vtkTypeMacro(albaVTKLog, vtkOutputWindow);

  /** Set log message displaying. */
  void SetEnabled(int enabled){m_Enabled = enabled;this->Modified();};

  /** Get log message displaying. */
  int GetEnabled(){return m_Enabled;};

  /** Enable log message displaying. */
  void EnabledOn(){this->SetEnabled(true);};

  /** Disable log message displaying. */
  void EnabledOff(){this->SetEnabled(false);};

  /** Put the text into the log file. New lines are converted to carriage return new lines. */
  virtual void DisplayText(const char*);
  
protected:
  albaVTKLog();
  ~albaVTKLog();
  
  int m_Enabled;
  
private:
  albaVTKLog(const albaVTKLog&);       // Not implemented.
  void operator=(const albaVTKLog&);  // Not implemented.
};
#endif
