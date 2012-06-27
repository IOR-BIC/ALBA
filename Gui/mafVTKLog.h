/*=========================================================================

 Program: MAF2
 Module: mafVTKLog
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVTKLog_h
#define __mafVTKLog_h

#include "vtkOutputWindow.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
class MAF_EXPORT mafVTKLog : public vtkOutputWindow
//----------------------------------------------------------------------------
{
public:

  static mafVTKLog *New();
  vtkTypeMacro(mafVTKLog, vtkOutputWindow);

  /** Set log message displaying. */
  void SetEnabled(int enabled){m_Enabled = enabled;this->Modified();};

  /** Get log message displaying. */
  int GetEnabled(){return m_Enabled;};

  /** Enable log message displaying. */
  void EnabledOn(){this->SetEnabled(TRUE);};

  /** Disable log message displaying. */
  void EnabledOff(){this->SetEnabled(FALSE);};

  /** Put the text into the log file. New lines are converted to carriage return new lines. */
  virtual void DisplayText(const char*);
  
protected:
  mafVTKLog();
  ~mafVTKLog();
  
  int m_Enabled;
  
private:
  mafVTKLog(const mafVTKLog&);       // Not implemented.
  void operator=(const mafVTKLog&);  // Not implemented.
};
#endif
