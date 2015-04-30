/*=========================================================================

 Program: MAF2
 Module: mafViewCompoundWindowing
 Authors: Eleonora Mambrini
 
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

#include "mafViewCompoundWindowing.h"
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
mafViewCompoundWindowing::mafViewCompoundWindowing( wxString label, int num_row, int num_col)
: mafViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_LutWidget       = NULL;
	m_LutSlider       = NULL;
	m_ColorLUT        = NULL;
}
//----------------------------------------------------------------------------
mafViewCompoundWindowing::~mafViewCompoundWindowing()
//----------------------------------------------------------------------------
{
	//m_ColorLUT        = NULL;
  if(m_LutWidget)
	  cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}

//----------------------------------------------------------------------------
void mafViewCompoundWindowing::CreateGuiView()
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
void mafViewCompoundWindowing::OnEvent(mafEventBase *maf_event)
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
void mafViewCompoundWindowing::VmeShow(mafNode *node, bool show)
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
void mafViewCompoundWindowing::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
 	if(m_Gui)
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
  m_LutSlider->Enable(enable);
}

//----------------------------------------------------------------------------
void mafViewCompoundWindowing::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeSelect(node, select);

  //UpdateWindowing( select && ActivateWindowing(node), node);
  //if(m_Gui)
    UpdateWindowing( select && ActivateWindowing(GetSceneGraph()->GetSelectedVme()), GetSceneGraph()->GetSelectedVme());
}

//----------------------------------------------------------------------------
void mafViewCompoundWindowing::UpdateWindowing(bool enable,mafNode *node)
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
      VolumeWindowing(Volume);
		}
		else
		{
      m_ColorLUT = NULL;
      if(m_LutWidget)
        m_LutWidget->SetLut(m_ColorLUT);
			m_LutSlider->SetRange(-100,100);
			m_LutSlider->SetSubRange(-100,100);
		}
	}
	else if(Image) {
		if(enable)
		{
      ImageWindowing(Image);
		}
		else
		{
      m_ColorLUT = NULL;
      if(m_LutWidget)
        m_LutWidget->SetLut(m_ColorLUT);
			m_LutSlider->SetRange(-100,100);
			m_LutSlider->SetSubRange(-100,100);
		}
	}
}

//----------------------------------------------------------------------------
bool mafViewCompoundWindowing::ActivateWindowing(mafNode *node)
//----------------------------------------------------------------------------
{
  bool conditions     = false;
  bool nodeHasPipe    = false;
  
  if(((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {
      
      if(m_ChildViewList[i]->GetNodePipe(node)) {
        nodeHasPipe = true;
      }
      conditions = (conditions && nodeHasPipe);
    }
  }

  else if(((mafVME *)node)->IsA("mafVMEImage")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {

      mafPipeImage3D *pipe = (mafPipeImage3D *)m_ChildViewList[i]->GetNodePipe(node);
      conditions = (conditions && (pipe && pipe->IsGrayImage()));
    }
  }

  return conditions;
}

//----------------------------------------------------------------------------
void mafViewCompoundWindowing::ImageWindowing(mafVMEImage *image)
//----------------------------------------------------------------------------
{
  double sr[2];
  vtkDataSet *data = image->GetOutput()->GetVTKData();
  data->Update();

  //Get scalar range of the image
  data->GetScalarRange(sr);

  m_ColorLUT = (vtkLookupTable*)((mafPipeImage3D *)m_ChildViewList[0]->GetNodePipe(image))->GetLUT();
  if(m_LutWidget)
    m_LutWidget->SetLut(m_ColorLUT);
  m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
  m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
}

//----------------------------------------------------------------------------
void mafViewCompoundWindowing::VolumeWindowing(mafVME *volume)
//----------------------------------------------------------------------------
{
  double sr[2];
  volume->GetOutput()->GetVTKData()->GetScalarRange(sr);
  
  mmaVolumeMaterial *currentSurfaceMaterial = ((mafVMEOutputVolume *)volume->GetOutput())->GetMaterial();
  m_ColorLUT = mafVMEVolumeGray::SafeDownCast(volume)->GetMaterial()->m_ColorLut;
  if(m_LutWidget)
    m_LutWidget->SetLut(m_ColorLUT);
  m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
  //m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
  m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);

}

//-------------------------------------------------------------------------
mafPipe* mafViewCompoundWindowing::GetNodePipe(mafNode *vme)
//-------------------------------------------------------------------------
{
	mafPipe* rtn = NULL;
	if (m_NumOfChildView ==1)
	{
	rtn = m_ChildViewList[0]->GetNodePipe(vme);
	}
	return rtn;

}