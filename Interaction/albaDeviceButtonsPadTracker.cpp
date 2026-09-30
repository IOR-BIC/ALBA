/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadTracker
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// base
#include "albaDeviceButtonsPadTracker.h"

// gui
#include "albaGUI.h"

// general
#include "albaSmartPointer.h"
#include "albaEventInteraction.h"
#include "mmuIdFactory.h"

// I/O
#include "albaStorageElement.h"
#include "albaInteractionFactory.h"

// geometric
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaOBB.h"

// avatars
#include "albaAvatar.h"
#include "albaAvatar3D.h"

// VTK
#include "vtkTimerLog.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
// ALBA_ID_IMP(albaDeviceButtonsPadTracker::TRACKER_3D_MOVE)
// ALBA_ID_IMP(albaDeviceButtonsPadTracker::TRACKER_BOUNDS_UPDATED)

albaCxxTypeMacro(albaDeviceButtonsPadTracker)

//------------------------------------------------------------------------------
albaDeviceButtonsPadTracker::albaDeviceButtonsPadTracker()
//------------------------------------------------------------------------------
{
  albaNEW(m_TrackerToCanonicalTransform);
  albaNEW(m_LastPoseMatrix);

  m_TrackerToCanonicalTransform->Identity(); // work arround for a bug in GetMTime
  
  m_Avatar          = NULL;
  m_DefaultAvatar   = NULL;
  m_TrackedBoxOrientation[0] = m_TrackedBoxOrientation[1] = m_TrackedBoxOrientation[2] = 0;
  m_LastPose        = 0;
  m_LastMoveTime    = 0;
  m_MoveEventTimeOut= 20;
  //m_LastMoveEvent   = NULL;
  m_AvatarFlag      = 0;
  m_TBPosition[0] = m_TBPosition[1] = m_TBPosition[2] = 0;
}

//------------------------------------------------------------------------------
albaDeviceButtonsPadTracker::~albaDeviceButtonsPadTracker()
//------------------------------------------------------------------------------
{
  SetDefaultAvatar(NULL);
  albaDEL(m_TrackerToCanonicalTransform);
  albaDEL(m_LastPoseMatrix);
}
//------------------------------------------------------------------------------
albaID albaDeviceButtonsPadTracker::GetTracker3DMoveId()
//------------------------------------------------------------------------------
{
  static const albaID tracker3DModeId = mmuIdFactory::GetNextId("TRACKER_3D_MOVE");
  return tracker3DModeId;
}
//------------------------------------------------------------------------------
albaID albaDeviceButtonsPadTracker::GetTrackerBoundsUpdatedId()
//------------------------------------------------------------------------------
{
  static const albaID trackerBoundsUpdatedId = mmuIdFactory::GetNextId("TRACKER_BOUNDS_UPDATED");
  return trackerBoundsUpdatedId;
}
//------------------------------------------------------------------------------
int albaDeviceButtonsPadTracker::InternalStore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  if (Superclass::InternalStore(node))
    return ALBA_ERROR;

  //StoreMatrix(writer,m_TrackerToCanonicalTransform->GetMatrix(),"TrackerToCanonicalMatrix");
  node->StoreVectorN("TrackedBoxBounds",m_TrackedBounds.m_Bounds,6);
  node->StoreVectorN("TrackedBoxOrientation",m_TrackedBoxOrientation,3);

  // store default avatar if present
  if (m_DefaultAvatar)
  {
    return (node->StoreObject("Avatar",m_DefaultAvatar)?ALBA_OK:ALBA_ERROR);
  }

  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaDeviceButtonsPadTracker::InternalRestore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  assert(node);
  
  if (Superclass::InternalRestore(node))
    return ALBA_ERROR;


  if (node->RestoreVectorN("TrackedBoxBounds",m_TrackedBounds.m_Bounds,6)==ALBA_OK)
  {
    m_TrackedBounds.Modified();
    if (node->RestoreVectorN("TrackedBoxOrientation",m_TrackedBoxOrientation,3)==ALBA_OK)
    {
      if (albaStorageElement *sub_node=node->FindNestedElement("Avatar"))
      {
        albaObject *obj=sub_node->RestoreObject();
        if (albaAvatar3D *avatar=albaAvatar3D::SafeDownCast(obj))
        {
          SetDefaultAvatar(albaAvatar3D::SafeDownCast(avatar));
        }
        else
        {
          albaErrorMessage("find wrong type of Avatar (%s) while restoring albaDeviceButtonsPadTracker.",obj->GetTypeName());
          obj->Delete();
        }
      }
      
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaDeviceButtonsPadTracker::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()!=ALBA_OK)
    return ALBA_ERROR;

  if (m_Avatar)
  {
    m_Avatar->Initialize();
    InvokeEvent(this,AVATAR_ADDED,MCH_UP,m_Avatar);
  }

  m_LastPose = 0; // initialize last pose flag
  return ALBA_OK;
}
//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::InternalShutdown()
//------------------------------------------------------------------------------
{
  if (m_Avatar)
  {
    InvokeEvent(this,AVATAR_REMOVED,MCH_UP,m_Avatar);
    m_Avatar->Shutdown();
  }

  Superclass::InternalShutdown();
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::SetAvatar(albaAvatar *avatar)
//------------------------------------------------------------------------------
{
  if (m_Avatar)
  {
    if (m_Initialized)
    {
      InvokeEvent(this,AVATAR_REMOVED,MCH_UP,m_Avatar);
      m_Avatar->Shutdown();
    }

    GetGui()->Remove(m_Avatar->GetGui());
    GetGui()->GetSizer()->Detach(m_Avatar->GetGui());
    //m_Avatar->GetGui()->SetParent(NULL);
    m_Avatar->GetGui()->Show(false);
    m_Avatar->GetGui()->Refresh();
    m_Avatar->SetTracker(NULL);
  }

  m_Avatar = avatar;

  if (avatar)
  {
    avatar->SetTracker(this);

    // update gui
    GetGui()->AddGui(avatar->GetGui());
    GetGui()->FitGui();
    GetGui()->Refresh();
    GetGui()->Update();
    if (m_Initialized)
    {
      avatar->Initialize();
    }
  }
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::SetDefaultAvatar(albaAvatar *avatar)
//------------------------------------------------------------------------------
{
  m_DefaultAvatar=avatar;
  SetAvatar(avatar);
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::RestoreDefaultAvatar()
//------------------------------------------------------------------------------
{
  SetAvatar(m_DefaultAvatar);
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::SetLastPoseMatrix(const albaMatrix &matrix)
//------------------------------------------------------------------------------
{ 
  // I had to add a test on elapsed time since it seems sometimes
  // an event gets lost! :-(((
  m_LastPoseMutex.Lock(); 
  albaTimeStamp elapsed_time = (vtkTimerLog::GetUniversalTime()-m_LastMoveTime);

  // This is a very tricky thing: remove a move event after a give timeout
  // The only problem is if some other thread is pushing an event while we 
  // are doing this operation, since that event wouldn't be the last one.
  if (m_LastPose==1&&elapsed_time>m_MoveEventTimeOut)
  {
 /*   
    int queue_size = GetQueueSize();
    for (int i=0;i<queue_size;i++)
    {
      albaEventBase *temp;
      albaID ch;

      // check if for some reason queue is already empty (it shouldn't!)
      if (!PopEvent(temp,ch))
        break;
      
      if (temp->GetID()==mflThreadedAgent::AsyncDispatchEvent)
      {
        albaEventBase *event=albaEventBase::SafeDownCast((vtkObject*)temp->GetData());
        assert(event);
        if (event->GetID()==TRACKER_3D_MOVE)
        {
          temp->UnRegister(this); // simply unregister
          continue;
        }
      }      
      PushEvent(temp,ch); // re-push
      // it has been already re-registered by Push
      temp->UnRegister(this);
    }*/
    m_LastPose = 0;
    m_Dispatched=true;
  }

  // If last MoveEvent has not been served yet don't send a new one
  if (m_LastPose==0)
  {
    // set "serving move event" flag
    m_LastPose=1;

    // update current matrix
    *m_LastPoseMatrix=matrix;
    m_LastPoseMatrix->SetTimeStamp(vtkTimerLog::GetUniversalTime()); // set the time stamp to the current time

    albaEventInteraction move_event(this,GetTracker3DMoveId(),m_LastPoseMatrix);    

    m_LastMoveTime=m_LastPoseMatrix->GetTimeStamp();
    AsyncInvokeEvent(&move_event,MCH_INPUT);
   // m_LastMoveEvent = move_event;
    m_LastPoseMutex.Unlock();
  }
  else
  {
    m_LastPoseMutex.Unlock();
    albaSleep(25); // wait for a while to give time to serve last event
  }
}

//------------------------------------------------------------------------------
albaMatrix &albaDeviceButtonsPadTracker::GetLastPoseMatrix()
//------------------------------------------------------------------------------
{
  return *m_LastPoseMatrix;
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::SendButtonEvent(albaEventInteraction *event)
//------------------------------------------------------------------------------
{
  if (m_Avatar&&m_Avatar->GetMode()==albaAvatar::MODE_2D)
  {
    event->SetXYFlag(true); // signal we were in 2D mode...
  }
  
  albaSmartPointer<albaMatrix> last_pose;
  last_pose->DeepCopy(m_LastPoseMatrix); // make a copy of current pose to ovoid overwriting
  event->SetMatrix(last_pose);  
  
  AsyncInvokeEvent(event,MCH_INPUT);
}
//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::ComputeTrackerToCanonicalTansform()
//------------------------------------------------------------------------------
{
	double scale,center[3],dims[3];

  if (!m_TrackedBounds.IsValid())
  {
    //vtkGenericWarningMacro("Invalid tracked box bounds!");
    return;
  }

  m_TrackedBounds.GetDimensions(dims);

  double max_dim=GetMax3(dims[0],dims[1],dims[2]);

  scale=1.0/(max_dim/2.0);

  // normalize and center the tracked volume...
  m_CanonicalBounds.m_Bounds[0]=-dims[0]*scale/2;
  m_CanonicalBounds.m_Bounds[1]=dims[0]*scale/2;
  m_CanonicalBounds.m_Bounds[2]=-dims[1]*scale/2;
  m_CanonicalBounds.m_Bounds[3]=dims[1]*scale/2;
  m_CanonicalBounds.m_Bounds[4]=-dims[2]*scale/2;
  m_CanonicalBounds.m_Bounds[5]=dims[2]*scale/2;
  m_CanonicalBounds.Modified();
  
	m_TrackedBounds.GetCenter(center);

	m_TrackerToCanonicalTransform->Identity();

  // In this release the first inBox is supposed to be
	// oriented as the main axes: angles=(0,0,0). In next release
	// the right rotation will be find to map from the inBox c.s.
	// to the outBox c.s.
	m_TrackerToCanonicalTransform->Translate(-center[0],-center[1],-center[2],POST_MULTIPLY);
  m_TrackerToCanonicalTransform->RotateY(-m_TrackedBoxOrientation[1],POST_MULTIPLY);
	m_TrackerToCanonicalTransform->RotateX(-m_TrackedBoxOrientation[0],POST_MULTIPLY);
	m_TrackerToCanonicalTransform->RotateZ(-m_TrackedBoxOrientation[2],POST_MULTIPLY);

	m_TrackerToCanonicalTransform->Scale(scale,scale,scale,POST_MULTIPLY);
  
  // inform consumers the m_CanonicalBounds has been recomputed
  InvokeEvent(this,GetTrackerBoundsUpdatedId(),MCH_INPUT);
}

//------------------------------------------------------------------------------
albaOBB &albaDeviceButtonsPadTracker::GetCanonicalBounds()
//------------------------------------------------------------------------------
{
  if (m_CanonicalBounds.GetMTime()<m_TrackedBounds.GetMTime())
    ComputeTrackerToCanonicalTansform();

  return m_CanonicalBounds;
}

//------------------------------------------------------------------------------
albaTransform *albaDeviceButtonsPadTracker::GetTrackerToCanonicalTransform()
//------------------------------------------------------------------------------
{
  if (m_CanonicalBounds.GetMTime()<m_TrackedBounds.GetMTime())
    ComputeTrackerToCanonicalTansform();

  return m_TrackerToCanonicalTransform;
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::TrackerToCanonical(const albaMatrix &source,albaMatrix &dest)
//------------------------------------------------------------------------------
{
  albaMatrix::Multiply4x4(GetTrackerToCanonicalTransform()->GetMatrix(),
    source,
    dest);

  dest.SetTimeStamp(source.GetTimeStamp());
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::TrackerToCanonical(albaTransform *trans)
//------------------------------------------------------------------------------
{
  trans->Concatenate(GetTrackerToCanonicalTransform()->GetMatrix(),POST_MULTIPLY);
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::CanonicalToTracker(const albaMatrix &source,albaMatrix &dest)
//------------------------------------------------------------------------------
{
  albaMatrix canonical_to_tracker;
  canonical_to_tracker=this->m_TrackerToCanonicalTransform->GetMatrix();
  canonical_to_tracker.Invert();

  albaMatrix::Multiply4x4(canonical_to_tracker,
    source,
    dest);
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::CanonicalToTracker(albaTransform *trans)
//------------------------------------------------------------------------------
{
  albaMatrix canonical_to_tracker;
  canonical_to_tracker=m_TrackerToCanonicalTransform->GetMatrix();
  canonical_to_tracker.Invert();
  trans->Concatenate(canonical_to_tracker,POST_MULTIPLY);
}

//----------------------------------------------------------------------------
int albaDeviceButtonsPadTracker::AvatarChooser(wxString &avatar_name,wxString &avatar_type)
//----------------------------------------------------------------------------
{
  albaInteractionFactory *iFactory=albaInteractionFactory::GetInstance();

  assert(iFactory);
  
  const std::set<std::string> *avatars=iFactory->GetAvatarNames();

  if (avatars->size()==0)
  {
    albaErrorMessage("No avatars available!","Avatar Chooser Error");
    return ALBA_ERROR;
  }

  wxString *avatar_names = new wxString[avatars->size()];
  albaString *avatar_types= new albaString[avatars->size()];

  std::set<std::string>::const_iterator it=avatars->begin();
  for (int id=0;it!=avatars->end();id++,it++)
  {
    avatar_types[id].Set(it->c_str());
    avatar_names[id]=iFactory->GetAvatarDescription(it->c_str());
  }

  int index = -1;

  wxSingleChoiceDialog chooser(m_Gui,"select an avatar","Avatar Chooser",avatars->size(),avatar_names);

  if (chooser.ShowModal()==wxID_OK )
  {
    index = chooser.GetSelection();
    if (index>=0)
    {
      avatar_type = avatar_types[index];
      avatar_name = avatar_names[index];
    }
  }

  delete [] avatar_names;
  delete [] avatar_types;
  return index;
}
//----------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::CreateGui()
//----------------------------------------------------------------------------
{
  Superclass::CreateGui();
  m_Gui->Divider(1);

  m_Gui->Bool(ID_AVATAR_CHECK,"avatar",&m_AvatarFlag,true,"enable the default avatar for the tracker");
  m_Gui->Button(ID_AVATAR_SELECT,"set avatar");
  m_Gui->Enable(ID_AVATAR_SELECT,false);

  m_Gui->Divider(2);
  m_Gui->VectorN(ID_TB_X_EXTENT,"X extent",	m_TrackedBounds.m_Bounds,2, MINFLOAT, MAXFLOAT, -1);
  m_Gui->VectorN(ID_TB_Y_EXTENT,"Y extent",	&(GetTrackedBounds().m_Bounds[2]),2, MINFLOAT, MAXFLOAT, -1);
  m_Gui->VectorN(ID_TB_Z_EXTENT,"Z extent",	&(GetTrackedBounds().m_Bounds[4]),2, MINFLOAT, MAXFLOAT, -1);
  m_Gui->Divider();
  m_Gui->Vector(ID_TB_POSITION,"position",	m_TBPosition, MINFLOAT, MAXFLOAT, -1);
  m_Gui->Vector(ID_TB_ORIENTATION,"orientation",	m_TrackedBoxOrientation, MINFLOAT, MAXFLOAT, -1);
  m_Gui->Divider();
}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::UpdateGui()
//----------------------------------------------------------------------------
{
  if (m_AvatarFlag=GetAvatar()!=NULL) 
  {
    albaAvatar *avatar=GetAvatar();
    avatar->UpdateGui();
  }

  if (m_Gui) m_Gui->Enable(ID_AVATAR_SELECT,m_AvatarFlag!=0);

  Superclass::UpdateGui(); // must be the last one since the base class calls m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadTracker::OnEvent(albaEventBase *event)
//----------------------------------------------------------------------------
{
  albaEvent *e=albaEvent::SafeDownCast(event);
  if (e && e->GetSender()==m_Gui)
  {
    switch(e->GetId()) 
    {
      // Recompute the transform from the Tracker to the Canonical coordinate systems
      case ID_TB_POSITION:
        {
          // Translate bounds to the new center
          m_TrackedBounds.SetCenter(m_TBPosition);
          m_Gui->Update();
          // force transform update
          ComputeTrackerToCanonicalTansform();
        }
        break;
      case ID_TB_ORIENTATION:
        m_TrackedBounds.SetOrientation(m_TrackedBoxOrientation[0],m_TrackedBoxOrientation[1],m_TrackedBoxOrientation[2]);
        // force transform update
        ComputeTrackerToCanonicalTansform();
        break;
      case ID_AVATAR_CHECK:
        m_Gui->Enable(ID_AVATAR_SELECT,m_AvatarFlag!=0);
        if (!m_AvatarFlag)
        {
          SetDefaultAvatar(NULL);
        }
        m_Gui->Update();
        break;
      case ID_AVATAR_SELECT:
        {
          wxString avatar_name,avatar_type;
          int sel=AvatarChooser(avatar_name,avatar_type);

          if (sel>=0)
          {
            // Set the default avatar for the tracker
            //no SafeDownCast in albaAvatar3D: how can I create an albaAvatar3D from albaAvatar
            albaAvatar *avatar = albaInteractionFactory::CreateAvatarInstance(avatar_type);
            assert(avatar);
            avatar->SetName(avatar_name);
            SetDefaultAvatar(avatar);
            m_Gui->Update();
          }
        }
        break;
        // this should be moved to avatar settings!!!
      case ID_TB_X_EXTENT:
      case ID_TB_Y_EXTENT:
      case ID_TB_Z_EXTENT:
        m_TrackedBounds.Modified();
        m_TrackedBounds.GetCenter(m_TBPosition);
        m_Gui->Update();
        // force transform update
        ComputeTrackerToCanonicalTansform();
        break;
      default:
        Superclass::OnEvent(e); // pass to parent class for processing Gui events
    }
    return;
  }

  if ( event->GetId()==AGENT_ASYNC_DISPATCH && event->GetSender()==this )
  { 
    m_LastPoseMutex.Lock();                     /// LOCK
    // avoid concurrent access to the flag
    albaEventBase *async_event=(albaEventBase *)event->GetData();
    albaID id=async_event->GetId();
    m_LastPoseMutex.Unlock();                   /// UNLOCK

    // process the event
    Superclass::OnEvent(event);

    m_LastPoseMutex.Lock();                     /// LOCK
    // if it was a move event clear the m_LastMoveEvent variable
    if (id==GetTracker3DMoveId())
      m_LastPose=0;
    m_LastPoseMutex.Unlock();                   /// UNLOCK
  }
  else
  {
    Superclass::OnEvent(event); // for other kinds of events let superclass to precess them
  }
  
}


