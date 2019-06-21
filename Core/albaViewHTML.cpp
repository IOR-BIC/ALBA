/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewHTML
 Authors: Paolo Quadrani    Silvano Imboden
 
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


#include "albaViewHTML.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEExternalData.h"

#include <wx/fs_inet.h>
#include <wx/html/htmlwin.h>
#include <wx/image.h>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewHTML);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewHTML::albaViewHTML(const wxString &label, int camera_position, bool show_axes, bool show_grid, int stereo)
:albaView(label)
{
  m_Html  = NULL;
  m_Url   = "http://www.cineca.it/index.html";
}
//----------------------------------------------------------------------------
albaViewHTML::~albaViewHTML()
{	
 	cppDEL(m_Html);
  wxImage::RemoveHandler("JPEGHANDLER");
  /*
  m_Html -> WriteCustomization(wxConfig::Get());
  delete wxConfig::Set(NULL);
  */
}

//----------------------------------------------------------------------------
albaView *albaViewHTML::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
   m_LightCopyEnabled = lightCopyEnabled;
   albaViewHTML *v = new albaViewHTML(m_Label);
   v->m_Listener = Listener;
   v->m_Id = m_Id;
   v->Create();
	 return v;
}
//----------------------------------------------------------------------------
void albaViewHTML::Create()
{
  wxJPEGHandler *jpegHandler = new wxJPEGHandler();
  jpegHandler->SetName("JPEGHANDLER");
  wxImage::AddHandler(jpegHandler);

	#if wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS
		wxFileSystem::AddHandler(new wxInternetFSHandler);
	#endif

	m_Html = new wxHtmlWindow(albaGetFrame());
  
	/*m_Html -> SetRelatedFrame(this, "HTML : %s");
	m_Html -> SetRelatedStatusBar(0);
	m_Html -> ReadCustomization(wxConfig::Get());
	m_Html -> LoadPage("test.htm");*/
  
	m_Win = m_Html;

  m_Rwi = new albaRWI(m_Win,ONE_LAYER);
  m_Rwi->SetListener(this);//SIL. 16-6-2004: 
  m_Sg  = new albaSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack,m_Rwi->m_AlwaysVisibleRenderer);
	m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;
}
//----------------------------------------------------------------------------
albaSceneGraph *albaViewHTML::GetSceneGraph()									  {return m_Sg;}
//----------------------------------------------------------------------------
albaRWIBase *albaViewHTML::GetDefaultRWI()											{ return m_Rwi->m_RwiBase;}
//----------------------------------------------------------------------------
void albaViewHTML::VmeSelect(albaVME *vme, bool select)					{ m_Sg->VmeSelect(vme, select);}
//----------------------------------------------------------------------------
void albaViewHTML::VmeAdd(albaVME *vme)													{ m_Sg->VmeAdd(vme);} 
//----------------------------------------------------------------------------
void albaViewHTML::VmeRemove(albaVME *vme)
{
  if(vme == m_ActiveNote)
  {
    m_Html->SetPage("");
    m_ActiveNote = NULL;
  }
  
  m_Sg->VmeRemove(vme); 
}
//----------------------------------------------------------------------------
void albaViewHTML::VmeShow  (albaVME *vme, bool show)
{ 
  if(show)
  {
    for(albaSceneNode *node = m_Sg->GetNodeList(); node; node=node->GetNext())
      GetLogicManager()->VmeShow(node->GetVme(), false);
  }
  else
    m_Html->SetPage("");
  
  m_Sg->VmeShow(vme, show); 
}
//----------------------------------------------------------------------------
void albaViewHTML::VmeCreatePipe(albaVME *vme) 
{
  wxString body;
  albaVME *ExternalNote = NULL;
  bool found = false;

  if(vme->GetTagArray()->IsTagPresent("HTML_INFO"))
    body = vme->GetTagArray()->GetTag("HTML_INFO")->GetValue();
  else
    return;

  for(int i = 0; i < vme->GetNumberOfChildren(); i++)
  {
    ExternalNote = vme->GetChild(i);
    if(ExternalNote->GetTagArray()->IsTagPresent("HTML_INFO"))
    {
      found = true;
      break;
    }
  }

  if(found)
    m_Html->LoadPage(((albaVMEExternalData *)ExternalNote)->GetAbsoluteFileName().GetCStr());
  else
    m_Html->SetPage(body);

  m_ActiveNote = vme;
}
//----------------------------------------------------------------------------
albaGUI *albaViewHTML::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

  m_Gui->SetListener(this);
	m_Gui->Label("");
	m_Gui->Button(ID_LOAD,"load html file");
	m_Gui->Label("");
  m_Gui->String(ID_URL,"url: ",&m_Url);
	m_Gui->Button(ID_BACK,"go back");
	m_Gui->Button(ID_FORWARD,"go forward");
  m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewHTML::OnEvent(albaEventBase *alba_event)
{
  switch(alba_event->GetId())
	{
		case ID_LOAD:
       OnLoad();
		break;
		case ID_BACK:
       OnBack();
		break;
		case ID_FORWARD:
       OnForward();
		break;
    case ID_URL:
      if(m_Url != wxEmptyString)
	      m_Html->LoadPage(m_Url);
    break;
    default:
      Superclass::OnEvent(alba_event);
    break;
	}
}
//----------------------------------------------------------------------------
void albaViewHTML::OnLoad()
{
	wxString wildc = "HTML files (*.htm;*.html)| *.htm;*.html";		//Added by Paolo 12-11-2003
	wxString p = wxFileSelector("Open HTML document", "", "", "", wildc);	//modified by Paolo 12-11-2003
	if (p != wxEmptyString)
	  m_Html->LoadPage(p);
}
//----------------------------------------------------------------------------
void albaViewHTML::OnForward()
{
	if (!m_Html->HistoryForward()) 
    wxLogMessage("albaViewHTML: - forward failed");
}
//----------------------------------------------------------------------------
void albaViewHTML::OnBack()
{
	if (!m_Html->HistoryBack()) 
    wxLogMessage("albaViewHTML: - back failed");
}

//----------------------------------------------------------------------------
void albaViewHTML::SetBackgroundColor(wxColor color)
{
	assert(m_Rwi);
	m_BackgroundColor = color;
	m_Rwi->SetBackgroundColor(color);
	m_Rwi->CameraUpdate();
}
