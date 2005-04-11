/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafWXLog.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:19 $
  Version:   $Revision: 1.2 $
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
  TimeStamp(&msg);
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
void mafWXLog::SetFileName(wxString filename)
// ----------------------------------------------------------------------------
{
 	if(m_fp) fclose(m_fp);
  m_fp = NULL;
  m_fp = fopen(filename.c_str(),"w");
}
