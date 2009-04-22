/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafWXLog.cpp,v $
  Language:  C++
  Date:      $Date: 2009-04-22 09:43:04 $
  Version:   $Revision: 1.2.22.2 $
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
  m_PTextCtrl = pTextCtrl;
  m_Fp				= NULL;
	m_LogToFile = false; 
}
// ----------------------------------------------------------------------------
mafWXLog::~mafWXLog()
// ----------------------------------------------------------------------------
{
  if(m_Fp) fclose(m_Fp);
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
  m_PTextCtrl->AppendText(msg);
    
  if(m_Fp && m_LogToFile)
  {
    fputs(msg.mb_str(), m_Fp);
		fflush(m_Fp);
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
 	if(m_Fp) fclose(m_Fp);
  m_Fp = NULL;
  m_Fp = fopen(filename.c_str(),"w");
  return m_Fp != NULL ? MAF_OK : MAF_ERROR;
}
