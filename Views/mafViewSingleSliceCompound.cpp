/*=========================================================================

 Program: MAF2
 Module: mafViewSingleSliceCompound
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

#include "mafViewSingleSliceCompound.h"
#include "mafViewSingleSlice.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafVME.h"
#include "mafVMEIterator.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "mafGUIFloatSlider.h"
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
: mafViewCompoundWindowing(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	/*m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;*/
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
mafView *mafViewSingleSliceCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
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
//-------------------------------------------------------------------------
mafGUI* mafViewSingleSliceCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

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
void mafViewSingleSliceCompound::OnLayout()
//----------------------------------------------------------------------------
{
  mafViewCompound::OnLayout();
  ((mafViewSingleSlice*)m_ChildViewList[ID_VIEW_SINGLE_SLICE])->UpdateText();
}
