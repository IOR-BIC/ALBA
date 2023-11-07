/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeMeasure
 Authors: Daniele Giunchi
 
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

#include "albaOpVolumeMeasure.h"
#include <wx/busyinfo.h>
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaSmartPointer.h"
#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVMEIterator.h"

#include "vtkTriangleFilter.h"
#include "vtkMassProperties.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpVolumeMeasure);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpVolumeMeasure::albaOpVolumeMeasure(wxString label) 
: albaOp(label)
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
albaOpVolumeMeasure::~albaOpVolumeMeasure()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
albaOp *albaOpVolumeMeasure::Copy()
//----------------------------------------------------------------------------
{
	return new albaOpVolumeMeasure(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpVolumeMeasure::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  if(node->IsA("albaVMESurface") || node->IsA("albaVMESurfaceParametric"))
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
void albaOpVolumeMeasure::OpRun()   
//----------------------------------------------------------------------------
{
  m_VmeSurface = m_Input;

	wxString measure[3] = {_("points"), _("lines"), _("angle")};

  // setup Gui
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

  m_Gui->Button(ID_COMPUTE_MEASURE,_("Compute"));
  m_Gui->Label(_("Measure result"),true);
    
  m_Gui->Label(_(" Volume="),&m_VolumeMeasure, false, false, false, 0.36);
	m_Gui->Label(_(" Surf. area="),&m_SurfaceArea, false, false, false, 0.36);
	m_Gui->Label(_(" N.S.I.="),&m_NormalizedShapeIndex, false, false, false, 0.36);
  
  m_Gui->Divider(1);
  m_Gui->Label(_("Measure description"),true);
  //m_Gui->Button(ID_ADD_TO_VME_TREE,"Add to msf");
	m_Gui->TwoButtons(ID_STORE_MEASURE, ID_REMOVE_MEASURE, "Store", "Remove");
  m_MeasureList = m_Gui->ListBox(ID_MEASURE_LIST);

  albaVME *root = m_Input->GetRoot();
  if(root->GetTagArray()->IsTagPresent("VOLUME_MEASURE"))
  {
    albaTagItem *measure_item = root->GetTagArray()->GetTag("VOLUME_MEASURE");
    int c = measure_item->GetNumberOfComponents();
    for(int i = 0; i < c; i++)
      m_MeasureList->Append(measure_item->GetValue(i));
  }
  
  if(m_MeasureList->GetCount() == 0)
  {
    //m_Gui->Enable(ID_ADD_TO_VME_TREE,false);
    m_Gui->Enable(ID_REMOVE_MEASURE,false);
  }

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->Button(ID_CLOSE_OP, _("Close"));
	m_Gui->Label("");

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpVolumeMeasure::OpDo()
//----------------------------------------------------------------------------
{
  
}

//----------------------------------------------------------------------------
void albaOpVolumeMeasure::OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
      m_MeasureText = wxGetTextFromUser("","Insert measure description", m_MeasureText.GetCStr());
      if(m_MeasureText == "") break;
      albaString t;
        t = m_VolumeMeasure + " " + m_SurfaceArea + " " + m_NormalizedShapeIndex + " " + m_MeasureText;
      m_MeasureList->Append(t.GetCStr());
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
			albaEventMacro(*e);
		break; 
	  }
	}
}

//----------------------------------------------------------------------------
void albaOpVolumeMeasure::OpStop(int result)
//----------------------------------------------------------------------------
{
	if (result == OP_RUN_CANCEL)
	{
		int c = m_MeasureList->GetCount();
		albaTagItem measure_item;
		measure_item.SetName("VOLUME_MEASURE");
		measure_item.SetNumberOfComponents(c);

		for (int i = 0; i < c; i++)
			measure_item.SetComponent(m_MeasureList->GetString(i), i);

		albaVME *root = m_Input->GetRoot();

		if (root->GetTagArray()->IsTagPresent("VOLUME_MEASURE"))
			root->GetTagArray()->DeleteTag("VOLUME_MEASURE");

		root->GetTagArray()->SetTag(measure_item);

		GetLogicManager()->VmeModified(root);
	}

	vtkDEL(m_TriangleFilter);
	vtkDEL(m_MassProperties);
	if (!this->m_TestMode)
	{
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void albaOpVolumeMeasure::VolumeCompute(albaVME *vme)
//----------------------------------------------------------------------------
{
	if (vme == NULL)
	{
		m_NormalizedShapeIndex = albaString::Format(_("Impossible compute the N.S.I."));
		m_SurfaceArea = albaString::Format(_("Impossible compute the surface area"));
		m_VolumeMeasure = albaString::Format(_("Impossible compute the surface volume"));
	}
	else
	{
		vtkNEW(m_TriangleFilter);
		vtkNEW(m_MassProperties);

		m_TriangleFilter->SetInput(vtkPolyData::SafeDownCast(vme->GetOutput()->GetVTKData()));
		m_TriangleFilter->Update();
		m_MassProperties->SetInput(m_TriangleFilter->GetOutput());
		m_MassProperties->Update();

		m_NormalizedShapeIndex = albaString::Format(_("%g"),m_MassProperties->GetNormalizedShapeIndex());
		m_SurfaceArea = albaString::Format(_("%g"),m_MassProperties->GetSurfaceArea());
		m_VolumeMeasure = albaString::Format(_("%g"), m_MassProperties->GetVolume());
	}
}
