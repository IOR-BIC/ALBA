/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProgressBarHelper
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaProgressBarHelper.h"
#include "albaObserver.h"
#include "wx\busyinfo.h"


//----------------------------------------------------------------------------
albaProgressBarHelper::albaProgressBarHelper(albaObserver *listener)
{
	m_Inited=m_TextMode=false;
	m_Progress=0;
	m_BusyInfo=NULL;
	m_BusyCursor=NULL;
	m_Listener=listener;
}

//----------------------------------------------------------------------------
albaProgressBarHelper::~albaProgressBarHelper()
{
	if(m_Inited)
		CloseProgressBar();
}


//----------------------------------------------------------------------------
void albaProgressBarHelper::InitProgressBar(wxString label,bool showBusyCursor)
{
	if(m_Inited)
		CloseProgressBar();

	m_Progress = 0;
	m_Inited=true;

	if (m_TextMode == false)
	{
		if(!label.empty())
			m_BusyInfo = new wxBusyInfo(label);
		if(showBusyCursor)
			m_BusyCursor = new wxBusyCursor(); //wxHOURGLASS_CURSOR
		albaEventMacro(albaEvent(this,PROGRESSBAR_SHOW));
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

	if (m_TextMode == false)
	{
		cppDEL(m_BusyInfo);
		cppDEL(m_BusyCursor);
		albaEventMacro(albaEvent(this,PROGRESSBAR_HIDE));
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
			albaEventMacro(albaEvent(this,PROGRESSBAR_SET_VALUE,progress));
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
	albaEventMacro(albaEvent(this,PROGRESSBAR_SET_VALUE,(long)0.0));
}

//----------------------------------------------------------------------------
void albaProgressBarHelper::SetBarText(albaString text)
{
	if (m_TextMode == false)
	{
		albaEventMacro(albaEvent(this, PROGRESSBAR_SET_TEXT, &text));
	}
	else
	{
		printf("%s&c\n",text.GetCStr(), 179);
	}
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

