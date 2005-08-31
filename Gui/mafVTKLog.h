/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKLog.h,v $
  Language:  C++
  Date:      $Date: 2005-08-31 09:08:39 $
  Version:   $Revision: 1.1 $
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

  /** Enable/Disable log message displaying. */
  vtkSetMacro(Enabled,int);

  /** Enable/Disable log message displaying. */
  vtkGetMacro(Enabled,int);

  /** Enable/Disable log message displaying. */
  vtkBooleanMacro(Enabled,int);

  /** Put the text into the log file. New lines are converted to carriage return new lines. */
  virtual void DisplayText(const char*);
  
protected:
  mafVTKLog();
  ~mafVTKLog();
  
  int Enabled;
  
private:
  mafVTKLog(const mafVTKLog&);       // Not implemented.
  void operator=(const mafVTKLog&);  // Not implemented.
};
#endif
