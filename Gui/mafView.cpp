/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-18 11:09:11 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden - Paolo Quadrani
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

#include "mafView.h"
#include "mafMatrix.h"
#include "mafVME.h"

#include "vtkMAFAssembly.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkRendererCollection.h"
#include "vtkAssemblyPath.h"
#include "vtkAssemblyNode.h"
#include "vtkProp3D.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafView);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafView::mafView( wxString label, bool external)
//----------------------------------------------------------------------------
{
	m_Label					= label;
  m_Name					= "";
	m_Win						= NULL;
  m_Frame					= NULL;
	m_Listener			= NULL;
	m_Next					= NULL;
  m_Guih					= NULL;
	m_Gui					 	= NULL;
	m_Plugged				= false;
  m_Id            = 0;
  m_Mult          = 0;
	m_ExternalFlag  = external;

  m_Picker2D          = NULL;
  m_Picker3D          = NULL;
  m_PickedVME         = NULL;
  m_PickedProp        = NULL;
  m_PickedPosition[0] = m_PickedPosition[1] = m_PickedPosition[2] = 0.0;
}
//----------------------------------------------------------------------------
mafView::~mafView()
//----------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//----------------------------------------------------------------------------
void mafView::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*maf_event);
}
/*
//----------------------------------------------------------------------------
void mafView::ShowGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui); 
	mafEvent settings_event(this,OP_SHOW_GUI,(wxWindow *)m_Gui);
	settings_event.SetBool(true);
	mafEventMacro(settings_event);
}
//----------------------------------------------------------------------------
void mafView::HideGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui); 
	mafEvent settings_event(this,OP_HIDE_GUI,(wxWindow *)m_Gui);
	settings_event.SetBool(true);
	mafEventMacro(settings_event);
}
*/
//-------------------------------------------------------------------------
void mafView::DeleteGui()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
mmgGui* mafView::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Label("view default gui");
  return m_Gui;
}
//-------------------------------------------------------------------------
bool mafView::Pick(int x, int y)
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
bool mafView::Pick(mafMatrix &m)
//-------------------------------------------------------------------------
{
  return false;
}
//----------------------------------------------------------------------------
void mafView::GetPickedPosition(double pos[3])
//----------------------------------------------------------------------------
{
  pos[0] = m_PickedPosition[0];
  pos[1] = m_PickedPosition[1];
  pos[2] = m_PickedPosition[2];
}
//----------------------------------------------------------------------------
void mafView::FindPickedVme(vtkAssemblyPath *ap)
//----------------------------------------------------------------------------
{
  vtkMAFAssembly *as = NULL;

  if(ap)
  {
    //scan the path from the leaf finding an assembly
    //which know the related vme.
    int pathlen = ap->GetNumberOfItems();
    for (int i=pathlen-1; i>=0; i--)
    {
      vtkAssemblyNode *an = (vtkAssemblyNode*)ap->GetItemAsObject(i);
      if (an)
      {
        vtkProp *p = an->GetProp();
        if(p && p->IsA("vtkMAFAssembly"))
        {
          as = (vtkMAFAssembly*)p;
          m_PickedVME = mafVME::SafeDownCast(as->GetVme());
          m_PickedProp = vtkProp3D::SafeDownCast(p);
          break;
        }
      }
    }
  }
}
