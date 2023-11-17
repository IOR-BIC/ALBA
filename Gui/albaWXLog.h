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
#ifndef __albaWXLog_H__
#define __albaWXLog_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaWXLog :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaWXLog : public wxLog
{
public:
  albaWXLog(wxTextCtrl *pTextCtrl);
 ~albaWXLog();
  
  /** Set the filename for the log file. */ 
  int SetFileName(wxString filename);
  
  /** Set the flag to rodirect the log on a file. */ 
  void LogToFile(bool on);

private:
  /** Implement sink function. */
  virtual void DoLogText(const wxString& msg);

  // the control we use
  wxTextCtrl *m_PTextCtrl;
  FILE    	 *m_Fp;
  bool        m_LogToFile;
};
#endif
