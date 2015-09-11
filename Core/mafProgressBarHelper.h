/*=========================================================================

 Program: MAF2
 Module: mafProgressBarHelper
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafProgressBarHelper_h
#define __mafProgressBarHelper_h

#include "mafDefines.h"

//----------------------------------------------------------------------------
// forward declarations
class mafObserver;
class wxBusyInfo;

/** mafProgressBarHelper Is an helper for manage progress bar advance
  */
class MAF_EXPORT mafProgressBarHelper 
{
public:

	mafProgressBarHelper(mafObserver *listener);
	virtual ~mafProgressBarHelper();

	/**Init the Progress Bar, if label is not an empty string a busy info is created*/
	void InitProgressBar(wxString label="");
	
	/**Close the Progress Bar*/
	void CloseProgressBar();
	
	/** Updates the progress bar */
	void UpdateProgressBar(long progress);

	/** Get TextMode */
	bool GetTextMode();

	/** Set Text Mode, By default is off */
	void SetTextMode(bool textMode);

protected:
 

	bool m_TextMode;
	bool m_Inited;
	long m_Progress;
	wxBusyInfo *m_BusyInfo;
	mafObserver    *m_Listener;
	
private:
  mafProgressBarHelper(const mafProgressBarHelper&); // Not implemented
  void operator=(const mafProgressBarHelper&); // Not implemented
};

#endif
