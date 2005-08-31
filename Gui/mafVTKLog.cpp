/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKLog.cpp,v $
  Language:  C++
  Date:      $Date: 2005-08-31 09:08:38 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
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

#include "mafVTKLog.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
mafVTKLog *mafVTKLog::New() 
//----------------------------------------------------------------------------
{
  vtkObject* ret = vtkObjectFactory::CreateInstance("mafVTKLog");
  if(ret)
    return (mafVTKLog*)ret;
  return new mafVTKLog;
}
//----------------------------------------------------------------------------
mafVTKLog::mafVTKLog() 
//----------------------------------------------------------------------------
{
  this->Enabled=1;
}
//----------------------------------------------------------------------------
mafVTKLog::~mafVTKLog() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVTKLog::DisplayText(const char* text)
//----------------------------------------------------------------------------
{
  if(!text)
  {
    return;
  }

  if (this->Enabled)
  {
    wxString message="[VTK]";
		message += text;

    // Strip CR
    for (;message.Last()=='\n';)
    {
      message.RemoveLast();
    }

    mafLogMessage(message.c_str());
  }
}
