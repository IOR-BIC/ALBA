/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafWXLog.cpp,v $
  Language:  C++
  Date:      $Date: 2008-10-17 11:51:19 $
  Version:   $Revision: 1.2.22.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafWXLog.h"
#include <wx/datetime.h>

// ----------------------------------------------------------------------------
mafWXLog::mafWXLog(wxTextCtrl *pTextCtrl)
// ----------------------------------------------------------------------------
{
  m_pTextCtrl = pTextCtrl;
  m_fp				= NULL;
	m_LogToFile = false; 
}
// ----------------------------------------------------------------------------
mafWXLog::~mafWXLog()
// ----------------------------------------------------------------------------
{
  if(m_fp) fclose(m_fp);
}
// ----------------------------------------------------------------------------
void mafWXLog::DoLogString(const wxChar *szString, time_t WXUNUSED(t))
// ----------------------------------------------------------------------------
{
  wxString msg;
  //TimeStamp(&msg);
  wxDateTime log_time = wxDateTime::UNow();
  msg = wxString::Format("%02d:%02d:%02d:%03d ",log_time.GetHour(), log_time.GetMinute(),log_time.GetSecond(), log_time.GetMillisecond());
  msg << szString << wxT('\n');
  m_pTextCtrl->AppendText(msg);
    
  if(m_fp && m_LogToFile)
  {
    fputs(msg.mb_str(), m_fp);
		fflush(m_fp);
  }		
}
// ----------------------------------------------------------------------------
void mafWXLog::LogToFile(bool on)
// ----------------------------------------------------------------------------
{
  m_LogToFile = on;
}
// ----------------------------------------------------------------------------
int mafWXLog::SetFileName(wxString filename)
// ----------------------------------------------------------------------------
{
 	if(m_fp) fclose(m_fp);
  m_fp = NULL;
  m_fp = fopen(filename.c_str(),"w");
  return m_fp != NULL ? MAF_OK : MAF_ERROR;
}
