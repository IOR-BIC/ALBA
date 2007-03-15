/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewHTML.cpp,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:00 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani    Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafViewHTML.h"
//local include
#include <wx/image.h>
#include <wx/html/htmlwin.h>
#include <wx/fs_inet.h>

#include "mafDecl.h"
#include "mmgGui.h"
#include "mafRWI.h"

//from mafSceneGraph
#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafTagArray.h"

#include "mafVME.h"
#include "mafVMEExternalData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewHTML);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewHTML::mafViewHTML(const wxString &label, int camera_position, bool show_axes, bool show_grid, int stereo)
:mafView(label)
//----------------------------------------------------------------------------
{
  m_Html  = NULL;
  m_Url   = "http://www.cineca.it/index.html";
}
//----------------------------------------------------------------------------
mafViewHTML::~mafViewHTML()
//----------------------------------------------------------------------------
{	
 	cppDEL(m_Html);
  wxImage::RemoveHandler("JPEGHANDLER");
  /*
  m_Html -> WriteCustomization(wxConfig::Get());
  delete wxConfig::Set(NULL);
  */
}
//----------------------------------------------------------------------------
mafView *mafViewHTML::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
   mafViewHTML *v = new mafViewHTML(m_Label);
   v->m_Listener = Listener;
   v->m_Id = m_Id;
   v->Create();
	 return v;
}
//----------------------------------------------------------------------------
void mafViewHTML::Create()
//----------------------------------------------------------------------------
{
  wxJPEGHandler *jpegHandler = new wxJPEGHandler();
  jpegHandler->SetName("JPEGHANDLER");
  wxImage::AddHandler(jpegHandler);

	#if wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS
		wxFileSystem::AddHandler(new wxInternetFSHandler);
	#endif

	m_Html = new wxHtmlWindow(mafGetFrame());
  
	/*m_Html -> SetRelatedFrame(this, "HTML : %s");
	m_Html -> SetRelatedStatusBar(0);
	m_Html -> ReadCustomization(wxConfig::Get());
	m_Html -> LoadPage("test.htm");*/
  

	m_Win = m_Html;

  m_Rwi = new mafRWI(m_Win,ONE_LAYER);
  m_Rwi->SetListener(this);//SIL. 16-6-2004: 
  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
	m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;
}
//----------------------------------------------------------------------------
mafSceneGraph *mafViewHTML::GetSceneGraph()									  {return m_Sg;}
//----------------------------------------------------------------------------
mafRWIBase *mafViewHTML::GetDefaultRWI()											{ return m_Rwi->m_RwiBase;}
//----------------------------------------------------------------------------
void mafViewHTML::VmeSelect(mafNode *vme, bool select)					{ m_Sg->VmeSelect(vme, select);}
//----------------------------------------------------------------------------
void mafViewHTML::VmeAdd(mafNode *vme)													{ m_Sg->VmeAdd(vme);} 
//----------------------------------------------------------------------------
void mafViewHTML::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(vme == m_ActiveNote)
  {
    m_Html->SetPage("");
    m_ActiveNote = NULL;
  }
  
  m_Sg->VmeRemove(vme); 
}
//----------------------------------------------------------------------------
void mafViewHTML::VmeShow  (mafNode *vme, bool show)
//----------------------------------------------------------------------------
{ 
  if(show)
  {
    for(mafSceneNode *node = m_Sg->m_List; node; node=node->m_Next)
      mafEventMacro(mafEvent(this,VME_SHOW,node->m_Vme,false));    
  }
  else
    m_Html->SetPage("");
  
  m_Sg->VmeShow(vme, show); 
}
//----------------------------------------------------------------------------
void mafViewHTML::VmeCreatePipe(mafNode *vme) 
//----------------------------------------------------------------------------
{
  wxString body;
  mafNode *ExternalNote = NULL;
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
    m_Html->LoadPage(((mafVMEExternalData *)ExternalNote)->GetAbsoluteFileName().GetCStr());
  else
    m_Html->SetPage(body);

  m_ActiveNote = vme;
}
//----------------------------------------------------------------------------
mmgGui *mafViewHTML::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mmgGui(this);
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
void mafViewHTML::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
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
      mafEventMacro(*maf_event);
    break;
	}
}
//----------------------------------------------------------------------------
void mafViewHTML::OnLoad()
//----------------------------------------------------------------------------
{
	wxString wildc = "HTML files (*.htm;*.html)| *.htm;*.html";		//Added by Paolo 12-11-2003
	wxString p = wxFileSelector("Open HTML document", "", "", "", wildc);	//modified by Paolo 12-11-2003
	if (p != wxEmptyString)
	  m_Html->LoadPage(p);
}
//----------------------------------------------------------------------------
void mafViewHTML::OnForward()
//----------------------------------------------------------------------------
{
	if (!m_Html->HistoryForward()) 
    wxLogMessage("mafViewHTML: - forward failed");
}
//----------------------------------------------------------------------------
void mafViewHTML::OnBack()
//----------------------------------------------------------------------------
{
	if (!m_Html->HistoryBack()) 
    wxLogMessage("mafViewHTML: - back failed");
}
