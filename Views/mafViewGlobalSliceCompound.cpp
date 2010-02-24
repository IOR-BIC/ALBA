/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewGlobalSliceCompound.cpp,v $
  Language:  C++
  Date:      $Date: 2010-02-24 16:59:59 $
  Version:   $Revision: 1.16.2.2 $
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

#include "mafViewGlobalSliceCompound.h"

#include "mafViewGlobalSlice.h"
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
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "mafGUIFloatSlider.h"
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
: medViewCompoundWindowing(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	/*m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;*/
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

//-------------------------------------------------------------------------
mafGUI* mafViewGlobalSliceCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
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
	//m_ViewGlobalSlice = new mafViewGlobalSlice("",CAMERA_ARB);
  m_ViewGlobalSlice = new mafViewGlobalSlice("",CAMERA_PERSPECTIVE);
	m_ViewGlobalSlice->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice_BES");
  m_ViewGlobalSlice->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice_BES");
	m_ViewGlobalSlice->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice_BES");
  m_ViewGlobalSlice->PlugVisualPipe("medVMELabeledVolume", "mafPipeVolumeSlice_BES");
  m_ViewGlobalSlice->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice_BES");
  m_ViewGlobalSlice->PlugVisualPipe("medVMEAnalog", "mafPipeBox", NON_VISIBLE);
  m_ViewGlobalSlice->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice");
  m_ViewGlobalSlice->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice");
	
	PlugChildView(m_ViewGlobalSlice);
}