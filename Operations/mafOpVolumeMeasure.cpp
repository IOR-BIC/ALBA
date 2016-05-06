/*=========================================================================

 Program: MAF2
 Module: mafOpVolumeMeasure
 Authors: Daniele Giunchi
 
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

#include "mafOpVolumeMeasure.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVMEIterator.h"

#include "vtkTriangleFilter.h"
#include "vtkMassProperties.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpVolumeMeasure);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpVolumeMeasure::mafOpVolumeMeasure(wxString label) 
: mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;
  
  m_TriangleFilter = NULL;
  m_MassProperties = NULL;
  m_VmeSurface    = NULL;
  m_MeasureText = "";
  m_VolumeMeasure = "";
	m_NormalizedShapeIndex = "";
	m_SurfaceArea = "";
}
//----------------------------------------------------------------------------
mafOpVolumeMeasure::~mafOpVolumeMeasure()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
mafOp *mafOpVolumeMeasure::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpVolumeMeasure(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpVolumeMeasure::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric"))
  {
    return true;
  }
	return false;
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum ID_VOLUME_MEASURE
{
  ID_CLOSE_OP = MINID,
  ID_MEASURE_TYPE,
  ID_COMPUTE_MEASURE,
  ID_STORE_MEASURE,
  ID_REMOVE_MEASURE,
  ID_MEASURE_LIST,
  //ID_ADD_TO_VME_TREE,
};
//----------------------------------------------------------------------------
void mafOpVolumeMeasure::OpRun()   
//----------------------------------------------------------------------------
{
  m_VmeSurface = m_Input;

	wxString measure[3] = {_("points"), _("lines"), _("angle")};

  // setup Gui
	m_Gui = new mafGUI(this);
	m_Gui->SetListener(this);

  m_Gui->Button(ID_COMPUTE_MEASURE,_("Compute"));
  m_Gui->Label(_("measure result:"),true);
  
  
  m_Gui->Label(_("volume= "),&m_VolumeMeasure);
	m_Gui->Label(_("surf. area= "),&m_SurfaceArea);
	m_Gui->Label(_("N.S.I.= "),&m_NormalizedShapeIndex);
  
  m_Gui->Divider();
  m_Gui->Label(_("Measure description."),true);
  m_Gui->Button(ID_STORE_MEASURE,_("Store"));
  //m_Gui->Button(ID_ADD_TO_VME_TREE,"Add to msf");
  m_Gui->Button(ID_REMOVE_MEASURE,_("Remove"));
  m_MeasureList = m_Gui->ListBox(ID_MEASURE_LIST);
	m_Gui->Button(ID_CLOSE_OP,_("Close"));

  mafVME *root = m_Input->GetRoot();
  if(root->GetTagArray()->IsTagPresent("VOLUME_MEASURE"))
  {
    mafTagItem *measure_item = root->GetTagArray()->GetTag("VOLUME_MEASURE");
    int c = measure_item->GetNumberOfComponents();
    for(int i = 0; i < c; i++)
      m_MeasureList->Append(measure_item->GetValue(i));
  }
  
  if(m_MeasureList->GetCount() == 0)
  {
    //m_Gui->Enable(ID_ADD_TO_VME_TREE,false);
    m_Gui->Enable(ID_REMOVE_MEASURE,false);
  }

  m_Gui->Divider(0);
  ShowGui();
}
//----------------------------------------------------------------------------
void mafOpVolumeMeasure::OpDo()
//----------------------------------------------------------------------------
{
  
}

//----------------------------------------------------------------------------
void mafOpVolumeMeasure::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
	case ID_COMPUTE_MEASURE:
    {
	 
     VolumeCompute(m_VmeSurface);
	   m_Gui->Update();
    }
    break;
    case ID_STORE_MEASURE:
    {
      m_MeasureText = wxGetTextFromUser("",_("Insert measure description"), _(m_MeasureText));
      if(m_MeasureText == "") break;
      mafString t;
        t = m_VolumeMeasure + _(" ") + m_SurfaceArea + " " + m_NormalizedShapeIndex + " " + m_MeasureText;
      m_MeasureList->Append(_(t));
      m_MeasureText = "";
      m_Gui->Enable(ID_REMOVE_MEASURE,true);
      //m_gui->Enable(ID_ADD_TO_VME_TREE,true);
    }
    break;
    case ID_REMOVE_MEASURE:
    {
      int sel = m_MeasureList->GetSelection();
      if(sel != -1)
        m_MeasureList->Delete(sel);
      if(m_MeasureList->GetCount() == 0)
      {
        m_Gui->Enable(ID_REMOVE_MEASURE,false);
      }
    }
    break;
    case ID_CLOSE_OP:
			OpStop(OP_RUN_CANCEL);
		break;
		default:
			mafEventMacro(*e);
		break; 
	  }
	}
}

//----------------------------------------------------------------------------
void mafOpVolumeMeasure::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    int c = m_MeasureList->GetCount();
    mafTagItem measure_item;
    measure_item.SetName("VOLUME_MEASURE");
    measure_item.SetNumberOfComponents(c);
    for(int i = 0; i < c; i++)
      measure_item.SetComponent(m_MeasureList->GetString(i).c_str(),i);
    mafVME *root = m_Input->GetRoot();
    if(root->GetTagArray()->IsTagPresent("VOLUME_MEASURE"))
      root->GetTagArray()->DeleteTag("VOLUME_MEASURE");
    root->GetTagArray()->SetTag(measure_item);
    mafEventMacro(mafEvent(this,VME_MODIFIED,root));
  }

    vtkDEL(m_TriangleFilter);
	  vtkDEL(m_MassProperties);
    if(!this->m_TestMode)
    {
      HideGui();
    }
	  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void mafOpVolumeMeasure::VolumeCompute(mafVME *vme)
//----------------------------------------------------------------------------
{
	if (vme == NULL)
	{
		m_NormalizedShapeIndex = wxString::Format(_("Impossible compute the N.S.I."));
		m_SurfaceArea = wxString::Format(_("Impossible compute the surface area"));
		m_VolumeMeasure = wxString::Format(_("Impossible compute the surface volume"));
	}
	else
	{
		vtkNEW(m_TriangleFilter);
		vtkNEW(m_MassProperties);

		m_TriangleFilter->SetInput(vtkPolyData::SafeDownCast(vme->GetOutput()->GetVTKData()));
		m_TriangleFilter->Update();
		m_MassProperties->SetInput(m_TriangleFilter->GetOutput());
		m_MassProperties->Update();

		m_NormalizedShapeIndex = wxString::Format(_("%g"),m_MassProperties->GetNormalizedShapeIndex());
		m_SurfaceArea = wxString::Format(_("%g"),m_MassProperties->GetSurfaceArea());
		m_VolumeMeasure = wxString::Format(_("%g"), m_MassProperties->GetVolume());
	}
}
