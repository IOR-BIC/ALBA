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

#include "albaProgressBarHelper.h"
#include "albaGUIBusyInfo.h"
#include "albaGUIMainFrame.h"

#include "vtkAlgorithm.h"
#include "vtkCommand.h"

albaGUIMainFrame *albaProgressBarHelper::m_MainWindow = NULL;

//----------------------------------------------------------------------------
/** Moves the progress bar of a helper on the start, progress and end events of a vtk filter.
  The filter keeps a reference to the callback, so the helper only detaches itself when it is destroyed. */
class albaProgressBarHelperVTKCallback : public vtkCommand
{
public:
	vtkTypeMacro(albaProgressBarHelperVTKCallback, vtkCommand);
	static albaProgressBarHelperVTKCallback *New() { return new albaProgressBarHelperVTKCallback; }

	void SetHelper(albaProgressBarHelper *helper) { m_Helper = helper; }

	virtual void Execute(vtkObject *caller, unsigned long eventId, void *)
	{
		vtkAlgorithm *filter = vtkAlgorithm::SafeDownCast(caller);
		if (m_Helper == NULL || filter == NULL)
			return;

		if (eventId == vtkCommand::StartEvent)
		{
			if (!m_Helper->IsInited())
			{
				m_Helper->InitProgressBar("", false);
				m_Opened = true;
			}
			if (filter->GetProgressText())
				m_Helper->SetBarText(filter->GetProgressText());
		}
		else if (eventId == vtkCommand::ProgressEvent)
		{
			if (albaProgressBarHelper::GetMainWindow())
				::wxSafeYield(NULL, true);
			m_Helper->UpdateProgressBar(filter->GetProgress() * 100);
		}
		else if (eventId == vtkCommand::EndEvent && m_Opened)
		{
			m_Helper->CloseProgressBar();
			m_Opened = false;
		}
	}

protected:
	albaProgressBarHelperVTKCallback() { m_Helper = NULL; m_Opened = false; }

	albaProgressBarHelper *m_Helper;
	bool m_Opened; ///< true if the bar was opened by the filter start
};

//----------------------------------------------------------------------------
albaProgressBarHelper::albaProgressBarHelper()
{
	m_Inited=m_TextMode=false;
	m_Progress=0;
	m_BusyInfo=NULL;
	m_BusyCursor=NULL;
}

//----------------------------------------------------------------------------
albaProgressBarHelper::~albaProgressBarHelper()
{
	for (int i = 0; i < m_VTKCallbacks.size(); i++)
	{
		m_VTKCallbacks[i]->SetHelper(NULL);
		m_VTKCallbacks[i]->Delete();
	}
	m_VTKCallbacks.clear();

	if(m_Inited)
		CloseProgressBar();
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::SetMainWindow(albaGUIMainFrame *win)
{
	m_MainWindow = win;
}

//----------------------------------------------------------------------------
albaGUIMainFrame *albaProgressBarHelper::GetMainWindow()
{
	return m_MainWindow;
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::InitProgressBar(wxString label,bool showBusyCursor)
{
	if(m_Inited)
		CloseProgressBar();

	m_Progress = 0;
	m_Inited=true;

	if(!label.IsEmpty())
		m_BusyInfo = new albaGUIBusyInfo(label,m_TextMode);

	if (m_TextMode == false)
	{
		if(showBusyCursor)
			m_BusyCursor = new wxBusyCursor(); //wxHOURGLASS_CURSOR
		if (m_MainWindow)
			m_MainWindow->ProgressBarShow();
	}
	else
	{
		printf("\n" + label + "\n");
		printf("%c", 179);
	}
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::CloseProgressBar()
{
	UpdateProgressBar(100);

	cppDEL(m_BusyInfo);

	if (m_TextMode == false)
	{
		cppDEL(m_BusyCursor);
		if (m_MainWindow)
			m_MainWindow->ProgressBarHide();
	}
	else
	{
		printf("%c\n", 179);
	}

	m_Progress = 0;
	m_Inited=false;
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::UpdateProgressBar(long progress)
{
	if (progress > 100)
	{
		albaErrorMacro("Wrong progress");
		return;
	}

	if (m_TextMode == false)
	{
		if(progress != m_Progress)
		{
			m_Progress = progress;
			if (m_MainWindow)
				m_MainWindow->ProgressBarSetVal(progress);
		}
	}
	else
	{
		while(progress - m_Progress > 2)
		{
			m_Progress += 2;
			printf("%c", 177);
		}
	}
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::ResetProgress()
{
	m_Progress = 0;
	if (m_TextMode == false && m_MainWindow)
		m_MainWindow->ProgressBarSetVal(0);
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::SetBarText(albaString text)
{
	if (m_TextMode == false)
	{
		if (m_MainWindow)
		{
			wxString msg = text.GetCStr();
			m_MainWindow->ProgressBarSetText(&msg);
		}
	}
	else
	{
		printf("%s&c\n",text.GetCStr(), 179);
	}
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::BindToVTKFilter(vtkAlgorithm *filter)
{
	if (filter == NULL)
		return;

	albaProgressBarHelperVTKCallback *callback = albaProgressBarHelperVTKCallback::New();
	callback->SetHelper(this);
	filter->AddObserver(vtkCommand::StartEvent, callback);
	filter->AddObserver(vtkCommand::ProgressEvent, callback);
	filter->AddObserver(vtkCommand::EndEvent, callback);
	m_VTKCallbacks.push_back(callback); // the reference of New is released in the destructor
}

//----------------------------------------------------------------------------
bool albaProgressBarHelper::GetTextMode()
{
	return m_TextMode;
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::SetTextMode(bool textMode)
{
	m_TextMode=textMode;
}
