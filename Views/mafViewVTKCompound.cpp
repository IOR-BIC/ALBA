/*=========================================================================

 Program: MAF2
 Module: mafViewVTKCompound
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

#include "mafViewVTKCompound.h"
#include "mafViewImage.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafPipeImage3D.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafVME.h"
#include "mafVMEIterator.h"
#include "mafVMEImage.h"
#include "mafVMEOutputVolume.h"
#include "mafGUIFloatSlider.h"
#include "mafVMEOutputImage.h"
#include "mafVME.h"

#include "mafPipeVectorFieldMapWithArrows.h"

#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkTexture.h"

#include "mmaVolumeMaterial.h"


//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
  ID_VIEW_VTK = 0,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewVTKCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewVTKCompound::mafViewVTKCompound( wxString label, int num_row, int num_col)
: mafViewCompoundWindowing(label,num_row,num_col)
//----------------------------------------------------------------------------
{
  m_ViewVTK = NULL;
}
//----------------------------------------------------------------------------
mafViewVTKCompound::~mafViewVTKCompound()
//----------------------------------------------------------------------------
{
	/*m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);*/
}
//----------------------------------------------------------------------------
mafView *mafViewVTKCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewVTKCompound *v = new mafViewVTKCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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

//-------------------------------------------------------------------------
mafGUI* mafViewVTKCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);
  if( mafViewVTK::SafeDownCast(m_ChildViewList[ID_VIEW_VTK]) && mafViewVTK::SafeDownCast(m_ChildViewList[ID_VIEW_VTK])->GetGui())
  {
     m_Gui->AddGui(((mafViewVTK*)m_ChildViewList[ID_VIEW_VTK])->GetGui());
  }
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider();
	m_Gui->FitGui();
	m_Gui->Update();
	return m_Gui;
}
//-------------------------------------------------------------------------
void mafViewVTKCompound::PackageView()
//-------------------------------------------------------------------------
{
  assert(m_ViewVTK);
	PlugChildView(m_ViewVTK);
}

//-------------------------------------------------------------------------
bool mafViewVTKCompound::ActivateWindowing(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool conditions     = false;
  
  if(vme->IsA("mafVMEImage")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {
      //m_ChildViewList[i]->VmeSelect(node, select);

      mafPipeImage3D *pipe = (mafPipeImage3D *)m_ChildViewList[i]->GetNodePipe(vme);
      conditions = (conditions && (pipe && pipe->IsGrayImage()));
    }
  }

  return conditions;
}

//-------------------------------------------------------------------------
void mafViewVTKCompound::SetExternalView(mafViewVTK *childView)
//-------------------------------------------------------------------------
{
  if(m_ViewVTK == NULL) {
    m_ViewVTK = childView;
  }
}
//-------------------------------------------------------------------------
void mafViewVTKCompound::CameraUpdate()
//-------------------------------------------------------------------------
{
  // Added patch to update scalar and vector attributes while changing timeframe with the timebar 
  // (valid only for mafPipeVectorFieldMapWithArrows).
  mafSceneGraph* sg = GetSceneGraph();

  // Do it for each node attached to the view
  for(mafSceneNode *node = sg->GetNodeList(); node; node=node->GetNext())
	{ 
    if (node->GetVme())
    {
  
      mafVME* vme = node->GetVme();

      assert(vme);
      mafPipe* maf_pipe = (mafPipe*)GetNodePipe(vme);
      if (maf_pipe && strcmp(maf_pipe->GetTypeName(),"mafPipeVectorFieldMapWithArrows")==0)
      {
        mafPipeVectorFieldMapWithArrows* pipe = (mafPipeVectorFieldMapWithArrows*)maf_pipe;
        if (pipe) {
          pipe->UpdateVTKPipe();
        }
      }
    }
  }

  Superclass::CameraUpdate();
}