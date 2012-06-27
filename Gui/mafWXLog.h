/*=========================================================================

 Program: MAF2
 Module: mafWXLog
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
