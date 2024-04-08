/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaView
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include "albaView.h"
#include "albaIndent.h"
#include "albaMatrix.h"
#include "albaVME.h"
#include "albaGUI.h"

#include "vtkALBAAssembly.h"
#include "vtkALBARayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkRendererCollection.h"
#include "vtkAssemblyPath.h"
#include "vtkAssemblyNode.h"
#include "vtkProp3D.h"

#define DEFAULT_BG_COLOR 0.28


//----------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaView);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaView::albaView(const wxString &label)
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

  m_NumberOfVisibleVme = 0;

  m_PrintData = (wxPrintData*) NULL ;

  m_Picker2D          = NULL;
  m_Picker3D          = NULL;
  m_PickedVME         = NULL;
  m_PickedProp        = NULL;
  m_PickedPosition[0] = m_PickedPosition[1] = m_PickedPosition[2] = 0.0;

	m_BackgroundColor = wxColour(DEFAULT_BG_COLOR * 255, DEFAULT_BG_COLOR * 255, DEFAULT_BG_COLOR * 255);

	m_CanSpin = true;
  m_HTMLText = "";
  m_LightCopyEnabled = false;
}
//----------------------------------------------------------------------------
albaView::~albaView()
{
  cppDEL(m_Gui);
}

//----------------------------------------------------------------------------
void albaView::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
      case ID_PRINT_INFO:
      {
        std::stringstream ss1;
        Print(ss1);
        ss1 << std::ends;  
        albaLogMessage("[VIEW PRINTOUT:]\n%s\n", ss1.str()); 
      }
      break;
		
	  case ID_HELP:
	  {
			albaEvent helpEvent;
			helpEvent.SetSender(this);
			albaString viewTypeName = this->GetTypeName();
			helpEvent.SetString(&viewTypeName);
			helpEvent.SetId(OPEN_HELP_PAGE);
			albaEventMacro(helpEvent);
	  }
	  break;

      default:
        albaEventMacro(*alba_event);
    }
  }
  else
  {
    albaEventMacro(*alba_event);
  }
}

//-------------------------------------------------------------------------
void albaView::DeleteGui()
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
albaGUI* albaView::CreateGui()
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

  albaString type_name = GetTypeName();

  albaEvent buildHelpGui;
  buildHelpGui.SetSender(this);
	buildHelpGui.SetString(&type_name);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  albaEventMacro(buildHelpGui);

	
	if (buildHelpGui.GetArg() == true)
		m_Gui->ButtonAndHelp(ID_PRINT_INFO, ID_HELP, m_Label, "Print view debug information");
	else
		m_Gui->Button(ID_PRINT_INFO, m_Label, "", "Print view debug information");

	m_Gui->Divider(1);

  return m_Gui;
}

//-------------------------------------------------------------------------
bool albaView::Pick(int x, int y)
{
  return false;
}
//-------------------------------------------------------------------------
bool albaView::Pick(albaMatrix &m)
{
  return false;
}
//----------------------------------------------------------------------------
void albaView::GetPickedPosition(double pos[3])
{
  pos[0] = m_PickedPosition[0];
  pos[1] = m_PickedPosition[1];
  pos[2] = m_PickedPosition[2];
}
//----------------------------------------------------------------------------
bool albaView::FindPickedVme(vtkAssemblyPath *ap)
{
  vtkALBAAssembly *as = NULL;

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
        vtkProp *p = an->GetViewProp();
        if(p && p->IsA("vtkALBAAssembly"))
        {
          as = (vtkALBAAssembly*)p;
          m_PickedVME = as->GetVme();
          m_PickedProp = vtkProp3D::SafeDownCast(p);
          return true;
          break;
        }
      }
    }
  }
  return false;
}

//-------------------------------------------------------------------------
void albaView::Print(std::ostream& os, const int tabs)// const
{
  albaIndent indent(tabs);
  os << indent << "albaView" << '\t' << this << "\n";
}

//----------------------------------------------------------------------------
char ** albaView::GetIcon()
{
#include "pic/VIEW.xpm"
		return VIEW_xpm;
}
