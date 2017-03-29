/*=========================================================================

 Program: MAF2
 Module: mafView
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafView.h"
#include "mafIndent.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafGUI.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkRendererCollection.h"
#include "vtkAssemblyPath.h"
#include "vtkAssemblyNode.h"
#include "vtkProp3D.h"

#include <wx/print.h>

//----------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafView);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafView::mafView(const wxString &label)
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
	 
  m_HTMLText = "";
  m_LightCopyEnabled = false;
}
//----------------------------------------------------------------------------
mafView::~mafView()
{
  cppDEL(m_Gui);
}

//----------------------------------------------------------------------------
void mafView::OnEvent(mafEventBase *maf_event)
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_PRINT_INFO:
      {
        std::strstream ss1;
        Print(ss1);
        ss1 << std::ends;  
        mafLogMessage("[VIEW PRINTOUT:]\n%s\n", ss1.str()); 
      }
      break;
		
	  case ID_HELP:
	  {
			mafEvent helpEvent;
			helpEvent.SetSender(this);
			mafString viewTypeName = this->GetTypeName();
			helpEvent.SetString(&viewTypeName);
			helpEvent.SetId(OPEN_HELP_PAGE);
			mafEventMacro(helpEvent);
	  }
	  break;

      default:
        mafEventMacro(*maf_event);
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}

//-------------------------------------------------------------------------
void mafView::DeleteGui()
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
mafGUI* mafView::CreateGui()
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
	m_Gui->SetListener(this);

  mafString type_name = GetTypeName();

  mafEvent buildHelpGui;
  buildHelpGui.SetSender(this);
	buildHelpGui.SetString(&type_name);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  mafEventMacro(buildHelpGui);

	
	if (buildHelpGui.GetArg() == true)
		m_Gui->ButtonAndHelp(ID_PRINT_INFO, ID_HELP, m_Label, "Print view debug information");
	else
		m_Gui->Button(ID_PRINT_INFO, m_Label, "", "Print view debug information");

	m_Gui->Divider(1);

  return m_Gui;
}

//-------------------------------------------------------------------------
bool mafView::Pick(int x, int y)
{
  return false;
}
//-------------------------------------------------------------------------
bool mafView::Pick(mafMatrix &m)
{
  return false;
}
//----------------------------------------------------------------------------
void mafView::GetPickedPosition(double pos[3])
{
  pos[0] = m_PickedPosition[0];
  pos[1] = m_PickedPosition[1];
  pos[2] = m_PickedPosition[2];
}
//----------------------------------------------------------------------------
bool mafView::FindPickedVme(vtkAssemblyPath *ap)
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

//----------------------------------------------------------------------------
void mafView::PrintBitmap(wxDC *dc, wxRect margins, wxBitmap *bmp)
{
  assert(dc);
  assert(bmp);
  float iw = bmp->GetWidth();
  float ih = bmp->GetHeight();
  float maxX = iw;
  float maxY = ih;

  // Add the margin to the graphic size
  maxX += (margins.GetLeft() + margins.GetRight());
  maxY += (margins.GetTop() + margins.GetBottom());

  // Get the size of the DC in pixels
  int w,h;
  dc->GetSize(&w, &h);

  // Calculate a suitable scaling factor
  float scaleX=(float)(w/maxX);
  float scaleY=(float)(h/maxY);

  // Use x or y scaling factor, whichever fits on the DC
  float actualScale = wxMin(scaleX,scaleY);

  // Calculate the position on the DC for centering the graphic
  float posX = (float)((w - (iw*actualScale))/2.0);
  float posY = (float)((h - (ih*actualScale))/2.0);

  // Set the scale and origin
  dc->SetUserScale(actualScale, actualScale);
  dc->SetDeviceOrigin( (long)posX, (long)posY );

  wxMemoryDC mdc; 
  mdc.SelectObject(*bmp);
  dc->SetBackground(*wxWHITE_BRUSH);
  dc->Clear();
  dc->Blit(0, 0, maxX, maxY, &mdc, 0, 0);
}
//-------------------------------------------------------------------------
void mafView::Print(std::ostream& os, const int tabs)// const
{
  mafIndent indent(tabs);
  os << indent << "mafView" << '\t' << this << "\n";
}

//----------------------------------------------------------------------------
char ** mafView::GetIcon()
{
#include "pic/VIEW.xpm"
		return VIEW_xpm;
}
