/*=========================================================================

 Program: MAF2
 Module: mafProgressBarHelper
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafProgressBarHelper.h"
#include "mafObserver.h"
#include "wx\busyinfo.h"


//----------------------------------------------------------------------------
mafProgressBarHelper::mafProgressBarHelper(mafObserver *listener)
{
	m_Inited=m_TextMode=false;
	m_Progress=0;
	m_BusyInfo=NULL;
	m_BusyCursor=NULL;
	m_Listener=listener;
}

//----------------------------------------------------------------------------
mafProgressBarHelper::~mafProgressBarHelper()
{
	if(m_Inited)
		CloseProgressBar();
}


//----------------------------------------------------------------------------
void mafProgressBarHelper::InitProgressBar(wxString label,bool showBusyCursor)
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
		mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
	}
	else
	{
		printf("\n" + label + "\n");
		printf("%c", 179);
	}
}

//----------------------------------------------------------------------------
void mafProgressBarHelper::CloseProgressBar()
{
	if (m_TextMode == false)
	{
		cppDEL(m_BusyInfo);
		cppDEL(m_BusyCursor);
		mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
	}
	else
	{
		printf("%c\n", 179);
	}

	m_Progress = 0;
	m_Inited=false;
}

//----------------------------------------------------------------------------
void mafProgressBarHelper::UpdateProgressBar(long progress)
{
	if (m_TextMode == false)
	{
		if(progress != m_Progress)
		{
			m_Progress = progress;
			mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
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
void mafProgressBarHelper::ResetProgress()
{
	mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)0.0));
}

//----------------------------------------------------------------------------
void mafProgressBarHelper::SetBarText(mafString text)
{
	if (m_TextMode == false)
	{
		mafEventMacro(mafEvent(this, PROGRESSBAR_SET_TEXT, &text));
	}
	else
	{
		printf("%s&c\n",text.GetCStr(), 179);
	}
}

//----------------------------------------------------------------------------
bool mafProgressBarHelper::GetTextMode()
{
	return m_TextMode;
}

//----------------------------------------------------------------------------
void mafProgressBarHelper::SetTextMode(bool textMode)
{
	m_TextMode=textMode;
}

