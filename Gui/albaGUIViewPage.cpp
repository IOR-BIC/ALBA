/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIViewPage
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaGUIViewPage.h"
#include "albaGUIViewArea.h"
#include "albaServiceLocator.h"
#include "albaAbsViewManager.h"
#include "albaDecl.h"
#include "albaView.h"

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIViewPage, wxPanel)
	EVT_CLOSE(albaGUIViewPage::OnCloseWindow)
	EVT_SIZE(albaGUIViewPage::OnSize)
	EVT_BUTTON(VIEW_CLICKED, albaGUIViewPage::OnSelect)
END_EVENT_TABLE()

bool albaGUIViewPage::m_Quitting = false;

//----------------------------------------------------------------------------
albaGUIViewPage::albaGUIViewPage(albaGUIViewArea *area, albaView *view)
	: wxPanel(area, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN)
{
	assert(view);
	m_View = view;
	m_ViewArea = area;
	m_AllowCloseFlag = true;

	Show(false);
	m_Win = m_View->GetWindow();
	m_Win->Reparent(this);
	m_Win->Show(true);
}
//----------------------------------------------------------------------------
albaGUIViewPage::~albaGUIViewPage()
{
}
//----------------------------------------------------------------------------
void albaGUIViewPage::OnSelect(wxCommandEvent &event)
{
	// a click on the view makes it the active one, also with the right button
	m_ViewArea->ActivateView(this);
}
//----------------------------------------------------------------------------
void albaGUIViewPage::OnSize(wxSizeEvent &event)
{
	int w, h;
	GetClientSize(&w, &h);
	if (w <= 0 || h <= 0 || !m_View)
		return;

	m_Win->SetSize(0, 0, w, h);
	m_Win->Layout();
	m_View->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaGUIViewPage::OnCloseWindow(wxCloseEvent &event)
{
	if (!m_AllowCloseFlag && event.CanVeto())
	{
		event.Veto();
		return;
	}

	// the view must be deleted from here and not from the destructor,
	// otherwise it is deleted also on the closing of the application,
	// when the view manager has been already destroyed
	if (m_View && albaServiceLocator::GetViewManager())
		albaServiceLocator::GetViewManager()->ViewDelete(m_View);
	m_View = NULL;
	m_ViewArea->RemoveView(this);
}
