/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSingleSliceCompound.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-19 10:59:04 $
  Version:   $Revision: 1.14 $
  Authors:   Daniele Giunchi
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

#include "mafViewSingleSliceCompound.h"
#include "mafViewSingleSlice.h"
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
#include "vtkCamera.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
  ID_VIEW_SINGLE_SLICE = 0,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSingleSliceCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewSingleSliceCompound::mafViewSingleSliceCompound( wxString label, int num_row, int num_col)
: mafViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;
}
//----------------------------------------------------------------------------
mafViewSingleSliceCompound::~mafViewSingleSliceCompound()
//----------------------------------------------------------------------------
{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
mafView *mafViewSingleSliceCompound::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewSingleSliceCompound *v = new mafViewSingleSliceCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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
void mafViewSingleSliceCompound::CreateGuiView()
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
void mafViewSingleSliceCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
		case ID_RANGE_MODIFIED:
			{
				//Windowing
				if(mafVME::SafeDownCast(GetSceneGraph()->GetSelectedVme()))
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
mmgGui* mafViewSingleSliceCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
	m_Gui->AddGui(((mafViewSingleSlice*)m_ChildViewList[ID_VIEW_SINGLE_SLICE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider();
	m_Gui->FitGui();
	m_Gui->Update();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafViewSingleSliceCompound::PackageView()
//-------------------------------------------------------------------------
{
	m_ViewSingleSlice = new mafViewSingleSlice("",CAMERA_CT);
	m_ViewSingleSlice->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
	m_ViewSingleSlice->PlugVisualPipe("mafVMEPolyline", "mafPipePolylineSlice");
  m_ViewSingleSlice->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice");
	m_ViewSingleSlice->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
	m_ViewSingleSlice->PlugVisualPipe("mafVMEMesh","mafPipeMeshSlice");
  m_ViewSingleSlice->PlugVisualPipe("mafVMEMeter","mafPipePolylineSlice");

	PlugChildView(m_ViewSingleSlice);
}
//----------------------------------------------------------------------------
void mafViewSingleSliceCompound::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(node, show);

	if(GetSceneGraph()->GetSelectedVme()==node)
	{
		//UpdateWindowing(show,node);
		UpdateWindowing(((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume") && show && (GetSceneGraph()->GetSelectedVme() == node),node);
	}

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafViewSingleSliceCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	//if a volume is visualized enable the widgets
	if(m_Gui)
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
  m_LutSlider->Enable(enable);
}
//----------------------------------------------------------------------------
void mafViewSingleSliceCompound::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeSelect(node, select);

	UpdateWindowing(((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume") && select && m_ChildViewList[ID_VIEW_SINGLE_SLICE]->GetNodePipe(node),node);
}
//----------------------------------------------------------------------------
void mafViewSingleSliceCompound::UpdateWindowing(bool enable,mafNode *node)
//----------------------------------------------------------------------------
{
	if(enable)
	{
		EnableWidgets(enable);
		mafVME *Volume=mafVME::SafeDownCast(node);
    mafVMEOutputVolume *volumeOutput = mafVMEOutputVolume::SafeDownCast(Volume->GetOutput());
		double sr[2];
		volumeOutput->GetVTKData()->GetScalarRange(sr);
		mmaVolumeMaterial *currentSurfaceMaterial = volumeOutput->GetMaterial();
		m_ColorLUT = volumeOutput->GetMaterial()->m_ColorLut;
		volumeOutput->GetMaterial()->UpdateProp();
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
//----------------------------------------------------------------------------
void mafViewSingleSliceCompound::OnLayout()
//----------------------------------------------------------------------------
{
  mafViewCompound::OnLayout();
  ((mafViewSingleSlice*)m_ChildViewList[ID_VIEW_SINGLE_SLICE])->UpdateText();
}