/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewSlicer.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:25:11 $
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

#include "medViewSlicer.h"
#include "mafViewSlice.h"
#include "mafVme.h"
#include "mafVMESlicer.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipeSurface.h"
#include "mafPipeSurfaceTextured.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafSceneGraph.h"
#include "mafEvent.h"
#include "mafAbsMatrixPipe.h"
#include "mafAttachCamera.h"
#include "mmiGenericMouse.h"
#include "mafVMESlicer.h"
#include "mafTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafNodeIterator.h"
#include "mafGUILutPreset.h"
#include "mafVMEOutputSurface.h"
#include "mafAttribute.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"

#include "vtkTransform.h"
#include "vtkLookupTable.h"
#include "vtkDataSet.h"
#include "vtkMath.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointSet.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "vtkPolyDataNormals.h"
#include "vtkCamera.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medViewSlicer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum ARBITRARY_SUBVIEW_ID
{
  ARBITRARY_VIEW = 0,
	SLICE_VIEW,
};
enum GIZMO_TYPE_ID
{
	GIZMO_TRANSLATE = 0,
	GIZMO_ROTATE,
};
enum AXIS_ID
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS,
};

//----------------------------------------------------------------------------
medViewSlicer::medViewSlicer(wxString label, bool show_ruler)
: mafViewCompound(label, 1, 2)
//----------------------------------------------------------------------------
{
	m_ViewArbitrary = NULL;
	m_ViewSlice = NULL;

	m_CurrentVolume = NULL;
	m_CurrentSlicer = NULL;
	
	m_AttachCamera = NULL;
	m_LutSlider = NULL;
	m_LutWidget = NULL;
	m_ColorLUT= NULL;

	m_SliceCenterSurface[0] = 0.0;
	m_SliceCenterSurface[1] = 0.0;
	m_SliceCenterSurface[2] = 0.0;

	m_SliceCenterSurfaceReset[0] = 0.0;
	m_SliceCenterSurfaceReset[1] = 0.0;
	m_SliceCenterSurfaceReset[2] = 0.0;

}
//----------------------------------------------------------------------------
medViewSlicer::~medViewSlicer()
//----------------------------------------------------------------------------
{
	m_CurrentVolume = NULL;
	m_ColorLUT = NULL;

  cppDEL(m_AttachCamera);
}
//----------------------------------------------------------------------------
void medViewSlicer::PackageView()
//----------------------------------------------------------------------------
{
	m_ViewArbitrary = new mafViewVTK("",CAMERA_PERSPECTIVE);
	//m_ViewArbitrary->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
	m_ViewArbitrary->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", MUTEX);
  m_ViewArbitrary->PlugVisualPipe("mafVMELabeledVolume", "mafPipeBox", MUTEX);
	m_ViewSlice = new mafViewVTK("",CAMERA_CT);
	m_ViewSlice->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
  m_ViewSlice->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
	
	PlugChildView(m_ViewArbitrary);
	PlugChildView(m_ViewSlice);

}
//----------------------------------------------------------------------------
void medViewSlicer::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	m_ChildViewList[ARBITRARY_VIEW]->VmeShow(node, show);
	m_ChildViewList[SLICE_VIEW]->VmeShow(node, show);
	mafVME *Vme=mafVME::SafeDownCast(node);
	Vme->Update();
	if (show)
	{
		if(((mafVME *)Vme)->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			mafVME *Volume=mafVME::SafeDownCast(Vme);
			m_CurrentVolume = Volume;

			// get the VTK volume
      vtkDataSet *data = ((mafVME *)node)->GetOutput()->GetVTKData();
      data->Update();	
		}
		else if(Vme->IsA("mafVMESurface") || Vme->IsA("mafVMESurfaceParametric"))
		{
			//a surface is visible only if there is a volume in the view
			if(m_CurrentVolume)
			{
        CameraUpdate();
			}
		}
    else if(Vme->IsA("mafVMESlicer"))
    {
      //Show Slicer
      m_CurrentSlicer = mafVMESlicer::SafeDownCast(node);
      double sr[2];
      mafVMEVolumeGray *vol = mafVMEVolumeGray::SafeDownCast(m_CurrentSlicer->GetSlicedVMELink());
      if(vol)
      {
        vol->GetOutput()->GetVTKData()->GetScalarRange(sr);
        
        mmaMaterial *currentSurfaceMaterial = m_CurrentSlicer->GetMaterial();
        m_ColorLUT = m_CurrentSlicer->GetMaterial()->m_ColorLut;
        m_CurrentSlicer->GetMaterial()->UpdateProp();
        m_ColorLUT->SetTableRange(sr[0], sr[1]);
        
        if(m_LutWidget)
        {
          m_LutWidget->SetLut(m_ColorLUT);
          if(m_LutSlider)
          {
            m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
            m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
          }
        }

      }
        
      //Set camera of slice view in way that it will follow the volume
      if(!m_AttachCamera)
        m_AttachCamera=new mafAttachCamera(m_Gui,((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->m_Rwi,this);
      m_AttachCamera->SetStartingMatrix(m_CurrentSlicer->GetOutput()->GetAbsMatrix());
      m_AttachCamera->SetVme(m_CurrentSlicer);
      ((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraReset(m_CurrentSlicer);
    }
	}
	else//if show=false
	{
		
        if (((mafVME *)Vme)->GetOutput()->IsA("mafVMEOutputVolume"))
		{		
			m_CurrentVolume = NULL;
			m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
		}
    else if(Vme->IsA("mafVMESlicer"))
    {
      m_AttachCamera->SetVme(NULL);
      m_CurrentSlicer = NULL;

      m_LutSlider->Enable(false);
      double normal[3] = {0,0,1};
      ((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_CT);
    }
	}
	//mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void medViewSlicer::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (maf_event->GetSender() == this->m_Gui || maf_event->GetSender() == this->m_LutSlider) // from this view gui
  {
    OnEventThis(maf_event); 
  }
	else
  {
    // if no one can handle this event send it to the operation listener
    mafEventMacro(*maf_event); 
  }	
	//mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medViewSlicer::OnEventThis(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_RANGE_MODIFIED:
      {
        if(m_CurrentVolume)
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
        if(m_ColorLUT && m_CurrentSlicer)
        {
          double *sr;
          sr = m_ColorLUT->GetRange();
          if(m_LutSlider) m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
        }
        else
        {
          wxMessageBox("There is no visualized Slicer");
        }
        
      }
      CameraUpdate();
			break;
		default:
			mafViewCompound::OnEvent(maf_event);
		}
	}
}
//----------------------------------------------------------------------------
mafView *medViewSlicer::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  medViewSlicer *v = new medViewSlicer(m_Label);
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
mafGUI* medViewSlicer::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);

	//m_Gui->Button(ID_RESET,"Reset","");
	m_Gui->Divider(2);

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

	m_Gui->Divider();
	m_Gui->Update();
	
	EnableWidgets(m_CurrentVolume != NULL);
	return m_Gui;
}
//----------------------------------------------------------------------------
void medViewSlicer::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && node == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
  }
  if(m_CurrentSlicer == node && node != NULL && m_AttachCamera)
  {
    m_AttachCamera->SetVme(NULL);
    m_CurrentSlicer = NULL;

    double normal[3] = {0,0,1};
    ((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_CT);
  }

  Superclass::VmeRemove(node);
}
//----------------------------------------------------------------------------
void medViewSlicer::CameraUpdate()
//----------------------------------------------------------------------------
{
	if (m_AttachCamera != NULL)
  {
		//Camera follows the slicer
    m_AttachCamera->UpdateCameraMatrix();
  }


  if(m_CurrentSlicer)
  {
    double normal[3];
    ((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

    mafNode *root=m_CurrentSlicer->GetRoot();
    mafNodeIterator *iter = root->NewIterator();
    for (mafNode *Inode = iter->GetFirstNode(); Inode; Inode = iter->GetNextNode())
    {
      if(Inode->IsA("mafVMESurface") || Inode->IsA("mafVMESurfaceParametric"))
      {
        mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewVTK *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(Inode));
        if(PipeSliceViewSurface)
        {
          double center[3], surfaceOriginTranslated[3];
         /* mafVMESurface *surface = mafVMESurface::SafeDownCast(Inode);
          surface->GetSurfaceOutput()->GetVTKData()->GetCenter(center);*/
          

          double b[6];
          m_CurrentSlicer->GetOutput()->GetBounds(b);
          center[0] = (b[1]+b[0])/2;
          center[1] = (b[3]+b[2])/2;
          center[2] = (b[5]+b[4])/2;

          surfaceOriginTranslated[0] = center[0] + normal[0] * 0.1;
          surfaceOriginTranslated[1] = center[1] + normal[1] * 0.1;
          surfaceOriginTranslated[2] = center[2] + normal[2] * 0.1;

          PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
          PipeSliceViewSurface->SetNormal(normal);
        }
      }
    }
    iter->Delete();

    ((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraReset(m_CurrentSlicer);
  }
  
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->CameraUpdate();
}
//----------------------------------------------------------------------------
void medViewSlicer::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mafGUI(this);
  
  m_LutSlider = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
  EnableWidgets(m_CurrentVolume != NULL);
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void medViewSlicer::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
		m_Gui->FitGui();
		m_Gui->Update();
  }
  
  m_LutSlider->Enable(m_CurrentSlicer != NULL);

}
//-------------------------------------------------------------------------
int medViewSlicer::GetNodeStatus(mafNode *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  mafSceneGraph *sgArb = ((mafViewVTK*)m_ChildViewList[ARBITRARY_VIEW])->GetSceneGraph();
  mafSceneGraph *sgSlice = ((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->GetSceneGraph();

  if (sgArb != NULL)
  {
    n = sgArb->Vme2Node(vme);
    if (((mafVME *)vme)->GetOutput()->IsA("mafVMEOutputVolume") ||
        vme->IsMAFType(mafVMESurface) ||
        vme->IsMAFType(mafVMESurfaceParametric))
    {
      if (n != NULL)
      {
      	n->m_PipeCreatable = true;
      }
    }
    else if (vme->IsMAFType(mafVMESlicer))
    {
      if (n != NULL)
      {
	      n->m_PipeCreatable = true;
	      n->m_Mutex = true;
      }
    }
  }
  return sgArb ? sgArb->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
