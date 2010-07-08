/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKLog.h,v $
  Language:  C++
  Date:      $Date: 2010-07-08 15:40:49 $
  Version:   $Revision: 1.1.22.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVTKLog_h
#define __mafVTKLog_h

#include "vtkOutputWindow.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
class mafVTKLog : public vtkOutputWindow
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
