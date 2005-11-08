/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-08 16:11:46 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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

#include "mafViewSlice.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"

#include "vtkDataSet.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewSlice::mafViewSlice(wxString label, int camera_position, bool show_axes, int stereo, bool external)
:mafViewVTK(label,camera_position,show_axes,stereo,external)
//----------------------------------------------------------------------------
{
  m_CurrentVolume = NULL;
}
//----------------------------------------------------------------------------
mafViewSlice::~mafViewSlice()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafView *mafViewSlice::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewSlice *v = new mafViewSlice(m_Label, m_CameraPosition, m_ShowAxes, m_StereoType, m_ExternalFlag);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewSlice::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->m_Pipe);
  if (vme->IsMAFType(mafVMEVolume))
  {
    m_CurrentVolume = n;
    pipe_name = "mafPipeVolumeStructuredSlice";
  }

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
    pipe->SetListener(this);
    if (pipe)
    {
      if (pipe_name.Equals("mafPipeVolumeStructuredSlice"))
      {
        int slice_mode;
        vtkDataSet *data = ((mafVME *)vme)->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
        switch(m_CameraPosition) 
        {
          case CAMERA_OS_X:
            slice_mode = mafPipeVolumeStructuredSlice::STRUCTURED_SLICE_X;
        	break;
          case CAMERA_OS_Y:
            slice_mode = mafPipeVolumeStructuredSlice::STRUCTURED_SLICE_Y;
          break;
          case CAMERA_OS_P:
            slice_mode = mafPipeVolumeStructuredSlice::STRUCTURED_SLICE_ORTHO;
          break;
          case CAMERA_PERSPECTIVE:
            slice_mode = mafPipeVolumeStructuredSlice::STRUCTURED_SLICE_Z;
          break;
          default:
            slice_mode = mafPipeVolumeStructuredSlice::STRUCTURED_SLICE_Z;
        }
        ((mafPipeVolumeStructuredSlice *)pipe)->InitializeSliceParameters(slice_mode,false);
      }
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
      if (m_NumberOfVisibleVme == 1)
      {
        mafEventMacro(mafEvent(this,CAMERA_RESET));
      }
      else
      {
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
    }
    else
    {
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
    }
  }
}
//----------------------------------------------------------------------------
void mafViewSlice::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 0)
  {
    m_NumberOfVisibleVme = 0;
  }
  else
  {
    m_NumberOfVisibleVme--;
  }
  if (vme->IsMAFType(mafVMEVolume))
  {
    m_CurrentVolume = NULL;
  }
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//-------------------------------------------------------------------------
int mafViewSlice::GetNodeStatus(mafNode *vme)
//-------------------------------------------------------------------------
{
  if (vme->IsMAFType(mafVMEVolume))
  {
    if (m_Sg != NULL)
    {
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      n->m_Mutex = true;
    }
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mmgGui *mafViewSlice::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      default:
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
void mafViewSlice::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) return;
  mafPipeVolumeStructuredSlice* pipe = (mafPipeVolumeStructuredSlice*) m_CurrentVolume->m_Pipe;
  pipe->SetLutRange(low_val, high_val); 
}
