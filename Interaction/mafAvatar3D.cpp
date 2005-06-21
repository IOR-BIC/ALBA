/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar3D.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-21 07:57:06 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

// base includes
#include "mafAvatar3D.h"

// interactors & devices
#include "mmdTracker.h"
//#include "mmi6DOFMove.h"

// events
#include "mafEventInteraction.h"
#include "mafEvent.h"

// gui
#include "mmgGui.h"

// Input Transformations
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafCameraTransform.h"
#include "mafOBB.h"

// Serialization
#include "mafStorageElement.h"

// Visualization
#include "vtkProp3D.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkOutlineSource.h"
#include "vtkPolyDataMapper.h"
//#include "vtkTransform.h"
#include "vtkAxes.h"
#include "vtkAssembly.h"
#include "vtkProperty.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkMatrix4x4.h"

// system includes
#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafAvatar3D);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAvatar3D::mafAvatar3D()
//------------------------------------------------------------------------------
{
  m_WorkingBoxActor = NULL;
  
  m_FittingMode       = mafCameraTransform::FIT_Y;

  vtkNEW(m_Picker3D);

  mafNEW(m_CanonicalToWorldTransform);
  m_CanonicalToWorldTransform->SetFittingMode(m_FittingMode);  
  m_CanonicalToWorldTransform->FollowOrientationOn();
  m_CanonicalToWorldTransform->FollowPositionOn();
  m_CanonicalToWorldTransform->FollowScaleOn();
  m_CanonicalToWorldTransform->SetPositionModeToAttachToFocalPoint();

  // create debug 2D actors for diplaying avatars' coordinates
  const char *textMsg="Waiting for coordinates...";

  vtkNEW(m_DebugTextMapper);
  //m_DebugTextMapper->SetFontFamilyToArial();
  //m_DebugTextMapper->SetFontSize(12);
  m_DebugTextMapper->SetInput(textMsg);
    
  vtkNEW(m_DebugTextActor);
  m_DebugTextActor->VisibilityOn();
  m_DebugTextActor->SetMapper(m_DebugTextMapper);
  m_DebugTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  //m_DebugTextActor->GetPositionCoordinate()->SetValue(m_DebugTextPosition);
  m_DebugTextPosition=m_DebugTextActor->GetPositionCoordinate()->GetValue();
  m_DebugTextActor->GetProperty()->SetColor(0, 1, 0);

  SetDebugTextPosition(0.15,0.15);

  vtkMAFSmartPointer<vtkActor> wb_actor;
  vtkNEW(m_WorkingBox); // use a member var to be able change size

  vtkMAFSmartPointer<vtkPolyDataMapper> wb_mapper;
  wb_mapper->SetInput(m_WorkingBox->GetOutput());
  wb_actor->SetMapper(wb_mapper);
  wb_actor->SetPickable(0);

  vtkMAFSmartPointer<vtkAxes> axes;
  vtkMAFSmartPointer<vtkPolyDataMapper> axes_mapper;
  vtkMAFSmartPointer<vtkActor> axes_actor;
  axes->SetScaleFactor(.5); // Set fixed dimensions...
  axes_mapper->SetInput(axes->GetOutput());
  axes_actor->SetMapper(axes_mapper);
  axes_actor->GetProperty()->SetInterpolationToFlat();
  axes_actor->SetPickable(0);
  
  vtkMAFSmartPointer<vtkAssembly> wb_asm;
  wb_asm->AddPart(wb_actor);
  wb_asm->AddPart(axes_actor);
  wb_asm->SetPickable(0);

  m_WorkingBoxActor = wb_asm;
  m_WorkingBoxActor->Register(NULL);

  m_DisplayWorkingBox       = 0;
  m_DisplayDebugText        = 0;
  m_CoordsFrame             = TRACKER_COORDS;

  m_WorkingBoxActor->SetVisibility(m_DisplayWorkingBox);
  m_DebugTextActor->SetVisibility(m_DisplayDebugText);
}

//------------------------------------------------------------------------------
mafAvatar3D::~mafAvatar3D()
//------------------------------------------------------------------------------
{
  Shutdown();
  
  mafDEL(m_CanonicalToWorldTransform);

  vtkDEL(m_Picker3D);
  vtkDEL(m_DebugTextActor);
  vtkDEL(m_DebugTextMapper);
  vtkDEL(m_WorkingBoxActor);
  vtkDEL(m_WorkingBox);
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetDebugTextPosition(double posx,double posy)
//------------------------------------------------------------------------------
{
  m_DebugTextPosition[0]=posx;
  m_DebugTextPosition[1]=posy;
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetPicker3D(vtkRayCast3DPicker *picker)
//------------------------------------------------------------------------------
{
  vtkDEL(m_Picker3D);
  m_Picker3D=picker;
  m_Picker3D->Register(NULL);
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
      double bounds[6];
      tracker->GetCanonicalBounds().CopyTo(bounds);
      m_WorkingBox->SetBounds(bounds);
      m_CanonicalToWorldTransform->SetBounds(&tracker->GetCanonicalBounds());
    //}
    //else
    //{
    //  m_WorkingBox->SetBounds(0,0,0,0,0,0);
    //  m_CanonicalToWorldTransform->SetBounds(NULL);
    //}
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  m_CanonicalToWorldTransform->SetRenderer(ren);
  Superclass::SetRenderer(ren);
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetFittingMode(int type)
//------------------------------------------------------------------------------
{
  m_FittingMode=type;
  m_CanonicalToWorldTransform->SetFittingMode(type);
  //Modified();
}

//------------------------------------------------------------------------------
int mafAvatar3D::InternalInitialize()
//------------------------------------------------------------------------------
{
  if(m_Renderer)
  {
    m_Renderer->AddActor(m_DebugTextActor);
    m_Renderer->AddActor(m_WorkingBoxActor);
    m_WorkingBoxActor->SetUserTransform(m_CanonicalToWorldTransform->GetVTKTransform());
  }
  
  return Superclass::InternalInitialize();
}
//------------------------------------------------------------------------------
void mafAvatar3D::InternalShutdown()
//------------------------------------------------------------------------------
{
  Superclass::InternalShutdown();
  
  if(m_Renderer)
  {
    m_Renderer->RemoveActor(m_DebugTextActor);
    m_Renderer->RemoveActor(m_WorkingBoxActor);
    m_WorkingBoxActor->SetUserTransform(NULL); // detach WBox from interactor
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetLastPoseMatrix(mafMatrix &matrix)
//------------------------------------------------------------------------------
{
  m_LastPoseMatrix=matrix;  
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetDisplayWorkingBox(int vis)
//------------------------------------------------------------------------------
{
  m_DisplayWorkingBox = vis;
  m_WorkingBoxActor->SetVisibility(vis);
  //Modified();
}

//------------------------------------------------------------------------------
void mafAvatar3D::SetDisplayDebugText(int vis)
//------------------------------------------------------------------------------
{
  m_DisplayDebugText = vis;
  m_DebugTextActor->SetVisibility(vis);
  //Modified();
}
//------------------------------------------------------------------------------
void mafAvatar3D::Hide()
//------------------------------------------------------------------------------
{
  Superclass::Hide();

  m_WorkingBoxActor->SetVisibility(0);
}

//------------------------------------------------------------------------------
void mafAvatar3D::Show()
//------------------------------------------------------------------------------
{
  Superclass::Show();
  
  m_WorkingBoxActor->SetVisibility(m_DisplayWorkingBox);
}

//------------------------------------------------------------------------------
void mafAvatar3D::UpdateDebugText(const char *title, mafMatrix &pose)
//------------------------------------------------------------------------------
{
  wxString text;
  double pos[3],rot[3];
  mafTransform::GetPosition(pose,pos);
  mafTransform::GetOrientation(pose,rot);
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

  m_DebugTextMapper->SetInput(text);

}

//------------------------------------------------------------------------------
void mafAvatar3D::CanonicalToWorld(const mafMatrix &source,mafMatrix &dest)
//------------------------------------------------------------------------------
{
  if (dest==NULL)
    dest=source;

  mafMatrix::Multiply4x4(this->m_CanonicalToWorldTransform->GetMatrix(),
			  source,
			  dest);
}

//------------------------------------------------------------------------------
void mafAvatar3D::CanonicalToWorld(mafTransform *trans)
//------------------------------------------------------------------------------
{
  //trans->PostMultiply();
  trans->Concatenate(this->m_CanonicalToWorldTransform->GetMatrix(),POST_MULTIPLY);
}

//------------------------------------------------------------------------------
void mafAvatar3D::TrackerToWorld(mafMatrix &tracker_pose,mafMatrix &world_pose,int use_scale,int use_rot, int use_trans)
//------------------------------------------------------------------------------
{
  mafMatrix scaled_world_pose;
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
    mafTransform::GetOrientation(scaled_world_pose,rot);
    mafTransform::SetOrientation(world_pose,rot);
  }

  if (use_scale)
  {
    // Then apply the scale factor
    double scale[3];
    mafTransform::GetScale(GetCanonicalToWorldTransform()->GetMatrix(),scale);
    mafTransform::Scale(world_pose,scale[0],scale[1],scale[2],PRE_MULTIPLY);
  }

  if (use_trans)
  {
    // finally set the translation vector
    mafTransform::CopyTranslation(scaled_world_pose,world_pose);
  }
  
  world_pose.SetTimeStamp(tracker_pose.GetTimeStamp());
}


//------------------------------------------------------------------------------
void mafAvatar3D::WorldToCanonical(const mafMatrix &source,mafMatrix &dest)
//------------------------------------------------------------------------------
{
  mafMatrix world_to_canonical;
  world_to_canonical=m_CanonicalToWorldTransform->GetMatrix();
  world_to_canonical.Invert();
  
  if (dest==NULL)
    dest=source;
  
  mafMatrix::Multiply4x4(world_to_canonical,
			  source,
			  dest);
}

//------------------------------------------------------------------------------
void mafAvatar3D::WorldToCanonical(mafTransform *trans)
//------------------------------------------------------------------------------
{
  mafMatrix world_to_canonical;
  world_to_canonical=m_CanonicalToWorldTransform->GetMatrix();
  world_to_canonical.Invert();
  
  //trans->PostMultiply();
  trans->Concatenate(world_to_canonical,POST_MULTIPLY);
}


//------------------------------------------------------------------------------
void mafAvatar3D::WorldToTracker(mafMatrix &world_pose,mafMatrix &tracker_pose,int use_scale,int use_rot, int use_trans)
//------------------------------------------------------------------------------
{ 
  mafMatrix canonical_pose;
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

    mafMatrix scaled_tracker_pose;
    GetTracker()->CanonicalToTracker(canonical_pose,scaled_tracker_pose);

    if (use_rot)
    {
      // First set the 3x3 rot matrix
      double rot[3];
      mafTransform::GetOrientation(scaled_tracker_pose,rot);
      mafTransform::SetOrientation(tracker_pose,rot);
    }
  
    if (use_scale)
    {
      // Then apply the scale factor
      double scale[3];
      mafTransform world_to_canonical;
      world_to_canonical.SetMatrix(m_CanonicalToWorldTransform->GetMatrix());
      world_to_canonical.Invert();
      world_to_canonical.GetScale(scale);
      mafTransform::Scale(tracker_pose,scale[0],scale[1],scale[2],PRE_MULTIPLY);
    }

    if (use_trans)
    {
      // finally set the translation vector
      mafTransform::CopyTranslation(scaled_tracker_pose,tracker_pose);
    }
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::WorldToNormalizedDisplay(mafMatrix &world_pose,double xy[2])
//------------------------------------------------------------------------------
{
  WorldToDisplay(world_pose,xy);
  m_Renderer->ViewportToNormalizedViewport(xy[0],xy[1]);
}

//------------------------------------------------------------------------------
void mafAvatar3D::WorldToDisplay(mafMatrix &world_pose,double xy[2])
//------------------------------------------------------------------------------
{
  if (m_Renderer)
  {
    double pos[4];
    mafTransform::GetPosition(world_pose,pos);
    pos[3]=1.0;

    // compute 2D view coordinates
    double xypos[3];

    m_Renderer->SetWorldPoint(pos);
    m_Renderer->WorldToDisplay();
    m_Renderer->GetDisplayPoint(xypos);
    xy[0]=xypos[0];
    xy[1]=xypos[1];
  }
}

//------------------------------------------------------------------------------
void mafAvatar3D::TrackerToDisplay(mafMatrix &tracker_pose,double xy[2])
//------------------------------------------------------------------------------
{
  assert(xy);

  if (m_Renderer)
  {
    // retrieve tracker position in world coordinates
    mafMatrix world_pose;
    TrackerToWorld(tracker_pose,world_pose);
    WorldToDisplay(world_pose,xy);
  }
}

//------------------------------------------------------------------------------
int mafAvatar3D::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  if(Superclass::InternalStore(node))
    return MAF_ERROR;

  node->StoreText("Name",GetName());
  node->StoreInteger("DisplayWorkingBox",GetDisplayWorkingBox());
  node->StoreInteger("DisplayDebugText",GetDisplayDebugText());
  double coords[2];
  coords[0]=GetDebugTextPosition()[0];
  coords[1]=GetDebugTextPosition()[1];
  node->StoreVectorN("DebugTextPosition",coords,2);
  node->StoreInteger("CoordsFrame",GetCoordsFrame());

  // write prop3D properties?
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafAvatar3D::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  if(Superclass::InternalRestore(node))
    return MAF_ERROR;
  
  int display_working_box = 0;
  node->RestoreInteger("DisplayWorkingBox",display_working_box);
  SetDisplayWorkingBox(display_working_box);
  
  int display_debug_text = 0;
  node->RestoreInteger("DisplayDebugText",display_debug_text);
  SetDisplayDebugText(display_debug_text);
  
  double coords[2]={0,0};
  node->RestoreVectorN("DebugTextPosition",coords,2);
  SetDebugTextPosition(coords[0],coords[1]);
  node->RestoreInteger("CoordsFrame",m_CoordsFrame);
  return MAF_OK;
  
}

//------------------------------------------------------------------------------
void mafAvatar3D::OnPostResetCamera(mafEventBase *event)
//------------------------------------------------------------------------------
{
  m_CanonicalToWorldTransform->RecomputeAll();
  Superclass::OnPostResetCamera(event);
}

//------------------------------------------------------------------------------
void mafAvatar3D::OnMove3DEvent(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  // Process tracker coordinates to get World coordinates
  mafMatrix *tracker_pose=e->GetMatrix();
  mafMatrix world_pose;

  TrackerToWorld(*tracker_pose,world_pose);
  
  SetLastPoseMatrix(world_pose);

  // debug...
  double scale[3];
  mafTransform::GetScale(world_pose,scale);

  if (m_Actor3D)
  {
    m_Actor3D->GetUserMatrix()->DeepCopy(world_pose.GetVTKMatrix());
  }
  
  // Rendering
  if (m_Renderer)
  {
    if (m_DisplayDebugText)
    {
      wxString label = GetTracker()->GetName();

      if (m_CoordsFrame==WORLD_COORDS)
      {   
        label << " (World): ";
        UpdateDebugText(label,world_pose);
      }
      else
      {            
        if (m_CoordsFrame==CANONICAL_COORDS)
        {
          mafMatrix normal_pose;
          GetTracker()->TrackerToCanonical(*tracker_pose,normal_pose);
          label << " (Canonical): ";
          UpdateDebugText(label,normal_pose);
        }
        else
        {
          label << " (Tracker): ";
          UpdateDebugText(label,*tracker_pose);
        }
      }
    }

    if (m_View)
    {
      // ask a rendering only if it has never rendered
      vtkRenderWindow *rw=m_Renderer->GetRenderWindow();
      if (rw&&!rw->GetNeverRendered())
        InvokeEvent(CAMERA_UPDATE,MCH_UP,m_View);
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
    double bounds[6];
    tracker->GetCanonicalBounds().CopyTo(bounds);
    m_WorkingBox->SetBounds(bounds);
    m_CanonicalToWorldTransform->SetBounds(&tracker->GetCanonicalBounds()); 
  //}
  //else
  //{
  //  m_WorkingBox->SetBounds(0,0,0,0,0,0);
  //  m_CanonicalToWorldTransform->SetBounds(NULL);
  //}
}

//------------------------------------------------------------------------------
vtkAbstractPropPicker *mafAvatar3D::GetPicker()
//------------------------------------------------------------------------------
{
  if (m_Mode==MODE_2D)
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
  return m_Picker3D;
}

//------------------------------------------------------------------------------
vtkCellPicker *mafAvatar3D::GetPicker2D()
//------------------------------------------------------------------------------
{
  return m_Picker2D;
}

//----------------------------------------------------------------------------
void mafAvatar3D::CreateGui()
//----------------------------------------------------------------------------
{
  Superclass::CreateGui();
  // this should be moved to avatar settings!!!
  wxString mappings[mafCameraTransform::NUM_OF_MAPPINGS];
  mappings[mafCameraTransform::MAX_SCALE]="max scale";
  mappings[mafCameraTransform::MIN_SCALE]="min scale";
  mappings[mafCameraTransform::FIT_X]="fit X";
  mappings[mafCameraTransform::FIT_Y]="fit Y";
  mappings[mafCameraTransform::ANISOTROPIC]="anisotropic";

  wxString coords_system[3];
  coords_system[mafAvatar3D::WORLD_COORDS]="world coords";
  coords_system[mafAvatar3D::TRACKER_COORDS]="tracker coords";
  coords_system[mafAvatar3D::CANONICAL_COORDS]="canonical coords";

  m_Gui->Combo(ID_FITTING_COMBO,"fitting mode",&m_FittingMode,mafCameraTransform::NUM_OF_MAPPINGS,mappings,
    "select the rule for mapping the tracker coordinates into world coordinates");
  m_Gui->Bool(ID_WBOX_BOOL,"working box",&m_DisplayWorkingBox,0,"display working box in the scene");
  m_Gui->Bool(ID_DEBUG_TEXT,"debug text",&m_DisplayDebugText,0,"display working box in the scene");
  m_Gui->VectorN(ID_DEBUG_TEXT_POSITION,"text pos",m_DebugTextPosition,2,MINFLOAT,MAXFLOAT,2,"where to position the text on the screen");
  m_Gui->Combo(ID_COORDS_COMBO,"coords frame",&m_CoordsFrame,3,coords_system,"select pose matrix frame");
  m_Gui->Divider();
}

//----------------------------------------------------------------------------
void mafAvatar3D::OnEvent(mafEventBase* event)
//----------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetId();
  mafID ch=event->GetChannel();

  if (ch==MCH_INPUT) //---- Inputs from the device
  {
    // Input events should all come from the tracker...
    assert(GetTracker()&&event->GetSender()==GetTracker());

    mafEventInteraction *e = (mafEventInteraction *)event;

    if (id==mmdTracker::TRACKER_3D_MOVE) // manage changes in the bounds of the tracker
    {
      OnMove3DEvent(e);
    }
    else if (id==mmdTracker::TRACKER_BOUNDS_UPDATED)
    {
      OnUpdateBoundsEvent(GetTracker());          
    }
  }
 
  switch(event->GetId()) 
  {
    case CAMERA_PRE_RESET:
      OnPreResetCamera(event);
    break; 
    case CAMERA_POST_RESET:
      OnPostResetCamera(event);
    break;
    case VIEW_SELECT:
      {
        mafEvent *e=mafEvent::SafeDownCast(event);
        OnViewSelected(e);
      }
    break;
    case ID_FITTING_COMBO:
      SetFittingMode(m_FittingMode);
    break;
    case ID_COORDS_COMBO:
      SetCoordsFrame(m_CoordsFrame);
    break;
    case ID_WBOX_BOOL:
      SetDisplayWorkingBox(m_DisplayWorkingBox);
    break;
    case ID_DEBUG_TEXT:
      SetDisplayDebugText(m_DisplayDebugText);
    break;
    case ID_DEBUG_TEXT_POSITION:
      SetDebugTextPosition(m_DebugTextPosition[0],m_DebugTextPosition[1]);
    break;
    default:
      Superclass::OnEvent(event);
  }
  
}
