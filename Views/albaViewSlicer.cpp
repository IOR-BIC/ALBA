/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewSlicer
 Authors: Daniele Giunchi
 
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

#include "albaGUI.h"
#include "albaViewSlicer.h"
#include "albaViewSlice.h"
#include "albaVme.h"
#include "albaVMESlicer.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaPipeImage3D.h"
#include "albaPipeSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaPipeSurface.h"
#include "albaPipeSurfaceTextured.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaSceneGraph.h"
#include "albaEvent.h"
#include "albaAbsMatrixPipe.h"
#include "albaAttachCamera.h"
#include "albaInteractorGenericMouse.h"
#include "albaVMESlicer.h"
#include "albaTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "albaVMEIterator.h"
#include "albaGUILutPreset.h"
#include "albaVMEOutputSurface.h"
#include "albaAttribute.h"
#include "albaGUILutSlider.h"
#include "albaGUILutSwatch.h"
#include "albaAttachCamera.h"

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
albaCxxTypeMacro(albaViewSlicer);
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
albaViewSlicer::albaViewSlicer(wxString label, bool show_ruler)
: albaViewCompoundWindowing(label, 1, 2)
//----------------------------------------------------------------------------
{
	m_ViewArbitrary = NULL;
	m_ViewSlice = NULL;

	m_CurrentVolume = NULL;
  m_CurrentImage  = NULL;
	m_CurrentSlicer = NULL;
	
	m_AttachCamera = NULL;

	m_SliceCenterSurface[0] = 0.0;
	m_SliceCenterSurface[1] = 0.0;
	m_SliceCenterSurface[2] = 0.0;

	m_SliceCenterSurfaceReset[0] = 0.0;
	m_SliceCenterSurfaceReset[1] = 0.0;
	m_SliceCenterSurfaceReset[2] = 0.0;

}
//----------------------------------------------------------------------------
albaViewSlicer::~albaViewSlicer()
//----------------------------------------------------------------------------
{
  cppDEL(m_AttachCamera);
}
//----------------------------------------------------------------------------
void albaViewSlicer::PackageView()
//----------------------------------------------------------------------------
{
	m_ViewArbitrary = new albaViewVTK("",CAMERA_PERSPECTIVE);
	//m_ViewArbitrary->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
	m_ViewArbitrary->PlugVisualPipe("albaVMESurface","albaPipeSurfaceTextured");
	m_ViewArbitrary->PlugVisualPipe("albaVMEVolumeGray", "albaPipeBox", MUTEX);
  m_ViewArbitrary->PlugVisualPipe("albaVMELabeledVolume", "albaPipeBox", MUTEX);
	
  m_ViewSlice = new albaViewVTK("",CAMERA_CT);
	m_ViewSlice->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
  m_ViewSlice->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMEGizmo", "albaPipeGizmo", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("albaVMEVolumeGray", "albaPipeBox", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("albaVMEProsthesis", "albaPipeSurfaceSlice");

	
	PlugChildView(m_ViewArbitrary);
	PlugChildView(m_ViewSlice);

}
//----------------------------------------------------------------------------
void albaViewSlicer::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
	m_ChildViewList[ARBITRARY_VIEW]->VmeShow(vme, show);
	m_ChildViewList[SLICE_VIEW]->VmeShow(vme, show);
	vme->Update();
	if (show)
	{
		if(vme->GetOutput()->IsA("albaVMEOutputVolume"))
		{
			m_CurrentVolume = vme;
		}
		else if(vme->IsA("albaVMESurface") || vme->IsA("albaVMESurfaceParametric"))
		{
			//a surface is visible only if there is a volume in the view
			if(m_CurrentVolume)
			{
        CameraUpdate();
			}
		}
    else if(vme->IsA("albaVMESlicer"))
    {
      //Show Slicer
      m_CurrentSlicer = albaVMESlicer::SafeDownCast(vme);
        
      //Set camera of slice view in way that it will follow the volume
      if(!m_AttachCamera)
        m_AttachCamera=new albaAttachCamera(m_Gui,((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->m_Rwi,this);
      m_AttachCamera->SetStartingMatrix(m_CurrentSlicer->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
      m_AttachCamera->SetVme(m_CurrentSlicer);
      ((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraReset(m_CurrentSlicer);
    }
    else if(vme->IsA("albaVMEImage")) {
      m_CurrentImage = albaVMEImage::SafeDownCast(vme);
	  }
  }
	else//if show=false
	{
		
    if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
		{		
			m_CurrentVolume = NULL;
      m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
		}
    else if(vme->IsA("albaVMESlicer"))
    {
      m_AttachCamera->SetVme(NULL);
      m_CurrentSlicer = NULL;
      m_ColorLUT = NULL;
      m_LutWidget->SetLut(m_ColorLUT);
      m_LutSlider->Enable(false);
      double normal[3] = {0,0,1};
      ((albaViewSlice*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_CT);
    }
    else if(vme->IsA("albaVMEImage"))
    {
      m_CurrentImage = NULL;
      m_ColorLUT = NULL;
      m_LutWidget->SetLut(m_ColorLUT);
      m_LutSlider->Enable(false);
      double normal[3] = {0,0,1};
      ((albaViewSlice*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_CT);
    }
	}

	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);

	GetLogicManager()->CameraUpdate();

	//EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void albaViewSlicer::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (alba_event->GetSender() == this->m_Gui || alba_event->GetSender() == this->m_LutSlider) // from this view gui
  {
    OnEventThis(alba_event); 
  }
	else
  {
    // if no one can handle this event send it to the operation listener
    Superclass::OnEvent(alba_event); 
  }	
}
//----------------------------------------------------------------------------
void albaViewSlicer::OnEventThis(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
    case ID_RANGE_MODIFIED:
      {
        if(m_CurrentVolume || m_CurrentImage)
        {
          double low, hi;
          m_LutSlider->GetSubRange(&low,&hi);
          m_ColorLUT->SetTableRange(low,hi);
					GetLogicManager()->CameraUpdate();
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
			albaViewCompound::OnEvent(alba_event);
		}
	}
}
//----------------------------------------------------------------------------
albaView *albaViewSlicer::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewSlicer *v = new albaViewSlicer(m_Label);
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
albaGUI* albaViewSlicer::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

	//m_Gui->Button(ID_RESET,"Reset","");
	m_Gui->Divider(2);

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

	m_Gui->Divider();
	m_Gui->Update();
	
	EnableWidgets(m_CurrentVolume != NULL);
	return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewSlicer::VmeRemove(albaVME *vme)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && vme == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
  }
  if(m_CurrentSlicer == vme && vme != NULL && m_AttachCamera)
  {
    m_AttachCamera->SetVme(NULL);
    m_CurrentSlicer = NULL;

    double normal[3] = {0,0,1};
    ((albaViewSlice*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_CT);
  }

  Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void albaViewSlicer::CameraUpdate()
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
    ((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

    albaVME *root=m_CurrentSlicer->GetRoot();
    albaVMEIterator *iter = root->NewIterator();
    for (albaVME *Inode = iter->GetFirstNode(); Inode; Inode = iter->GetNextNode())
    {
      if(Inode->IsA("albaVMESurface") || Inode->IsA("albaVMESurfaceParametric"))
      {
        albaPipeSurfaceSlice *PipeSliceViewSurface = albaPipeSurfaceSlice::SafeDownCast(((albaViewVTK *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(Inode));
        if(PipeSliceViewSurface)
        {
          double center[3], surfaceOriginTranslated[3];
         /* albaVMESurface *surface = albaVMESurface::SafeDownCast(Inode);
          surface->GetSurfaceOutput()->GetVTKData()->GetCenter(center);*/
          

          double b[6];
          m_CurrentSlicer->GetOutput()->GetBounds(b);
          center[0] = (b[1]+b[0])/2;
          center[1] = (b[3]+b[2])/2;
          center[2] = (b[5]+b[4])/2;

          surfaceOriginTranslated[0] = center[0] + normal[0] * 0.1;
          surfaceOriginTranslated[1] = center[1] + normal[1] * 0.1;
          surfaceOriginTranslated[2] = center[2] + normal[2] * 0.1;

          PipeSliceViewSurface->SetSlice(surfaceOriginTranslated,normal);
        }
      }
    }
    iter->Delete();

    ((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraReset(m_CurrentSlicer);
  }
  
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewSlicer::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new albaGUI(this);
  
  m_LutSlider = new albaGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
  EnableWidgets(m_CurrentVolume != NULL);
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void albaViewSlicer::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
		m_Gui->FitGui();
		m_Gui->Update();
  }
  
  //m_LutSlider->Enable(m_CurrentSlicer != NULL);
  m_LutSlider->Enable(enable);

}
//-------------------------------------------------------------------------
int albaViewSlicer::GetNodeStatus(albaVME *vme)
//-------------------------------------------------------------------------
{
  albaSceneNode *n = NULL;
  albaSceneGraph *sgArb = ((albaViewVTK*)m_ChildViewList[ARBITRARY_VIEW])->GetSceneGraph();
  albaSceneGraph *sgSlice = ((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->GetSceneGraph();

  if (sgArb != NULL)
  {
    n = sgArb->Vme2Node(vme);
    if (vme->GetOutput()->IsA("albaVMEOutputVolume") ||
        vme->IsALBAType(albaVMESurface) ||
        vme->IsALBAType(albaVMESurfaceParametric))
    {
      if (n != NULL)
      {
      	n->SetPipeCreatable(true);
      }
    }
    else if (vme->IsALBAType(albaVMESlicer))
    {
      if (n != NULL)
      {
	      n->SetPipeCreatable(true);
	      n->SetMutex( true);
      }
    }
  }
  return sgArb ? sgArb->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}

//-------------------------------------------------------------------------
void albaViewSlicer::UpdateWindowing(bool enable,albaVME *vme)
//-------------------------------------------------------------------------
{
  EnableWidgets(enable);

  //Windowing can be applied on Slicers or on Images
  albaVMESlicer  *Slicer		= NULL;
  albaVMEImage   *Image	  = NULL;

  
  if(vme->IsA("albaVMESlicer")) {
    Slicer = albaVMESlicer::SafeDownCast(vme);
  }
  else if(vme->IsA("albaVMEImage")) {
    Image = albaVMEImage::SafeDownCast(vme);
  }

  if(Slicer) {
    if(enable)
    {
      SlicerWindowing(Slicer);
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
      ImageWindowing(Image);
    }
    else
    {
      m_LutSlider->SetRange(-100,100);
      m_LutSlider->SetSubRange(-100,100);
    }
  }

}


//-------------------------------------------------------------------------
bool albaViewSlicer::ActivateWindowing(albaVME *vme) 
//-------------------------------------------------------------------------
{
  bool conditions     = false;
  bool nodeHasPipe    = false;
	  
  vme->Update();

  if(vme->IsA("albaVMESlicer") && m_CurrentSlicer){

    albaVMESlicer *slicer = albaVMESlicer::SafeDownCast(vme);
    albaVMEVolumeGray *vol = albaVMEVolumeGray::SafeDownCast(m_CurrentSlicer->GetSlicedVMELink());
    if(vol) {
      conditions = true;
    }
    conditions = conditions && m_CurrentVolume;
  }

  else if(vme->IsA("albaVMEImage")){

    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {

      albaPipeImage3D *pipe = (albaPipeImage3D *)m_ChildViewList[i]->GetNodePipe(vme);
      conditions = (conditions && (pipe && pipe->IsGrayImage()));
    }
    //conditions = conditions & m_CurrentImage;
  }

  return conditions;
}


//-------------------------------------------------------------------------
void albaViewSlicer::SlicerWindowing(albaVMESlicer *slicer)
//-------------------------------------------------------------------------
{

  albaVMEVolumeGray *vol = albaVMEVolumeGray::SafeDownCast(
    slicer->GetSlicedVMELink());
  if(vol)
  {
    double sr[2];
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
}