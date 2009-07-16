/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewCompoundWindowing.cpp,v $
  Language:  C++
  Date:      $Date: 2009-07-16 09:02:56 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Eleonora Mambrini
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

#include "medViewCompoundWindowing.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafPipeImage3D.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVMEVolumeGray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafGUIFloatSlider.h"
#include "mmaVolumeMaterial.h"

#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
medViewCompoundWindowing::medViewCompoundWindowing( wxString label, int num_row, int num_col)
: mafViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_LutWidget       = NULL;
	m_LutSlider       = NULL;
	m_ColorLUT        = NULL;
}
//----------------------------------------------------------------------------
medViewCompoundWindowing::~medViewCompoundWindowing()
//----------------------------------------------------------------------------
{
	//m_ColorLUT        = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}

//----------------------------------------------------------------------------
void medViewCompoundWindowing::CreateGuiView()
//----------------------------------------------------------------------------
{
	m_GuiView = new mafGUI(this);
  
  m_LutSlider = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
  EnableWidgets(false);
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void medViewCompoundWindowing::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
		case ID_RANGE_MODIFIED:
			{
				//Windowing
				if(mafVME::SafeDownCast(GetSceneGraph()->GetSelectedVme()))
				{
					double low, hi;
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

//----------------------------------------------------------------------------
void medViewCompoundWindowing::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(node, show);
  
  if(GetSceneGraph()->GetSelectedVme()==node)
	{
    UpdateWindowing( show && this->ActivateWindowing(node), node);
  }

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medViewCompoundWindowing::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
 	if(m_Gui)
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
  m_LutSlider->Enable(enable);
}

//----------------------------------------------------------------------------
void medViewCompoundWindowing::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeSelect(node, select);

  UpdateWindowing( select && this->ActivateWindowing(node), node);
}

//----------------------------------------------------------------------------
void medViewCompoundWindowing::UpdateWindowing(bool enable,mafNode *node)
//----------------------------------------------------------------------------
{
  EnableWidgets(enable);

  //Windowing can be applied on Volumes or on Images
  mafVME      *Volume		= NULL;
	mafVMEImage *Image	  = NULL;
	
  mafVME *Vme = mafVME::SafeDownCast(node);
	
  if((mafVME *)(Vme->GetOutput()->IsA("mafVMEOutputVolume"))) {
		Volume = mafVME::SafeDownCast(node);
	}
	else if(Vme->IsA("mafVMEImage")) {
		Image = mafVMEImage::SafeDownCast(node);
	}

  if(Volume) {
		if(enable && (mafVMEOutputVolume::SafeDownCast(Volume->GetOutput())))
		{
			double sr[2];
			Volume->GetOutput()->GetVTKData()->GetScalarRange(sr);
			mmaVolumeMaterial *currentSurfaceMaterial = ((mafVMEOutputVolume *)Volume->GetOutput())->GetMaterial();
      m_ColorLUT = mafVMEVolumeGray::SafeDownCast(Volume)->GetMaterial()->m_ColorLut;
			m_LutWidget->SetLut(m_ColorLUT);
			m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
      m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
			//m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);
		}
		else
		{
			m_LutSlider->SetRange(-100,100);
			m_LutSlider->SetSubRange(-100,100);
		}
	}
	else if(Image) {
		if(enable)
		{
			double sr[2];
			vtkDataSet *data = ((mafVME *)node)->GetOutput()->GetVTKData();
			data->Update();

			//Get scalar range of the image
			data->GetScalarRange(sr);

      m_ColorLUT = (vtkLookupTable*)((mafPipeImage3D *)m_ChildViewList[0]->GetNodePipe(node))->GetLUT();
      m_LutWidget->SetLut(m_ColorLUT);
			m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
			m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);

		}
		else
		{
			m_LutSlider->SetRange(-100,100);
			m_LutSlider->SetSubRange(-100,100);
		}
	}
}

bool medViewCompoundWindowing::ActivateWindowing(mafNode *node)
{
  bool conditions     = false;
  bool nodeHasPipe    = false;
  
  if(((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {
      //m_ChildViewList[i]->VmeSelect(node, select);
      
      if(m_ChildViewList[i]->GetNodePipe(node)) {
        nodeHasPipe = true;
      }
      conditions = (conditions && nodeHasPipe);
    }
  }

  else if(((mafVME *)node)->IsA("mafVMEImage")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {
      //m_ChildViewList[i]->VmeSelect(node, select);

      mafPipeImage3D *pipe = (mafPipeImage3D *)m_ChildViewList[i]->GetNodePipe(node);
      conditions = (conditions && (pipe && pipe->IsGrayImage()));
    }
  }

  return conditions;
}
