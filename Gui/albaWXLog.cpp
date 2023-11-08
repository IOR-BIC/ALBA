/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWXLog
 Authors: Silvano Imboden
 
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
#include "albaWXLog.h"
#include <wx/datetime.h>

// ----------------------------------------------------------------------------
albaWXLog::albaWXLog(wxTextCtrl *pTextCtrl)
// ----------------------------------------------------------------------------
{
  m_PTextCtrl = pTextCtrl;
  m_Fp				= NULL;
	m_LogToFile = false; 
}
// ----------------------------------------------------------------------------
albaWXLog::~albaWXLog()
// ----------------------------------------------------------------------------
{
  if(m_Fp) fclose(m_Fp);
}
// ----------------------------------------------------------------------------
void albaWXLog::DoLogText(const wxString& msg)
// ----------------------------------------------------------------------------
{
  wxString printMsg;
  //TimeStamp(&msg);
  wxDateTime log_time = wxDateTime::UNow();
	printMsg.Format("%02d:%02d:%02d:%03d ",log_time.GetHour(), log_time.GetMinute(),log_time.GetSecond(), log_time.GetMillisecond());
	printMsg += msg;

	m_PTextCtrl->AppendText(printMsg);
    
  if(m_Fp && m_LogToFile)
  {
    fputs(msg.mb_str(), m_Fp);
		fflush(m_Fp);
  }		
}
// ----------------------------------------------------------------------------
void albaWXLog::LogToFile(bool on)
// ----------------------------------------------------------------------------
{
  m_LogToFile = on;
}
// ----------------------------------------------------------------------------
int albaWXLog::SetFileName(wxString filename)
// ----------------------------------------------------------------------------
{
 	if(m_Fp) fclose(m_Fp);
  m_Fp = NULL;
  m_Fp = fopen(filename.ToAscii(),"w");
  return m_Fp != NULL ? ALBA_OK : ALBA_ERROR;
}
