/*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: mafViewSliceOnCurveCompound.cpp,v $ 
Language: C++ 
Date: $Date: 2012-04-06 09:36:51 $ 
Version: $Revision: 1.1.2.6 $ 
Authors: Eleonora Mambrini
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewSliceOnCurveCompound.h"
#include "mafViewSlice.h"

#include "mafGUI.h"
#include "mafRWI.h"

#include "mafMatrix.h"
#include "mafTransform.h"

#include "mafNodeRoot.h"
#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafVMEPolyline.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEPolylineGraph.h"

#include "mafPipeVolumeDRR.h"
#include "mafPipeVolumeMIP.h"
#include "mafPipeVolumeVR.h"
#include "mafPipeIsosurface.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkCamera.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkTextMapper.h"
#include "vtkRenderer.h"

#include "mafGUIFloatSlider.h"
#include "mafEventInteraction.h"
#include "mafEventSource.h"
#include "mafIndent.h"

#include "mafVMESurface.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"

#include "mafVMEGizmo.h"

#include "mafPipeVolumeSlice_BES.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSliceOnCurveCompound);
//----------------------------------------------------------------------------
#include "mafMemDbg.h"

//----------------------------------------------------------------------------
mafViewSliceOnCurveCompound::mafViewSliceOnCurveCompound(wxString label) : mafViewCompoundWindowing(label, 1, 3)
//----------------------------------------------------------------------------
{  
  m_LayoutConfiguration = mafViewSliceOnCurveCompound::LAYOUT_MPS_VERT; 
  m_VolumePipeConfiguration = 0;		//DRR by the default
  m_ShowPolylineInMainView = 0;     //Polyline is not visible in main view (by the default)
  m_ShowGizmoCoords = 1;

  m_Gizmo = NULL;
#ifdef GIZMO_PATH
  m_GizmoPos = 0;
  m_GizmoLength = -1;
#endif // GIZMO_PATH

  m_SliceCameraAutoFocus = m_SliceCameraAutoRotate = 0;
  m_SliceCameraNavigate3D = 0;

  m_CurrentVolume = NULL;
  m_CurrentPolyLine = NULL;
  m_CurrentPolyLineGizmo = NULL;

  m_TextMapper = NULL;
  m_TextActor = NULL;
  m_OldPos[0] = m_OldPos[1] = m_OldPos[2] = 0.;

  // Added by Losi 11.25.2009
  m_EnableGPU=FALSE;
}

#include "mafVMERoot.h"
//----------------------------------------------------------------------------
mafViewSliceOnCurveCompound::~mafViewSliceOnCurveCompound()
//----------------------------------------------------------------------------
{  
  if (m_Gizmo != NULL)
  {
    //BES: 8.4.2008 - mafGizmoXXXX connects its VME gizmo to the VME tree,
    //thus logically, it needs to disconnect it when it is being destroyed
    //disconnection triggers an event at root node that is forwarded to 
    //the listener. There is, however, somewhere a bug. When application
    //terminates, listener is destroyed but reference on it is kept in node root,
    //which means that the application crashes here.
    //Probably the problem roots from multiple inheritance.
    //This is hack to deal with it:

    // find the root
    mafObserver* listener = NULL;
    mafVMERoot* root = mafVMERoot::SafeDownCast(m_Gizmo->GetOutput()->GetParent());
    if (root != NULL)
    {
      listener = root->GetListener();
      root->SetListener(NULL);
    }

    DestroyGizmo();  

    if (root != NULL)
      root->SetListener(listener);
  }

  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);
}

//----------------------------------------------------------------------------
mafView *mafViewSliceOnCurveCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSliceOnCurveCompound *v = this->NewInstance(); 
  v->m_Label = m_Label;
  v->m_Listener = Listener;
  v->m_Id = m_Id;

  for (int i = 0;i < m_PluggedChildViewList.size(); i++) {
    v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
  }

  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->Create();
  return v;
}

//VME manipulation
#pragma region VmeAdd, VmeRemove, VmeShow, GetNodeStatus

//----------------------------------------------------------------------------
void mafViewSliceOnCurveCompound::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  wxWindowDisabler wait1;
  wxBusyCursor wait2;

  if (((mafVME *)node)->GetOutput()->IsA("mafVMEOutputPolyline"))
  {
    //some polyline curve, e.g., mafVMEPolyLine or mafVMEPolylineGraph
    if (show)
    {
      //we may have only one curve => we will need to hide them
      HideSameVMEs(m_ChildViewList[POLYLINE_VIEW], node);     

      if (m_ShowPolylineInMainView)
        HideSameVMEs(m_ChildViewList[MAIN_VIEW], node);
    }

    m_ChildViewList[POLYLINE_VIEW]->VmeShow(node, show);

    if (m_ShowPolylineInMainView)
      m_ChildViewList[MAIN_VIEW]->VmeShow(node, show);

    //create or destroy gizmo
    if (show)
      CreateGizmo(node);
    else
      DestroyGizmo();    
  }
  else
  {	
    //check if we have Volume on the output
    if (((mafVME *)node)->GetOutput() != NULL && ((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume"))
    {
      if (show)
      {
        //hide all other volumes, we may have only one volume
        for (int i = 0; i < m_NumOfChildView; i++) 
        {
          if (i != POLYLINE_VIEW)
            HideSameVMEs(m_ChildViewList[i], node);
        }

        m_CurrentVolume = node;
      }
      else
      {
        assert(m_CurrentVolume == node);
        m_CurrentVolume = NULL;
      }
    }


    for (int i = 0; i < m_NumOfChildView; i++) 
    {
      if (i != POLYLINE_VIEW)
        m_ChildViewList[i]->VmeShow(node, show);
    }	
  }  

  if(GetSceneGraph()->GetSelectedVme()==node)
  {
    UpdateWindowing( show && this->ActivateWindowing(node), node);
  }

  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

}

//------------------------------------------------------------------------
//Hides VMEs of the same type as pNode that are currently displayed in pView
//The display status of pNode is not changed
void mafViewSliceOnCurveCompound::HideSameVMEs(mafView* pView, mafNode* pNode)
//------------------------------------------------------------------------
{
  if (pView == NULL || mafVME::SafeDownCast(pNode) == NULL ||
    (((mafVME *)pNode)->GetOutput() == NULL))
    return; //invalid call

  mafSceneGraph* pSc = pView->GetSceneGraph();
  mafSceneNode* pScNode = pSc->GetNodeList();  
  const mafTypeID& typeId = ((mafVME *)pNode)->GetOutput()->GetTypeId();
  while (pScNode != NULL)
  {
    mafVMEOutput* pOutput = NULL;
    if (mafVME::SafeDownCast(pScNode->m_Vme) != NULL)
      pOutput = ((mafVME *)pScNode->m_Vme)->GetOutput();

    if (pOutput != NULL && pOutput->IsA(typeId))
    {
      if (pScNode->m_Pipe != NULL && pScNode->m_Vme != pNode)
      {            
        mafEventMacro(mafEvent(this, VME_SHOW, pScNode->m_Vme, false));
      }
    }

    pScNode = pScNode->m_Next;
  }      
}

//----------------------------------------------------------------------------
//return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... 
//having mafViewCompound::GetNodeStatus allow mafGUICheckTree to not know about mafSceneGraph
int mafViewSliceOnCurveCompound::GetNodeStatus(mafNode *node)
//----------------------------------------------------------------------------
{
  if (((mafVME*)node)->GetOutput()->IsA("mafVMEOutputPolyline"))
  {
    mafSceneGraph* pSc = m_ChildViewList[POLYLINE_VIEW]->GetSceneGraph();
    if (pSc == NULL)
      return NODE_NON_VISIBLE;

    mafSceneNode* pNode = pSc->Vme2Node(node);
    if (pNode != NULL)          //it may be NULL during the termination
      pNode->m_Mutex = true;    //force it as MUTEX

    return pSc->GetNodeStatus(node);
  }

  return m_ChildViewList[MAIN_VIEW]->GetNodeStatus(node);
}

//------------------------------------------------------------------------
//return the current pipe for the specified vme (if any exist at this moment) */
/*virtual*/ mafPipe* mafViewSliceOnCurveCompound::GetNodePipe(mafNode *vme)
//------------------------------------------------------------------------
{
  return m_ChildViewList[MAIN_VIEW]->GetNodePipe(vme);
}
#pragma endregion //VME manipulation


//GUI
#pragma region GUI stuff
//-------------------------------------------------------------------------
//Creates views and text mappers
/*virtual*/ void mafViewSliceOnCurveCompound::Create()
//-------------------------------------------------------------------------
{
  Superclass::Create();

  // text stuff  
  m_TextMapper = vtkTextMapper::New();
  m_TextMapper->SetInput("");
  //  m_TextMapper->GetTextProperty()->AntiAliasingOff();

  m_TextActor = vtkActor2D::New();
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->SetPosition(50,3); //20 because of coordinate symbols
  m_TextActor->GetProperty()->SetColor(0.7, 0.7, 0.7);    //purple

  mafRWI* Rwi = ((mafViewVTK*)m_ChildViewList[POLYLINE_VIEW])->m_Rwi;
  if (Rwi != NULL)
    Rwi->m_RenFront->AddActor(m_TextActor);

  assert(Rwi != NULL);
  //AACC 17.7.08 navigation stuff...
  ((mafViewVTK*)m_ChildViewList[MAIN_VIEW])->GetRWI()->GetCamera()->GetPosition(m_OldPos);
  //End AACC
}

//-------------------------------------------------------------------------
mafGUI* mafViewSliceOnCurveCompound::CreateGui()
//-------------------------------------------------------------------------
{	
  assert(m_Gui == NULL);
  mafView::CreateGui();  

  wxComboBox* combo = m_Gui->Combo(ID_LAYOUT_CHOOSER,_("Layout"), &m_LayoutConfiguration);
  const char** layouts = GetLayoutsNames();
  while (*layouts != NULL)
  {
    combo->Append(*layouts);
    layouts++;
  }

  m_Gui->Divider(2);

  //setup volume pipes
  combo = m_Gui->Combo(ID_VOL_PIPE,_("Volume pipe"), &m_VolumePipeConfiguration);
  const VPIPE_ENTRY* pVolPipes = GetVolumePipesDesc();
  while (pVolPipes->szClassName != NULL)
  {
    combo->Append(pVolPipes->szUserFriendlyName);
    pVolPipes++;
  }

  combo->SetToolTip(_("Selects the visual pipe that should be used for volumes. "
    "To modify the visual properties associated with the pipe (e.g., lookup table), "
    "see Visual Properties tab in Data Tree."));

  m_Gui->Divider(1);  
  m_Gui->Bool(ID_SHOW_POLYLINE_IN_MAINVIEW,_("Show Gizmo in the main view"), &m_ShowPolylineInMainView, 1, 
    _("Selects whether the polyline (curve) and the gizmo should be displayed also in the main view"));
  m_Gui->Bool(ID_SHOW_GIZMOCOORDS, _("Show Gizmo coords"), &m_ShowGizmoCoords, 1,
    _("If checked, the gizmo coordinates are displayed."));

#ifdef GIZMO_PATH
  m_Gui->FloatSlider(ID_TRACKER, "Position", &m_GizmoPos, 0, 1);
#endif

  m_Gui->Divider(1);

  m_Gui->Bool(ID_SLICECAMERA_AUTOFOCUS, "AutoFocus", &m_SliceCameraAutoFocus, 1,
    _("Toggles automatic focus of the camera in the slice view"));

  m_Gui->Bool(ID_SLICECAMERA_AUTOROTATE, "AutoRotate", &m_SliceCameraAutoRotate, 1,
    _("Toggles automatic rotation of the camera in the slice view"));

  m_Gui->Bool(ID_SLICECAMERA_NAVIGATE_3D, _("Navigate 3D"), &m_SliceCameraNavigate3D, 1, 
    _("Toggles automatic navigation in the 3D view"));

  m_Gui->Divider();

  // Added by Losi 11.25.2009
  if (m_CurrentVolume)
  {
    mafPipeVolumeSlice_BES *p = NULL;
    p = mafPipeVolumeSlice_BES::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
    if (p) // Is this required?
    {
      p->SetEnableGPU(m_EnableGPU);
    }
  };
  m_Gui->Divider(1);
  //m_Gui->Bool(ID_ENABLE_GPU,"Enable GPU",&m_EnableGPU,1);
  //m_Gui->Divider();

  return m_Gui;
}

//------------------------------------------------------------------------
// Redefine this method to define a custom layout.
/*virtual*/ void mafViewSliceOnCurveCompound::LayoutSubViewCustom(int width, int height)
//------------------------------------------------------------------------
{
  int border = 2;
  int x_pos, y_pos;

  if (m_LayoutConfiguration == LAYOUT_SMP_HORZ)
  {
    int step_width  = (width-border)  / (m_NumOfChildView - 1);
    int step_height = (height-2*border)/ 3*2;
    m_ChildViewList[SLICE_VIEW]->GetWindow()->SetSize(0,0,width,step_height);
#ifndef WIN32
    m_ChildViewList[SLICE_VIEW]->SetWindowSize(width,step_height);
#endif

    for (int r = 0; r < m_NumOfChildView; r++)
    {
      if (r == SLICE_VIEW)
        continue;

      x_pos = r*(step_width + border);
      y_pos = step_height;
      m_ChildViewList[r]->GetWindow()->SetSize(x_pos,y_pos,step_width,height - step_height);
#ifndef WIN32
      m_ChildViewList[r]->SetWindowSize(step_width,height - step_height);
#endif    
    }
  }
  else
  {
    assert(m_LayoutConfiguration == LAYOUT_SMP_VERT);

    int step_width  = (width-border)  / 3*2;
    int step_height = (height-2*border)/ (m_NumOfChildView - 1);
    m_ChildViewList[SLICE_VIEW]->GetWindow()->SetSize(0,0,step_width, height);
#ifndef WIN32
    m_ChildViewList[SLICE_VIEW]->SetWindowSize(step_width,height);
#endif

    for (int r = 0; r < m_NumOfChildView; r++)
    {
      if (r == SLICE_VIEW)
        continue;

      x_pos = step_width;
      y_pos = r*(step_height + border);
      m_ChildViewList[r]->GetWindow()->SetSize(x_pos,y_pos,width - step_width,step_height);
#ifndef WIN32
      m_ChildViewList[r]->SetWindowSize(width - step_width,step_height);
#endif    
    }
  }
}
#pragma endregion	//GUI stuff

//----------------------------------------------------------------------------
void mafViewSliceOnCurveCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_VOL_PIPE:
      ChangeVolumePipe(GetVolumePipesDesc()[m_VolumePipeConfiguration].szClassName);
      break;

    case ID_SHOW_POLYLINE_IN_MAINVIEW:
      OnShowPolylineInMainView();
      break;

#ifdef GIZMO_PATH
    case ID_TRACKER:
      SetSlicePosition(m_GizmoPos*m_GizmoLength);			
      break;  
#else
    case ID_TRANSFORM:      
      OnGizmoMoved();
      break;
#endif

    case ID_SLICECAMERA_AUTOFOCUS:
      if (m_SliceCameraAutoFocus && m_Gizmo != NULL)
        OnGizmoMoved();
      break;

    case ID_SLICECAMERA_AUTOROTATE:
      if (m_SliceCameraAutoRotate && m_Gizmo != NULL)
        OnGizmoMoved();
      break;

    case ID_SLICECAMERA_NAVIGATE_3D:
      OnGizmoMoved();
      break;

    case ID_SHOW_GIZMOCOORDS:
      OnShowGizmoCoords();
      m_ChildViewList[POLYLINE_VIEW]->CameraUpdate();
      break;
    
    // Added by Losi 11.25.2009 
    case ID_ENABLE_GPU:
      {
        if (m_CurrentVolume)
        {
          mafPipeVolumeSlice_BES *p = NULL;
          p = mafPipeVolumeSlice_BES::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
          if(p)
          {
            p->SetEnableGPU(m_EnableGPU);
            this->CameraUpdate();
          }
        }
      }
      break;

    default:
      Superclass::OnEvent(maf_event);
    }
  }
}

//Gizmo stuff
#pragma region Gizmo stuff
//------------------------------------------------------------------------
//creates the gizmo path
/*virtual*/ void mafViewSliceOnCurveCompound::CreateGizmo(mafNode* node)
//------------------------------------------------------------------------
{
  assert(m_Gizmo == NULL);
  if (m_Gizmo != NULL)    
    return; //already constructed  

  mafVMEPolyline* polyline = mafVMEPolyline::SafeDownCast(node);
  mafVMEPolylineGraph* polyline_gr = mafVMEPolylineGraph::SafeDownCast(node);

  if (polyline == NULL && polyline_gr == NULL)
  {
    mafLogMessage("Unsupported VME for mafViewSliceOnCurveCompound::CreateGizmo");
    return;
  }     

#ifdef GIZMO_PATH
  mafVMEOutputPolyline* outputLine = mafVMEOutputPolyline::SafeDownCast(((mafVME*)node)->GetOutput());
  m_GizmoLength = (outputLine != NULL ? outputLine->CalculateLength() : 0);
  assert(outputLine != NULL);
#endif // GIZMO_PATH

  m_CurrentPolyLine = node;

  //construct new gizmo
#ifdef GIZMO_PATH
  //gizmo path works with polylines only
  if (polyline != NULL)
    polyline->Register(this);  
  else
  {
    mafNEW(polyline);

    vtkPolyData* pdata = vtkPolyData::SafeDownCast(polyline_gr->GetOutput()->GetVTKData());
    polyline->SetData(pdata, 0);
    polyline->GetOutput()->GetVTKData()->Update();  //to force construction of output
    polyline->Update();                   //to confirm data, data now goes to output
  }

  m_CurrentPolyLineGizmo = polyline;
#else
  //Stefano's gizmo
  //gizmo path works with polylines graphs only
  if (polyline_gr != NULL)
    polyline_gr->Register(this);  
  else
  {
    mafNEW(polyline_gr);

    vtkPolyData* pdata = vtkPolyData::SafeDownCast(polyline->GetOutput()->GetVTKData());
    polyline_gr->SetData(pdata, 0);
    polyline_gr->GetOutput()->GetVTKData()->Update();  //to force construction of output
    polyline_gr->Update();                   //to confirm data, data now goes to output
  }

  m_CurrentPolyLineGizmo = polyline_gr;
#endif

#ifdef GIZMO_PATH
  m_Gizmo = new mafGizmoPath(m_CurrentPolyLine, this);

  double clr[3] = {1, 0, 0};
  m_Gizmo->SetColor(clr);
  m_Gizmo->SetConstraintPolyline((mafVME*)m_CurrentPolyLineGizmo);    
#else
  m_Gizmo = mafGizmoPolylineGraph::New((mafVME*)m_CurrentPolyLine, this);
  m_Gizmo->SetConstraintPolylineGraph((mafVMEPolylineGraph*)m_CurrentPolyLineGizmo);
#endif

  m_ChildViewList[POLYLINE_VIEW]->VmeShow(m_Gizmo->GetOutput(), true);
  if (m_ShowPolylineInMainView)
    m_ChildViewList[MAIN_VIEW]->VmeShow(m_Gizmo->GetOutput(), true);

  //////TODO: remove this
  //m_SliceCameraAutoFocus = m_SliceCameraAutoRotate = 1;

  //    LARGE_INTEGER liBegin;
  //    ::QueryPerformanceCounter(&liBegin);
  //
  //    for (int x = 0; x <= 200; x++) {
  //      SetSlicePosition(m_GizmoLength* x / 200.0);
  //    }
  //
  //    LARGE_INTEGER liEnd, liFreq;
  //    ::QueryPerformanceCounter(&liEnd);
  //    ::QueryPerformanceFrequency(&liFreq);
  //
  //    wxMessageBox(wxString::Format("OnCurve in %d ms",
  //      (int)(((liEnd.QuadPart - liBegin.QuadPart)*1000) / liFreq.QuadPart)));
  //////END


  ResetSlicePosition();  
}

//------------------------------------------------------------------------
//destroys the gizmo path
/*virtual*/ void mafViewSliceOnCurveCompound::DestroyGizmo()
//------------------------------------------------------------------------
{
  if (m_Gizmo != NULL)
  {
    //destroy the current gizmo
    //BES: 3.2.2009 - gizmo output is destroyed during m_Gizmo destruction, 
    //however, it is still referenced in scenegraphs (probably some openMAF bug)
    //which leads into crash when a scene graph is being destroyed (close)
    //=> we need to remove vme from the graph (it calls also vmeshow(false))
    mafVMEGizmo* g = m_Gizmo->GetOutput(); 
    int nCount = (int)m_ChildViewList.size();
    for (int i = 0; i < nCount; i++){
      m_ChildViewList[i]->VmeRemove(g);
    }

    //mafEventMacro(mafEvent(this, VME_REMOVING, g));

    mafDEL(m_Gizmo); //unfortunately this must not be done because of crash if you close the view frame    
    mafDEL(m_CurrentPolyLineGizmo);    
    m_CurrentPolyLine = NULL;
  }  
}

//------------------------------------------------------------------------
//this method is called to set slice for the slice child view
/*virtual*/ void mafViewSliceOnCurveCompound::SetSlice(double* Origin, double* Normal)
//------------------------------------------------------------------------
{
  mafViewSlice* vs = mafViewSlice::SafeDownCast(m_ChildViewList[SLICE_VIEW]);
  if (vs != NULL)
    vs->SetSlice(Origin, Normal);    
}

//------------------------------------------------------------------------
//handles the change of gizmo on the "curve"
/*virtual*/ void mafViewSliceOnCurveCompound::OnGizmoMoved()
//------------------------------------------------------------------------
{
  double pos[3], normal[3];

  mafVME* g = mafVME::SafeDownCast(m_Gizmo->GetOutput());
  const mafMatrix* gmat = g->GetOutput()->GetMatrix();
  mafTransform::GetPosition(*gmat, pos);    //get the position

  mafMatrix matrix;  
  mafTransform::CopyRotation(*gmat, matrix);     

  double tmp[3] = {0, 0, 1};  
  matrix.MultiplyPoint(tmp, normal);
  vtkMath::Normalize(normal);

  //set slice  
  SetSlice(pos, normal);  

  //and update camera
  mafView* vs = m_ChildViewList[SLICE_VIEW];
  if ((m_SliceCameraAutoFocus | m_SliceCameraAutoRotate) != 0)
  {   
    //modify the camera (see mafRWI.cpp)
    vtkCamera* camera = vs->GetRWI()->GetCamera();

    double fp[3], cp[3];
    camera->GetFocalPoint(fp);
    camera->GetPosition(cp);

    double lvect[3] = {cp[0] - fp[0], cp[1] - fp[1], cp[2] - fp[2] };
    double nsize = vtkMath::Norm(lvect);

    if (m_SliceCameraAutoFocus)
    {
      //set the point of camera focus and reposition the camera so the FOV is preserved
      camera->SetFocalPoint(pos);
      camera->SetPosition(pos[0] + lvect[0], pos[1] + lvect[1], pos[2] + lvect[2]);      
    }

    if (m_SliceCameraAutoRotate)
    {
      //reposition camera so it 
      camera->GetFocalPoint(fp);
      camera->SetPosition(fp[0] + normal[0]*nsize, 
        fp[1] + normal[1]*nsize, fp[2] + normal[2]*nsize);
    }

    vs->CameraReset();
  }  

  if (m_SliceCameraNavigate3D != 0)
  {
    mafViewVTK* mv = mafViewVTK::SafeDownCast(m_ChildViewList[MAIN_VIEW]);
    if (mv!=NULL)
    {
      //modify the camera (see mafRWI.cpp)
      vtkCamera* camera = mv->GetRWI()->GetCamera();

      double lvect[3] = {pos[0] - m_OldPos[0], pos[1] - m_OldPos[1], pos[2] - m_OldPos[2] };

      //reposition camera so it 

      if (vtkMath::Norm(lvect)>0.3)
      {
        camera->SetPosition(m_OldPos);
        mafLogMessage("Camera position: %f, %f, %f ",m_OldPos[0],m_OldPos[1],m_OldPos[2]);
        mafLogMessage("Focal position: %f, %f, %f ",pos[0],pos[1],pos[2]);
        camera->SetFocalPoint(pos);
        camera->SetViewAngle(90.);
        mv->CameraUpdate();
        m_OldPos[0] = pos[0];
        m_OldPos[1] = pos[1];
        m_OldPos[2] = pos[2];
      }		 
    }
  }

  //update text
  OnShowGizmoCoords();
  this->CameraUpdate();
}


//Update the slice according to the new position.
void mafViewSliceOnCurveCompound::SetSlicePosition(double abscisa, vtkIdType branchId)
{
  if (m_Gizmo == NULL)
    return;	//we have no polyline	

  m_Gizmo->SetCurvilinearAbscissa(
#ifndef GIZMO_PATH
    branchId,
#endif
    abscisa);

  OnGizmoMoved();
}
#pragma endregion



//Visualization Pipe Construction
#pragma region Visualization Pipe Construction
//----------------------------------------------------------------------------
/*virtual*/ void mafViewSliceOnCurveCompound::PackageView()
//----------------------------------------------------------------------------
{
  //create child views
  PlugChildView(new mafViewVTK(_("main"), CAMERA_PERSPECTIVE));		//main 3D view	
  PlugChildView(new mafViewVTK(_("main"), CAMERA_PERSPECTIVE));		//polyline curve view
  PlugChildView(new mafViewSlice(_("slice"), CAMERA_PERSPECTIVE, true /*CAMERA_OS_Z*/));			//slice view

  //Plug visualization pipes to all views
  PlugVolumePipe();
  PlugSurfacePipe();
  PlugMeshPipe();  
  PlugPolylinePipe();
}

//plugs a new volume visualization pipe
/*virtual*/ void mafViewSliceOnCurveCompound::PlugVolumePipe()
{	
  for (int i = 0; i < 2; i++)
  {
    std::vector< mafView* >& vws = i == 0 ? m_PluggedChildViewList : m_ChildViewList;
    if (MAIN_VIEW < vws.size())
    {
      mafViewVTK* v = ((mafViewVTK*)vws[MAIN_VIEW]);
      v->PlugVisualPipe("mafVMEVolumeGray", GetVolumePipesDesc()[m_VolumePipeConfiguration].szClassName, MUTEX);	
      v->PlugVisualPipe("mafVMELabeledVolume", GetVolumePipesDesc()[m_VolumePipeConfiguration].szClassName, MUTEX);
      v->PlugVisualPipe("mafVMEVolumeRGB", GetVolumePipesDesc()[m_VolumePipeConfiguration].szClassName, MUTEX);
      v->PlugVisualPipe("mafVMEVolumeLarge", GetVolumePipesDesc()[m_VolumePipeConfiguration].szClassName, MUTEX);
    }

    if (SLICE_VIEW < vws.size())
    {	
        mafViewVTK* vs = ((mafViewVTK*)vws[SLICE_VIEW]);

        vs->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice_BES", MUTEX);
        vs->PlugVisualPipe("mafVMELabeledVolume", "mafPipeVolumeSlice_BES", MUTEX);
        vs->PlugVisualPipe("mafVMEVolumeRGB", "mafPipeVolumeSlice_BES", MUTEX);
        vs->PlugVisualPipe("mafVMEVolumeLarge", "mafPipeVolumeSlice_BES", MUTEX);
    }
  }
}

//plugs a new surface visualization pipe
/*virtual*/ void mafViewSliceOnCurveCompound::PlugSurfacePipe()
{
  for (int i = 0; i < 2; i++)
  {
    std::vector< mafView* >& vws = i == 0 ? m_PluggedChildViewList : m_ChildViewList;
    if (SLICE_VIEW < vws.size())
    {	
      mafViewVTK* vs = ((mafViewVTK*)vws[SLICE_VIEW]);
      vs->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice_BES");
      vs->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice_BES");
    }
  }
}

//plugs a new mesh visualization pipe
/*virtual*/ void mafViewSliceOnCurveCompound::PlugMeshPipe()
{
  for (int i = 0; i < 2; i++)
  {
    std::vector< mafView* >& vws = i == 0 ? m_PluggedChildViewList : m_ChildViewList;
    if (SLICE_VIEW < vws.size())
    {	
      mafViewVTK* vs = ((mafViewVTK*)vws[SLICE_VIEW]);
      vs->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice_BES");
    }
  }
}

//------------------------------------------------------------------------
//plugs a new polyline (graph) visualization pipe
/*virtual*/ void mafViewSliceOnCurveCompound::PlugPolylinePipe()
//------------------------------------------------------------------------
{
  //Plug polyline pipe
  for (int i = 0; i < 2; i++)
  {
    std::vector< mafView* >& vws = i == 0 ? m_PluggedChildViewList : m_ChildViewList;
    if (POLYLINE_VIEW < vws.size())
    {	
      mafViewVTK* vs = ((mafViewVTK*)vws[POLYLINE_VIEW]);
      vs->PlugVisualPipe("mafVMEPolyline", "mafPipePolyline", MUTEX);	  
      vs->PlugVisualPipe("mafVMEPolylineGraph", "mafVisualPipePolylineGraph", MUTEX);	
    }
  }
}

//change the visualization pipe for volumes
/*virtual*/ void mafViewSliceOnCurveCompound::ChangeVolumePipe(const char* pipename)
{
  //hide items forcing so the destruction of previous visualization pipeline
  if (m_CurrentVolume != NULL)      
    m_ChildViewList[MAIN_VIEW]->VmeShow(m_CurrentVolume, false);  

  //plug a new volume pipeline
  PlugVolumePipe();

  //show volume VMEs once again
  if (m_CurrentVolume != NULL)
  {
    m_ChildViewList[MAIN_VIEW]->VmeShow(m_CurrentVolume, true);	

    //force GUI construction for new pipe
    mafEventMacro( mafEvent( this, VME_SELECTED, m_CurrentVolume));
  }
}
#pragma endregion

#pragma region Misc
//------------------------------------------------------------------------
//Returns number of volumes pipes available for the user
/*virtual*/ int mafViewSliceOnCurveCompound::GetNumberOfVolumePipes()
//------------------------------------------------------------------------
{
  int nRet = 0;
  const VPIPE_ENTRY* pList = GetVolumePipesDesc();
  if (pList != NULL)
  {
    while (pList->szClassName != NULL) {
      pList++; nRet++; //increase the number
    }
  }

  return nRet;
}

//------------------------------------------------------------------------
//update the text displayed in the polyline view
/*virtual*/ void mafViewSliceOnCurveCompound::UpdateGizmoStatusText(const char* szText)
//------------------------------------------------------------------------
{
  m_TextMapper->SetInput(szText);
  m_TextMapper->Modified();
}

//-------------------------------------------------------------------------
void mafViewSliceOnCurveCompound::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  os << indent << "mafViewSliceOnCurveCompound" << '\t' << this << std::endl;

  //print components view information

  for(int v = 0; v < m_NumOfChildView; v++)
  {
    m_ChildViewList[v]->Print(os, 1);
  }
}
#pragma endregion 

//Event Handlers
#pragma region Event Handlers
//------------------------------------------------------------------------
//handles the change of the visibility of polyline in the main view 
/*virtual*/ void mafViewSliceOnCurveCompound::OnShowPolylineInMainView()
//------------------------------------------------------------------------
{
  if (m_CurrentPolyLine != NULL)
  {
    m_ChildViewList[MAIN_VIEW]->VmeShow(m_CurrentPolyLine, m_ShowPolylineInMainView != 0);
    m_ChildViewList[MAIN_VIEW]->VmeShow(m_Gizmo->GetOutput(), m_ShowPolylineInMainView != 0);
    m_ChildViewList[MAIN_VIEW]->CameraUpdate();
  }
}

//------------------------------------------------------------------------
//handles the change of the visibility of gizmo status text
/*virtual*/ void mafViewSliceOnCurveCompound::OnShowGizmoCoords()
//------------------------------------------------------------------------
{
  if (!m_ShowGizmoCoords || m_Gizmo == NULL)
    UpdateGizmoStatusText("");
  else
  {
    double pos[3];

    mafVME* g = mafVME::SafeDownCast(m_Gizmo->GetOutput());
    const mafMatrix* gmat = g->GetOutput()->GetMatrix();
    mafTransform::GetPosition(*gmat, pos);    //get the position

    //update text
    wxString szText = wxString::Format("X = %.2f, Y = %.2f, Z = %.2f", pos[0], pos[1], pos[2]);
    UpdateGizmoStatusText(szText);
  }
}

const char** mafViewSliceOnCurveCompound::GetLayoutsNames()
{ 

  static const char* layoutNames[] = {"one row","Mps horz","Mps vert","Smp horz", "Smp vert",NULL,};
  return layoutNames;  
}

const mafViewSliceOnCurveCompound::VPIPE_ENTRY* mafViewSliceOnCurveCompound::GetVolumePipesDesc()
{
  static const mafViewSliceOnCurveCompound::VPIPE_ENTRY volumePipes[] = {{"mafPipeVolumeDRR", "DRR"},{"mafPipeVolumeMIP", "MIP"},{"mafPipeVolumeVR", "VR"},{"mafPipeIsosurface", "ISO"},{NULL, NULL},};
  return volumePipes;
}

#pragma  endregion