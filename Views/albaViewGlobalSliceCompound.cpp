/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewGlobalSliceCompound
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

#include "albaViewGlobalSliceCompound.h"
#include "albaViewGlobalSlice.h"
#include "albaViewVTK.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaGUIViewWin.h"
#include "albaGUI.h"
#include "albaGUILutSlider.h"
#include "albaGUILutSwatch.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaPipeSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "albaGUIFloatSlider.h"
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

enum VIEW_WIDGET_ID
{
	ID_FIRST = MINID,
	ID_LAST
};
//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewGlobalSliceCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewGlobalSliceCompound::albaViewGlobalSliceCompound( wxString label, int num_row, int num_col)
: albaViewCompoundWindowing(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	/*m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;*/
}
//----------------------------------------------------------------------------
albaViewGlobalSliceCompound::~albaViewGlobalSliceCompound()
//----------------------------------------------------------------------------
{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
albaView *albaViewGlobalSliceCompound::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewGlobalSliceCompound *v = new albaViewGlobalSliceCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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
albaGUI* albaViewGlobalSliceCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

	m_Gui->AddGui(((albaViewGlobalSlice*)m_ChildViewList[ID_VIEW_GLOBAL_SLICE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider(0);
	m_Gui->FitGui();
	m_Gui->Update();
  return m_Gui;
}
//-------------------------------------------------------------------------
void albaViewGlobalSliceCompound::PackageView()
//-------------------------------------------------------------------------
{
	m_ViewGlobalSlice = new albaViewGlobalSlice("",CAMERA_OS_P);
	m_ViewGlobalSlice->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice");
	m_ViewGlobalSlice->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeOrthoSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMELabeledVolume", "albaPipeVolumeOrthoSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMEAnalog", "albaPipeBox", NON_VISIBLE);
  m_ViewGlobalSlice->PlugVisualPipe("albaVMELandmark", "albaPipeSurfaceSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMELandmarkCloud", "albaPipeSurfaceSlice");
	m_ViewGlobalSlice->PlugVisualPipe("albaVMEProsthesis", "albaPipeSurfaceSlice");

	PlugChildView(m_ViewGlobalSlice);
}

