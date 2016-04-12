/*=========================================================================

 Program: MAF2
 Module: mafViewSliceBlend
 Authors: Matteo Giacomoni
 
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

#include "mafDecl.h"
#include "mafIndent.h"
#include "mafGUI.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipePolylineSlice.h"
#include "mafPipeMeshSlice.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEVolumeGray.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"
#include "mafGUIFloatSlider.h"
#include "mafGUILutSlider.h"
#include "mafPipePolylineGraphEditor.h"
#include "mafViewSliceBlend.h"
#include "mafPipeVolumeSliceBlend.h"

#include "vtkDataSet.h"
#include "vtkMAFRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkCoordinate.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkCamera.h"
#include "vtkTransform.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSliceBlend);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewSliceBlend::mafViewSliceBlend(wxString label, int camera_position, bool show_axes, bool show_grid, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid, stereo)
//----------------------------------------------------------------------------
{
  // default values
  m_CurrentVolume = NULL;
  m_Border        = NULL;

  //Default values of slices
  m_Slice1[0] = m_Slice1[1] = m_Slice1[2] = 0.0;
  m_Slice2[0] = m_Slice2[1] = m_Slice2[2] = 0.0;

  m_SliceInitialized = false;

  //Default values of opacity is 0.5
  m_Opacity = 0.5;

  m_CurrentSurface.clear();
}
//----------------------------------------------------------------------------
mafViewSliceBlend::~mafViewSliceBlend()
//----------------------------------------------------------------------------
{
  BorderDelete();
  m_CurrentSurface.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewSliceBlend::Copy(mafObserver *Listener, bool lightCopyEnabled /* = false */)
//----------------------------------------------------------------------------
{
  mafViewSliceBlend *v = new mafViewSliceBlend(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::Create()
  //----------------------------------------------------------------------------
{
  RWI_LAYERS num_layers = m_CameraPositionId != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;

  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::InitializeSlice(double slice1[3],double slice2[3])
//----------------------------------------------------------------------------
{
  memcpy(m_Slice1,slice1,sizeof(m_Slice1));
  memcpy(m_Slice2,slice2,sizeof(m_Slice2));
  m_SliceInitialized = true;
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  int result = GetNodeStatus(vme);

  //If node can't be visualized
  if (result == NODE_NON_VISIBLE)
  {
    return;
  }

  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->GetPipe());

  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;

		mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      // Initialize mafPipeVolumeSliceBlend
      if (pipe_name.Equals("mafPipeVolumeSliceBlend"))
      {
        m_CurrentVolume = n;
        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->m_Vme);
        int slice_mode;
        vtkDataSet *data = ((mafVME *)vme)->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
        // check the type of camera
        switch(m_CameraPositionId)
        {
        case CAMERA_OS_X:
          slice_mode = SLICE_X;
          break;
        case CAMERA_OS_Y:
          slice_mode = SLICE_Y;
          break;
        case CAMERA_OS_P:
          slice_mode = SLICE_ORTHO;
          break;
        case CAMERA_PERSPECTIVE:
          slice_mode = SLICE_ARB;
          break;
        default:
          slice_mode = SLICE_Z;
        }
        //check if slices are initialized
        if (m_SliceInitialized)
        {
          //If slice position is already set
          ((mafPipeVolumeSliceBlend *)pipe)->InitializeSliceParameters(slice_mode,m_Slice1,m_Slice2,false);
        }
        else
        {
          //If slice position isn't already set
          ((mafPipeVolumeSliceBlend *)pipe)->InitializeSliceParameters(slice_mode,false);
        }
        ((mafPipeVolumeSliceBlend *)pipe)->SetSliceOpacity(m_Opacity);
      }
      pipe->Create(n);
      n->SetPipe(pipe);
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);

	m_NumberOfVisibleVme--;

  //if vme is a volume detach camera
  if (vme->IsMAFType(mafVMEVolume))
  {
    m_CurrentVolume = NULL;
    if (m_AttachCamera)
    {
      m_AttachCamera->SetVme(NULL);
    }
  }
  assert(n && n->GetPipe());
	n->DeletePipe();

  if(vme->IsMAFType(mafVMELandmark))
  {
    //Update list of surfaces visualized
    UpdateSurfacesList(vme);
  }
}
//-------------------------------------------------------------------------
int mafViewSliceBlend::GetNodeStatus(mafNode *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  if (m_Sg != NULL)
  {
    if (vme->IsMAFType(mafVMEVolume))
    {
      n = m_Sg->Vme2Node(vme);
      //Only a volume can be visualized
      n->m_Mutex = true;
    }
    else if (vme->IsMAFType(mafVMESlicer))
    {
      n = m_Sg->Vme2Node(vme);
      n->SetPipeCreatable(false);
    }
    else if (vme->IsMAFType(mafVMEImage))
    {
      n = m_Sg->Vme2Node(vme);
      //n->m_Mutex = true;
      //It's impossible visualize vme image
      n->SetPipeCreatable(false);
    }
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mafGUI *mafViewSliceBlend::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);

  if (m_CurrentVolume)
  {
    // if a Volume is present use his bounds
    double b[6];
    mafVMEVolumeGray::SafeDownCast(m_CurrentVolume->m_Vme)->GetOutput()->GetBounds(b);
    m_Slice1Position = b[4];
    m_Slice2Position = b[5];
  }
  else
  {
    //Otherwise use 0,1 as range
    m_Slice1Position = 0;
    m_Slice2Position = 1;
  }

  //Create attach camera for the volume
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->FloatSlider(ID_OPACITY,&m_Opacity,0.0,1.0,_("Down"),_("Top"));
  m_Gui->FitGui();
  m_Gui->Enable(ID_OPACITY,m_CurrentVolume!=NULL);
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::OnEvent(mafEventBase *maf_event)
  //----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_OPACITY:
      {
        mafPipeVolumeSliceBlend *pipe = (mafPipeVolumeSliceBlend *)m_CurrentVolume->GetPipe();
        //Set new opacity for the pipe mafPipeVolumeSliceBlend
        pipe->SetSliceOpacity(m_Opacity);
        CameraUpdate();
      }
      break;
    default:
      //Other events
      mafEventMacro(*maf_event);
      break;
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  //If a volume is visualized set his lut range
  if(!m_CurrentVolume) 
    return;
	mafPipe * pipe = m_CurrentVolume->GetPipe();
  mafString pipe_name = pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeSliceBlend"))
  {
    mafPipeVolumeSliceBlend *pipe = (mafPipeVolumeSliceBlend *)pipe;
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::SetSliceLocalOrigin(double origin0[3],double origin1[3])
//----------------------------------------------------------------------------
{
  //If a volume is visualized set slice position
  if(m_CurrentVolume)
  {
    memcpy(m_Slice1,origin0,sizeof(origin0));
    memcpy(m_Slice2,origin1,sizeof(origin1));
		mafPipe * pipe = m_CurrentVolume->GetPipe();
    mafString pipe_name = pipe->GetTypeName();
    if (pipe_name.Equals("mafPipeVolumeSliceBlend"))
    {
      mafPipeVolumeSliceBlend *pipe = (mafPipeVolumeSliceBlend *)pipe;
      //Set origin0 for slice 0
      pipe->SetSlice(0,origin0);
      //Set origin1 for slice 1
      pipe->SetSlice(1,origin1);
    }
  }
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::SetSlice(int nSlice,double pos[3])
  //----------------------------------------------------------------------------
{
  if(nSlice>1 || nSlice<0)
    return;

  if(m_CurrentVolume)
  {
    //Check wicth slices to set position
    if(nSlice==0)
      memcpy(m_Slice1,pos,sizeof(m_Slice1));
    else if(nSlice==1)
      memcpy(m_Slice2,pos,sizeof(m_Slice2));
		mafPipe * pipe = m_CurrentVolume->GetPipe();
    mafString pipe_name = pipe->GetTypeName();
    if (pipe_name.Equals("mafPipeVolumeSliceBlend"))
    {
      mafPipeVolumeSliceBlend *pipe = (mafPipeVolumeSliceBlend *)pipe;
      pipe->SetSlice(nSlice,pos);
    }
  }
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::BorderCreate(double col[3])
//----------------------------------------------------------------------------
{
  if(m_Border) 
  {
    //Before create a new border delete previous border
    BorderDelete();
  }

  vtkPlaneSource *ps = vtkPlaneSource::New();
  ps->SetOrigin(0, 0, 0);
  ps->SetPoint1(1, 0, 0);
  ps->SetPoint2(0, 1, 0);

  vtkOutlineFilter *of = vtkOutlineFilter::New();
  of->SetInput((vtkDataSet *)ps->GetOutput());

  vtkCoordinate *coord = vtkCoordinate::New();
  coord->SetCoordinateSystemToNormalizedViewport();
  coord->SetValue(1, 1, 0);

  vtkPolyDataMapper2D *pdmd = vtkPolyDataMapper2D::New();
  pdmd->SetInput(of->GetOutput());
  pdmd->SetTransformCoordinate(coord);

  vtkProperty2D *pd = vtkProperty2D::New();
  pd->SetDisplayLocationToForeground();
  pd->SetLineWidth(3);
  pd->SetColor(col[0],col[1],col[2]);

  m_Border = vtkActor2D::New();
  m_Border->SetMapper(pdmd);
  m_Border->SetProperty(pd);
  m_Border->SetPosition(0,0);

  //Add border to ren front
  m_Rwi->m_RenFront->AddActor(m_Border);

  vtkDEL(ps);
  vtkDEL(of);
  vtkDEL(coord);
  vtkDEL(pdmd);
  vtkDEL(pd);
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::BorderDelete()
//----------------------------------------------------------------------------
{
  if(m_Border)
  {
    //Remove border from ren front
    m_Rwi->m_RenFront->RemoveActor(m_Border);
    vtkDEL(m_Border);
  }  
}

//----------------------------------------------------------------------------
void mafViewSliceBlend::UpdateSurfacesList(mafNode *node)
  //----------------------------------------------------------------------------
{
  // Remove node form surface list
  for(int i=0;i<m_CurrentSurface.size();i++)
  {
    if (m_CurrentSurface[i]==m_Sg->Vme2Node(node))
    {
      std::vector<mafSceneNode*>::iterator startIterator;
      m_CurrentSurface.erase(m_CurrentSurface.begin()+i);
    }
  }
}

//----------------------------------------------------------------------------
void mafViewSliceBlend::VmeShow(mafNode *node, bool show)
  //----------------------------------------------------------------------------
{
  if (node->IsMAFType(mafVMEVolume))
  {
    if (show)
    {
      // Attach the camera to the volume visualized
      if(m_AttachCamera)
        m_AttachCamera->SetVme(node);
    }
    else
    {
      //detach volume from the camera
      if(m_AttachCamera)
        m_AttachCamera->SetVme(NULL);
    }
    // CameraUpdate();
    // CameraReset(node);
    // m_Rwi->CameraUpdate();
  }

  Superclass::VmeShow(node, show);

  //m_Rwi->CameraReset(node);
  //m_Rwi->CameraUpdate();

  if (m_CurrentVolume!=NULL)
  {
    // if a Volume is present use his bounds for slices positions

    double b[6];
    mafVMEVolumeGray::SafeDownCast(m_CurrentVolume->m_Vme)->GetOutput()->GetBounds(b);
    m_Slice1Position = b[4];
    m_Slice2Position = b[5];

    m_Slice1[2] = m_Slice1Position;
    SetSlice(0,m_Slice1);
    m_Slice2[2] = m_Slice2Position;
    SetSlice(1,m_Slice2);
    CameraUpdate();

    m_Gui->Update();
  }
  else
  {
    //Otherwise use 0,1 as range 
    m_Slice1Position = 0;
    m_Slice2Position = 1;

    m_Gui->Update();
  }

  m_Gui->Enable(ID_OPACITY,m_CurrentVolume!=NULL);
}
//----------------------------------------------------------------------------
void mafViewSliceBlend::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(vme->IsA("mafVMEPolyline")||vme->IsA("mafVMESurface")||vme->IsA("mafVMEPolylineEditor"))
  {
    //remove surfaces from surfaces list
    this->UpdateSurfacesList(vme);
  }
  Superclass::VmeRemove(vme);
}

//-------------------------------------------------------------------------
void mafViewSliceBlend::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "mafViewSliceBlend" << '\t' << this << std::endl;
  os << indent << "Name" << '\t' << m_Label << std::endl;
  os << std::endl;
  m_Sg->Print(os,1);
}
//-------------------------------------------------------------------------
void mafViewSliceBlend::SetNormal(double normal[3])
//-------------------------------------------------------------------------
{
  //set normal to the surfaces pipes
  if(!m_CurrentSurface.empty())
  {
    for(int i=0;i<m_CurrentSurface.size();i++)
    {
			mafPipe * curSurfPipe = m_CurrentSurface.at(i)->GetPipe();
      if(curSurfPipe)
      {
        mafString pipe_name = curSurfPipe->GetTypeName();
        if (pipe_name.Equals("mafPipeSurfaceSlice"))
        {
          mafPipeSurfaceSlice *pipe = (mafPipeSurfaceSlice *)curSurfPipe;
          pipe->SetNormal(normal); 
        }
      }
    }
  }
}
//-------------------------------------------------------------------------
void mafViewSliceBlend::MultiplyPointByInputVolumeABSMatrix(double *point)
//-------------------------------------------------------------------------
{
  if(m_CurrentVolume && m_CurrentVolume->m_Vme)
  {
    mafMatrix *mat = ((mafVME *)m_CurrentVolume->m_Vme)->GetOutput()->GetMatrix();
    double coord[4];
    coord[0] = point[0];
    coord[1] = point[1];
    coord[2] = point[2];
    double result[4];

    vtkTransform *newT = vtkTransform::New();
    newT->SetMatrix(mat->GetVTKMatrix());
    newT->TransformPoint(coord, result);
    vtkDEL(newT);

    point[0] = result[0];
    point[1] = result[1];
    point[2] = result[2];
  }

}
