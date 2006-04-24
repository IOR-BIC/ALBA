/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewHTML.cpp,v $
  Language:  C++
  Date:      $Date: 2006-04-24 08:14:55 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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
#include "wxMozillaBrowser.h"
#include "wxMozillaSettings.h"

#include "mafVME.h"

#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewHTML);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewHTML::mafViewHTML(wxString label, bool external)
:mafView(label,external)
//----------------------------------------------------------------------------
{
  m_ExternalFlag    = external;
  m_MozillaBrowser  = NULL;
}
//----------------------------------------------------------------------------
mafViewHTML::~mafViewHTML() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafView *mafViewHTML::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewHTML *v = new mafViewHTML(m_Label, m_ExternalFlag);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewHTML::Create()
//----------------------------------------------------------------------------
{
/*  wxConfig *config = new wxConfig("mozilla.org\\GRE\\wxMozilla");
  if(!config->Read("GreHome"))
  {
    config->Write("GreHome",);
  }*/
  wxString cd = wxGetCwd();
  wxMozillaSettings::SetMozillaPath(cd);
  m_MozillaBrowser = new wxMozillaBrowser(mafGetFrame(),-1,wxPoint(0,0),wxSize(100,100));
  m_MozillaBrowser->Show(TRUE);
  //m_MozillaBrowser->LoadURL("www.google.com");
  m_Win = m_MozillaBrowser;
}
//----------------------------------------------------------------------------
void mafViewHTML::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafViewHTML::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafViewHTML::VmeShow(mafNode *vme, bool show)												{}
void mafViewHTML::VmeUpdateProperty(mafNode *vme, bool fromTag)	        {}
//----------------------------------------------------------------------------
int  mafViewHTML::GetNodeStatus(mafNode *vme)
//----------------------------------------------------------------------------
{
  return NODE_NON_VISIBLE;
}
//----------------------------------------------------------------------------
void mafViewHTML::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafViewHTML::VmeSelect(mafNode *vme, bool select)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafViewHTML::GetVisualPipeName(mafNode *node, mafString &pipe_name)
//----------------------------------------------------------------------------
{
  assert(node->IsA("mafVME"));
  mafVME *v = ((mafVME*)node);

  mafString vme_type = v->GetTypeName();
  pipe_name = v->GetVisualPipe();
}
//----------------------------------------------------------------------------
void mafViewHTML::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafViewHTML::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mmgGui *mafViewHTML::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewHTML::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*maf_event);
}
//----------------------------------------------------------------------------
void mafViewHTML::SetWindowSize(int w, int h)
//----------------------------------------------------------------------------
{
}
