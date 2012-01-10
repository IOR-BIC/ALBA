/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewSliceBlend.cpp,v $
Language:  C++
Date:      $Date: 2012-01-10 15:48:47 $
Version:   $Revision: 1.1.2.3 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/


#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
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
#include "medPipePolylineGraphEditor.h"
#include "medViewSliceBlend.h"
#include "medPipeVolumeSliceBlend.h"

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
mafCxxTypeMacro(medViewSliceBlend);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medViewSliceBlend::medViewSliceBlend(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid, show_ruler, stereo)
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
medViewSliceBlend::~medViewSliceBlend()
//----------------------------------------------------------------------------
{
  BorderDelete();
  m_CurrentSurface.clear();
}
//----------------------------------------------------------------------------
mafView *medViewSliceBlend::Copy(mafObserver *Listener, bool lightCopyEnabled /* = false */)
//----------------------------------------------------------------------------
{
  medViewSliceBlend *v = new medViewSliceBlend(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void medViewSliceBlend::Create()
  //----------------------------------------------------------------------------
{
  RWI_LAYERS num_layers = m_CameraPositionId != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;

  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
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
void medViewSliceBlend::InitializeSlice(double slice1[3],double slice2[3])
//----------------------------------------------------------------------------
{
  memcpy(m_Slice1,slice1,sizeof(m_Slice1));
  memcpy(m_Slice2,slice2,sizeof(m_Slice2));
  m_SliceInitialized = true;
}
//----------------------------------------------------------------------------
void medViewSliceBlend::VmeCreatePipe(mafNode *vme)
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
  assert(n && !n->m_Pipe);

  if (pipe_name != "")
  {
    if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 1)
    {
      m_NumberOfVisibleVme = 1;
    }
    else
    {
      m_NumberOfVisibleVme++;
    }
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      // Initialize medPipeVolumeSliceBlend
      if (pipe_name.Equals("medPipeVolumeSliceBlend"))
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
          ((medPipeVolumeSliceBlend *)pipe)->InitializeSliceParameters(slice_mode,m_Slice1,m_Slice2,false);
        }
        else
        {
          //If slice position isn't already set
          ((medPipeVolumeSliceBlend *)pipe)->InitializeSliceParameters(slice_mode,false);
        }
        ((medPipeVolumeSliceBlend *)pipe)->SetSliceOpacity(m_Opacity);
      }
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void medViewSliceBlend::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 0)
    m_NumberOfVisibleVme = 0;
  else
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
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);

  if(vme->IsMAFType(mafVMELandmark))
  {
    //Update list of surfaces visualized
    UpdateSurfacesList(vme);
  }
}
//-------------------------------------------------------------------------
int medViewSliceBlend::GetNodeStatus(mafNode *vme)
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
      n->m_PipeCreatable = false;
    }
    else if (vme->IsMAFType(mafVMEImage))
    {
      n = m_Sg->Vme2Node(vme);
      //n->m_Mutex = true;
      //It's impossible visualize vme image
      n->m_PipeCreatable = false;
    }
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mafGUI *medViewSliceBlend::CreateGui()
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
void medViewSliceBlend::OnEvent(mafEventBase *maf_event)
  //----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_OPACITY:
      {
        medPipeVolumeSliceBlend *pipe = (medPipeVolumeSliceBlend *)m_CurrentVolume->m_Pipe;
        //Set new opacity for the pipe medPipeVolumeSliceBlend
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
void medViewSliceBlend::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  //If a volume is visualized set his lut range
  if(!m_CurrentVolume) 
    return;
  mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
  if (pipe_name.Equals("medPipeVolumeSliceBlend"))
  {
    medPipeVolumeSliceBlend *pipe = (medPipeVolumeSliceBlend *)m_CurrentVolume->m_Pipe;
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void medViewSliceBlend::SetSliceLocalOrigin(double origin0[3],double origin1[3])
//----------------------------------------------------------------------------
{
  //If a volume is visualized set slice position
  if(m_CurrentVolume)
  {
    memcpy(m_Slice1,origin0,sizeof(origin0));
    memcpy(m_Slice2,origin1,sizeof(origin1));
    mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
    if (pipe_name.Equals("medPipeVolumeSliceBlend"))
    {
      medPipeVolumeSliceBlend *pipe = (medPipeVolumeSliceBlend *)m_CurrentVolume->m_Pipe;
      //Set origin0 for slice 0
      pipe->SetSlice(0,origin0);
      //Set origin1 for slice 1
      pipe->SetSlice(1,origin1);
    }
  }
}
//----------------------------------------------------------------------------
void medViewSliceBlend::SetSlice(int nSlice,double pos[3])
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
    mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
    if (pipe_name.Equals("medPipeVolumeSliceBlend"))
    {
      medPipeVolumeSliceBlend *pipe = (medPipeVolumeSliceBlend *)m_CurrentVolume->m_Pipe;
      pipe->SetSlice(nSlice,pos);
    }
  }
}
//----------------------------------------------------------------------------
void medViewSliceBlend::BorderCreate(double col[3])
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
void medViewSliceBlend::BorderDelete()
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
void medViewSliceBlend::UpdateSurfacesList(mafNode *node)
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
void medViewSliceBlend::VmeShow(mafNode *node, bool show)
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
void medViewSliceBlend::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(vme->IsA("mafVMEPolyline")||vme->IsA("mafVMESurface")||vme->IsA("medVMEPolylineEditor"))
  {
    //remove surfaces from surfaces list
    this->UpdateSurfacesList(vme);
  }
  Superclass::VmeRemove(vme);
}

//-------------------------------------------------------------------------
void medViewSliceBlend::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "medViewSliceBlend" << '\t' << this << std::endl;
  os << indent << "Name" << '\t' << m_Label << std::endl;
  os << std::endl;
  m_Sg->Print(os,1);
}
//-------------------------------------------------------------------------
void medViewSliceBlend::SetNormal(double normal[3])
//-------------------------------------------------------------------------
{
  //set normal to the surfaces pipes
  if(!m_CurrentSurface.empty())
  {
    for(int i=0;i<m_CurrentSurface.size();i++)
    {
      if(m_CurrentSurface.at(i) && m_CurrentSurface.at(i)->m_Pipe)
      {
        mafString pipe_name = m_CurrentSurface.at(i)->m_Pipe->GetTypeName();
        if (pipe_name.Equals("mafPipeSurfaceSlice"))
        {
          mafPipeSurfaceSlice *pipe = (mafPipeSurfaceSlice *)m_CurrentSurface[i]->m_Pipe;
          pipe->SetNormal(normal); 
        }
      }
    }
  }
}
//-------------------------------------------------------------------------
void medViewSliceBlend::MultiplyPointByInputVolumeABSMatrix(double *point)
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
