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
#include <vector>

//----------------------------------------------------------------------------
// forward declarations
class albaGUIBusyInfo;
class albaGUIMainFrame;
class albaProgressBarHelperVTKCallback;
class vtkAlgorithm;

/** albaProgressBarHelper Is an helper for manage progress bar advance: it is the only class that drives
  the progress bar of the main window. Without a main window (e.g. in the tests) the bar is not shown.
  */
class ALBA_EXPORT albaProgressBarHelper
{
public:

	albaProgressBarHelper();
	virtual ~albaProgressBarHelper();

	/** Set the main window that shows the progress bar: the logic sets it when it creates the window and resets it (NULL) when it is destroyed */
	static void SetMainWindow(albaGUIMainFrame *win);

	/** Return the main window that shows the progress bar, NULL if none */
	static albaGUIMainFrame *GetMainWindow();

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

	/** Show the progress of the filter while this helper exists: the bar is opened when the filter starts (if it is not already open),
	updated while it runs and closed when it ends (if it was opened by the filter) */
	void BindToVTKFilter(vtkAlgorithm *filter);

	/** Return true if the progress bar has been initialized and not closed */
	bool IsInited() { return m_Inited; }

	/** Return the current progress */
	long GetProgress() { return m_Progress; }

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
	std::vector<albaProgressBarHelperVTKCallback *> m_VTKCallbacks;

	static albaGUIMainFrame *m_MainWindow;

private:
  albaProgressBarHelper(const albaProgressBarHelper&); // Not implemented
  void operator=(const albaProgressBarHelper&); // Not implemented
};

#endif
