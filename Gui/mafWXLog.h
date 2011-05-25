/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafWXLog.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 10:12:13 $
  Version:   $Revision: 1.2.22.3 $
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
class MAF_EXPORT mafWXLog : public wxLog
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
  wxTextCtrl *m_PTextCtrl;
  FILE    	 *m_Fp;
  bool        m_LogToFile;
};
#endif
