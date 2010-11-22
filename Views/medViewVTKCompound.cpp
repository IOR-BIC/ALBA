/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewVTKCompound.cpp,v $
  Language:  C++
  Date:      $Date: 2010-11-22 11:04:58 $
  Version:   $Revision: 1.1.2.4 $
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

#include "medViewVTKCompound.h"
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
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafVMEImage.h"
#include "mafVMEOutputVolume.h"
#include "mafGUIFloatSlider.h"
#include "mafVMEOutputImage.h"
#include "vtkImageData.h"
#include "vtkTexture.h"

#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"

#include "mmaVolumeMaterial.h"


//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
  ID_VIEW_VTK = 0,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(medViewVTKCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medViewVTKCompound::medViewVTKCompound( wxString label, int num_row, int num_col)
: medViewCompoundWindowing(label,num_row,num_col)
//----------------------------------------------------------------------------
{
  m_ViewVTK = NULL;
}
//----------------------------------------------------------------------------
medViewVTKCompound::~medViewVTKCompound()
//----------------------------------------------------------------------------
{
	/*m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);*/
}
//----------------------------------------------------------------------------
mafView *medViewVTKCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  medViewVTKCompound *v = new medViewVTKCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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
mafGUI* medViewVTKCompound::CreateGui()
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
void medViewVTKCompound::PackageView()
//-------------------------------------------------------------------------
{
  assert(m_ViewVTK);
	PlugChildView(m_ViewVTK);
}

//-------------------------------------------------------------------------
bool medViewVTKCompound::ActivateWindowing(mafNode *node)
//-------------------------------------------------------------------------
{
  bool conditions     = false;
  
  if(((mafVME *)node)->IsA("mafVMEImage")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {
      //m_ChildViewList[i]->VmeSelect(node, select);

      mafPipeImage3D *pipe = (mafPipeImage3D *)m_ChildViewList[i]->GetNodePipe(node);
      conditions = (conditions && (pipe && pipe->IsGrayImage()));
    }
  }

  return conditions;
}

//-------------------------------------------------------------------------
void medViewVTKCompound::SetExternalView(mafViewVTK *childView)
//-------------------------------------------------------------------------
{
  if(m_ViewVTK == NULL) {
    m_ViewVTK = childView;
  }
}