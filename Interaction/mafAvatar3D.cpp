/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar3D.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:54 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mafAvatar3D.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mmgAvatar3DSettings.h"

// base includes
#include "mafAvatar3D.h"

// interactors & devices
#include "mmdTracker.h"
//#include "mmi6DOFMove.h"

// events
#include "mafInteractionDecl.h"
#include "mafEventBase.h"

// factory
#include "mafInteractionFactory.h"
#include "mafAttribute.h"
// Input Transformations
#include "mflMatrix.h"
#include "mflTransform.h"
#include "mflCameraTransform.h"
#include "mflBounds.h"

// Serialization
#include "mflXMLWriter.h"
#include "mflEventInteraction.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h"


// Visualization
#include "vtkProp3D.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkOutlineSource.h"
#include "vtkPolydataMapper.h"
#include "vtkTransform.h"
#include "vtkAxes.h"
#include "vtkAssembly.h"
#include "vtkProperty.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"

// system includes
#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafAvatar3D)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAvatar3D::mafAvatar3D()
//------------------------------------------------------------------------------
{
  vtkNEW(LastPoseMatrix);
  WorkingBoxActor = NULL;
  
  FittingMode       = mflCameraTransform::FIT_Y;

  vtkNEW(Picker3D);

  vtkNEW(CanonicalToWorldTransform);
  CanonicalToWorldTransform->SetFittingMode(FittingMode);  
  CanonicalToWorldTransform->FollowOrientationOn();
  CanonicalToWorldTransform->FollowPositionOn();
  CanonicalToWorldTransform->FollowScaleOn();
  CanonicalToWorldTransform->SetPositionModeToAttachToFocalPoint();

  // create debug 2D actors for diplaying avatars' coordinates
  const char *textMsg="Waiting for coordinates...";

  vtkNEW(DebugTextMapper);
  DebugTextMapper->SetFontFamilyToArial();
  DebugTextMapper->SetFontSize(12);
  DebugTextMapper->SetInput(textMsg);
    
  vtkNEW(DebugTextActor);
  DebugTextActor->VisibilityOn();
  DebugTextActor->SetMapper(DebugTextMapper);
  DebugTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  //DebugTextActor->GetPositionCoordinate()->SetValue(DebugTextPosition);
  DebugTextPosition=DebugTextActor->GetPositionCoordinate()->GetValue();
  DebugTextActor->GetProperty()->SetColor(0, 1, 0);

  SetDebugTextPosition(0.15,0.15);

  mflSmartPointer<vtkActor> wb_actor;
  vtkNEW(WorkingBox); // use a member var to be able change size

  mflSmartPointer<vtkPolyDataMapper> wb_mapper;
  wb_mapper->SetInput(WorkingBox->GetOutput());
  wb_actor->SetMapper(wb_mapper);
  wb_actor->SetPickable(0);

  mflSmartPointer<vtkAxes> axes;
  mflSmartPointer<vtkPolyDataMapper> axes_mapper;
  mflSmartPointer<vtkActor> axes_actor;
  axes->SetScaleFactor(.5); // Set fixed dimensions...
  axes_mapper->SetInput(axes->GetOutput());
  axes_actor->SetMapper(axes_mapper);
  axes_actor->GetProperty()->SetInterpolationToFlat();
  axes_actor->SetPickable(0);
  
  mflSmartPointer<vtkAssembly> wb_asm;
  wb_asm->AddPart(wb_actor);
  wb_asm->AddPart(axes_actor);
  wb_asm->SetPickable(0);

  WorkingBoxActor = wb_asm;
  WorkingBoxActor->Register(this);

  DisplayWorkingBox       = 0;
  DisplayDebugText        = 0;
  CoordsFrame             = TRACKER_COORDS;

  WorkingBoxActor->SetVisibility(DisplayWorkingBox);
  DebugTextActor->SetVisibility(DisplayDebugText);
}

//------------------------------------------------------------------------------
mafAvatar3D::~mafAvatar3D()
//------------------------------------------------------------------------------
{
  Shutdown();
  
  vtkDEL(Picker3D);
  vtkDEL(DebugTextActor);
  vtkDEL(DebugTextMapper);
  vtkDEL(WorkingBoxActor);
  vtkDEL(WorkingBox);
  vtkDEL(CanonicalToWorldTransform);
  vtkDEL(LastPoseMatrix);
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetTracker(mmdTracker *tracker)
//------------------------------------------------------------------------------
{
  Superclass::SetTracker(tracker);
  if (tracker)
  {

    //if (tracker->GetCanonicalBounds()->IsValid())
    //{
      // update the canonical tracked bounds
      float bounds[6];
      tracker->GetCanonicalBounds()->CopyTo(bounds);
      WorkingBox->SetBounds(bounds);
      CanonicalToWorldTransform->SetBounds(tracker->GetCanonicalBounds());
    //}
    //else
    //{
    //  WorkingBox->SetBounds(0,0,0,0,0,0);
    //  CanonicalToWorldTransform->SetBounds(NULL);
    //}
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  CanonicalToWorldTransform->SetRenderer(ren);
  Superclass::SetRenderer(ren);
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetFittingMode(int type)
//------------------------------------------------------------------------------
{
  FittingMode=type;
  CanonicalToWorldTransform->SetFittingMode(type);
  Modified();
}

//------------------------------------------------------------------------------
int mafAvatar3D::InternalInitialize()
//------------------------------------------------------------------------------
{
  if(Renderer)
  {
    Renderer->AddActor(DebugTextActor);
    Renderer->AddActor(WorkingBoxActor);
    WorkingBoxActor->SetUserTransform(CanonicalToWorldTransform);
  }
  
  return Superclass::InternalInitialize();
}
//------------------------------------------------------------------------------
void mafAvatar3D::InternalShutdown()
//------------------------------------------------------------------------------
{
  Superclass::InternalShutdown();
  
  if(Renderer)
  {
    Renderer->RemoveActor(DebugTextActor);
    Renderer->RemoveActor(WorkingBoxActor);
    WorkingBoxActor->SetUserTransform(NULL); // detach WBox from interactor
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetLastPoseMatrix(mflMatrix *matrix)
//------------------------------------------------------------------------------
{
  LastPoseMatrix->DeepCopy(matrix);  
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetDisplayWorkingBox(int vis)
//------------------------------------------------------------------------------
{
  DisplayWorkingBox = vis;
  WorkingBoxActor->SetVisibility(vis);
  Modified();
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetDisplayDebugText(int vis)
//------------------------------------------------------------------------------
{
  DisplayDebugText = vis;
  DebugTextActor->SetVisibility(vis);
  Modified();
}
//------------------------------------------------------------------------------
void mafAvatar3D::Hide()
//------------------------------------------------------------------------------
{
  Superclass::Hide();

  WorkingBoxActor->SetVisibility(0);
}

//------------------------------------------------------------------------------
void mafAvatar3D::Show()
//------------------------------------------------------------------------------
{
  Superclass::Show();
  
  WorkingBoxActor->SetVisibility(DisplayWorkingBox);
}

//------------------------------------------------------------------------------
void mafAvatar3D::UpdateDebugText(const char *title, vtkMatrix4x4 *pose)
//------------------------------------------------------------------------------
{
  assert(pose);

  wxString text;
  double pos[3],rot[3];
  mflTransform::GetPosition(pose,pos);
  mflTransform::GetOrientation(pose,rot);
  text.Printf("%s: Pos=(%.3f,%.3f,%.3f) Rot=(%.3f,%.3f,%.3f)",title,pos[0],pos[1],pos[2],\
    rot[0],rot[1],rot[2]);
  
  // currently I have to retrieve button state directly from the
  // tracker
  if (GetTracker())
  {
    wxString button_state;
    for (int i=0;i<GetTracker()->GetNumberOfButtons();i++)
    {
      
      button_state.Printf(" B%d=%d",i,GetTracker()->GetButtonState(i));
      text<<button_state;
    }
  }

  DebugTextMapper->SetInput(text);

}

//------------------------------------------------------------------------------
void mafAvatar3D::CanonicalToWorld(vtkMatrix4x4 *source,vtkMatrix4x4 *dest)
//------------------------------------------------------------------------------
{
  assert(source);

  if (dest==NULL)
    dest=source;

  vtkMatrix4x4::Multiply4x4(this->CanonicalToWorldTransform->GetMatrix(),
			  source,
			  dest);
}

//------------------------------------------------------------------------------
void mafAvatar3D::CanonicalToWorld(vtkTransform *trans)
//------------------------------------------------------------------------------
{
  trans->PostMultiply();
  trans->Concatenate(this->CanonicalToWorldTransform);
}

//------------------------------------------------------------------------------
void mafAvatar3D::CanonicalToWorld(mflTransform *trans)
//------------------------------------------------------------------------------
{
  //trans->PostMultiply();
  trans->Concatenate(this->CanonicalToWorldTransform->GetMatrix(),POST_MULTIPLY);
}

//------------------------------------------------------------------------------
void mafAvatar3D::TrackerToWorld(mflMatrix *tracker_pose,mflMatrix *world_pose,int use_scale,int use_rot, int use_trans)
//------------------------------------------------------------------------------
{
  assert(tracker_pose);
  assert(world_pose);
  
  mflSmartPointer<mflMatrix> scaled_world_pose;
  GetTracker()->TrackerToCanonical(tracker_pose,scaled_world_pose);
  CanonicalToWorld(scaled_world_pose);

  // Extract orientation and translation from transformed
  // tracker pose, obtaining a 6DOF matrix.
  // If required add also the scale factor of the CanonicalToWorld to
  // allow scaling normalized avatars to the current working
  // box size.

  if (use_rot)
  {
    // First set the 3x3 rot matrix
    double rot[3];
    mflTransform::GetOrientation(scaled_world_pose,rot);
    mflTransform::SetOrientation(world_pose,rot);
  }

  if (use_scale)
  {
    // Then apply the scale factor
    double scale[3];
    GetCanonicalToWorldTransform()->GetScale(scale);
    mflTransform::Scale(world_pose,scale[0],scale[1],scale[2],PRE_MULTIPLY);
  }

  if (use_trans)
  {
    // finally set the translation vector
    mflTransform::CopyTranslation(scaled_world_pose,world_pose);
  }
  
  world_pose->SetTimeStamp(tracker_pose->GetTimeStamp());
}


//------------------------------------------------------------------------------
void mafAvatar3D::WorldToCanonical(vtkMatrix4x4 *source,vtkMatrix4x4 *dest)
//------------------------------------------------------------------------------
{
  assert(source);

  mflSmartPointer<vtkMatrix4x4> world_to_canonical;
  world_to_canonical->DeepCopy(this->CanonicalToWorldTransform->GetMatrix());
  world_to_canonical->Invert();
  
  if (dest==NULL)
    dest=source;
  
  vtkMatrix4x4::Multiply4x4(world_to_canonical,
			  source,
			  dest);
}

//------------------------------------------------------------------------------
void mafAvatar3D::WorldToCanonical(vtkTransform *trans)
//------------------------------------------------------------------------------
{
  mflSmartPointer<vtkMatrix4x4> world_to_canonical;
  world_to_canonical->DeepCopy(this->CanonicalToWorldTransform->GetMatrix());
  world_to_canonical->Invert();
  

  trans->PostMultiply();
  trans->Concatenate(world_to_canonical);
}

//------------------------------------------------------------------------------
void mafAvatar3D::WorldToCanonical(mflTransform *trans)
//------------------------------------------------------------------------------
{
  mflSmartPointer<vtkMatrix4x4> world_to_canonical;
  world_to_canonical->DeepCopy(this->CanonicalToWorldTransform->GetMatrix());
  world_to_canonical->Invert();
  
  //trans->PostMultiply();
  trans->Concatenate(world_to_canonical,POST_MULTIPLY);
}


//------------------------------------------------------------------------------
void mafAvatar3D::WorldToTracker(vtkMatrix4x4 *world_pose,vtkMatrix4x4 *tracker_pose,int use_scale,int use_rot, int use_trans)
//------------------------------------------------------------------------------
{
  assert(tracker_pose);
  assert(world_pose);

  
  mflSmartPointer<vtkMatrix4x4> canonical_pose;
  WorldToCanonical(world_pose,canonical_pose);
   
  if (use_scale&&use_rot&&use_trans)
  {
    GetTracker()->CanonicalToTracker(canonical_pose,tracker_pose);
  }
  else
  {
    // Extract orientation and translation from transformed
    // tracker pose, obtaining a 6DOF matrix.
    // If required add also the scale factor of the CanonicalToWorld to
    // allow scaling normalized avatars to the current working
    // box size.

    mflSmartPointer<vtkMatrix4x4> scaled_tracker_pose;
    GetTracker()->CanonicalToTracker(canonical_pose,scaled_tracker_pose);

    if (use_rot)
    {
      // First set the 3x3 rot matrix
      double rot[3];
      mflTransform::GetOrientation(scaled_tracker_pose,rot);
      mflTransform::SetOrientation(tracker_pose,rot);
    }
  
    if (use_scale)
    {
      // Then apply the scale factor
      double scale[3];
      mflSmartPointer<mflTransform> world_to_canonical;
      world_to_canonical->SetMatrix(this->CanonicalToWorldTransform->GetMatrix());
      world_to_canonical->Inverse();
      world_to_canonical->GetScale(scale);
      mflTransform::Scale(tracker_pose,scale[0],scale[1],scale[2],PRE_MULTIPLY);
    }

    if (use_trans)
    {
      // finally set the translation vector
      mflTransform::CopyTranslation(scaled_tracker_pose,tracker_pose);
    }
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::WorldToNormalizedDisplay(mflMatrix *world_pose,float xy[2])
//------------------------------------------------------------------------------
{
  WorldToDisplay(world_pose,xy);
  Renderer->ViewportToNormalizedViewport(xy[0],xy[1]);
}

//------------------------------------------------------------------------------
void mafAvatar3D::WorldToDisplay(mflMatrix *world_pose,float xy[2])
//------------------------------------------------------------------------------
{
  if (Renderer)
  {
    float pos[4];
    mflTransform::GetPosition(world_pose,pos);
    pos[3]=1.0;

    // compute 2D view coordinates
    float xypos[3];

    Renderer->SetWorldPoint(pos);
    Renderer->WorldToDisplay();
    Renderer->GetDisplayPoint(xypos);
    xy[0]=xypos[0];
    xy[1]=xypos[1];
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::TrackerToDisplay(mflMatrix *tracker_pose,float xy[2])
//------------------------------------------------------------------------------
{
  assert(tracker_pose);
  assert(xy);

  if (Renderer)
  {
    // retrieve tracker position in world coordinates
    mflSmartPointer<mflMatrix> world_pose;
    TrackerToWorld(tracker_pose,world_pose);
    WorldToDisplay(world_pose,xy);
  }
}

//------------------------------------------------------------------------------
int mafAvatar3D::InternalStore(mflXMLWriter *writer)
//------------------------------------------------------------------------------
{
  if(Superclass::InternalStore(writer))
    return -1;
  writer->DisplayTaggedXML("Name",GetName());
  writer->DisplayTaggedXML("DisplayWorkingBox",GetDisplayWorkingBox());
  writer->DisplayTaggedXML("DisplayDebugText",GetDisplayDebugText());
  double coords[2];
  coords[0]=GetDebugTextPosition()[0];
  coords[1]=GetDebugTextPosition()[1];
  mafAttribute::StoreVectorN(writer,coords,2,"DebugTextPosition");
  writer->DisplayTaggedXML("CoordsFrame",GetCoordsFrame());

  // write prop3D properties?
  return 0;
}

//------------------------------------------------------------------------------
int mafAvatar3D::InternalRestore(vtkXMLDataElement *node,vtkXMLDataParser *parser)
//------------------------------------------------------------------------------
{
  if(Superclass::InternalRestore(node,parser))
    return -1;
  
  int display_working_box = 0;
  mafAttribute::RestoreNumeric(node,parser,display_working_box,"DisplayWorkingBox");
  SetDisplayWorkingBox(display_working_box);
  
  int display_debug_text = 0;
  mafAttribute::RestoreNumeric(node,parser,display_debug_text,"DisplayDebugText");
  SetDisplayDebugText(display_debug_text);
  
  double coords[2]={0,0};
  mafAttribute::RestoreVectorN(node,parser,coords,2,"DebugTextPosition");
  SetDebugTextPosition(coords[0],coords[1]);
  
  mafAttribute::RestoreNumeric(node,parser,CoordsFrame,"CoordsFrame");
  return 0;
  
}

//------------------------------------------------------------------------------
void mafAvatar3D::CreateSettings()
//------------------------------------------------------------------------------
{
  Settings = new mmgAvatar3DSettings(this); 
}

//------------------------------------------------------------------------------
void mafAvatar3D::OnPostResetCamera(mflEvent *event)
//------------------------------------------------------------------------------
{
  CanonicalToWorldTransform->RecomputeAll();
  Superclass::OnPostResetCamera(event);
}

//------------------------------------------------------------------------------
void mafAvatar3D::OnMove3DEvent(mflEventInteraction *e)
//------------------------------------------------------------------------------
{
  // Process tracker coordinates to get World coordinates
  mflMatrix *tracker_pose=e->GetMatrix();
  mflSmartPointer<mflMatrix> world_pose;

  TrackerToWorld(tracker_pose,world_pose);
  
  SetLastPoseMatrix(world_pose);

  // debug...
  double scale[3];
  mflTransform::GetScale(world_pose,scale);

  if (Actor3D)
  {
    Actor3D->GetUserMatrix()->DeepCopy(world_pose);
  }
  
  // Rendering
  if (Renderer)
  {
    if (DisplayDebugText)
    {
      wxString label = GetTracker()->GetName();

      if (CoordsFrame==WORLD_COORDS)
      {   
        label << " (World): ";
        UpdateDebugText(label,world_pose);
      }
      else
      {            
        if (CoordsFrame==CANONICAL_COORDS)
        {
          mflMatrix normal_pose;
          GetTracker()->TrackerToCanonical(tracker_pose,&normal_pose);
          label << " (Canonical): ";
          UpdateDebugText(label,&normal_pose);
        }
        else
        {
          label << " (Tracker): ";
          UpdateDebugText(label,tracker_pose);
        }
      }
    }

    if (View)
    {
      // ask a rendering only if it has never rendered
      vtkRenderWindow *rw=Renderer->GetRenderWindow();
      if (rw&&!rw->GetNeverRendered())
        ForwardEvent(mafSmartEvent(this,CameraUpdateEvent,View));
    }
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::OnUpdateBoundsEvent(mmdTracker *tracker)
//------------------------------------------------------------------------------
{
  assert(tracker);
  // Manage spurious changes in the canonical bounds.
  // If bounds are changed while an interaction is ongoing...
  // but should never happen!
  //if (tracker->GetCanonicalBounds()->IsValid())
  //{
    float bounds[6];
    tracker->GetCanonicalBounds()->CopyTo(bounds);
    WorkingBox->SetBounds(bounds);
    CanonicalToWorldTransform->SetBounds(tracker->GetCanonicalBounds()); 
  //}
  //else
  //{
  //  WorkingBox->SetBounds(0,0,0,0,0,0);
  //  CanonicalToWorldTransform->SetBounds(NULL);
  //}
}

//------------------------------------------------------------------------------
void mafAvatar3D::ProcessEvent(mflEvent *event,mafID ch)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetID();
  
  if (ch==CameraUpdateChannel) //---- ResetCamera and ViewSelected events
  {
    if (id==PreResetCameraEvent)
    {
      OnPreResetCamera(event);
    }
    else if (id==PostResetCameraEvent)
    {
      OnPostResetCamera(event);
    }
    else if (id==ViewSelectedEvent)
    {
      mafEventBase *e=mafEventBase::SafeDownCast(event);
      OnViewSelected(e);
    }
  }
  else if (ch==mafDevice::DeviceInputChannel) //---- Inputs from the device
  {
    // Input events should all come from the tracker...
    assert(GetTracker()&&event->GetSender()==GetTracker());
    
    mflEventInteraction *e = (mflEventInteraction *)event;
    
    if (id==mmdTracker::Move3DEvent) // manage changes in the bounds of the tracker
    {
      OnMove3DEvent(e);
    }
    else if (id==mmdTracker::BoundsUpdatedEvent)
    {
      OnUpdateBoundsEvent(GetTracker());          
    }
  }

  Superclass::ProcessEvent(event,ch);
}

//------------------------------------------------------------------------------
vtkAbstractPropPicker *mafAvatar3D::GetPicker()
//------------------------------------------------------------------------------
{
  if (Mode==MODE_2D)
  {
    return GetPicker2D();
  }
  else
  {
    return GetPicker3D();
  }
}

//------------------------------------------------------------------------------
vtkRayCast3DPicker *mafAvatar3D::GetPicker3D()
//------------------------------------------------------------------------------
{
  return Picker3D;
}

//------------------------------------------------------------------------------
vtkCellPicker *mafAvatar3D::GetPicker2D()
//------------------------------------------------------------------------------
{
  return Picker2D;
}