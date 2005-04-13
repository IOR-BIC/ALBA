/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRWI.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-13 13:07:53 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
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

#include "mafRWI.h"

#include "mafDecl.h"  // per CAMERA_POSITIONS
#include "mafEvent.h"
//#include "mafAxes.h"
//#include "mafPipe.h"
//#include "mafSceneGraph.h"
//#include "mafSceneNode.h"

#include "mafNode.h"
//#include "mafNodeLandmark.h"
//#include "mafNodeLandmarkCloud.h"
//#include "mflMatrixPipeDirectCinematic.h"
//#include "mflAssembly.h"

//#include "vtkGridActor.h"  // users must see GRID_XYZ const declared in vtkGridActor
#include "vtkLight.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
//#include "vtkAutoStereoRenderWindow.h"
#include "vtkRenderWindow.h"
#include "vtkMath.h"
#include "vtkActor.h"
#include "vtkActor2D.h"

//----------------------------------------------------------------------------
mafRWI::mafRWI(wxWindow *parent, RWI_LAYERS layers, bool use_grid, int stereo)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_sg = NULL;

  m_l = vtkLight::New();
  m_l->SetLightTypeToCameraLight();  

  m_c = vtkCamera::New();
	m_c->SetViewAngle(20); 
  m_c->ParallelProjectionOn(); 

	m_r1 = vtkRenderer::New();
  m_r1->SetBackground(0.4,0.4,0.4);
  m_r1->SetActiveCamera(m_c);
  m_r1->AddLight(m_l);
  m_r1->BackingStoreOff();
  m_r1->LightFollowCameraOn(); 
	
  if(stereo)
  {
    m_rw = vtkRenderWindow::New(); //SIL. 13-11-2003: - temporary removed - cause error in OpenGLMApper during close
    m_rw->StereoCapableWindowOn();
    m_rw->StereoRenderOn();
    SetStereo(stereo);
  }
  else
    m_rw = vtkRenderWindow::New(); 
  m_rw->AddRenderer(m_r1);

  m_rwi = new mafRWIBase(parent, -1);
	m_rwi->SetRenderWindow(m_rw);
  
	m_r2 = NULL;
	if(layers == TWO_LAYER)
	{
		m_r2 = vtkRenderer::New();
		m_r2->SetBackground(0.4,0.4,0.4);
		m_r2->SetActiveCamera(m_c);
		m_r2->AddLight(m_l);
		m_r2->BackingStoreOff();
		m_r2->LightFollowCameraOn(); 
		m_r2->SetInteractive(0); 

    m_r1->SetLayer(0); 
		m_r2->SetLayer(1); 
    m_rw->SetNumberOfLayers(2);
    m_rw->AddRenderer(m_r2);
  }

	m_grid = NULL;
/*
  m_show_grid = use_grid;

  if(use_grid)
	{
     m_grid= vtkGridActor::New();
		 m_r1->AddActor(m_grid);
		 m_r1->AddActor2D(m_grid->GetLabelActor());
	   SetGridNormal(GRID_Y);
	}

	m_axes = new mafAxes(m_r1);
  m_axes->SetVisibility(0);
  */
}
//----------------------------------------------------------------------------
mafRWI::~mafRWI( ) 
//----------------------------------------------------------------------------
{
	  /*
    if(m_grid) m_r1->RemoveActor(m_grid);
		if(m_grid) m_r1->RemoveActor2D(m_grid->GetLabelActor());
		vtkDEL(m_grid);
		
		wxDEL(m_axes); //SIL. 31-10-2003: must be removed before deleting renderers
		*/
    vtkDEL(m_l);
		vtkDEL(m_c);
		if(m_r1) 
    {
      m_r1->GetActors();        //Paolo 23-06-2004
      m_rw->RemoveRenderer(m_r1);
    }
    vtkDEL(m_r1);
    if(m_r2) 
    {
      m_r2->GetActors();        //Paolo 23-06-2004
      m_rw->RemoveRenderer(m_r2);
    }
		vtkDEL(m_r2);
		if(m_rw) 
      m_rw->SetInteractor(NULL);    //Paolo 23-06-2004
    vtkDEL(m_rw);
    if(m_rwi) 
      m_rwi->SetRenderWindow(NULL); //Paolo 23-06-2004
		vtkDEL(m_rwi);  //SIL. 13-11-2003: The renderer has to be Deleted as last
}
//-----------------------------------------------------------------------------------------
void mafRWI::CameraSet(int cam_position)
//-----------------------------------------------------------------------------------------
{
  int x,y,z,vx,vy,vz;
  m_cam_position = cam_position;

  if(cam_position == CAMERA_PERSPECTIVE
	|| cam_position == CAMERA_PERSPECTIVE_FRONT
	|| cam_position == CAMERA_PERSPECTIVE_BACK
	|| cam_position == CAMERA_PERSPECTIVE_LEFT
	|| cam_position == CAMERA_PERSPECTIVE_RIGHT 
	|| cam_position == CAMERA_PERSPECTIVE_TOP 
	|| cam_position == CAMERA_PERSPECTIVE_BOTTOM 
  )
		m_c->ParallelProjectionOff();
	else
		m_c->ParallelProjectionOn();

  switch (cam_position) 
	{
		case CAMERA_FRONT:
		case CAMERA_PERSPECTIVE_FRONT:
			x=0; y=0; z=1; vx=0; vy=1; vz=0;
		break;
		case CAMERA_BACK:
		case CAMERA_PERSPECTIVE_BACK:
			x=0; y=0; z=-1;vx=0; vy=1; vz=0;
		break;
		case CAMERA_LEFT:
		case CAMERA_PERSPECTIVE_LEFT:
			x=-1 ;y=0; z=0; vx=0; vy=1; vz=0;
		break;
		case CAMERA_RIGHT:
		case CAMERA_PERSPECTIVE_RIGHT:
			x=1;y=0; z=0; vx=0; vy=1; vz=0;
		break;
		case CAMERA_TOP:
		case CAMERA_PERSPECTIVE_TOP:
			x=0; y=1; z=0; vx=0; vy=0; vz=-1;
		break;
		case CAMERA_BOTTOM:
		case CAMERA_PERSPECTIVE_BOTTOM:
			x=0; y=-1;z=0; vx=0; vy=0; vz=1;
		break;
		case CAMERA_PERSPECTIVE:
	  //x=1; y=1; z=1; vx=0; vy=1; vz=0; //SIL. 23-6-2003 modified
			x=0; y=1; z=4; vx=0; vy=1; vz=0; 
		break;
		case CAMERA_RX_FRONT:
			x=0; y=-1; z=0; vx=0; vy=0; vz=1;
		break;
		case CAMERA_RX_LEFT:
			x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
		break;
		case CAMERA_RX_RIGHT:
			x=-1;y=0; z=0; vx=0; vy=0; vz=1;
		break;
		case CAMERA_DRR_FRONT:
			m_c->ParallelProjectionOff();
			x=0; y=-1; z=0; vx=0; vy=0; vz=1;	//modified by Paolo 10-6-2003
		break;
		case CAMERA_DRR_LEFT:
			m_c->ParallelProjectionOff();
			x=1 ;y=0; z=0; vx=0; vy=0; vz=1;	//modified by Paolo 10-6-2003
		break;
		case CAMERA_DRR_RIGHT:
			m_c->ParallelProjectionOff();
			x=-1;y=0; z=0; vx=0; vy=0; vz=1;	//modified by Paolo 10-6-2003
		break;
		case CAMERA_CT:
			x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
		break;
		case CAMERA_OS_X:
			x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
		break;
		case CAMERA_OS_Y:
			x=0; y=-1; z=0; vx=0; vy=0; vz=1;
		break;
		case CAMERA_OS_Z:
			x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
		break;
		case CAMERA_OS_P:
      m_c->ParallelProjectionOff();     // Paolo 09/06/2004
			x=-1; y=-1; z=1; vx=0; vy=0; vz=1;
		break;
    // ste beg
    case CAMERA_RXFEM_XNEG:
      x=-1;y=0; z=0; vx=0; vy=0; vz=1;
    break;
    case CAMERA_RXFEM_XPOS:
      x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
    break;
    case CAMERA_RXFEM_YNEG:
      x=0; y=-1;z=0; vx=0; vy=0; vz=1;
    break;
    case CAMERA_RXFEM_YPOS:
      x=0; y=1; z=0; vx=0; vy=0; vz=1;
    break;
    case CAMERA_RXFEM_ZNEG:
      x=0; y=0; z=-1;vx=1; vy=0; vz=0;
    break;
    case CAMERA_RXFEM_ZPOS:
      x=0; y=0; z=1; vx=1; vy=0; vz=0;
    break;
    // ste end
  }
  m_c->SetFocalPoint(0,0,0);
  m_c->SetPosition(x*100,y*100,z*100);
  m_c->SetViewUp(vx,vy,vz);
  
	CameraReset();

}
//----------------------------------------------------------------------------
void mafRWI::SetSize(int x, int y, int w,int h)
//----------------------------------------------------------------------------
{
   ((wxWindow*)m_rwi)->SetSize(x,y,w,h);
}
//----------------------------------------------------------------------------
void mafRWI::Show(bool show)
//----------------------------------------------------------------------------
{
   m_rwi->Show(show);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridNormal(int normal_id)
//----------------------------------------------------------------------------
{
//   if(m_grid) m_grid->SetGridNormal(normal_id);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridPosition(float position)
//----------------------------------------------------------------------------
{
//   if(m_grid) m_grid->SetGridPosition(position);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridVisibility(bool show)
//----------------------------------------------------------------------------
{
/* 
  if(m_grid)
	 {
	   m_show_grid = show;
		 m_grid->SetVisibility(m_show_grid );
	   m_grid->GetLabelActor()->SetVisibility(m_show_grid );
	 }
   */
}
//----------------------------------------------------------------------------
void mafRWI::SetAxesVisibility(bool show)
//----------------------------------------------------------------------------
{
   //if(m_axes) m_axes->SetVisibility(show);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridColor(wxColor col)
//----------------------------------------------------------------------------
{
   //if(m_grid) m_grid->SetGridColor(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
}
//----------------------------------------------------------------------------
void mafRWI::SetBackgroundColor(wxColor col)
//----------------------------------------------------------------------------
{
            m_r1->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
   if(m_r2) m_r2->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
}
//----------------------------------------------------------------------------
void mafRWI::SetStereo(int stereo_type)
//----------------------------------------------------------------------------
{
  if(m_r2) {wxLogMessage("LAL WARNING: SetStereo is disabled for RWI with two layers"); return;}
	
	if (stereo_type < VTK_STEREO_CRYSTAL_EYES)
    stereo_type = 0; 
	  
  if(m_stereo_type == stereo_type) 
	   return;
 
	m_stereo_type = stereo_type;
  
  //warning: non portable
  m_rw->SetWindowId( (HWND) 0 );

  
  int *size = m_rw->GetSize();
	//m_rw->RemoveRenderer(m_r1);
  //vtkDEL(m_rw);
	//m_rw = vtkRenderWindow::New();
  //m_rw->SetSize(size);
  m_rw->SetStereoCapableWindow(m_stereo_type != 0);
  m_rw->SetStereoType(m_stereo_type);
  m_rw->SetStereoRender(m_stereo_type != 0);
	

  //warning: non portable
	m_rw->SetWindowId( (HWND) m_rwi->GetHWND() );

  //m_rw->AddRenderer(m_r1);
  //m_rwi->SetRenderWindow(m_rw);
  m_rwi->ReInitialize();
}
//----------------------------------------------------------------------------
void mafRWI::CameraUpdate()
//----------------------------------------------------------------------------
{
  //if(!m_rwi->IsShown()) return; //TODO: capire perche' non funziona piu' - risulta sempre nascosta
  if (m_rw->GetGenericWindowId() == 0) 
		return;

	m_r1->ResetCameraClippingRange(); 
  m_rw->Render();

}
//----------------------------------------------------------------------------
void mafRWI::CameraReset(mafNode *vme)
//----------------------------------------------------------------------------
{
  if (m_rw->GetGenericWindowId() == 0) 
		return;

//	if(m_grid && m_show_grid) m_grid->VisibilityOff();

  mafEventMacro(mafEvent(this,CAMERA_PRE_RESET,m_r1));  //SIL. 16-6-2004: - Attention - I'm sending m_r1, I suppose that m_r2 is never required 
  CameraReset(ComputeVisibleBounds(vme));
//	if(m_grid && m_show_grid) m_grid->VisibilityOn();

  mafEventMacro(mafEvent(this,CAMERA_POST_RESET,m_r1)); //SIL. 16-6-2004:
  m_r1->ResetCameraClippingRange(); //per la griglia
  m_rw->Render();
}
//----------------------------------------------------------------------------
float* mafRWI::ComputeVisibleBounds(mafNode *vme)
//----------------------------------------------------------------------------
{
  static float b1[6],b2[6]; // static so it is possible to return it
  b1[0]=b1[2]=b1[4]=0;
  b1[1]=b1[3]=b1[5]=100;
  return b1;
/*
	if(vme) 
		if(!vme->IsA("mafNodeRoot") ) 
			if(vme->GetCurrentData() ) 
				if(m_sg) 
					if(mafSceneNode *n = m_sg->Vme2Node(vme) )
						if(n->IsVisible())
	{
		vme->UpdateCurrentData();
		vme->GetCurrentData()->GetBounds(b1);
    float loc_p1[3],loc_p2[3],abs_p1[3], abs_p2[3];
    loc_p1[0] = b1[0];
    loc_p1[1] = b1[2];
    loc_p1[2] = b1[4];
    loc_p2[0] = b1[1];
    loc_p2[1] = b1[3];
    loc_p2[2] = b1[5];

    double r=0;
    if(vme->IsA("mafNodeLandmark"))
       r = ((mafNodeLandmark*)vme)->GetRadius();
    if(vme->IsA("mafNodeLandmarkCloud"))
       r = ((mafNodeLandmarkCloud*)vme)->GetRadius();
		loc_p1[0] -= r;
		loc_p1[1] -= r;
		loc_p1[2] -= r;
		loc_p2[0] += r;
		loc_p2[1] += r;
		loc_p2[2] += r;
    
    vtkLinearTransform *t=vme->GetAbsMatrixPipe();
		t->TransformPoint(loc_p1,abs_p1);
		t->TransformPoint(loc_p2,abs_p2);
    // TODO: test the usage of vtkMatrix4x4::MultiplyPoint instead

		
		b1[0] = abs_p1[0];
		b1[1] = abs_p2[0];
		b1[2] = abs_p1[1];
		b1[3] = abs_p2[1];
		b1[4] = abs_p1[2];
		b1[5] = abs_p2[2];

    //t->Delete();
		return b1;
	}
	m_r1->ComputeVisiblePropBounds(b1);   
	return b1;
  */
}
//----------------------------------------------------------------------------
void mafRWI::CameraReset(float bounds[6])
//----------------------------------------------------------------------------
{
  double view_up[3], view_look[3], view_right[3];

  m_c->OrthogonalizeViewUp();
  m_c->ComputeViewPlaneNormal();
  m_c->GetViewUp(view_up);
  vtkMath::Normalize(view_up);
  m_c->GetViewPlaneNormal(view_look);
  vtkMath::Cross(view_up, view_look, view_right);
  vtkMath::Normalize(view_right);
	
  //convert camera vectors to float
	float vu[3],vl[3],vr[3];
	for(int i=0; i<3; i++)
  {
	  vu[i]=view_up[i];
	  vl[i]=view_look[i];
	  vr[i]=view_right[i];
	} 	

  float height;	
  float width;	
  float depth;	
	float center[3];
  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;
	
	float diag[3];
	diag[0] = (bounds[1] - bounds[0]);
	diag[1] = (bounds[3] - bounds[2]);
	diag[2] = (bounds[5] - bounds[4]);

	//--------------------------------------
	if(m_c->GetParallelProjection())
	{
		height = fabs( 0.52 * vtkMath::Dot(vu,diag) );
		width  = fabs( 0.52 * vtkMath::Dot(vr,diag) );
		depth  = fabs( 0.52 * vtkMath::Dot(vl,diag) );
	}
  //--------------------------------------
  else 
	{
		height = width = depth = 0.5 * vtkMath::Norm(diag);	
  }
  //--------------------------------------

	height = (height == 0) ? 1.0 : height;	
  
	//check aspect ratio - and eventually compensate height
	float view_aspect  = (m_rwi->m_w*1.0)/(m_rwi->m_h*1.0);
	float scene_aspect = (width)/(height);
  if( scene_aspect > view_aspect )
  {
    height = width/view_aspect; 
	}

  float distance;
	distance  = height/tan(m_c->GetViewAngle()*vtkMath::Pi()/360.0);
	distance += depth/2;

  // update the camera
  m_c->SetFocalPoint(center[0],center[1],center[2]);
  m_c->SetPosition(center[0]+distance*vl[0],
                   center[1]+distance*vl[1],
                   center[2]+distance*vl[2]);

  // setup parallel scale
  m_c->SetParallelScale(height);
	
	//m_r1->ResetCameraClippingRange(bounds);
}




