/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3ContourWidget
 Authors: Mel Krokos
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"

#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlaneSource.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkProperty2D.h"
#include "vtkCallbackCommand.h"

#include "albaOpMML3ContourWidget.h"
#include "albaOpMML3ModelView.h"
#include "albaOpMML3ParameterView.h"
#include <algorithm>


vtkCxxRevisionMacro(albaOpMML3ContourWidget, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(albaOpMML3ContourWidget);
vtkCxxSetObjectMacro(albaOpMML3ContourWidget, m_PlaneProperty, vtkProperty);



//----------------------------------------------------------------------------
// Constructor
albaOpMML3ContourWidget::albaOpMML3ContourWidget() 
: vtkPolyDataSourceWidget()
//----------------------------------------------------------------------------
{
  this->m_OperationID = albaOpMML3ContourWidget::NoOp;
  this->m_PlaceMode = false; // center mode off
  this->m_ScalingMode = false; // scaling mode off
  this->m_RotatingMode = false; // rotating mode off
  this->m_TranslationMode = false; // translating mode off
  this->m_State = albaOpMML3ContourWidget::Start;
  this->EventCallbackCommand->SetCallback(albaOpMML3ContourWidget::ProcessEvents);

  this->m_NormalToXAxis = 0;
  this->m_NormalToYAxis = 0;
  this->m_NormalToZAxis = 0;
  this->m_Representation = VTK_PLANE_SURFACE;


  this->SetHandleRadius(1.5);


  //----------------------------------------------------------------------------
  // Represent the plane
  //----------------------------------------------------------------------------
  this->m_PlaneSource = vtkPlaneSource::New();
  this->m_PlaneSource->SetXResolution(4);
  this->m_PlaneSource->SetYResolution(4);

  this->m_PlaneMapper = vtkPolyDataMapper::New();
  this->m_PlaneMapper->SetInput(this->m_PlaneSource->GetOutput());
  this->m_PlaneActor = vtkActor::New();
  this->m_PlaneActor->SetMapper(this->m_PlaneMapper);




  //----------------------------------------------------------------------------
  // Create the handles
  //----------------------------------------------------------------------------
  this->m_HandleActor = new vtkActor* [4];
  this->m_HandleMapper = new vtkPolyDataMapper* [4];
  this->m_HandleGeometry = new vtkSphereSource* [4];
  for (int i=0; i<4; i++){
    this->m_HandleGeometry[i] = vtkSphereSource::New();
    this->m_HandleGeometry[i]->SetThetaResolution(16);
    this->m_HandleGeometry[i]->SetPhiResolution(8);

    this->m_HandleMapper[i] = vtkPolyDataMapper::New();
    this->m_HandleMapper[i]->SetInput(this->m_HandleGeometry[i]->GetOutput());

    this->m_HandleActor[i] = vtkActor::New();
    this->m_HandleActor[i]->SetMapper(this->m_HandleMapper[i]);
  }

  // handles off
  for (int i=0; i<4; i++)
    this->m_HandleActor[i]->VisibilityOff();


  // rotational handle
  this->m_RotationalHandleActor = vtkActor::New();
  this->m_RotationalHandleMapper = vtkPolyDataMapper::New();
  this->m_RotationalHandleGeometry = vtkSphereSource::New();
  this->m_RotationalHandleGeometry->SetThetaResolution(16);
  this->m_RotationalHandleGeometry->SetPhiResolution(8);
  this->m_RotationalHandleMapper->SetInput(this->m_RotationalHandleGeometry->GetOutput());
  this->m_RotationalHandleActor->SetMapper(this->m_RotationalHandleMapper);

  // rotational handle off
  this->m_RotationalHandleActor->VisibilityOff();


  // center handle
  this->m_CenterHandleActor = vtkActor::New();
  this->m_CenterHandleMapper = vtkPolyDataMapper::New();
  this->m_CenterHandleGeometry = vtkSphereSource::New();
  this->m_CenterHandleGeometry->SetThetaResolution(16);
  this->m_CenterHandleGeometry->SetPhiResolution(8);
  this->m_CenterHandleMapper->SetInput(this->m_CenterHandleGeometry->GetOutput());
  this->m_CenterHandleActor->SetMapper(this->m_CenterHandleMapper);

  // center handle off
  this->m_CenterHandleActor->VisibilityOff();


  // Create the + plane normal
  this->m_LineSource = vtkLineSource::New();
  this->m_LineSource->SetResolution(1);
  this->m_LineMapper = vtkPolyDataMapper::New();
  this->m_LineMapper->SetInput(this->m_LineSource->GetOutput());
  this->m_LineActor = vtkActor::New();
  this->m_LineActor->SetMapper(this->m_LineMapper);

  this->m_ConeSource = vtkConeSource::New();
  this->m_ConeSource->SetResolution(12);
  this->m_ConeSource->SetAngle(25.0);
  this->m_ConeMapper = vtkPolyDataMapper::New();
  this->m_ConeMapper->SetInput(this->m_ConeSource->GetOutput());
  this->m_ConeActor = vtkActor::New();
  this->m_ConeActor->SetMapper(this->m_ConeMapper);

  // Create the - plane normal
  this->m_LineSource2 = vtkLineSource::New();
  this->m_LineSource2->SetResolution(1);
  this->m_LineMapper2 = vtkPolyDataMapper::New();
  this->m_LineMapper2->SetInput(this->m_LineSource2->GetOutput());
  this->m_LineActor2 = vtkActor::New();
  this->m_LineActor2->SetMapper(this->m_LineMapper2);

  this->m_ConeSource2 = vtkConeSource::New();
  this->m_ConeSource2->SetResolution(12);
  this->m_ConeSource2->SetAngle(25.0);
  this->m_ConeMapper2 = vtkPolyDataMapper::New();
  this->m_ConeMapper2->SetInput(this->m_ConeSource2->GetOutput());
  this->m_ConeActor2 = vtkActor::New();
  this->m_ConeActor2->SetMapper(this->m_ConeMapper2);


  //----------------------------------------------------------------------------
  // Initialize position of widget and handles
  //----------------------------------------------------------------------------
  double bounds[6] = {-0.5, 0.5, -0.5, 0.5, 0.0, 0.0} ;
  this->PlaceWidget(bounds) ;
  SetHandle0(0, -0.5, 0) ;
  SetHandle1(0.5, 0, 0) ;
  SetHandle2(-0.5, 0, 0) ;
  SetHandle3(0, 0.5, 0) ;
  SetRotationalHandle(0,0,0) ;
  SetCenterHandle(0,0,0) ;



  //Manage the picking stuff
  this->m_HandlePicker = vtkCellPicker::New();
  this->m_HandlePicker->SetTolerance(0.001);
  for (int i=0; i<4; i++)
  {
    this->m_HandlePicker->AddPickList(this->m_HandleActor[i]);
  }
  this->m_HandlePicker->PickFromListOn();

  this->m_PlanePicker = vtkCellPicker::New();
  this->m_PlanePicker->SetTolerance(0.005); //need some fluff
  this->m_PlanePicker->AddPickList(this->m_PlaneActor);
  this->m_PlanePicker->AddPickList(this->m_RotationalHandleActor);
  this->m_PlanePicker->AddPickList(this->m_CenterHandleActor);
  //this->PlanePicker->AddPickList(this->ConeActor);
  //this->PlanePicker->AddPickList(this->LineActor);
  //this->PlanePicker->AddPickList(this->ConeActor2);
  //this->PlanePicker->AddPickList(this->LineActor2);
  this->m_PlanePicker->PickFromListOn();

  this->m_CurrentHandle = NULL;

  // Set up the initial properties
  this->CreateDefaultProperties();

  this->SelectRepresentation();
}



//----------------------------------------------------------------------------
// Destructor
albaOpMML3ContourWidget::~albaOpMML3ContourWidget()
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->Delete();
  this->m_PlaneMapper->Delete();
  this->m_PlaneActor->Delete();

  for (int i=0; i<4; i++){
    this->m_HandleGeometry[i]->Delete();
    this->m_HandleMapper[i]->Delete();
    this->m_HandleActor[i]->Delete();
  }
  delete [] this->m_HandleActor;
  delete [] this->m_HandleMapper;
  delete [] this->m_HandleGeometry;

  m_RotationalHandleActor->Delete() ;
  m_RotationalHandleMapper->Delete() ;
  m_RotationalHandleGeometry->Delete() ;

  m_CenterHandleActor->Delete() ;
  m_CenterHandleMapper->Delete() ;
  m_CenterHandleGeometry->Delete() ;

  this->m_LineActor->Delete();
  this->m_LineMapper->Delete();
  this->m_LineSource->Delete();

  this->m_ConeActor->Delete();
  this->m_ConeMapper->Delete();
  this->m_ConeSource->Delete();

  this->m_LineActor2->Delete();
  this->m_LineMapper2->Delete();
  this->m_LineSource2->Delete();

  this->m_ConeActor2->Delete();
  this->m_ConeMapper2->Delete();
  this->m_ConeSource2->Delete();

  this->m_HandlePicker->Delete();
  this->m_PlanePicker->Delete();

  DeleteDefaultProperties() ;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetEnabled(int enabling)
//----------------------------------------------------------------------------
{
  if ( ! this->Interactor )
  {
    vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
    return;
  }

  if ( enabling ) 
  {
    vtkDebugMacro(<<"Enabling plane widget");

    if ( this->Enabled ) //already enabled, just return
    {
      return;
    }

    if ( ! this->CurrentRenderer )
    {
      this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
        this->Interactor->GetLastEventPosition()[0],
        this->Interactor->GetLastEventPosition()[1]));
      if (this->CurrentRenderer == NULL)
      {
        return;
      }
    }

    this->Enabled = 1;

    // listen for the following events
    vtkRenderWindowInteractor *i = this->Interactor;
    i->AddObserver(vtkCommand::MouseMoveEvent, this->EventCallbackCommand, 
      this->Priority);
    i->AddObserver(vtkCommand::LeftButtonPressEvent, 
      this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::LeftButtonReleaseEvent, 
      this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::MiddleButtonPressEvent, 
      this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::MiddleButtonReleaseEvent, 
      this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::RightButtonPressEvent, 
      this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::RightButtonReleaseEvent, 
      this->EventCallbackCommand, this->Priority);

    // Add the plane
    this->CurrentRenderer->AddActor(this->m_PlaneActor);
    this->m_PlaneActor->SetProperty(this->m_PlaneProperty);

    // turn on the handles
    for (int j=0; j<4; j++)
    {
      this->CurrentRenderer->AddActor(this->m_HandleActor[j]);
      //this->Handle[j]->SetProperty(this->HandleProperty);
    }

    // colours
    this->m_HandleActor[0]->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
    this->m_HandleActor[3]->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
    this->m_HandleActor[2]->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
    this->m_HandleActor[1]->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue

    this->CurrentRenderer->AddActor(this->m_RotationalHandleActor);
    this->m_RotationalHandleActor->SetProperty(this->m_HandleProperty);

    this->CurrentRenderer->AddActor(this->m_CenterHandleActor);
    this->m_CenterHandleActor->SetProperty(this->m_HandleProperty);

    // add the normal vector
    /*this->CurrentRenderer->AddActor(this->LineActor);
    this->LineActor->SetProperty(this->HandleProperty);
    this->CurrentRenderer->AddActor(this->ConeActor);
    this->ConeActor->SetProperty(this->HandleProperty);
    this->CurrentRenderer->AddActor(this->LineActor2);
    this->LineActor2->SetProperty(this->HandleProperty);
    this->CurrentRenderer->AddActor(this->ConeActor2);
    this->ConeActor2->SetProperty(this->HandleProperty);*/

    this->SelectRepresentation();
    this->InvokeEvent(vtkCommand::EnableEvent,NULL);
  }

  else //disabling----------------------------------------------------------
  {
    vtkDebugMacro(<<"Disabling plane widget");

    if ( ! this->Enabled ) //already disabled, just return
    {
      return;
    }

    this->Enabled = 0;

    // don't listen for events any more
    this->Interactor->RemoveObserver(this->EventCallbackCommand);

    // turn off the plane
    this->CurrentRenderer->RemoveActor(this->m_PlaneActor);

    // turn off the handles
    for (int i=0; i<4; i++)
    {
      this->CurrentRenderer->RemoveActor(this->m_HandleActor[i]);
    }

    this->CurrentRenderer->RemoveActor(this->m_RotationalHandleActor);
    this->CurrentRenderer->RemoveActor(this->m_CenterHandleActor);
    // turn off the normal vector
    /* this->CurrentRenderer->RemoveActor(this->LineActor);
    this->CurrentRenderer->RemoveActor(this->ConeActor);
    this->CurrentRenderer->RemoveActor(this->LineActor2);
    this->CurrentRenderer->RemoveActor(this->ConeActor2);*/

    this->m_CurrentHandle = NULL;
    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->SetCurrentRenderer(NULL) ;
  }

  this->Interactor->Render();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                           unsigned long event,
                                           void* clientdata, 
                                           void* vtkNotUsed(calldata))
                                           //----------------------------------------------------------------------------
{
  albaOpMML3ContourWidget* self = reinterpret_cast<albaOpMML3ContourWidget *>( clientdata );

  //okay, let's do the right thing
  switch(event)
  {
  case vtkCommand::LeftButtonPressEvent:
    self->OnLeftButtonDown();
    break;
  case vtkCommand::LeftButtonReleaseEvent:
    self->OnLeftButtonUp();
    break;
  case vtkCommand::MiddleButtonPressEvent:
    //self->OnMiddleButtonDown();
    self->OnLeftButtonDown();
    break;
  case vtkCommand::MiddleButtonReleaseEvent:
    //self->OnMiddleButtonUp();
    self->OnLeftButtonUp();
    break;
  case vtkCommand::RightButtonPressEvent:
    //self->OnRightButtonDown();
    self->OnLeftButtonDown();
    break;
  case vtkCommand::RightButtonReleaseEvent:
    //self->OnRightButtonUp();
    self->OnLeftButtonUp();
    break;
  case vtkCommand::MouseMoveEvent:
    self->OnMouseMove();
    break;
  }
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::PositionHandles(float X, float Y)
//----------------------------------------------------------------------------
{
  /* //int res = this->PlaneSource->GetXResolution();
  float *o = this->PlaneSource->GetOrigin();
  float *pt1 = this->PlaneSource->GetPoint1();
  float *pt2 = this->PlaneSource->GetPoint2();

  float x[3];
  x[0] = o[0] + (pt1[0]-o[0]) + (pt2[0]-o[0]);
  x[1] = o[1] + (pt1[1]-o[1]) + (pt2[1]-o[1]);
  x[2] = o[2] + (pt1[2]-o[2]) + (pt2[2]-o[2]);

  float handle0[3]; // south - green
  handle0[0] = o[0] + X * (pt1[0] - o[0]);
  handle0[1] = o[1] + X * (pt1[1] - o[1]);
  handle0[2] = o[2] + X * (pt1[2] - o[2]);
  this->HandleGeometry[0]->SetCenter(handle0);

  float handle1[3]; // east - blue
  handle1[0] = o[0] + (pt1[0] - o[0]) + Y * (x[0] - pt1[0]);
  handle1[1] = o[1] + (pt1[1] - o[1]) + Y * (x[1] - pt1[1]);
  handle1[2] = o[2] + (pt1[2] - o[2]) + Y * (x[2] - pt1[2]);
  this->HandleGeometry[1]->SetCenter(handle1);

  float handle2[3]; // west - yellow
  handle2[0] = o[0] + Y * (pt2[0] - o[0]);
  handle2[1] = o[1] + Y * (pt2[1] - o[1]);
  handle2[2] = o[2] + Y * (pt2[2] - o[2]);
  this->HandleGeometry[2]->SetCenter(handle2);

  float handle3[3]; // north - red
  handle3[0] = o[0] + (pt2[0] - o[0]) + X * (x[0] - pt2[0]);
  handle3[1] = o[1] + (pt2[1] - o[1]) + X * (x[1] - pt2[1]);
  handle3[2] = o[2] + (pt2[2] - o[2]) + X * (x[2] - pt2[2]);
  this->HandleGeometry[3]->SetCenter(handle3);

  float RotHandle[3];
  RotHandle[0] = o[0] + 0.5 * (pt1[0] - o[0]) + 0.5 * (pt2[0] - o[0]);
  RotHandle[1] = o[1] + 0.5 * (pt1[1] - o[1]) + 0.5 * (pt2[1] - o[1]);
  RotHandle[2] = o[2] + 0.5 * (pt1[2] - o[2]) + 0.5 * (pt2[2] - o[2]);

  this->m_RotationalHandleGeometry->SetCenter(RotHandle);

  // set up the outline
  if ( this->Representation == VTK_PLANE_OUTLINE )
  {
  this->PlaneOutline->GetPoints()->SetPoint(0,o);
  this->PlaneOutline->GetPoints()->SetPoint(1,pt1);
  this->PlaneOutline->GetPoints()->SetPoint(2,x);
  this->PlaneOutline->GetPoints()->SetPoint(3,pt2);
  this->PlaneOutline->Modified();
  }
  this->SelectRepresentation();

  // Create the normal vector
  float center[3];
  this->PlaneSource->GetCenter(center);
  this->LineSource->SetPoint1(center);
  this->LineSource2->SetPoint1(center);
  float p2[3];
  this->PlaneSource->GetNormal(this->Normal);
  vtkMath::Normalize(this->Normal);
  float d = sqrt( 
  vtkMath::Distance2BetweenPoints(
  this->PlaneSource->GetPoint1(),this->PlaneSource->GetPoint2()) );

  p2[0] = center[0] + 0.35 * d * this->Normal[0];
  p2[1] = center[1] + 0.35 * d * this->Normal[1];
  p2[2] = center[2] + 0.35 * d * this->Normal[2];
  this->LineSource->SetPoint2(p2);
  this->ConeSource->SetCenter(p2);
  this->ConeSource->SetDirection(this->Normal);

  p2[0] = center[0] - 0.35 * d * this->Normal[0];
  p2[1] = center[1] - 0.35 * d * this->Normal[1];
  p2[2] = center[2] - 0.35 * d * this->Normal[2];
  this->LineSource2->SetPoint2(p2);
  this->ConeSource2->SetCenter(p2);
  this->ConeSource2->SetDirection(this->Normal);*/
}



//----------------------------------------------------------------------------
int albaOpMML3ContourWidget::HighlightHandle(vtkProp *prop)
//----------------------------------------------------------------------------
{
  // first unhighlight anything picked
  if ( this->m_CurrentHandle )
  {
    //this->CurrentHandle->SetProperty(this->HandleProperty);
  }

  this->m_CurrentHandle = (vtkActor *)prop;

  if ( this->m_CurrentHandle )
  {
    this->ValidPick = 1;
    this->m_HandlePicker->GetPickPosition(this->LastPickPosition);
    //this->CurrentHandle->SetProperty(this->SelectedHandleProperty);
    for (int i=0; i<4; i++) //find handle
    {
      if ( this->m_CurrentHandle == this->m_HandleActor[i] )
      {
        return i;
      }
    }
  }

  return -1;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::HighlightNormal(int highlight)
//----------------------------------------------------------------------------
{
  if ( highlight )
  {
    this->ValidPick = 1;
    this->m_PlanePicker->GetPickPosition(this->LastPickPosition);
    this->m_LineActor->SetProperty(this->m_SelectedHandleProperty);
    this->m_ConeActor->SetProperty(this->m_SelectedHandleProperty);
    this->m_LineActor2->SetProperty(this->m_SelectedHandleProperty);
    this->m_ConeActor2->SetProperty(this->m_SelectedHandleProperty);
  }
  else
  {
    this->m_LineActor->SetProperty(this->m_HandleProperty);
    this->m_ConeActor->SetProperty(this->m_HandleProperty);
    this->m_LineActor2->SetProperty(this->m_HandleProperty);
    this->m_ConeActor2->SetProperty(this->m_HandleProperty);
  }
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::HighlightPlane(int highlight)
//----------------------------------------------------------------------------
{
  if ( highlight )
  {
    this->ValidPick = 1;
    this->m_PlanePicker->GetPickPosition(this->LastPickPosition);
    this->m_PlaneActor->SetProperty(this->m_SelectedPlaneProperty);
  }
  else
  {
    this->m_PlaneActor->SetProperty(this->m_PlaneProperty);
  }
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::OnLeftButtonDown()
//----------------------------------------------------------------------------
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
  {
    this->m_State = albaOpMML3ContourWidget::Outside; // Widget state = Outside
    return;
  }

  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then try to pick the plane.
  // Note that the plane picker's objects include the rotational handle.
  // Note that the widget state "moving" is set for everything except the rotation handle.
  vtkAssemblyPath *path;
  this->m_HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->m_HandlePicker->GetPath();
  if ( path != NULL ){
    this->m_State = albaOpMML3ContourWidget::Moving; // Widget state = Moving

    this->HighlightHandle(path->GetFirstNode()->GetProp());

    if (GetScalingMode()){
      if (this->m_CurrentHandle){
        if (this->m_CurrentHandle == this->m_HandleActor[0]){
          // begin s-s op
          this->m_Operation = albaOpMML3ContourWidget::SouthScale;
        }
        else if (this->m_CurrentHandle == this->m_HandleActor[1]){
          // begin s-e op
          this->m_Operation = albaOpMML3ContourWidget::EastScale;
        }
        else if (this->m_CurrentHandle == this->m_HandleActor[2]){
          // begin s-w op
          this->m_Operation = albaOpMML3ContourWidget::WestScale;
        }
        else if (this->m_CurrentHandle == this->m_HandleActor[3]){
          // begin s-n op
          this->m_Operation = albaOpMML3ContourWidget::NorthScale;
        }
      }
    }
  }
  else{
    this->m_PlanePicker->Pick(X,Y,0.0,this->CurrentRenderer);
    path = this->m_PlanePicker->GetPath();
    if ( path != NULL ){
      vtkProp *prop = path->GetFirstNode()->GetProp();
      if (/* prop == this->ConeActor || prop == this->LineActor ||
          prop == this->ConeActor2 || prop == this->LineActor2*/
          prop == this->m_RotationalHandleActor)
      {
        this->m_State = albaOpMML3ContourWidget::Rotating; // Widget state = Rotating
        this->HighlightNormal(1);

        // begin r-op
        this->m_Operation = albaOpMML3ContourWidget::Rotation;
      }
      /* else
      if (prop == this->CenterHandle)
      {
      this->State = albaOpMML3ContourWidget::Centralising;
      this->HighlightNormal(1);
      }*/
      else{
        this->m_State = albaOpMML3ContourWidget::Moving; // Widget state = Moving
        this->HighlightPlane(1);

        if (GetTranslationMode()){
          // begin t-op
          this->m_Operation = albaOpMML3ContourWidget::Translation;
        }
        else if (GetCenterMode()){
          // begin p-op
          this->m_Operation = albaOpMML3ContourWidget::Placement;
        }
      }
    }
    else{
      // Nothing picked
      this->m_State = albaOpMML3ContourWidget::Outside;
      this->HighlightHandle(NULL);
      return;
    }
  }

  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  this->Interactor->Render();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::OnLeftButtonUp()
//----------------------------------------------------------------------------
{
  if ( this->m_State == albaOpMML3ContourWidget::Outside ||
    this->m_State == albaOpMML3ContourWidget::Start )
  {
    return;
  }

  // 1. switch off display information
  m_Model->GetVisualPipe2D()->SetTextXVisibility(0) ;
  m_Model->GetVisualPipe2D()->SetTextYVisibility(0) ;

  // 2. save undo parameters
  double params[5];
  //float flag;

  // slice id
  params[0] = m_Model->GetCurrentIdOfScans();

  // slice zeta
  params[1] = m_Model->GetZetaOfCurrentSlice();

  // operation type
  params[2] = m_Operation;

  switch (m_Operation)
  {
  case albaOpMML3ContourWidget::Placement : //
    params[3] = m_Model->GetPHSpline()->Evaluate(params[1]);
    params[4] = m_Model->GetPVSpline()->Evaluate(params[1]);
    break;

  case albaOpMML3ContourWidget::Translation : //
    params[3] = m_Model->GetTHSpline()->Evaluate(params[1]);
    params[4] = m_Model->GetTVSpline()->Evaluate(params[1]);
    break;

  case albaOpMML3ContourWidget::Rotation : //
    params[3] = m_Model->GetRASpline()->Evaluate(params[1]);
    params[4] = 0.0;
    break;

  case albaOpMML3ContourWidget::NorthScale : //
    params[3] = m_Model->GetSNSpline()->Evaluate(params[1]);
    params[4] = 0.0;
    /*M->ScalingFlagStack->GetTuple(params[0], &flag);
    if (flag == 0.0)
    flag = 1.0;
    M->ScalingFlagStack->SetTuple(params[0], &flag);*/
    if (!m_Model->GetScalingOccured())
    {
      m_Model->SetScalingOccured(true) ;
      m_Model->SetScalingOccuredOperationId(GetNextOperationId()) ;

      // grey out twist, h/v translation views

      // redraw curves
      m_PH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_PH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_PH->Render();

      m_PV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_PV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_PV->Render();

      m_RA->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_RA->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_RA->Render();

      m_TH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_TH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_TH->Render();

      m_TV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_TV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_TV->Render();
    }

    break;

  case albaOpMML3ContourWidget::SouthScale : //
    params[3] = m_Model->GetSSSpline()->Evaluate(params[1]);
    params[4] = 0.0;
    /*M->ScalingFlagStack->GetTuple(params[0], &flag);
    if (flag == 0.0)
    flag = 1.0;
    M->ScalingFlagStack->SetTuple(params[0], &flag);*/
    if (!m_Model->GetScalingOccured())
    {
      m_Model->SetScalingOccured(true) ;
      m_Model->SetScalingOccuredOperationId(GetNextOperationId()) ;

      // grey out twist, h/v translation views

      // redraw curves
      m_PH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_PH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_PH->Render();

      m_PV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_PV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_PV->Render();

      m_RA->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_RA->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_RA->Render();

      m_TH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_TH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_TH->Render();

      m_TV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_TV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_TV->Render();
    }
    break;

  case albaOpMML3ContourWidget::EastScale : //
    params[3] = m_Model->GetSESpline()->Evaluate(params[1]);
    params[4] = 0.0;
    /*M->ScalingFlagStack->GetTuple(params[0], &flag);
    if (flag == 0.0)
    flag = 1.0;
    M->ScalingFlagStack->SetTuple(params[0], &flag);*/
    if (!m_Model->GetScalingOccured())
    {
      m_Model->SetScalingOccured(true) ;
      m_Model->SetScalingOccuredOperationId(GetNextOperationId()) ;

      // grey out twist, h/v translation views

      // redraw curves
      m_PH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_PH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_PH->Render();

      m_PV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_PV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_PV->Render();

      m_RA->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_RA->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_RA->Render();

      m_TH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_TH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_TH->Render();

      m_TV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_TV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_TV->Render();
    }
    break;

  case albaOpMML3ContourWidget::WestScale : //
    params[3] = m_Model->GetSWSpline()->Evaluate(params[1]);
    params[4] = 0.0;
    /*M->ScalingFlagStack->GetTuple(params[0], &flag);
    if (flag == 0.0)
    flag = 1.0;
    M->ScalingFlagStack->SetTuple(params[0], &flag);*/
    if (!m_Model->GetScalingOccured())
    {
      m_Model->SetScalingOccured(true) ;
      m_Model->SetScalingOccuredOperationId(GetNextOperationId()) ;

      // grey out twist, h/v translation views

      // redraw curves
      m_PH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_PH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_PH->Render();

      m_PV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_PV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_PV->Render();

      m_RA->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_RA->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_RA->Render();

      m_TH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_TH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_TH->Render();

      m_TV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
      m_TV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      m_TV->Render();
    }
    break;

  }

  // exceeded max ?
  assert(GetNextOperationId() < 2000);

  // save
  m_Model->SetOperationsStackTuple(GetNextOperationId(), params);

  // new next op
  SetNextOperationId(GetNextOperationId() + 1);

  this->m_State = albaOpMML3ContourWidget::Start;
  this->HighlightHandle(NULL);
  this->HighlightPlane(0);
  this->HighlightNormal(0);
  this->SizeHandles();

  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  this->Interactor->Render();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::OnMiddleButtonDown()
//----------------------------------------------------------------------------
{
  /*
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
  {
  this->State = albaOpMML3ContourWidget::Outside;
  return;
  }

  // Okay, we can process this. If anything is picked, then we
  // can start pushing the plane.
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
  {
  this->State = albaOpMML3ContourWidget::Pushing;
  this->HighlightPlane(1);
  this->HighlightNormal(1);
  this->HighlightHandle(path->GetFirstNode()->GetProp());
  }
  else
  {
  this->PlanePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->PlanePicker->GetPath();
  if ( path == NULL ) //nothing picked
  {
  this->State = albaOpMML3ContourWidget::Outside;
  return;
  }
  else
  {
  this->State = albaOpMML3ContourWidget::Pushing;
  this->HighlightNormal(1);
  this->HighlightPlane(1);
  }
  }

  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  this->Interactor->Render();
  */
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::OnMiddleButtonUp()
//----------------------------------------------------------------------------
{
  /*
  if ( this->State == albaOpMML3ContourWidget::Outside ||
  this->State == albaOpMML3ContourWidget::Start )
  {
  return;
  }

  this->State = albaOpMML3ContourWidget::Start;
  this->HighlightPlane(0);
  this->HighlightNormal(0);
  this->HighlightHandle(NULL);
  this->SizeHandles();

  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  this->Interactor->Render();
  */
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::OnRightButtonDown()
//----------------------------------------------------------------------------
{
  /*
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
  {
  this->State = albaOpMML3ContourWidget::Outside;
  return;
  }

  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then pick the bounding box.
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
  {
  this->State = albaOpMML3ContourWidget::Scaling;
  this->HighlightPlane(1);
  this->HighlightHandle(path->GetFirstNode()->GetProp());
  }
  else //see if we picked the plane or a normal
  {
  this->PlanePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->PlanePicker->GetPath();
  if ( path == NULL )
  {
  this->State = albaOpMML3ContourWidget::Outside;
  return;
  }
  else
  {
  this->State = albaOpMML3ContourWidget::Scaling;
  this->HighlightPlane(1);
  }
  }

  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  this->Interactor->Render();
  */
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::OnRightButtonUp()
//----------------------------------------------------------------------------
{
  /*
  if ( this->State == albaOpMML3ContourWidget::Outside ||
  this->State == albaOpMML3ContourWidget::Start )
  {
  return;
  }

  this->State = albaOpMML3ContourWidget::Start;
  this->HighlightPlane(0);
  this->SizeHandles();

  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  this->Interactor->Render();
  */
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::OnMouseMove()
//----------------------------------------------------------------------------
{
  // See whether we're active
  if ( this->m_State == albaOpMML3ContourWidget::Outside || this->m_State == albaOpMML3ContourWidget::Start )
    return;

  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Do different things depending on state
  // Calculations everybody does
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z;

  vtkRenderer *renderer = this->Interactor->FindPokedRenderer(X,Y);
  vtkCamera *camera = renderer->GetActiveCamera();
  if ( !camera )
    return;

  // Compute the two points defining the motion vector
  this->ComputeWorldToDisplay(this->LastPickPosition[0], this->LastPickPosition[1], this->LastPickPosition[2], focalPoint);
  z = focalPoint[2];
  this->ComputeDisplayToWorld((double)(this->Interactor->GetLastEventPosition()[0]), 
    (double)(this->Interactor->GetLastEventPosition()[1]), z, prevPickPoint);
  this->ComputeDisplayToWorld((double)X, (double)Y, z, pickPoint);

  // Process the motion
  if ( this->m_State == albaOpMML3ContourWidget::Moving){
    // interaction operations
    // using a contour widget
    if (GetScalingMode()){ // scaling
      if (this->m_CurrentHandle){
        // display information on.
        // off in OnLeftButtonUp()
        m_Model->GetVisualPipe2D()->SetTextYVisibility(1) ;

        if (this->m_CurrentHandle == this->m_HandleActor[0]){ // south scaling
          this->ScaleSouth(prevPickPoint, pickPoint);
        }
        else if (this->m_CurrentHandle == this->m_HandleActor[1]) // east scaling
        {
          this->ScaleEast(prevPickPoint, pickPoint);
        }
        else if (this->m_CurrentHandle == this->m_HandleActor[2]) // west scaling
        {
          this->ScaleWest(prevPickPoint, pickPoint);
        }
        else if (this->m_CurrentHandle == this->m_HandleActor[3]) // north scaling
        {
          this->ScaleNorth(prevPickPoint, pickPoint);
        }
      }
    }
    else if (GetTranslationMode()) // translation
    {
      // display information on.
      // off in OnLeftButtonUp()
      m_Model->GetVisualPipe2D()->SetTextXVisibility(1) ;
      m_Model->GetVisualPipe2D()->SetTextYVisibility(1) ;

      this->Translate(prevPickPoint, pickPoint);
    }
    else if (GetCenterMode()) // place
    {
      // display information on.
      // off in OnLeftButtonUp()
      m_Model->GetVisualPipe2D()->SetTextXVisibility(1) ;
      m_Model->GetVisualPipe2D()->SetTextYVisibility(1) ;

      this->Place(prevPickPoint, pickPoint);
    }
  }
  else if ( this->m_State == albaOpMML3ContourWidget::Scaling )
  {
    this->Scale(prevPickPoint, pickPoint, X, Y);
  }
  else if ( this->m_State == albaOpMML3ContourWidget::Pushing )
  {
    this->Push(prevPickPoint, pickPoint);
  }
  else if ( this->m_State == albaOpMML3ContourWidget::Rotating ) // rotation
  {
    // display information on.
    // off in OnLeftButtonUp()
    m_Model->GetVisualPipe2D()->SetTextXVisibility(1) ;
    m_Model->GetVisualPipe2D()->SetTextYVisibility(1) ;
    this->Rotate(prevPickPoint, pickPoint);
  }

  // Interact, if desired
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent,NULL);

  this->Interactor->Render();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::Scale(double *p1, double *p2, int vtkNotUsed(X), int Y)
//----------------------------------------------------------------------------
{
  //Get the motion vector
  double v[3];
  v[0] = p2[0] - p1[0];
  v[1] = p2[1] - p1[1];
  v[2] = p2[2] - p1[2];

  //int res = this->PlaneSource->GetXResolution();
  double *o = this->m_PlaneSource->GetOrigin();
  double *pt1 = this->m_PlaneSource->GetPoint1();
  double *pt2 = this->m_PlaneSource->GetPoint2();

  double center[3];
  center[0] = o[0] + (pt1[0]-o[0])/2.0 + (pt2[0]-o[0])/2.0;
  center[1] = o[1] + (pt1[1]-o[1])/2.0 + (pt2[1]-o[1])/2.0;
  center[2] = o[2] + (pt1[2]-o[2])/2.0 + (pt2[2]-o[2])/2.0;

  // Compute the scale factor
  double sf = vtkMath::Norm(v) / sqrt(vtkMath::Distance2BetweenPoints(pt1,pt2));
  if ( Y > this->Interactor->GetLastEventPosition()[1] )
  {
    sf = 1.0 + sf;
  }
  else
  {
    sf = 1.0 - sf;
  }

  // Move the corner points
  double origin[3], point1[3], point2[3];
  for (int i=0; i<3; i++)
  {
    origin[i] = sf * (o[i] - center[i]) + center[i];
    point1[i] = sf * (pt1[i] - center[i]) + center[i];
    point2[i] = sf * (pt2[i] - center[i]) + center[i];
  }

  this->m_PlaneSource->SetOrigin(origin);
  this->m_PlaneSource->SetPoint1(point1);
  this->m_PlaneSource->SetPoint2(point2);
  this->m_PlaneSource->Update();

  this->PositionHandles(0.5, 0.5);
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::Push(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  //Get the motion vector
  float v[3];
  v[0] = p2[0] - p1[0];
  v[1] = p2[1] - p1[1];
  v[2] = p2[2] - p1[2];

  this->m_PlaneSource->Push( vtkMath::Dot(v,this->m_Normal) );
  this->m_PlaneSource->Update();
  this->PositionHandles(0.5, 0.5);
}


//----------------------------------------------------------------------------
// Create and set default properties
// NB This allocates vtk objects 
// You must call DeletetDefaultProperties() when finished
void albaOpMML3ContourWidget::CreateDefaultProperties()
//----------------------------------------------------------------------------
{
  // Handle properties
  this->m_HandleProperty = vtkProperty::New();
  this->m_HandleProperty->SetColor(1,1,1);

  this->m_SelectedHandleProperty = vtkProperty::New();
  this->m_SelectedHandleProperty->SetColor(1,0,0);

  // Plane properties
  this->m_PlaneProperty = vtkProperty::New();
  this->m_PlaneProperty->SetAmbient(1.0);
  this->m_PlaneProperty->SetAmbientColor(1.0,1.0,1.0);

  this->m_SelectedPlaneProperty = vtkProperty::New();
  this->SelectRepresentation();
  this->m_SelectedPlaneProperty->SetAmbient(1.0);
  this->m_SelectedPlaneProperty->SetAmbientColor(0.5,0.5,0.5);
}




//----------------------------------------------------------------------------
// Delete default properties
void albaOpMML3ContourWidget::DeleteDefaultProperties()
//----------------------------------------------------------------------------
{
  m_HandleProperty->Delete() ;
  m_SelectedHandleProperty->Delete() ;
  m_PlaneProperty->Delete() ;
  m_SelectedPlaneProperty->Delete() ;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::PlaceWidget(double bds[6])
//----------------------------------------------------------------------------
{
  int i;
  double bounds[6], center[3];

  this->AdjustBounds(bds, bounds, center);

  if (this->Input || this->Prop3D)
  {
    if ( this->m_NormalToYAxis )
    {
      this->m_PlaneSource->SetOrigin(bounds[0],center[1],bounds[4]);
      this->m_PlaneSource->SetPoint1(bounds[1],center[1],bounds[4]);
      this->m_PlaneSource->SetPoint2(bounds[0],center[1],bounds[5]);
    }
    else if ( this->m_NormalToZAxis )
    {
      this->m_PlaneSource->SetOrigin(bounds[0],bounds[2],center[2]);
      this->m_PlaneSource->SetPoint1(bounds[1],bounds[2],center[2]);
      this->m_PlaneSource->SetPoint2(bounds[0],bounds[3],center[2]);
    }
    else //default or x-normal
    {
      this->m_PlaneSource->SetOrigin(center[0],bounds[2],bounds[4]);
      this->m_PlaneSource->SetPoint1(center[0],bounds[3],bounds[4]);
      this->m_PlaneSource->SetPoint2(center[0],bounds[2],bounds[5]);
    }
  }

  this->m_PlaneSource->Update();

  // Position the handles at the end of the planes
  this->PositionHandles(0.5, 0.5);

  for (i=0; i<6; i++)
  {
    this->InitialBounds[i] = bounds[i];
  }


  if (this->Input || this->Prop3D)
  {
    this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
      (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
      (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));
  }
  else
  {
    // this means we have to make use of the PolyDataSource, so
    // we just calculate the magnitude of the longest diagonal on
    // the plane and use that as InitialLength
    double origin[3], point1[3], point2[3];
    this->m_PlaneSource->GetOrigin(origin);
    this->m_PlaneSource->GetPoint1(point1);
    this->m_PlaneSource->GetPoint2(point2);
    double sqr1 = 0, sqr2 = 0;
    for (i = 0; i < 3; i++)
    {
      sqr1 += (point1[i] - origin[i]) * (point1[i] - origin[i]);
      sqr2 += (point2[i] - origin[i]) * (point2[i] - origin[i]);
    }

    this->InitialLength = sqrt(sqr1 + sqr2);
  }

  // Set the radius on the sphere handles
  this->SizeHandles();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SizeHandles()
//----------------------------------------------------------------------------
{
  // float radius = this->vtk3DWidget::SizeHandles(1.25);

  float radius = GetHandleRadius();

  for(int i=0; i<4; i++)
  {
    this->m_HandleGeometry[i]->SetRadius(radius);
  }

  this->m_RotationalHandleGeometry->SetRadius(radius);
  this->m_CenterHandleGeometry->SetRadius(radius);

  // Set the height and radius of the cone
  /*this->ConeSource->SetHeight(2.0*radius);
  this->ConeSource->SetRadius(radius);
  this->ConeSource2->SetHeight(2.0*radius);
  this->ConeSource2->SetRadius(radius);*/
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SelectRepresentation()
//----------------------------------------------------------------------------
{
  if ( this->m_Representation == VTK_PLANE_OFF ){
    m_PlaneActor->SetVisibility(0) ;
  }
  else if ( this->m_Representation == VTK_PLANE_SURFACE ){
    m_PlaneActor->SetVisibility(1) ;
    this->m_PlaneMapper->SetInput( this->m_PlaneSource->GetOutput() );
    this->m_PlaneActor->GetProperty()->SetRepresentationToSurface();
  }
  else if ( this->m_Representation == VTK_PLANE_WIREFRAME ){
    m_PlaneActor->SetVisibility(1) ;
    this->m_PlaneMapper->SetInput( this->m_PlaneSource->GetOutput() );
    this->m_PlaneActor->GetProperty()->SetRepresentationToWireframe();
  }
  else{
    // unknown representation code
    assert(false) ;
  }
}



//----------------------------------------------------------------------------
/** Description:
Set/Get the resolution (number of subdivisions) of the plane. */
void albaOpMML3ContourWidget::SetResolution(int r)
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetXResolution(r); 
  this->m_PlaneSource->SetYResolution(r); 
}



//----------------------------------------------------------------------------
int albaOpMML3ContourWidget::GetResolution()
//----------------------------------------------------------------------------
{ 
  return this->m_PlaneSource->GetXResolution(); 
}



//----------------------------------------------------------------------------
// Description:
// Set/Get the origin of the plane.
void albaOpMML3ContourWidget::SetOrigin(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetOrigin(x,y,z);
  this->PositionHandles(0.5, 0.5);
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetOrigin(double x[3]) 
//----------------------------------------------------------------------------
{
  this->SetOrigin(x[0], x[1], x[2]);
}



//----------------------------------------------------------------------------
double* albaOpMML3ContourWidget::GetOrigin() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetOrigin();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::GetOrigin(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetOrigin(xyz);
}



//----------------------------------------------------------------------------
// Description:
// Set/Get the position of the point defining the first axis of the plane.
void albaOpMML3ContourWidget::SetPoint1(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetPoint1(x,y,z);
  //this->PositionHandles(0.5, 0.5);
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetPoint1(double x[3]) 
//----------------------------------------------------------------------------
{
  this->SetPoint1(x[0], x[1], x[2]);
}



//----------------------------------------------------------------------------
double* albaOpMML3ContourWidget::GetPoint1() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetPoint1();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::GetPoint1(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetPoint1(xyz);
}



//----------------------------------------------------------------------------
// Description:
// Set/Get the position of the point defining the second axis of the plane.
void albaOpMML3ContourWidget::SetPoint2(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetPoint2(x,y,z);
  //this->PositionHandles(0.5, 0.5);
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetPoint2(double x[3]) 
//----------------------------------------------------------------------------
{
  this->SetPoint2(x[0], x[1], x[2]);
}



//----------------------------------------------------------------------------
double* albaOpMML3ContourWidget::GetPoint2() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetPoint2();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::GetPoint2(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetPoint2(xyz);
}



//----------------------------------------------------------------------------
// Description:
// Set the center of the plane.
void albaOpMML3ContourWidget::SetCenter(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetCenter(x, y, z);
  this->PositionHandles(0.5, 0.5);
}



//----------------------------------------------------------------------------
// Description:
// Set the center of the plane.
void albaOpMML3ContourWidget::SetCenter(double c[3]) 
//----------------------------------------------------------------------------
{
  this->SetCenter(c[0], c[1], c[2]);
}



//----------------------------------------------------------------------------
// Description:
// Get the center of the plane.
double* albaOpMML3ContourWidget::GetCenter() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetCenter();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::GetCenter(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetCenter(xyz);
}



//----------------------------------------------------------------------------
// Description:
// Set the normal to the plane.
void albaOpMML3ContourWidget::SetNormal(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetNormal(x, y, z);
  this->PositionHandles(0.5, 0.5);
}



//----------------------------------------------------------------------------
// Description:
// Set the normal to the plane.
void albaOpMML3ContourWidget::SetNormal(double n[3]) 
//----------------------------------------------------------------------------
{
  this->SetNormal(n[0], n[1], n[2]);
}



//----------------------------------------------------------------------------
// Description:
// Get the normal to the plane.
double* albaOpMML3ContourWidget::GetNormal() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetNormal();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::GetNormal(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetNormal(xyz);
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::GetPolyData(vtkPolyData *pd)
//----------------------------------------------------------------------------
{ 
  pd->ShallowCopy(this->m_PlaneSource->GetOutput()); 
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::GetPlane(vtkPlane *plane)
//----------------------------------------------------------------------------
{
  if ( plane == NULL )
  {
    return;
  }

  plane->SetNormal(this->GetNormal());
  plane->SetOrigin(this->GetCenter());
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::UpdatePlacement(void)
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->Update();
  this->PositionHandles(0.5, 0.5);
}


/*
//----------------------------------------------------------------------------
// MK BEGIN 26/05/04
// Description:
// Set the motion vector
void albaOpMML3ContourWidget::SetMotionVector(float x, float y, float z) 
//----------------------------------------------------------------------------
{
  this->m_Motion[0] = x;
  this->m_Motion[1] = y;
  this->m_Motion[2] = z;
}



//----------------------------------------------------------------------------
// Description:
// Set the motion vector.
void albaOpMML3ContourWidget::SetMotionVector(float m[3]) 
//----------------------------------------------------------------------------
{
  this->m_Motion[0] = m[0];
  this->m_Motion[1] = m[1];
  this->m_Motion[2] = m[2];
}



//----------------------------------------------------------------------------
// Description:
// Get the motion vector
float* albaOpMML3ContourWidget::GetMotionVector() 
//----------------------------------------------------------------------------
{
  return this->m_Motion;
}



//----------------------------------------------------------------------------
// Description:
// Get the motion vector
void albaOpMML3ContourWidget::GetMotionVector(float xyz[3]) 
//----------------------------------------------------------------------------
{
  xyz[0] = this->m_Motion[0];
  xyz[1] = this->m_Motion[1];
  xyz[2] = this->m_Motion[2];
}
*/


//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::TranslationModeOn()
//----------------------------------------------------------------------------
{
  this->m_PlaceMode = false;
  this->m_ScalingMode = false ;
  this->m_RotatingMode = false ;
  this->m_TranslationMode = true;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::TranslationModeOff()
//----------------------------------------------------------------------------
{
  this->m_TranslationMode = false;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::PlaceModeOn()
//----------------------------------------------------------------------------
{
  //this->CenterHandle->VisibilityOn();
  this->m_PlaceMode = true;
  this->m_ScalingMode = false ;
  this->m_RotatingMode = false ;
  this->m_TranslationMode = false ;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::PlaceModeOff()
//----------------------------------------------------------------------------
{
  //this->CenterHandle->VisibilityOff();
  this->m_PlaceMode = false;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::RotationModeOn()
//----------------------------------------------------------------------------
{
  //this->m_RotationalHandleActor->VisibilityOn();
  //this->Setm_RotationalHandle(50.0, 0.0, 0.0);
  this->m_PlaceMode = false;
  this->m_ScalingMode = false ;
  this->m_RotatingMode = true ;
  this->m_TranslationMode = false ;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::RotationModeOff()
//----------------------------------------------------------------------------
{
  //this->m_RotationalHandleActor->VisibilityOff();
  this->m_RotatingMode = false;
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::ScalingModeOn()
//----------------------------------------------------------------------------
{
  this->m_PlaceMode = false;
  this->m_ScalingMode = true ;
  this->m_RotatingMode = false ;
  this->m_TranslationMode = false ;

  //this->ScalingHandlesOn();

  // 
  //this->SetRepresentationToWireframe();
  //this->GetPlaneProperty()->SetOpacity(1.0);
  //this->GetSelectedPlaneProperty()->SetOpacity(1.0);

  //
  //this->PlanePicker->DeletePickList(this->PlaneActor);
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::ScalingModeOff()
//----------------------------------------------------------------------------
{
  this->m_ScalingMode = false;

  //
  //this->ScalingHandlesOff();

  // 
  //this->SetRepresentationToOutline();
  //this->GetPlaneProperty()->SetOpacity(1.0);
  //this->GetSelectedPlaneProperty()->SetOpacity(1.0);

  //
  //this->PlanePicker->AddPickList(this->PlaneActor);
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetHandle0(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[0]->SetCenter(xyz);
  this->m_HandleGeometry[0]->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetHandle0(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[0]->SetCenter(x, y, z);
  this->m_HandleGeometry[0]->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetHandle1(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[1]->SetCenter(xyz);
  this->m_HandleGeometry[1]->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetHandle1(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[1]->SetCenter(x, y, z);
  this->m_HandleGeometry[1]->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetHandle2(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[2]->SetCenter(xyz);
  this->m_HandleGeometry[2]->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetHandle2(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[2]->SetCenter(x, y, z);
  this->m_HandleGeometry[2]->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetHandle3(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[3]->SetCenter(xyz);
  this->m_HandleGeometry[3]->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetHandle3(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[3]->SetCenter(x, y, z);
  this->m_HandleGeometry[3]->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetRotationalHandle(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_RotationalHandleGeometry->SetCenter(x, y, z);
  this->m_RotationalHandleGeometry->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetRotationalHandle(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_RotationalHandleGeometry->SetCenter(xyz);
  this->m_RotationalHandleGeometry->Modified();
}




//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetCenterHandle(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_CenterHandleGeometry->SetCenter(x, y, z);
  this->m_CenterHandleGeometry->Modified();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetCenterHandle(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_CenterHandleGeometry->SetCenter(xyz);
  this->m_CenterHandleGeometry->Modified();
}




//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::ScalingHandlesOn()
//----------------------------------------------------------------------------
{
  for(int i=0; i<4; i++)
  {
    this->m_HandleActor[i]->VisibilityOn();
  }
}

//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::ScalingHandlesOff()
//----------------------------------------------------------------------------
{
  for(int i=0; i<4; i++)
  {
    this->m_HandleActor[i]->VisibilityOff();
  }
}





//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetRotationHandleVisibility()
//----------------------------------------------------------------------------
{
  m_RotationalHandleActor->VisibilityOn();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::RotationHandleOn()
//----------------------------------------------------------------------------
{
  m_RotationalHandleActor->VisibilityOn();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::RotationHandleOff()
//----------------------------------------------------------------------------
{
  m_RotationalHandleActor->VisibilityOff();
}





//----------------------------------------------------------------------------
// Update the positions of the scaling handles
// This should place the handles where the cutting planes intersect the contour
void albaOpMML3ContourWidget::UpdateScalingHandles()
//----------------------------------------------------------------------------
{
  // get zeta level
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = m_Model->GetPHSpline()->Evaluate(zeta);
  ctrans[1] = m_Model->GetPVSpline()->Evaluate(zeta);

  // get twist
  double twist = m_Model->GetRASpline()->Evaluate(zeta);

  // get h/v translation
  double trans[2];
  trans[0] = m_Model->GetTHSpline()->Evaluate(zeta);
  trans[1] = m_Model->GetTVSpline()->Evaluate(zeta);

  // get scaling
  double scale[4];
  scale[0] = m_Model->GetSSSpline()->Evaluate(zeta); // east
  scale[1] = m_Model->GetSESpline()->Evaluate(zeta); // west
  scale[2] = m_Model->GetSWSpline()->Evaluate(zeta); // north
  scale[3] = m_Model->GetSNSpline()->Evaluate(zeta); // south



  // Get bounds of contour segments after clipping planes applied, but before transforms
  double SEBounds[6], SWBounds[6], NEBounds[6], NWBounds[6] ;
  m_Model->UpdateCuttingPlanesTransform() ;
  m_Model->GetVisualPipe2D()->GetOriginalSegmentBoundsNE(NEBounds) ;
  m_Model->GetVisualPipe2D()->GetOriginalSegmentBoundsNW(NWBounds) ;
  m_Model->GetVisualPipe2D()->GetOriginalSegmentBoundsSE(SEBounds) ;
  m_Model->GetVisualPipe2D()->GetOriginalSegmentBoundsSW(SWBounds) ;



  // original center
  double center[3];
  m_Model->GetOriginalContourCenter(center);


  // get positions of handles on untransformed contour
  double h3[3]; // north - red
  h3[0] = center[0] - trans[0];
  h3[1] = std::min(NEBounds[3], NWBounds[3]);
  h3[2] = 0.0;
  double h0[3]; // south - green
  h0[0] = center[0] - trans[0];
  h0[1] = std::max(SEBounds[2], SWBounds[2]);
  h0[2] = 0.0;
  double h1[3]; // east - blue
  h1[0] = std::min(NEBounds[1], SEBounds[1]);
  h1[1] = center[1] - trans[1];
  h1[2] = 0.0;
  double h2[3]; // west - magenta
  h2[0] = std::max(NWBounds[0], SWBounds[0]);
  h2[1] = center[1] - trans[1];
  h2[2] = 0.0;


  //----------------------------------------------------------------------------
  // transform the handles to the final positions
  //----------------------------------------------------------------------------

  // north - red
  vtkTransform* h3Transf = vtkTransform::New();
  h3Transf->Identity();

  h3Transf->Translate(ctrans[0], ctrans[1], 0.0) ;
  h3Transf->Translate(center[0], center[1], 0.0) ;
  h3Transf->RotateZ(twist) ;
  h3Transf->Scale(1.0, scale[3], 1.0) ;
  h3Transf->Translate(-center[0], -center[1], 0.0) ;
  h3Transf->Translate(trans[0], trans[1], 0.0) ;

  h3Transf->TransformPoint(h3, h3);
  SetHandle3(h3);

  h3Transf->Delete();


  // south - green	
  vtkTransform* h0Transf = vtkTransform::New();
  h0Transf->Identity();

  h0Transf->Translate(ctrans[0], ctrans[1], 0.0) ;
  h0Transf->Translate(center[0], center[1], 0.0) ;
  h0Transf->RotateZ(twist) ;
  h0Transf->Scale(1.0, scale[0], 1.0) ;
  h0Transf->Translate(-center[0], -center[1], 0.0) ;
  h0Transf->Translate(trans[0], trans[1], 0.0) ;

  h0Transf->TransformPoint(h0, h0);
  SetHandle0(h0);

  h0Transf->Delete();


  // east - blue
  vtkTransform* h1Transf = vtkTransform::New();
  h1Transf->Identity();

  h1Transf->Translate(ctrans[0], ctrans[1], 0.0) ;
  h1Transf->Translate(center[0], center[1], 0.0) ;
  h1Transf->RotateZ(twist) ;
  h1Transf->Scale(scale[1], 1.0, 1.0) ;
  h1Transf->Translate(-center[0], -center[1], 0.0) ;
  h1Transf->Translate(trans[0], trans[1], 0.0) ;

  h1Transf->TransformPoint(h1, h1);
  SetHandle1(h1);

  h1Transf->Delete();


  // west - magenta
  vtkTransform* h2Transf = vtkTransform::New();
  h2Transf->Identity();

  h2Transf->Translate(ctrans[0], ctrans[1], 0.0) ;
  h2Transf->Translate(center[0], center[1], 0.0) ;
  h2Transf->RotateZ(twist) ;
  h2Transf->Scale(scale[2], 1.0, 1.0) ;
  h2Transf->Translate(-center[0], -center[1], 0.0) ;
  h2Transf->Translate(trans[0], trans[1], 0.0) ;

  h2Transf->TransformPoint(h2, h2);
  SetHandle2(h2);

  h2Transf->Delete();
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::UpdateRotationHandle()
//----------------------------------------------------------------------------
{
  // get zeta level
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // original center
  double center[3];
  m_Model->GetOriginalContourCenter(center);

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = m_Model->GetPHSpline()->Evaluate(zeta);
  ctrans[1] = m_Model->GetPVSpline()->Evaluate(zeta);

  SetRotationalHandle(center[0] + ctrans[0], center[1] + ctrans[1], 0.0);
}



//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::UpdateWidgetTransform()
//----------------------------------------------------------------------------
{
  // get zeta level
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // original bounds and center
  double bounds[6], center[3] ;
  m_Model->GetOriginalContourBounds(bounds);
  m_Model->GetOriginalContourCenter(center) ;

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = m_Model->GetPHSpline()->Evaluate(zeta);
  ctrans[1] = m_Model->GetPVSpline()->Evaluate(zeta);

  // get twist
  double twist = m_Model->GetRASpline()->Evaluate(zeta);

  // get h/v translation
  double trans[2];
  trans[0] = m_Model->GetTHSpline()->Evaluate(zeta);
  trans[1] = m_Model->GetTVSpline()->Evaluate(zeta);

  // get scaling
  double scale[4];
  scale[0] = m_Model->GetSESpline()->Evaluate(zeta); // east
  scale[1] = m_Model->GetSWSpline()->Evaluate(zeta); // west
  scale[2] = m_Model->GetSNSpline()->Evaluate(zeta); // north
  scale[3] = m_Model->GetSSSpline()->Evaluate(zeta); // south;



  // untransformed pose of plane
  double Point1[3], Point2[3], Origin[3] ;
  Point1[0] = bounds[1];
  Point1[1] = bounds[2];
  Point1[2] = 0.0;
  Point2[0] = bounds[0];
  Point2[1] = bounds[3];
  Point2[2] = 0.0;
  Origin[0] = Point2[0];
  Origin[1] = Point1[1];
  Origin[2] = 0.0;


  // transformed origin
  vtkTransform *OrTransf = vtkTransform::New();
  OrTransf->Identity();
  OrTransf->Translate(center[0], center[1], 0.0); // axes origin
  OrTransf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
  OrTransf->TransformPoint(Origin, Origin);
  OrTransf->Delete();

  // transformed point 1
  vtkTransform *P1Transf = vtkTransform::New();
  P1Transf->Identity();
  P1Transf->Translate(center[0], center[1], 0.0); // axes origin
  P1Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
  P1Transf->RotateZ(twist);                       // r operation
  P1Transf->TransformPoint(Point1, Point1);
  P1Transf->Delete();

  // transformed point 2
  vtkTransform *P2Transf = vtkTransform::New();
  P2Transf->Identity();
  P2Transf->Translate(center[0], center[1], 0.0); // axes origin
  P2Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
  P2Transf->RotateZ(twist);                       // r operation
  P2Transf->Delete();

  SetOrigin(Origin);
  SetPoint1(Point1);
  SetPoint2(Point2);
  m_PlaneMapper->Update() ;
}



//----------------------------------------------------------------------------
// Set the north scaling 
// Inputs are the previous and current positions of the mouse.
// The mouse position is projected onto the widget axis and a scaling value calculated.
// The splines are updated and used to update the widget and the contour.
void albaOpMML3ContourWidget::ScaleNorth(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  double mvec[3];
  mvec[0] = p2[0] - p1[0]; 
  mvec[1] = p2[1] - p1[1]; 
  mvec[2] = 0.0; // no change along z

  // get current slice id and zeta 
  int currentId = m_Model->GetCurrentIdOfScans();
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // evaluate splines
  double chtrans = m_Model->GetPHSpline()->Evaluate(zeta); // place h
  double cvtrans = m_Model->GetPVSpline()->Evaluate(zeta); // place v
  double twist = m_Model->GetRASpline()->Evaluate(zeta);   // rotate
  double htrans = m_Model->GetTHSpline()->Evaluate(zeta);  // translate h
  double vtrans = m_Model->GetTVSpline()->Evaluate(zeta);  // translate v
  double nscale = m_Model->GetSNSpline()->Evaluate(zeta);  // scale n
  double sscale = m_Model->GetSSSpline()->Evaluate(zeta);  // scale s
  double escale = m_Model->GetSESpline()->Evaluate(zeta);  // scale e
  double wscale = m_Model->GetSWSpline()->Evaluate(zeta);  // scale w

  // rotated y unit vector
  double y[3] = {0.0, 1.0, 0.0} ;
  vtkTransform* TransformY = vtkTransform::New();
  TransformY->Identity();
  TransformY->RotateZ(twist);
  TransformY->TransformPoint(y, y);
  TransformY->Delete();

  // projection on rotated y axis
  vtkMath* Math = vtkMath::New();
  double yprojection = Math->Dot(mvec, y);
  Math->Delete();

  if (yprojection == 0.0) // no motion
    return;

  // overall scale
  double scale = nscale + 0.05*yprojection ;


  // exceeded max limits?
  if (scale > m_SN->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < m_SN->GetMinY())
    return;// display information vtrans
  m_Model->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update n spline
  // display values
  m_SN->RemovePoint(currentId);
  m_SN->AddPoint(currentId, scale);
  m_SN->Render();

  // actual values
  m_Model->GetSNSpline()->RemovePoint(zeta);
  m_Model->GetSNSpline()->AddPoint(zeta, scale);


  // update widget and scaling handles
  UpdateWidgetTransform();
  UpdateScalingHandles();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();
}



//----------------------------------------------------------------------------
// Set the south scaling 
void albaOpMML3ContourWidget::ScaleSouth(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  double mvec[3];
  mvec[0] = p2[0] - p1[0]; 
  mvec[1] = p2[1] - p1[1]; 
  mvec[2] = 0.0; // no change along z

  // get current slice id and zeta 
  int currentId = m_Model->GetCurrentIdOfScans();
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // evaluate splines
  double chtrans = m_Model->GetPHSpline()->Evaluate(zeta); // place h
  double cvtrans = m_Model->GetPVSpline()->Evaluate(zeta); // place v
  double twist = m_Model->GetRASpline()->Evaluate(zeta);   // rotate
  double htrans = m_Model->GetTHSpline()->Evaluate(zeta);  // translate h
  double vtrans = m_Model->GetTVSpline()->Evaluate(zeta);  // translate v
  double nscale = m_Model->GetSNSpline()->Evaluate(zeta);  // scale n
  double sscale = m_Model->GetSSSpline()->Evaluate(zeta);  // scale s
  double escale = m_Model->GetSESpline()->Evaluate(zeta);  // scale e
  double wscale = m_Model->GetSWSpline()->Evaluate(zeta);  // scale w

  // rotated y unit vector
  double y[3] = {0.0, 1.0, 0.0} ;
  vtkTransform* TransformY = vtkTransform::New();
  TransformY->Identity();
  TransformY->RotateZ(twist);
  TransformY->TransformPoint(y, y);
  TransformY->Delete();

  // projection on rotated y axis
  vtkMath* Math = vtkMath::New();
  double yprojection = Math->Dot(mvec, y);
  Math->Delete();

  if (yprojection == 0.0) // no motion
    return;

  // overall scale
  double scale = sscale - 0.05*yprojection;

  // exceeded max limits?
  if (scale > m_SS->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < m_SS->GetMinY())
    return;

  // display information vtrans
  m_Model->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update s spline
  // display values
  m_SS->RemovePoint(currentId);
  m_SS->AddPoint(currentId, scale);
  m_SS->Render();

  // actual values
  m_Model->GetSSSpline()->RemovePoint(zeta);
  m_Model->GetSSSpline()->AddPoint(zeta, scale);


  // update widget and scaling handles
  UpdateWidgetTransform();
  UpdateScalingHandles();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();
}



//----------------------------------------------------------------------------
// Set the east scaling
void albaOpMML3ContourWidget::ScaleEast(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  double mvec[3];
  mvec[0] = p2[0] - p1[0]; 
  mvec[1] = p2[1] - p1[1]; 
  mvec[2] = 0.0; // no change along z

  // get current slice id and zeta 
  int currentId = m_Model->GetCurrentIdOfScans();
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // evaluate splines
  double chtrans = m_Model->GetPHSpline()->Evaluate(zeta); // place h
  double cvtrans = m_Model->GetPVSpline()->Evaluate(zeta); // place v
  double twist = m_Model->GetRASpline()->Evaluate(zeta);   // rotate
  double htrans = m_Model->GetTHSpline()->Evaluate(zeta);  // translate h
  double vtrans = m_Model->GetTVSpline()->Evaluate(zeta);  // translate v
  double nscale = m_Model->GetSNSpline()->Evaluate(zeta);  // scale n
  double sscale = m_Model->GetSSSpline()->Evaluate(zeta);  // scale s
  double escale = m_Model->GetSESpline()->Evaluate(zeta);  // scale e
  double wscale = m_Model->GetSWSpline()->Evaluate(zeta);  // scale w

  // rotated x unit vector
  double x[3] = {1.0, 0.0, 0.0} ;
  vtkTransform* TransformX = vtkTransform::New();
  TransformX->Identity();
  TransformX->RotateZ(twist);
  TransformX->TransformPoint(x, x);
  TransformX->Delete();

  // projection on rotated x axis
  vtkMath* Math = vtkMath::New();
  double xprojection = Math->Dot(mvec, x);
  Math->Delete();

  if (xprojection == 0.0) // no motion
    return;

  // overall scale
  double scale = escale + 0.05*xprojection ;


  // exceeded max limits?
  if (scale > m_SE->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < m_SE->GetMinY())
    return;

  // display information vtrans
  m_Model->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update e spline
  // display values
  m_SE->RemovePoint(currentId);
  m_SE->AddPoint(currentId, scale);
  m_SE->Render();

  // actual values
  m_Model->GetSESpline()->RemovePoint(zeta);
  m_Model->GetSESpline()->AddPoint(zeta, scale);


  // update widget and scaling handles
  UpdateWidgetTransform();
  UpdateScalingHandles();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();
}



//----------------------------------------------------------------------------
// Set the west scaling
void albaOpMML3ContourWidget::ScaleWest(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  double mvec[3];
  mvec[0] = p2[0] - p1[0]; 
  mvec[1] = p2[1] - p1[1]; 
  mvec[2] = 0.0; // no change along z

  // get current slice id and zeta 
  int currentId = m_Model->GetCurrentIdOfScans();
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // evaluate splines
  double chtrans = m_Model->GetPHSpline()->Evaluate(zeta); // place h
  double cvtrans = m_Model->GetPVSpline()->Evaluate(zeta); // place v
  double twist = m_Model->GetRASpline()->Evaluate(zeta);   // rotate
  double htrans = m_Model->GetTHSpline()->Evaluate(zeta);  // translate h
  double vtrans = m_Model->GetTVSpline()->Evaluate(zeta);  // translate v
  double nscale = m_Model->GetSNSpline()->Evaluate(zeta);  // scale n
  double sscale = m_Model->GetSSSpline()->Evaluate(zeta);  // scale s
  double escale = m_Model->GetSESpline()->Evaluate(zeta);  // scale e
  double wscale = m_Model->GetSWSpline()->Evaluate(zeta);  // scale w

  // rotated x unit vector
  double x[3] = {1.0, 0.0, 0.0} ;
  vtkTransform* TransformX = vtkTransform::New();
  TransformX->Identity();
  TransformX->RotateZ(twist);
  TransformX->TransformPoint(x, x);
  TransformX->Delete();

  // projection on rotated x axis
  vtkMath* Math = vtkMath::New();
  double xprojection = Math->Dot(mvec, x);
  Math->Delete();

  if (xprojection == 0.0) // no motion
    return;

  // overall scale
  double scale = wscale - 0.05*xprojection ;

  // exceeded max limits?
  if (scale > m_SW->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < m_SW->GetMinY())
    return;

  // display information vtrans
  m_Model->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update w spline
  // display values
  m_SW->RemovePoint(currentId);
  m_SW->AddPoint(currentId, scale);
  m_SW->Render();

  // actual values
  m_Model->GetSWSpline()->RemovePoint(zeta);
  m_Model->GetSWSpline()->AddPoint(zeta, scale);


  // update widget and scaling handles
  UpdateWidgetTransform();
  UpdateScalingHandles();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();
}




//----------------------------------------------------------------------------
// Set the value for the place vector
void albaOpMML3ContourWidget::Place(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  double mvec[3];
  mvec[0] = p2[0] - p1[0];
  mvec[1] = p2[1] - p1[1];
  mvec[2] = 0.0; // no change along z

  // get current slice id and zeta 
  int currentId = m_Model->GetCurrentIdOfScans();
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // evaluate splines
  double ctrans[2];
  ctrans[0] = m_Model->GetPHSpline()->Evaluate(zeta);
  ctrans[1] = m_Model->GetPVSpline()->Evaluate(zeta);

  // horizontal/vertical displacement
  double dx = mvec[0];
  double dy = mvec[1];

  // exceeded max limits?
  if ((ctrans[0] + dx) > m_PH->GetMaxY())
    return;
  if ((ctrans[1] + dy) > m_PV->GetMaxY())
    return;

  // exceeded min limits?
  if ((ctrans[0] + dx) < m_PH->GetMinY())
    return;
  if ((ctrans[1] + dy) < m_PV->GetMinY())
    return;

  // display information vtrans
  m_Model->SetText(1, ctrans[1] + dy, 3, 0); // text actor 1

  // display information htrans
  m_Model->SetText(2, ctrans[0] + dx, 3, 0); // text actor 2

  // update ch spline
  // display values
  m_PH->RemovePoint(currentId);
  m_PH->AddPoint(currentId, ctrans[0] + dx);
  m_PH->Render();

  // actual values
  m_Model->GetPHSpline()->RemovePoint(zeta);
  m_Model->GetPHSpline()->AddPoint(zeta, ctrans[0] + dx);

  // update cv spline
  // display values
  m_PV->RemovePoint(currentId);
  m_PV->AddPoint(currentId, ctrans[1] + dy);
  m_PV->Render();

  // actual values
  m_Model->GetPVSpline()->RemovePoint(zeta);
  m_Model->GetPVSpline()->AddPoint(zeta, ctrans[1] + dy);

  // update widget
  UpdateWidgetTransform();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();

  // update contour axes
  m_Model->UpdateContourAxesTransform();

  // update global axes
  m_Model->UpdateGlobalAxesTransform();
}




//----------------------------------------------------------------------------
// Set the value for the translate vector
void albaOpMML3ContourWidget::Translate(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  double mvec[3];
  mvec[0] = p2[0] - p1[0]; 
  mvec[1] = p2[1] - p1[1]; 
  mvec[2] = 0.0; // no change along z

  // get current slice id and zeta
  double s = m_Model->GetCurrentIdOfScans();
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // get twist
  double twist = m_Model->GetRASpline()->Evaluate(zeta);

  // get h/v translation
  double trans[2];
  trans[0] = m_Model->GetTHSpline()->Evaluate(zeta);
  trans[1] = m_Model->GetTVSpline()->Evaluate(zeta);

  // rotated x and y unit vectors
  double x[3] = {1.0, 0.0, 0.0} ;
  double y[3] = {0.0, 1.0, 0.0} ;
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();
  Transform->RotateZ(twist);
  Transform->TransformPoint(x, x);
  Transform->TransformPoint(y, y);
  Transform->Delete();

  vtkMath* Math = vtkMath::New();
  double h = Math->Dot(mvec, x); // projection on rotated x axis
  double v = Math->Dot(mvec, y); // projection on rotated y axis
  Math->Delete();

  // exceeded max limits?
  if ((trans[0] + h) > m_TH->GetMaxY())
    return;
  if ((trans[1] + v) > m_TV->GetMaxY())
    return;

  // exceeded min limits?
  if ((trans[0] + h) < m_TH->GetMinY())
    return;
  if ((trans[1] + v) < m_TV->GetMinY())
    return;

  // display information vtrans
  m_Model->SetText(1, trans[1] + v, 3, 0); // text actor 1

  // display information htrans
  m_Model->SetText(2, trans[0] + h, 3, 0); // text actor 2

  // update h spline
  // display values
  m_TH->RemovePoint(s);
  m_TH->AddPoint(s, trans[0] + h);
  m_TH->Render();

  // actual values
  m_Model->GetTHSpline()->RemovePoint(zeta);
  m_Model->GetTHSpline()->AddPoint(zeta, trans[0] + h);

  // update v spline
  // display values
  m_TV->RemovePoint(s);
  m_TV->AddPoint(s, trans[1] + v);
  m_TV->Render();

  // actual values
  m_Model->GetTVSpline()->RemovePoint(zeta);
  m_Model->GetTVSpline()->AddPoint(zeta, trans[1] + v);

  // update vertical cuts
  m_Model->UpdateCuttingPlanesTransform();

  // update widget
  UpdateWidgetTransform();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();

  // update contour axes
  m_Model->UpdateContourAxesTransform();

  // update global axes
  m_Model->UpdateGlobalAxesTransform();
}



//----------------------------------------------------------------------------
// Set the rotation value
void albaOpMML3ContourWidget::Rotate(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // motion vector (world space)
  double mvec[3];
  mvec[0] = p2[0] - p1[0];
  mvec[1] = p2[1] - p1[1];
  mvec[2] = p2[2] - p1[2]; 

  // get current slice id and zeta
  double s = m_Model->GetCurrentIdOfScans();
  double zeta = m_Model->GetZetaOfCurrentSlice();

  // get twist
  double twist = m_Model->GetRASpline()->Evaluate(zeta);

  // rotation angle
  double theta = mvec[1]; // angle depends on y displacement 

  // exceeded max limit?
  if ((twist + theta) > m_RA->GetMaxY())
    return;

  // exceeded min limit?
  if ((twist + theta) < m_RA->GetMinY())
    return;

  // display information
  m_Model->SetText(1, twist + theta, 2, 0);

  // update twist spline
  // display values
  m_RA->RemovePoint(s);
  m_RA->AddPoint(s, twist + theta);
  m_RA->Render();

  // actual values
  m_Model->GetRASpline()->RemovePoint(zeta);
  m_Model->GetRASpline()->AddPoint(zeta, twist + theta);

  // update widget
  UpdateWidgetTransform();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();

  // update contour axes
  m_Model->UpdateContourAxesTransform();

  // update global axes
  m_Model->UpdateGlobalAxesTransform();
}




//----------------------------------------------------------------------------
void albaOpMML3ContourWidget::SetRepresentation(int representation)
//----------------------------------------------------------------------------
{
  if (this->m_Representation != (representation<VTK_PLANE_OFF?VTK_PLANE_OFF:(representation>VTK_PLANE_SURFACE?VTK_PLANE_SURFACE:representation))) \
  {
    this->m_Representation = (representation<VTK_PLANE_OFF?VTK_PLANE_OFF:(representation>VTK_PLANE_SURFACE?VTK_PLANE_SURFACE:representation)); \
      this->Modified(); \
  }
}







//----------------------------------------------------------------------------
// print self
void albaOpMML3ContourWidget::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{

  this->UpdatePlacement() ;
  this->UpdateRotationHandle() ;
  this->UpdateScalingHandles() ;
  this->UpdateWidgetTransform() ;

  os << "MML Contour Widget" << std::endl ;
  os << std::endl ;

  os << "Plane Representation: " ;
  if (this->m_Representation == VTK_PLANE_OFF)
    os << "Off\n" ;
  else if ( this->m_Representation == VTK_PLANE_WIREFRAME )
    os << "Wireframe\n";
  else if ( this->m_Representation == VTK_PLANE_SURFACE )
    os << "Surface\n";
  else
    os << m_Representation << " Unknown\n";

  os << "Normal To X Axis: " << (this->m_NormalToXAxis ? "On" : "Off") << "\n";
  os << "Normal To Y Axis: " << (this->m_NormalToYAxis ? "On" : "Off") << "\n";
  os << "Normal To Z Axis: " << (this->m_NormalToZAxis ? "On" : "Off") << "\n";


  // position of plane
  int res = this->m_PlaneSource->GetXResolution();
  double *origin = this->m_PlaneSource->GetOrigin();
  double *pt1 = this->m_PlaneSource->GetPoint1();
  double *pt2 = this->m_PlaneSource->GetPoint2();

  os << "Plane visibility: " << m_PlaneActor->GetVisibility() << "\n";
  os << "Resolution: " << res << "\n";
  os << "Origin: (" << origin[0] << ", " << origin[1] << ", "<< origin[2] << ")\n";
  os << "Point 1: (" << pt1[0] << ", " << pt1[1] << ", " << pt1[2] << ")\n";
  os << "Point 2: (" << pt2[0] << ", " << pt2[1] << ", " << pt2[2] << ")\n";
  os << "\n" ;



  // position of handles
  double *pos ;
  int vis ;

  vis = m_CenterHandleActor->GetVisibility() ;
  pos = m_CenterHandleActor->GetCenter() ;
  os << "center handle: " << "visibility = " << vis << " position: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl ;

  vis = m_RotationalHandleActor->GetVisibility() ;
  pos = m_RotationalHandleActor->GetCenter() ;
  os << "rotation handle: " << "visibility = " << vis << " position: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl ;

  for (int i = 0 ;  i < 4 ;  i++){
    vis = m_HandleActor[i]->GetVisibility() ;
    pos = m_HandleActor[i]->GetCenter() ;
    os << "handle " << i << ": " << "visibility = " << vis  << " position: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl ;
  }
  os << std::endl ;
  os << std::endl ;

}

