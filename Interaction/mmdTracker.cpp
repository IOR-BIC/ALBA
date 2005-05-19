/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdTracker.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-19 16:27:39 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

// base
#include "mmdTracker.h"

// gui
#include "mmgGui.h"

// general
#include "mafSmartPointer.h"
#include "mafEventInteraction.h"
#include "mmuIdFactory.h"

// I/O
#include "mafStorageElement.h"
#include "mafInteractionFactory.h"

// geometric
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafOBB.h"

// avatars
#include "mafAvatar.h"
#include "mafAvatar3D.h"

// VTK
#include "vtkTimerLog.h"



//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mmdTracker::TRACKER_3D_MOVE)
MAF_ID_IMP(mmdTracker::TRACKER_BOUNDS_UPDATED)

//------------------------------------------------------------------------------
mmdTracker::mmdTracker()
//------------------------------------------------------------------------------
{
  mafNEW(m_TrackerToCanonicalTransform);
  mafNEW(m_LastPoseMatrix);

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
mmdTracker::~mmdTracker()
//------------------------------------------------------------------------------
{
  SetDefaultAvatar(NULL);
  mafDEL(m_TrackerToCanonicalTransform);
  mafDEL(m_LastPoseMatrix);
}

//------------------------------------------------------------------------------
int mmdTracker::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  if (Superclass::InternalStore(node))
    return MAF_ERROR;

  //StoreMatrix(writer,m_TrackerToCanonicalTransform->GetMatrix(),"TrackerToCanonicalMatrix");
  node->StoreVectorN("TrackedBoxBounds",m_TrackedBounds.m_Bounds,6);
  node->StoreVectorN("TrackedBoxOrientation",m_TrackedBoxOrientation,3);

  // store default avatar if present
  if (m_DefaultAvatar)
  {
    return (node->StoreObject("Avatar",m_DefaultAvatar)?MAF_OK:MAF_ERROR);
  }

  return MAF_OK;
}
//------------------------------------------------------------------------------
int mmdTracker::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  assert(node);
  
  if (Superclass::InternalRestore(node))
    return MAF_ERROR;


  if (node->RestoreVectorN("TrackedBoxBounds",m_TrackedBounds.m_Bounds,6)==MAF_OK)
  {
    m_TrackedBounds.Modified();
    if (node->RestoreVectorN("TrackedBoxOrientation",m_TrackedBoxOrientation,3))
    {
      if (mafStorageElement *sub_node=node->FindNestedElement("Avatar"))
      {
        mafObject *obj=sub_node->RestoreObject();
        if (mafAvatar3D *avatar=mafAvatar3D::SafeDownCast(obj))
        {
          SetDefaultAvatar(mafAvatar3D::SafeDownCast(avatar));
        }
        else
        {
          mafErrorMessage("find wrong type of Avatar (%s) while restoring mmdTracker.",obj->GetTypeName());
          obj->Delete();
        }
      }
      
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mmdTracker::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize())
    return MAF_ERROR;

  if (m_Avatar)
  {
    m_Avatar->Initialize();
  }

  m_LastPose = 0; // initialize last pose flag
  return MAF_OK;
}
//------------------------------------------------------------------------------
void mmdTracker::InternalShutdown()
//------------------------------------------------------------------------------
{
  if (m_Avatar)
  {
    m_Avatar->Shutdown();
  }

  Superclass::InternalShutdown();
}

//------------------------------------------------------------------------------
void mmdTracker::SetAvatar(mafAvatar *avatar)
//------------------------------------------------------------------------------
{
  if (m_Avatar)
  {
    InvokeEvent(AVATAR_REMOVED,MCH_UP,m_Avatar);
 
    if (m_Initialized)
    {
      m_Avatar->Shutdown();
    }

    GetGui()->Remove(m_Avatar->GetGui());
    GetGui()->GetSizer()->Remove(m_Avatar->GetGui());
    //m_Avatar->GetGui()->SetParent(NULL);
    m_Avatar->GetGui()->Show(false);
    m_Avatar->GetGui()->Refresh();
    m_Avatar->SetTracker(NULL);
  }

  m_Avatar = avatar;

  if (avatar)
  {
    avatar->SetTracker(this);
    
    InvokeEvent(AVATAR_ADDED,MCH_UP,avatar);

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
void mmdTracker::SetDefaultAvatar(mafAvatar *avatar)
//------------------------------------------------------------------------------
{
  m_DefaultAvatar=avatar;
  SetAvatar(avatar);
}

//------------------------------------------------------------------------------
void mmdTracker::RestoreDefaultAvatar()
//------------------------------------------------------------------------------
{
  SetAvatar(m_DefaultAvatar);
}

//------------------------------------------------------------------------------
void mmdTracker::SetLastPoseMatrix(const mafMatrix &matrix)
//------------------------------------------------------------------------------
{ 
  // I had to add a test on elapsed time since it seems sometimes
  // an event gets lost! :-(((
  m_LastPoseMutex.Lock(); 
  mafTimeStamp elapsed_time = (m_LastPoseMatrix->GetTimeStamp()-m_LastMoveTime);

  // This is a very tricky thing: remove a move event after a give timeout
  // The only problem is if some other thread is pushing an event while we 
  // are doing this operation, since that event wouldn't be the last one.
  if (m_LastPose==1&&elapsed_time>m_MoveEventTimeOut)
  {
 /*   
    int queue_size = GetQueueSize();
    for (int i=0;i<queue_size;i++)
    {
      mafEventBase *temp;
      mafID ch;

      // check if for some reason queue is already empty (it shouldn't!)
      if (!PopEvent(temp,ch))
        break;
      
      if (temp->GetID()==mflThreadedAgent::AsyncDispatchEvent)
      {
        mafEventBase *event=mafEventBase::SafeDownCast((vtkObject*)temp->GetData());
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
    m_LastPoseMatrix->SetTimeStamp(vtkTimerLog::GetCurrentTime()); // set the time stamp to the current time

    mafEventInteraction move_event(this,TRACKER_3D_MOVE,m_LastPoseMatrix);    

    m_LastMoveTime=m_LastPoseMatrix->GetTimeStamp();
    AsyncInvokeEvent(&move_event,MCH_INPUT);
   // m_LastMoveEvent = move_event;
    m_LastPoseMutex.Unlock();
  }
  else
  {
    m_LastPoseMutex.Unlock();
    mafSleep(25); // wait for a while to give time to serve last event
  }
}

//------------------------------------------------------------------------------
mafMatrix &mmdTracker::GetLastPoseMatrix()
//------------------------------------------------------------------------------
{
  return *m_LastPoseMatrix;
}

//------------------------------------------------------------------------------
void mmdTracker::SendButtonEvent(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  if (m_Avatar&&m_Avatar->GetMode()==mafAvatar::MODE_2D)
  {
    event->SetXYFlag(true); // signal we were in 2D mode...
  }
  
  mafSmartPointer<mafMatrix> last_pose;
  last_pose->DeepCopy(m_LastPoseMatrix); // make a copy of current pose to ovoid overwriting
  event->SetMatrix(last_pose);  
  
  AsyncInvokeEvent(event,MCH_INPUT);
}
//------------------------------------------------------------------------------
void mmdTracker::ComputeTrackerToCanonicalTansform()
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
  InvokeEvent(TRACKER_BOUNDS_UPDATED,MCH_INPUT);
}

//------------------------------------------------------------------------------
mafOBB &mmdTracker::GetCanonicalBounds()
//------------------------------------------------------------------------------
{
  if (m_CanonicalBounds.GetMTime()<m_TrackedBounds.GetMTime())
    ComputeTrackerToCanonicalTansform();

  return m_CanonicalBounds;
}

//------------------------------------------------------------------------------
mafTransform *mmdTracker::GetTrackerToCanonicalTransform()
//------------------------------------------------------------------------------
{
  if (m_CanonicalBounds.GetMTime()<m_TrackedBounds.GetMTime())
    ComputeTrackerToCanonicalTansform();

  return m_TrackerToCanonicalTransform;
}

//------------------------------------------------------------------------------
void mmdTracker::TrackerToCanonical(const mafMatrix &source,mafMatrix &dest)
//------------------------------------------------------------------------------
{
  mafMatrix::Multiply4x4(GetTrackerToCanonicalTransform()->GetMatrix(),
    source,
    dest);

  dest.SetTimeStamp(source.GetTimeStamp());
}

//------------------------------------------------------------------------------
void mmdTracker::TrackerToCanonical(mafTransform *trans)
//------------------------------------------------------------------------------
{
  trans->Concatenate(GetTrackerToCanonicalTransform()->GetMatrix(),POST_MULTIPLY);
}

//------------------------------------------------------------------------------
void mmdTracker::CanonicalToTracker(const mafMatrix &source,mafMatrix &dest)
//------------------------------------------------------------------------------
{
  mafMatrix canonical_to_tracker;
  canonical_to_tracker=this->m_TrackerToCanonicalTransform->GetMatrix();
  canonical_to_tracker.Invert();

  mafMatrix::Multiply4x4(canonical_to_tracker,
    source,
    dest);
}

//------------------------------------------------------------------------------
void mmdTracker::CanonicalToTracker(mafTransform *trans)
//------------------------------------------------------------------------------
{
  mafMatrix canonical_to_tracker;
  canonical_to_tracker=m_TrackerToCanonicalTransform->GetMatrix();
  canonical_to_tracker.Invert();
  trans->Concatenate(canonical_to_tracker,POST_MULTIPLY);
}

//----------------------------------------------------------------------------
int mmdTracker::AvatarChooser(wxString &avatar_name,wxString &avatar_type)
//----------------------------------------------------------------------------
{
  mafInteractionFactory *iFactory=mafInteractionFactory::GetInstance();

  assert(iFactory);
  
  const std::set<std::string> avatars=iFactory->GetAvatarNames();

  if (avatars.size()==0)
  {
    mafErrorMessage("No avatars available!","Avatar Chooser Error");
    return MAF_ERROR;
  }

  wxString *avatar_names = new wxString[avatars.size()];
  mafString *avatar_types= new mafString[avatars.size()];

  std::set<std::string>::const_iterator it=avatars.begin();
  for (int id=0;it!=avatars.end();id++,it++)
  {
    avatar_types[id].Set(it->c_str());
    avatar_names[id]=iFactory->GetAvatarDescription(it->c_str());
  }

  int index = -1;

  wxSingleChoiceDialog chooser(m_Gui,"select an avatar","Avatar Chooser",avatars.size(),avatar_names);

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
void mmdTracker::CreateGui()
//----------------------------------------------------------------------------
{
  Superclass::CreateGui();
  m_Gui->Divider(1);

  m_Gui->Bool(ID_AVATAR_CHECK,"avatar",&m_AvatarFlag,true,"enable the default avatar for the tracker");
  m_Gui->Button(ID_AVATAR_SELECT,"set avatar");
  m_Gui->Enable(ID_AVATAR_SELECT,false);

  m_Gui->Divider(1);
  m_Gui->VectorN(ID_TB_X_EXTENT,"X extent",	m_TrackedBounds.m_Bounds,2, MINFLOAT, MAXFLOAT, -1);
  m_Gui->VectorN(ID_TB_Y_EXTENT,"Y extent",	&(GetTrackedBounds().m_Bounds[2]),2, MINFLOAT, MAXFLOAT, -1);
  m_Gui->VectorN(ID_TB_Z_EXTENT,"Z extent",	&(GetTrackedBounds().m_Bounds[4]),2, MINFLOAT, MAXFLOAT, -1);
  m_Gui->Divider();
  m_Gui->Vector(ID_TB_POSITION,"position",	m_TBPosition, MINFLOAT, MAXFLOAT, -1);
  m_Gui->Vector(ID_TB_ORIENTATION,"orientation",	m_TrackedBoxOrientation, MINFLOAT, MAXFLOAT, -1);
  m_Gui->Divider();
}

//----------------------------------------------------------------------------
void mmdTracker::UpdateGui()
//----------------------------------------------------------------------------
{
  if (m_AvatarFlag=GetAvatar()!=NULL) 
  {
    mafAvatar *avatar=GetAvatar();
    avatar->UpdateGui();
  }

  if (m_Gui) m_Gui->Enable(ID_AVATAR_SELECT,m_AvatarFlag!=0);

  Superclass::UpdateGui(); // must be the last one since the base class calls m_Gui->Update();
}

//----------------------------------------------------------------------------
void mmdTracker::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e=mafEvent::SafeDownCast(event))
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
            //no SafeDownCast in mafAvatar3D: how can I create an mafAvatar3D from mafAvatar
            mafAvatar *avatar = mafInteractionFactory::CreateAvatarInstance(avatar_type);
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
    }
  }

  if ( event->GetId()==AGENT_ASYNC_DISPATCH && event->GetSender()==this )
  { 
    m_LastPoseMutex.Lock();                     /// LOCK
    // avoid concurrent access to the flag
    mafEventBase *async_event=(mafEventBase *)event->GetData();
    mafID id=async_event->GetId();
    m_LastPoseMutex.Unlock();                   /// UNLOCK

    // process the event
    Superclass::OnEvent(event);

    m_LastPoseMutex.Lock();                     /// LOCK
    // if it was a move event clear the m_LastMoveEvent variable
    if (id==TRACKER_3D_MOVE)
      m_LastPose=0;
    m_LastPoseMutex.Unlock();                   /// UNLOCK
  }
  else
  {
    Superclass::OnEvent(event);
  }
  
}


