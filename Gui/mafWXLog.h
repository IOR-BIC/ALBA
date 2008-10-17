/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafWXLog.h,v $
  Language:  C++
  Date:      $Date: 2008-10-17 11:50:33 $
  Version:   $Revision: 1.2.22.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafWXLog_H__
#define __mafWXLog_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafWXLog :
//----------------------------------------------------------------------------
/** */
class WXDLLEXPORT mafWXLog : public wxLog
{
public:
  mafWXLog(wxTextCtrl *pTextCtrl);
 ~mafWXLog();
  
  /** Set the filename for the log file. */ 
  int SetFileName(wxString filename);
  
  /** Set the flag to rodirect the log on a file. */ 
  void LogToFile(bool on);

private:
  /** Implement sink function. */
  virtual void DoLogString(const wxChar *szString, time_t t);

  // the control we use
  wxTextCtrl *m_pTextCtrl;
  FILE    	 *m_fp;
  bool        m_LogToFile;
};
#endif
