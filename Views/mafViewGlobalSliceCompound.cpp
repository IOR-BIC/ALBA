/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewGlobalSliceCompound.cpp,v $
  Language:  C++
  Date:      $Date: 2007-01-22 06:55:22 $
  Version:   $Revision: 1.5 $
  Authors:   Matteo Giacomoni
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

#include "mafViewGlobalSliceCompound.h"
#include "mafViewGlobalSlice.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mmgViewWin.h"
#include "mmgGui.h"
#include "mmgLutSlider.h"
#include "mmgLutSwatch.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "mmgFloatSlider.h"
#include "mmaVolumeMaterial.h"

#include "vtkLookupTable.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
  ID_VIEW_GLOBAL_SLICE = 0,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewGlobalSliceCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewGlobalSliceCompound::mafViewGlobalSliceCompound( wxString label, int num_row, int num_col)
: mafViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;
}
//----------------------------------------------------------------------------
mafViewGlobalSliceCompound::~mafViewGlobalSliceCompound()
//----------------------------------------------------------------------------
{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
mafView *mafViewGlobalSliceCompound::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewGlobalSliceCompound *v = new mafViewGlobalSliceCompound(m_Label, m_ViewRowNum, m_ViewColNum);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  for (int i=0;i<m_PluggedChildViewList.size();i++)
  {
    v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
  }
  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewGlobalSliceCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
	m_GuiView = new mmgGui(this);
  
  m_LutSlider = new mmgLutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
  EnableWidgets(false);
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafViewGlobalSliceCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
		case ID_RANGE_MODIFIED:
			{
				//Windowing
				if(mafVMEVolumeGray::SafeDownCast(GetSceneGraph()->GetSelectedVme()))
				{
					int low, hi;
					m_LutSlider->GetSubRange(&low,&hi);
					m_ColorLUT->SetTableRange(low,hi);
					mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
			}
			break;
		case ID_LUT_CHOOSER:
			{
				double *sr;
				sr = m_ColorLUT->GetRange();
				m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
			}
			break;
    default:
      mafEventMacro(*maf_event);
  }
}
//-------------------------------------------------------------------------
mmgGui* mafViewGlobalSliceCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
	m_Gui->AddGui(((mafViewGlobalSlice*)m_ChildViewList[ID_VIEW_GLOBAL_SLICE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider(0);
	m_Gui->FitGui();
	m_Gui->Update();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafViewGlobalSliceCompound::PackageView()
//-------------------------------------------------------------------------
{
	m_ViewGlobalSlice = new mafViewGlobalSlice("",CAMERA_PERSPECTIVE);
	m_ViewGlobalSlice->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
  m_ViewGlobalSlice->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice");
	m_ViewGlobalSlice->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
	
	PlugChildView(m_ViewGlobalSlice);
}
//----------------------------------------------------------------------------
void mafViewGlobalSliceCompound::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(node, show);

	if(GetSceneGraph()->GetSelectedVme()==node)
	{
		UpdateWindowing(show,node);
	}

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafViewGlobalSliceCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	//if a volume is visualized enable the widgets
	if(m_Gui)
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
  m_LutSlider->Enable(enable);
}
//----------------------------------------------------------------------------
void mafViewGlobalSliceCompound::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeSelect(node, select);

	UpdateWindowing(node->IsA("mafVMEVolumeGray") && select && m_ChildViewList[ID_VIEW_GLOBAL_SLICE]->GetNodePipe(node),node);
}
//----------------------------------------------------------------------------
void mafViewGlobalSliceCompound::UpdateWindowing(bool enable,mafNode *node)
//----------------------------------------------------------------------------
{
	if(enable)
	{
		EnableWidgets(enable);
		mafVMEVolumeGray *Volume=mafVMEVolumeGray::SafeDownCast(node);
		double sr[2];
		Volume->GetVolumeOutput()->GetVTKData()->GetScalarRange(sr);
		mmaVolumeMaterial *currentSurfaceMaterial = Volume->GetMaterial();
		m_ColorLUT = Volume->GetMaterial()->m_ColorLut;
		m_LutWidget->SetLut(m_ColorLUT);
		m_LutWidget->Enable(true);
		m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
		m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);
	}
	else
	{
		EnableWidgets(enable);
		m_LutSlider->SetRange(-100,100);
		m_LutSlider->SetSubRange(-100,100);
	}
}