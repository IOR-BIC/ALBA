/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProgressBarHelper
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaProgressBarHelper_h
#define __albaProgressBarHelper_h

#include "albaDefines.h"
#include "albaString.h"

//----------------------------------------------------------------------------
// forward declarations
class albaObserver;
class albaGUIBusyInfo;

/** albaProgressBarHelper Is an helper for manage progress bar advance
  */
class ALBA_EXPORT albaProgressBarHelper 
{
public:

	albaProgressBarHelper(albaObserver *listener);
	virtual ~albaProgressBarHelper();

	/**Init the Progress Bar, if label is not an empty string a busy info is created*/
	void InitProgressBar(wxString label="", bool showBusyCursor=true);
	
	/**Close the Progress Bar*/
	void CloseProgressBar();
	
	/** Updates the progress bar */
	void UpdateProgressBar(long progress);

	/** Resets the progress to zero **/
	void ResetProgress();

	/** Sets the progress bar text */
	void SetBarText(albaString text);

	/** Get TextMode */
	bool GetTextMode();

	/** Set Text Mode, By default is off */
	void SetTextMode(bool textMode);

protected:
 

	bool m_TextMode;
	bool m_Inited;
	long m_Progress;
	albaGUIBusyInfo *m_BusyInfo;
	wxBusyCursor *m_BusyCursor;
	albaObserver    *m_Listener;
	
private:
  albaProgressBarHelper(const albaProgressBarHelper&); // Not implemented
  void operator=(const albaProgressBarHelper&); // Not implemented
};

#endif
