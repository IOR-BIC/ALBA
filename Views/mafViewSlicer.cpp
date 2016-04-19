/*=========================================================================

 Program: MAF2
 Module: mafViewSlicer
 Authors: Daniele Giunchi
 
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

#include "mafGUI.h"
#include "mafViewSlicer.h"
#include "mafViewSlice.h"
#include "mafVme.h"
#include "mafVMESlicer.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafPipeImage3D.h"
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
#include "mafInteractorGenericMouse.h"
#include "mafVMESlicer.h"
#include "mafTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafVMEIterator.h"
#include "mafGUILutPreset.h"
#include "mafVMEOutputSurface.h"
#include "mafAttribute.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafAttachCamera.h"

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
mafCxxTypeMacro(mafViewSlicer);
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
mafViewSlicer::mafViewSlicer(wxString label, bool show_ruler)
: mafViewCompoundWindowing(label, 1, 2)
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
mafViewSlicer::~mafViewSlicer()
//----------------------------------------------------------------------------
{
  cppDEL(m_AttachCamera);
}
//----------------------------------------------------------------------------
void mafViewSlicer::PackageView()
//----------------------------------------------------------------------------
{
	m_ViewArbitrary = new mafViewVTK("",CAMERA_PERSPECTIVE);
	//m_ViewArbitrary->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
	m_ViewArbitrary->PlugVisualPipe("mafVMESurface","mafPipeSurfaceTextured");
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
void mafViewSlicer::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
	m_ChildViewList[ARBITRARY_VIEW]->VmeShow(vme, show);
	m_ChildViewList[SLICE_VIEW]->VmeShow(vme, show);
	vme->Update();
	if (show)
	{
		if(vme->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			m_CurrentVolume = vme;

			// data update
      vme->GetOutput()->GetVTKData()->Update();
		}
		else if(vme->IsA("mafVMESurface") || vme->IsA("mafVMESurfaceParametric"))
		{
			//a surface is visible only if there is a volume in the view
			if(m_CurrentVolume)
			{
        CameraUpdate();
			}
		}
    else if(vme->IsA("mafVMESlicer"))
    {
      //Show Slicer
      m_CurrentSlicer = mafVMESlicer::SafeDownCast(vme);
        
      //Set camera of slice view in way that it will follow the volume
      if(!m_AttachCamera)
        m_AttachCamera=new mafAttachCamera(m_Gui,((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->m_Rwi,this);
      m_AttachCamera->SetStartingMatrix(m_CurrentSlicer->GetOutput()->GetAbsMatrix());
      m_AttachCamera->SetVme(m_CurrentSlicer);
      ((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraReset(m_CurrentSlicer);
    }
    else if(vme->IsA("mafVMEImage")) {
      m_CurrentImage = mafVMEImage::SafeDownCast(vme);
	  }
  }
	else//if show=false
	{
		
    if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
		{		
			m_CurrentVolume = NULL;
      m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
		}
    else if(vme->IsA("mafVMESlicer"))
    {
      m_AttachCamera->SetVme(NULL);
      m_CurrentSlicer = NULL;
      m_ColorLUT = NULL;
      m_LutWidget->SetLut(m_ColorLUT);
      m_LutSlider->Enable(false);
      double normal[3] = {0,0,1};
      ((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_CT);
    }
    else if(vme->IsA("mafVMEImage"))
    {
      m_CurrentImage = NULL;
      m_ColorLUT = NULL;
      m_LutWidget->SetLut(m_ColorLUT);
      m_LutSlider->Enable(false);
      double normal[3] = {0,0,1};
      ((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_CT);
    }
	}

	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);

  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

	//EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewSlicer::OnEvent(mafEventBase *maf_event)
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
void mafViewSlicer::OnEventThis(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        }
      }
      break;

	case ID_HELP:
		{
			mafEvent helpEvent;
			helpEvent.SetSender(this);
			mafString viewLabel = this->m_Label;
			helpEvent.SetString(&viewLabel);
			helpEvent.SetId(OPEN_HELP_PAGE);
			mafEventMacro(helpEvent);
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
mafView *mafViewSlicer::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSlicer *v = new mafViewSlicer(m_Label);
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
mafGUI* mafViewSlicer::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);

  mafEvent buildHelpGui;
  buildHelpGui.SetSender(this);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  mafEventMacro(buildHelpGui);

  if (buildHelpGui.GetArg() == true)
  {
	  m_Gui->Button(ID_HELP, "Help","");	
  }


	//m_Gui->Button(ID_RESET,"Reset","");
	m_Gui->Divider(2);

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

	m_Gui->Divider();
	m_Gui->Update();
	
	EnableWidgets(m_CurrentVolume != NULL);
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSlicer::VmeRemove(mafVME *vme)
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
    ((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_CT);
  }

  Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafViewSlicer::CameraUpdate()
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

    mafVME *root=m_CurrentSlicer->GetRoot();
    mafVMEIterator *iter = root->NewIterator();
    for (mafVME *Inode = iter->GetFirstNode(); Inode; Inode = iter->GetNextNode())
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
void mafViewSlicer::CreateGuiView()
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
void mafViewSlicer::EnableWidgets(bool enable)
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
int mafViewSlicer::GetNodeStatus(mafVME *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  mafSceneGraph *sgArb = ((mafViewVTK*)m_ChildViewList[ARBITRARY_VIEW])->GetSceneGraph();
  mafSceneGraph *sgSlice = ((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->GetSceneGraph();

  if (sgArb != NULL)
  {
    n = sgArb->Vme2Node(vme);
    if (vme->GetOutput()->IsA("mafVMEOutputVolume") ||
        vme->IsMAFType(mafVMESurface) ||
        vme->IsMAFType(mafVMESurfaceParametric))
    {
      if (n != NULL)
      {
      	n->SetPipeCreatable(true);
      }
    }
    else if (vme->IsMAFType(mafVMESlicer))
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
void mafViewSlicer::UpdateWindowing(bool enable,mafVME *vme)
//-------------------------------------------------------------------------
{
  EnableWidgets(enable);

  //Windowing can be applied on Slicers or on Images
  mafVMESlicer  *Slicer		= NULL;
  mafVMEImage   *Image	  = NULL;

  
  if(vme->IsA("mafVMESlicer")) {
    Slicer = mafVMESlicer::SafeDownCast(vme);
  }
  else if(vme->IsA("mafVMEImage")) {
    Image = mafVMEImage::SafeDownCast(vme);
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
bool mafViewSlicer::ActivateWindowing(mafVME *vme) 
//-------------------------------------------------------------------------
{
  bool conditions     = false;
  bool nodeHasPipe    = false;
	  
  vme->Update();

  if(vme->IsA("mafVMESlicer") && m_CurrentSlicer){

    mafVMESlicer *slicer = mafVMESlicer::SafeDownCast(vme);
    mafVMEVolumeGray *vol = mafVMEVolumeGray::SafeDownCast(m_CurrentSlicer->GetSlicedVMELink());
    if(vol) {
      conditions = true;
    }
    conditions = conditions && m_CurrentVolume;
  }

  else if(vme->IsA("mafVMEImage")){

    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {

      mafPipeImage3D *pipe = (mafPipeImage3D *)m_ChildViewList[i]->GetNodePipe(vme);
      conditions = (conditions && (pipe && pipe->IsGrayImage()));
    }
    //conditions = conditions & m_CurrentImage;
  }

  return conditions;
}


//-------------------------------------------------------------------------
void mafViewSlicer::SlicerWindowing(mafVMESlicer *slicer)
//-------------------------------------------------------------------------
{

  mafVMEVolumeGray *vol = mafVMEVolumeGray::SafeDownCast(
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