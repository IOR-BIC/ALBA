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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaVTKLog.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
albaVTKLog *albaVTKLog::New() 
//----------------------------------------------------------------------------
{
  vtkObject* ret = vtkObjectFactory::CreateInstance("albaVTKLog");
  if(ret)
    return (albaVTKLog*)ret;
  return new albaVTKLog;
}
//----------------------------------------------------------------------------
albaVTKLog::albaVTKLog() 
//----------------------------------------------------------------------------
{
  this->m_Enabled=1;
}
//----------------------------------------------------------------------------
albaVTKLog::~albaVTKLog() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVTKLog::DisplayText(const char* text)
//----------------------------------------------------------------------------
{
  if(!text)
  {
    return;
  }

  if (this->m_Enabled)
  {
    wxString message="[VTK]";
		message += text;

    // Strip CR
    for (;message.Last()=='\n';)
    {
      message.RemoveLast();
    }

    albaLogMessage(message.ToAscii());
  }
}
