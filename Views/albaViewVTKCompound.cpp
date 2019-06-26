/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewVTKCompound
 Authors: Eleonora Mambrini
 
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

#include "albaViewVTKCompound.h"
#include "albaViewImage.h"
#include "albaViewVTK.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaPipeImage3D.h"
#include "albaGUIViewWin.h"
#include "albaGUI.h"
#include "albaGUILutSlider.h"
#include "albaGUILutSwatch.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaVMEImage.h"
#include "albaVMEOutputVolume.h"
#include "albaGUIFloatSlider.h"
#include "albaVMEOutputImage.h"
#include "albaVME.h"

#include "albaPipeVectorFieldMapWithArrows.h"

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
albaCxxTypeMacro(albaViewVTKCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewVTKCompound::albaViewVTKCompound( wxString label, int num_row, int num_col)
: albaViewCompoundWindowing(label,num_row,num_col)
{
  m_ViewVTK = NULL;
}
//----------------------------------------------------------------------------
albaViewVTKCompound::~albaViewVTKCompound()
{
	/*m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);*/
}

//----------------------------------------------------------------------------
albaView *albaViewVTKCompound::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewVTKCompound *v = new albaViewVTKCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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
albaGUI* albaViewVTKCompound::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

  if( albaViewVTK::SafeDownCast(m_ChildViewList[ID_VIEW_VTK]) && albaViewVTK::SafeDownCast(m_ChildViewList[ID_VIEW_VTK])->GetGui())
  {
     m_Gui->AddGui(((albaViewVTK*)m_ChildViewList[ID_VIEW_VTK])->GetGui());
  }
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider();
	m_Gui->FitGui();
	m_Gui->Update();

	return m_Gui;
}
//-------------------------------------------------------------------------
void albaViewVTKCompound::PackageView()
{
  assert(m_ViewVTK);
	PlugChildView(m_ViewVTK);
}

//-------------------------------------------------------------------------
bool albaViewVTKCompound::ActivateWindowing(albaVME *vme)
{
  bool conditions     = false;
  
  if(vme->IsA("albaVMEImage")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {
      //m_ChildViewList[i]->VmeSelect(node, select);

      albaPipeImage3D *pipe = (albaPipeImage3D *)m_ChildViewList[i]->GetNodePipe(vme);
      conditions = (conditions && (pipe && pipe->IsGrayImage()));
    }
  }

  return conditions;
}

//-------------------------------------------------------------------------
void albaViewVTKCompound::SetExternalView(albaViewVTK *childView)
{
  if(m_ViewVTK == NULL) {
    m_ViewVTK = childView;
  }
}
//-------------------------------------------------------------------------
void albaViewVTKCompound::CameraUpdate()
{
  // Added patch to update scalar and vector attributes while changing timeframe with the timebar 
  // (valid only for albaPipeVectorFieldMapWithArrows).
  albaSceneGraph* sg = GetSceneGraph();

  // Do it for each node attached to the view
  for(albaSceneNode *node = sg->GetNodeList(); node; node=node->GetNext())
	{ 
    if (node->GetVme())
    {  
      albaVME* vme = node->GetVme();

      assert(vme);
      albaPipe* alba_pipe = (albaPipe*)GetNodePipe(vme);
      if (alba_pipe && strcmp(alba_pipe->GetTypeName(),"albaPipeVectorFieldMapWithArrows")==0)
      {
        albaPipeVectorFieldMapWithArrows* pipe = (albaPipeVectorFieldMapWithArrows*)alba_pipe;
        if (pipe) {
          pipe->UpdateVTKPipe();
        }
      }
    }
  }

  Superclass::CameraUpdate();
}