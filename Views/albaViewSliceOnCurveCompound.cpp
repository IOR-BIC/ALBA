/*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: albaViewSliceOnCurveCompound.cpp,v $ 
Language: C++ 
Date: $Date: 2012-04-06 09:36:51 $ 
Version: $Revision: 1.1.2.6 $ 
Authors: Eleonora Mambrini
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaViewSliceOnCurveCompound.h"
#include "albaViewSlice.h"

#include "albaGUI.h"
#include "albaRWI.h"

#include "albaMatrix.h"
#include "albaTransform.h"

#include "albaSceneNode.h"
#include "albaSceneGraph.h"
#include "albaVMEPolyline.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEPolylineGraph.h"

#include "albaPipeVolumeDRR.h"
#include "albaPipeVolumeMIP.h"
#include "albaPipeVolumeVR.h"
#include "albaPipeIsosurface.h"

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

#include "albaGUIFloatSlider.h"
#include "albaEventInteraction.h"
#include "albaIndent.h"

#include "albaVMESurface.h"
#include "albaVMEVolume.h"
#include "albaVMEOutputVolume.h"

#include "albaVMEGizmo.h"

#include "albaPipeVolumeArbSlice.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewSliceOnCurveCompound);
//----------------------------------------------------------------------------
#include "albaMemDbg.h"

//----------------------------------------------------------------------------
albaViewSliceOnCurveCompound::albaViewSliceOnCurveCompound(wxString label) : albaViewCompoundWindowing(label, 1, 3)
//----------------------------------------------------------------------------
{  
  m_LayoutConfiguration = albaViewSliceOnCurveCompound::LAYOUT_MPS_VERT; 
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
  m_EnableGPU=false;
}

#include "albaVMERoot.h"
//----------------------------------------------------------------------------
albaViewSliceOnCurveCompound::~albaViewSliceOnCurveCompound()
//----------------------------------------------------------------------------
{  
  if (m_Gizmo != NULL)
  {
    //BES: 8.4.2008 - albaGizmoXXXX connects its VME gizmo to the VME tree,
    //thus logically, it needs to disconnect it when it is being destroyed
    //disconnection triggers an event at root node that is forwarded to 
    //the listener. There is, however, somewhere a bug. When application
    //terminates, listener is destroyed but reference on it is kept in node root,
    //which means that the application crashes here.
    //Probably the problem roots from multiple inheritance.
    //This is hack to deal with it:

    // find the root
    albaObserver* listener = NULL;
    albaVMERoot* root = albaVMERoot::SafeDownCast(m_Gizmo->GetOutput()->GetParent());
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
albaView *albaViewSliceOnCurveCompound::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewSliceOnCurveCompound *v = this->NewInstance(); 
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
void albaViewSliceOnCurveCompound::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
  wxWindowDisabler wait1;
  wxBusyCursor wait2;

  if (vme->GetOutput()->IsA("albaVMEOutputPolyline"))
  {
    //some polyline curve, e.g., albaVMEPolyLine or albaVMEPolylineGraph
    if (show)
    {
      //we may have only one curve => we will need to hide them
      HideSameVMEs(m_ChildViewList[POLYLINE_VIEW], vme);     

      if (m_ShowPolylineInMainView)
        HideSameVMEs(m_ChildViewList[MAIN_VIEW], vme);
    }

    m_ChildViewList[POLYLINE_VIEW]->VmeShow(vme, show);

    if (m_ShowPolylineInMainView)
      m_ChildViewList[MAIN_VIEW]->VmeShow(vme, show);

    //create or destroy gizmo
    if (show)
      CreateGizmo(vme);
    else
      DestroyGizmo();    
  }
  else
  {	
    //check if we have Volume on the output
    if (vme->GetOutput() != NULL && vme->GetOutput()->IsA("albaVMEOutputVolume"))
    {
      if (show)
      {
        //hide all other volumes, we may have only one volume
        for (int i = 0; i < m_NumOfChildView; i++) 
        {
          if (i != POLYLINE_VIEW)
            HideSameVMEs(m_ChildViewList[i], vme);
        }

        m_CurrentVolume = vme;
      }
      else
      {
        assert(m_CurrentVolume == vme);
        m_CurrentVolume = NULL;
      }
    }


    for (int i = 0; i < m_NumOfChildView; i++) 
    {
      if (i != POLYLINE_VIEW)
        m_ChildViewList[i]->VmeShow(vme, show);
    }	
  }  

	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);

	GetLogicManager()->CameraUpdate();
}

//------------------------------------------------------------------------
//Hides VMEs of the same type as pNode that are currently displayed in pView
//The display status of pNode is not changed
void albaViewSliceOnCurveCompound::HideSameVMEs(albaView *view, albaVME *vme)
//------------------------------------------------------------------------
{
  if (view == NULL || vme == NULL || vme->GetOutput() == NULL)
    return; //invalid call

  albaSceneGraph* pSc = view->GetSceneGraph();
  albaSceneNode* pScNode = pSc->GetNodeList();  
  const albaTypeID& typeId = vme->GetOutput()->GetTypeId();
  while (pScNode != NULL)
  {
    albaVMEOutput* pOutput = NULL;
		albaVME * otherVME = pScNode->GetVme();
    if (otherVME != NULL)
      pOutput = otherVME->GetOutput();

    if (pOutput != NULL && pOutput->IsA(typeId))
    {
      if (pScNode->GetPipe() != NULL && otherVME != vme)
      {            
        GetLogicManager()->VmeShow(otherVME, false);
      }
    }

    pScNode = pScNode->GetNext();
  }      
}

//----------------------------------------------------------------------------
//return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... 
//having albaViewCompound::GetNodeStatus allow albaGUICheckTree to not know about albaSceneGraph
int albaViewSliceOnCurveCompound::GetNodeStatus(albaVME *vme)
//----------------------------------------------------------------------------
{
  if (vme->GetOutput()->IsA("albaVMEOutputPolyline"))
  {
    albaSceneGraph* pSc = m_ChildViewList[POLYLINE_VIEW]->GetSceneGraph();
    if (pSc == NULL)
      return NODE_NON_VISIBLE;

    albaSceneNode* pNode = pSc->Vme2Node(vme);
    if (pNode != NULL)          //it may be NULL during the termination
      pNode->SetMutex(true);    //force it as MUTEX

    return pSc->GetNodeStatus(vme);
  }

  return m_ChildViewList[MAIN_VIEW]->GetNodeStatus(vme);
}

//------------------------------------------------------------------------
//return the current pipe for the specified vme (if any exist at this moment) */
/*virtual*/ albaPipe* albaViewSliceOnCurveCompound::GetNodePipe(albaVME *vme)
//------------------------------------------------------------------------
{
  return m_ChildViewList[MAIN_VIEW]->GetNodePipe(vme);
}
#pragma endregion //VME manipulation


//GUI
#pragma region GUI stuff
//-------------------------------------------------------------------------
//Creates views and text mappers
/*virtual*/ void albaViewSliceOnCurveCompound::Create()
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

  albaRWI* Rwi = ((albaViewVTK*)m_ChildViewList[POLYLINE_VIEW])->m_Rwi;
  if (Rwi != NULL)
    Rwi->m_RenFront->AddActor(m_TextActor);

  assert(Rwi != NULL);
  //AACC 17.7.08 navigation stuff...
  ((albaViewVTK*)m_ChildViewList[MAIN_VIEW])->GetRWI()->GetCamera()->GetPosition(m_OldPos);
  //End AACC
}

//-------------------------------------------------------------------------
albaGUI* albaViewSliceOnCurveCompound::CreateGui()
//-------------------------------------------------------------------------
{	
  assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();  

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
		albaPipeVolumeArbSlice *p = NULL;
    p = albaPipeVolumeArbSlice::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
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
/*virtual*/ void albaViewSliceOnCurveCompound::LayoutSubView(int width, int height)
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
void albaViewSliceOnCurveCompound::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
					albaPipeVolumeArbSlice *p = NULL;
          p = albaPipeVolumeArbSlice::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
          if(p)
          {
            p->SetEnableGPU(m_EnableGPU);
            this->CameraUpdate();
          }
        }
      }
      break;

    default:
      Superclass::OnEvent(alba_event);
    }
  }
}

//Gizmo stuff
#pragma region Gizmo stuff
//------------------------------------------------------------------------
//creates the gizmo path
/*virtual*/ void albaViewSliceOnCurveCompound::CreateGizmo(albaVME *vme)
//------------------------------------------------------------------------
{
  assert(m_Gizmo == NULL);
  if (m_Gizmo != NULL)    
    return; //already constructed  

  albaVMEPolyline* polyline = albaVMEPolyline::SafeDownCast(vme);
  albaVMEPolylineGraph* polyline_gr = albaVMEPolylineGraph::SafeDownCast(vme);

  if (polyline == NULL && polyline_gr == NULL)
  {
    albaLogMessage("Unsupported VME for albaViewSliceOnCurveCompound::CreateGizmo");
    return;
  }     

#ifdef GIZMO_PATH
  albaVMEOutputPolyline* outputLine = albaVMEOutputPolyline::SafeDownCast(vme->GetOutput());
  m_GizmoLength = (outputLine != NULL ? outputLine->CalculateLength() : 0);
  assert(outputLine != NULL);
#endif // GIZMO_PATH

  m_CurrentPolyLine = vme;

  //construct new gizmo
#ifdef GIZMO_PATH
  //gizmo path works with polylines only
  if (polyline != NULL)
    polyline->Register(this);  
  else
  {
    albaNEW(polyline);

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
    albaNEW(polyline_gr);

    vtkPolyData* pdata = vtkPolyData::SafeDownCast(polyline->GetOutput()->GetVTKData());
    polyline_gr->SetData(pdata, 0);
    polyline_gr->GetOutput()->GetVTKData()->Update();  //to force construction of output
    polyline_gr->Update();                   //to confirm data, data now goes to output
  }

  m_CurrentPolyLineGizmo = polyline_gr;
#endif

#ifdef GIZMO_PATH
  m_Gizmo = new albaGizmoPath(m_CurrentPolyLine, this);

  double clr[3] = {1, 0, 0};
  m_Gizmo->SetColor(clr);
  m_Gizmo->SetConstraintPolyline(m_CurrentPolyLineGizmo);    
#else
  m_Gizmo = albaGizmoPolylineGraph::New(m_CurrentPolyLine, this);
  m_Gizmo->SetConstraintPolylineGraph((albaVMEPolylineGraph*)m_CurrentPolyLineGizmo);
#endif

  m_ChildViewList[POLYLINE_VIEW]->VmeShow(m_Gizmo->GetOutput(), true);
  if (m_ShowPolylineInMainView)
    m_ChildViewList[MAIN_VIEW]->VmeShow(m_Gizmo->GetOutput(), true);

  ResetSlicePosition();  
}

//------------------------------------------------------------------------
//destroys the gizmo path
/*virtual*/ void albaViewSliceOnCurveCompound::DestroyGizmo()
//------------------------------------------------------------------------
{
  if (m_Gizmo != NULL)
  {
    //destroy the current gizmo
    //BES: 3.2.2009 - gizmo output is destroyed during m_Gizmo destruction, 
    //however, it is still referenced in scenegraphs (probably some ALBA bug)
    //which leads into crash when a scene graph is being destroyed (close)
    //=> we need to remove vme from the graph (it calls also vmeshow(false))
    albaVMEGizmo* g = m_Gizmo->GetOutput(); 
    int nCount = (int)m_ChildViewList.size();
    for (int i = 0; i < nCount; i++){
      m_ChildViewList[i]->VmeRemove(g);
    }

    //albaEventMacro(albaEvent(this, VME_REMOVING, g));

    albaDEL(m_Gizmo); //unfortunately this must not be done because of crash if you close the view frame    
    albaDEL(m_CurrentPolyLineGizmo);    
    m_CurrentPolyLine = NULL;
  }  
}

//------------------------------------------------------------------------
//this method is called to set slice for the slice child view
/*virtual*/ void albaViewSliceOnCurveCompound::SetSlice(double* Origin, double* Normal)
//------------------------------------------------------------------------
{
  albaViewSlice* vs = albaViewSlice::SafeDownCast(m_ChildViewList[SLICE_VIEW]);
  if (vs != NULL)
    vs->SetSlice(Origin, Normal);    
}

//------------------------------------------------------------------------
//handles the change of gizmo on the "curve"
/*virtual*/ void albaViewSliceOnCurveCompound::OnGizmoMoved()
//------------------------------------------------------------------------
{
  double pos[3], normal[3];

  albaVME* g = m_Gizmo->GetOutput();
  const albaMatrix* gmat = g->GetOutput()->GetMatrix();
  albaTransform::GetPosition(*gmat, pos);    //get the position

  albaMatrix matrix;  
  albaTransform::CopyRotation(*gmat, matrix);     

  double tmp[3] = {0, 0, 1};  
  matrix.MultiplyPoint(tmp, normal);
  vtkMath::Normalize(normal);

  //set slice  
  SetSlice(pos, normal);  

  //and update camera
  albaView* vs = m_ChildViewList[SLICE_VIEW];
  if ((m_SliceCameraAutoFocus | m_SliceCameraAutoRotate) != 0)
  {   
    //modify the camera (see albaRWI.cpp)
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
    albaViewVTK* mv = albaViewVTK::SafeDownCast(m_ChildViewList[MAIN_VIEW]);
    if (mv!=NULL)
    {
      //modify the camera (see albaRWI.cpp)
      vtkCamera* camera = mv->GetRWI()->GetCamera();

      double lvect[3] = {pos[0] - m_OldPos[0], pos[1] - m_OldPos[1], pos[2] - m_OldPos[2] };

      //reposition camera so it 

      if (vtkMath::Norm(lvect)>0.3)
      {
        camera->SetPosition(m_OldPos);
        albaLogMessage("Camera position: %f, %f, %f ",m_OldPos[0],m_OldPos[1],m_OldPos[2]);
        albaLogMessage("Focal position: %f, %f, %f ",pos[0],pos[1],pos[2]);
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
void albaViewSliceOnCurveCompound::SetSlicePosition(double abscisa, vtkIdType branchId)
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
/*virtual*/ void albaViewSliceOnCurveCompound::PackageView()
//----------------------------------------------------------------------------
{
  //create child views
  PlugChildView(new albaViewVTK(_("main"), CAMERA_PERSPECTIVE));		//main 3D view	
  PlugChildView(new albaViewVTK(_("main"), CAMERA_PERSPECTIVE));		//polyline curve view
  PlugChildView(new albaViewSlice(_("slice"), CAMERA_PERSPECTIVE, true /*CAMERA_OS_Z*/));			//slice view

  //Plug visualization pipes to all views
  PlugVolumePipe();
  PlugSurfacePipe();
  PlugMeshPipe();  
  PlugPolylinePipe();
}

//plugs a new volume visualization pipe
/*virtual*/ void albaViewSliceOnCurveCompound::PlugVolumePipe()
{	
  for (int i = 0; i < 2; i++)
  {
    std::vector< albaView* >& vws = i == 0 ? m_PluggedChildViewList : m_ChildViewList;
    if (MAIN_VIEW < vws.size())
    {
      albaViewVTK* v = ((albaViewVTK*)vws[MAIN_VIEW]);
      v->PlugVisualPipe("albaVMEVolumeGray", GetVolumePipesDesc()[m_VolumePipeConfiguration].szClassName, MUTEX);	
      v->PlugVisualPipe("albaVMELabeledVolume", GetVolumePipesDesc()[m_VolumePipeConfiguration].szClassName, MUTEX);
      v->PlugVisualPipe("albaVMEVolumeRGB", GetVolumePipesDesc()[m_VolumePipeConfiguration].szClassName, MUTEX);
    }

		if (SLICE_VIEW < vws.size())
		{
			albaViewVTK* vs = ((albaViewVTK*)vws[SLICE_VIEW]);

			vs->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeArbSlice", MUTEX);
			vs->PlugVisualPipe("albaVMELabeledVolume", "albaPipeVolumeArbSlice", MUTEX);
			vs->PlugVisualPipe("albaVMEVolumeRGB", "albaPipeVolumeArbSlice", MUTEX);
		}
  }
}

//plugs a new surface visualization pipe
/*virtual*/ void albaViewSliceOnCurveCompound::PlugSurfacePipe()
{
  for (int i = 0; i < 2; i++)
  {
    std::vector< albaView* >& vws = i == 0 ? m_PluggedChildViewList : m_ChildViewList;
    if (SLICE_VIEW < vws.size())
    {	
      albaViewVTK* vs = ((albaViewVTK*)vws[SLICE_VIEW]);
      vs->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
      vs->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice");
    }
  }
}

//plugs a new mesh visualization pipe
/*virtual*/ void albaViewSliceOnCurveCompound::PlugMeshPipe()
{
  for (int i = 0; i < 2; i++)
  {
    std::vector< albaView* >& vws = i == 0 ? m_PluggedChildViewList : m_ChildViewList;
    if (SLICE_VIEW < vws.size())
    {	
      albaViewVTK* vs = ((albaViewVTK*)vws[SLICE_VIEW]);
      vs->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice");
    }
  }
}

//------------------------------------------------------------------------
//plugs a new polyline (graph) visualization pipe
/*virtual*/ void albaViewSliceOnCurveCompound::PlugPolylinePipe()
//------------------------------------------------------------------------
{
  //Plug polyline pipe
  for (int i = 0; i < 2; i++)
  {
    std::vector< albaView* >& vws = i == 0 ? m_PluggedChildViewList : m_ChildViewList;
    if (POLYLINE_VIEW < vws.size())
    {	
      albaViewVTK* vs = ((albaViewVTK*)vws[POLYLINE_VIEW]);
      vs->PlugVisualPipe("albaVMEPolyline", "albaPipePolyline", MUTEX);	  
      vs->PlugVisualPipe("albaVMEPolylineGraph", "albaVisualPipePolylineGraph", MUTEX);	
    }
  }
}

//change the visualization pipe for volumes
/*virtual*/ void albaViewSliceOnCurveCompound::ChangeVolumePipe(const char* pipename)
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
    albaEventMacro( albaEvent( this, VME_SELECTED, m_CurrentVolume));
  }
}
#pragma endregion

#pragma region Misc
//------------------------------------------------------------------------
//Returns number of volumes pipes available for the user
/*virtual*/ int albaViewSliceOnCurveCompound::GetNumberOfVolumePipes()
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
/*virtual*/ void albaViewSliceOnCurveCompound::UpdateGizmoStatusText(const char* szText)
//------------------------------------------------------------------------
{
  m_TextMapper->SetInput(szText);
  m_TextMapper->Modified();
}

//-------------------------------------------------------------------------
void albaViewSliceOnCurveCompound::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);
  os << indent << "albaViewSliceOnCurveCompound" << '\t' << this << std::endl;

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
/*virtual*/ void albaViewSliceOnCurveCompound::OnShowPolylineInMainView()
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
/*virtual*/ void albaViewSliceOnCurveCompound::OnShowGizmoCoords()
//------------------------------------------------------------------------
{
  if (!m_ShowGizmoCoords || m_Gizmo == NULL)
    UpdateGizmoStatusText("");
  else
  {
    double pos[3];

    albaVME* g = m_Gizmo->GetOutput();
    const albaMatrix* gmat = g->GetOutput()->GetMatrix();
    albaTransform::GetPosition(*gmat, pos);    //get the position

    //update text
    wxString szText = albaString::Format("X = %.2f, Y = %.2f, Z = %.2f", pos[0], pos[1], pos[2]);
    UpdateGizmoStatusText(szText);
  }
}

const char** albaViewSliceOnCurveCompound::GetLayoutsNames()
{ 

  static const char* layoutNames[] = {"one row","Mps horz","Mps vert","Smp horz", "Smp vert",NULL,};
  return layoutNames;  
}

const albaViewSliceOnCurveCompound::VPIPE_ENTRY* albaViewSliceOnCurveCompound::GetVolumePipesDesc()
{
  static const albaViewSliceOnCurveCompound::VPIPE_ENTRY volumePipes[] = {{"albaPipeVolumeDRR", "DRR"},{"albaPipeVolumeMIP", "MIP"},{"albaPipeVolumeVR", "VR"},{"albaPipeIsosurface", "ISO"},{NULL, NULL},};
  return volumePipes;
}

#pragma  endregion