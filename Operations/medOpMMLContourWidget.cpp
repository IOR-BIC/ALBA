/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMMLContourWidget.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-24 08:19:32 $
  Version:   $Revision: 1.3 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h"

//#include "MuscleRegistrationProject.h"   ///????????????
#include "medOpMMLContourWidget.h"

#include "vtkActor.h"
#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkCellPicker.h"
#include "vtkConeSource.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkPlanes.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkProperty2D.h"


vtkCxxRevisionMacro(medOpMMLContourWidget, "$Revision: 1.3 $");
vtkStandardNewMacro(medOpMMLContourWidget);
vtkCxxSetObjectMacro(medOpMMLContourWidget, m_PlaneProperty, vtkProperty);

//----------------------------------------------------------------------------
medOpMMLContourWidget::medOpMMLContourWidget() 
: vtkPolyDataSourceWidget()
//----------------------------------------------------------------------------
{
  this->m_OperationID = 0; // first operation id is 0
  this->m_BCenterMode = FALSE; // center mode off
  this->m_BScalingMode = FALSE; // scaling mode off
  this->m_BRotatingMode = FALSE; // rotating mode off
  this->m_BTranslatingMode = FALSE; // translating mode off
  this->m_State = medOpMMLContourWidget::Start;
  this->EventCallbackCommand->SetCallback(medOpMMLContourWidget::ProcessEvents);
  
  this->m_NormalToXAxis = 0;
  this->m_NormalToYAxis = 0;
  this->m_NormalToZAxis = 0;
  this->m_Representation = VTK_PLANE_WIREFRAME;
 
  //
  this->SetHandleRadius(1.5);


  //Build the representation of the widget
  int i;
  // Represent the plane
  this->m_PlaneSource = vtkPlaneSource::New();
  this->m_PlaneSource->SetXResolution(4);
  this->m_PlaneSource->SetYResolution(4);
  this->m_PlaneOutline = vtkPolyData::New();
  this->m_PlaneOutlineTubes = vtkTubeFilter::New(); // MK
  this->m_PlaneOutlineTubes->SetInput(this->m_PlaneOutline); // Mk
  
  vtkPoints *pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  vtkCellArray *outline = vtkCellArray::New();
  outline->InsertNextCell(4);
  outline->InsertCellPoint(0);
  outline->InsertCellPoint(1);
  outline->InsertCellPoint(2);
  outline->InsertCellPoint(3);
  this->m_PlaneOutline->SetPoints(pts);
  pts->Delete();
  this->m_PlaneOutline->SetPolys(outline);
  outline->Delete();
  
  this->m_PlaneMapper = vtkPolyDataMapper::New();
  this->m_PlaneMapper->SetInput(this->m_PlaneSource->GetOutput());
  this->m_PlaneActor = vtkActor::New();
  this->m_PlaneActor->SetMapper(this->m_PlaneMapper);

  // Create the handles
  this->m_Handle = new vtkActor* [4];
  this->m_HandleMapper = new vtkPolyDataMapper* [4];
  this->m_HandleGeometry = new vtkSphereSource* [4];
  for (i=0; i<4; i++)
    {
    this->m_HandleGeometry[i] = vtkSphereSource::New();
    this->m_HandleGeometry[i]->SetThetaResolution(16);
    this->m_HandleGeometry[i]->SetPhiResolution(8);
    this->m_HandleMapper[i] = vtkPolyDataMapper::New();
    this->m_HandleMapper[i]->SetInput(this->m_HandleGeometry[i]->GetOutput());
    this->m_Handle[i] = vtkActor::New();
    this->m_Handle[i]->SetMapper(this->m_HandleMapper[i]);
    }

  // handles off
  for (i=0; i<4; i++)
  {
    this->m_Handle[i]->VisibilityOff();
  }

  // rotational handle
  this->m_RotationalHandle = vtkActor::New();
  this->m_RotationalHandleMapper = vtkPolyDataMapper::New();
  this->m_RotationalHandleGeometry = vtkSphereSource::New();
  this->m_RotationalHandleGeometry->SetThetaResolution(16);
  this->m_RotationalHandleGeometry->SetPhiResolution(8);
  this->m_RotationalHandleMapper->SetInput(this->m_RotationalHandleGeometry->GetOutput());
  this->m_RotationalHandle = vtkActor::New();
  this->m_RotationalHandle->SetMapper(this->m_RotationalHandleMapper);

  // rotational handle off
  this->m_RotationalHandle->VisibilityOff();

  // center handle
  this->m_CenterHandle = vtkActor::New();
  this->m_CenterHandleMapper = vtkPolyDataMapper::New();
  this->m_CenterHandleGeometry = vtkSphereSource::New();
  this->m_CenterHandleGeometry->SetThetaResolution(16);
  this->m_CenterHandleGeometry->SetPhiResolution(8);
  this->m_CenterHandleMapper->SetInput(this->m_CenterHandleGeometry->GetOutput());
  this->m_CenterHandle = vtkActor::New();
  this->m_CenterHandle->SetMapper(this->m_CenterHandleMapper);

  // center handle off
  this->m_CenterHandle->VisibilityOff();
   
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

  this->m_Transform = vtkTransform::New();

  // Define the point coordinates
  double bounds[6];
  bounds[0] = -0.5;
  bounds[1] = 0.5;
  bounds[2] = -0.5;
  bounds[3] = 0.5;
  bounds[4] = -0.5;
  bounds[5] = 0.5;

  // Initial creation of the widget, serves to initialize it
  this->PlaceWidget(bounds);

  //Manage the picking stuff
  this->m_HandlePicker = vtkCellPicker::New();
  this->m_HandlePicker->SetTolerance(0.001);
  for (i=0; i<4; i++)
    {
    this->m_HandlePicker->AddPickList(this->m_Handle[i]);
    }
  this->m_HandlePicker->PickFromListOn();

  this->m_PlanePicker = vtkCellPicker::New();
  this->m_PlanePicker->SetTolerance(0.005); //need some fluff
  this->m_PlanePicker->AddPickList(this->m_PlaneActor);
  this->m_PlanePicker->AddPickList(this->m_RotationalHandle);
  this->m_PlanePicker->AddPickList(this->m_CenterHandle);
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
medOpMMLContourWidget::~medOpMMLContourWidget()
//----------------------------------------------------------------------------
{
  this->m_PlaneActor->Delete();
  this->m_PlaneMapper->Delete();
  this->m_PlaneSource->Delete();
  this->m_PlaneOutline->Delete();

  for (int i=0; i<4; i++)
    {
    this->m_HandleGeometry[i]->Delete();
    this->m_HandleMapper[i]->Delete();
    this->m_Handle[i]->Delete();
    }
  delete [] this->m_Handle;
  delete [] this->m_HandleMapper;
  delete [] this->m_HandleGeometry;
  
  this->m_ConeActor->Delete();
  this->m_ConeMapper->Delete();
  this->m_ConeSource->Delete();

  this->m_LineActor->Delete();
  this->m_LineMapper->Delete();
  this->m_LineSource->Delete();

  this->m_ConeActor2->Delete();
  this->m_ConeMapper2->Delete();
  this->m_ConeSource2->Delete();

  this->m_LineActor2->Delete();
  this->m_LineMapper2->Delete();
  this->m_LineSource2->Delete();

  this->m_HandlePicker->Delete();
  this->m_PlanePicker->Delete();

  this->m_HandleProperty->Delete();
  this->m_SelectedHandleProperty->Delete();
  this->m_PlaneProperty->Delete();
  this->m_SelectedPlaneProperty->Delete();
  
  this->m_Transform->Delete();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetEnabled(int enabling)
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
      this->CurrentRenderer = this->Interactor->FindPokedRenderer(
        this->Interactor->GetLastEventPosition()[0],
        this->Interactor->GetLastEventPosition()[1]);
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
      this->CurrentRenderer->AddActor(this->m_Handle[j]);
      //this->Handle[j]->SetProperty(this->HandleProperty);
      }

	// colours
	this->m_Handle[0]->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	this->m_Handle[3]->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	this->m_Handle[2]->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
	this->m_Handle[1]->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue

	this->CurrentRenderer->AddActor(this->m_RotationalHandle);
	this->m_RotationalHandle->SetProperty(this->m_HandleProperty);

	this->CurrentRenderer->AddActor(this->m_CenterHandle);
	this->m_CenterHandle->SetProperty(this->m_HandleProperty);

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
      this->CurrentRenderer->RemoveActor(this->m_Handle[i]);
      }

	this->CurrentRenderer->RemoveActor(this->m_RotationalHandle);
	this->CurrentRenderer->RemoveActor(this->m_CenterHandle);
    // turn off the normal vector
   /* this->CurrentRenderer->RemoveActor(this->LineActor);
    this->CurrentRenderer->RemoveActor(this->ConeActor);
    this->CurrentRenderer->RemoveActor(this->LineActor2);
    this->CurrentRenderer->RemoveActor(this->ConeActor2);*/

    this->m_CurrentHandle = NULL;
    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->CurrentRenderer = NULL;
    }

  this->Interactor->Render();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                   unsigned long event,
                                   void* clientdata, 
                                   void* vtkNotUsed(calldata))
//----------------------------------------------------------------------------
{
  medOpMMLContourWidget* self = reinterpret_cast<medOpMMLContourWidget *>( clientdata );

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
void medOpMMLContourWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->m_HandleProperty )
    {
    os << indent << "Handle Property: " << this->m_HandleProperty << "\n";
    }
  else
    {
    os << indent << "Handle Property: (none)\n";
    }
  if ( this->m_SelectedHandleProperty )
    {
    os << indent << "Selected Handle Property: " 
       << this->m_SelectedHandleProperty << "\n";
    }
  else
    {
    os << indent << "SelectedHandle Property: (none)\n";
    }

  if ( this->m_PlaneProperty )
    {
    os << indent << "Plane Property: " << this->m_PlaneProperty << "\n";
    }
  else
    {
    os << indent << "Plane Property: (none)\n";
    }
  if ( this->m_SelectedPlaneProperty )
    {
    os << indent << "Selected Plane Property: " 
       << this->m_SelectedPlaneProperty << "\n";
    }
  else
    {
    os << indent << "Selected Plane Property: (none)\n";
    }

  os << indent << "Plane Representation: ";
  if ( this->m_Representation == VTK_PLANE_WIREFRAME )
    {
    os << "Wireframe\n";
    }
  else if ( this->m_Representation == VTK_PLANE_SURFACE )
    {
    os << "Surface\n";
    }
  else //( this->Representation == VTK_PLANE_OUTLINE )
    {
    os << "Outline\n";
    }

  os << indent << "Normal To X Axis: " 
     << (this->m_NormalToXAxis ? "On" : "Off") << "\n";
  os << indent << "Normal To Y Axis: " 
     << (this->m_NormalToYAxis ? "On" : "Off") << "\n";
  os << indent << "Normal To Z Axis: " 
     << (this->m_NormalToZAxis ? "On" : "Off") << "\n";

  int res = this->m_PlaneSource->GetXResolution();
  double *o = this->m_PlaneSource->GetOrigin();
  double *pt1 = this->m_PlaneSource->GetPoint1();
  double *pt2 = this->m_PlaneSource->GetPoint2();

  os << indent << "Resolution: " << res << "\n";
  os << indent << "Origin: (" << o[0] << ", "
     << o[1] << ", "
     << o[2] << ")\n";
  os << indent << "Point 1: (" << pt1[0] << ", "
     << pt1[1] << ", "
     << pt1[2] << ")\n";
  os << indent << "Point 2: (" << pt2[0] << ", "
     << pt2[1] << ", "
     << pt2[2] << ")\n";
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::PositionHandles(float X, float Y)
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
int medOpMMLContourWidget::HighlightHandle(vtkProp *prop)
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
      if ( this->m_CurrentHandle == this->m_Handle[i] )
        {
        return i;
        }
      }
    }
  
  return -1;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::HighlightNormal(int highlight)
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
void medOpMMLContourWidget::HighlightPlane(int highlight)
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
void medOpMMLContourWidget::OnLeftButtonDown()
//----------------------------------------------------------------------------
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
    {
    this->m_State = medOpMMLContourWidget::Outside;
    return;
    }
  
  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then try to pick the plane.
  vtkAssemblyPath *path;
  this->m_HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->m_HandlePicker->GetPath();
  if ( path != NULL )
    {
    this->m_State = medOpMMLContourWidget::Moving;
    this->HighlightHandle(path->GetFirstNode()->GetProp());

	if (GetScalingMode())
	{
			if (this->m_CurrentHandle)
			{
				if (this->m_CurrentHandle == this->m_Handle[0])
				{
					// begin s-s op
					this->m_Operation = medOpMMLContourWidget::SouthScale;
				}
				else
				if (this->m_CurrentHandle == this->m_Handle[1])
				{
					// begin s-e op
					this->m_Operation = medOpMMLContourWidget::EastScale;
				}
				else
				if (this->m_CurrentHandle == this->m_Handle[2])
				{
					// begin s-w op
					this->m_Operation = medOpMMLContourWidget::WestScale;
				}
				else
				if (this->m_CurrentHandle == this->m_Handle[3])
				{
					// begin s-n op
					this->m_Operation = medOpMMLContourWidget::NorthScale;
				}
			}
	}

    }
  else
    {
    this->m_PlanePicker->Pick(X,Y,0.0,this->CurrentRenderer);
    path = this->m_PlanePicker->GetPath();
    if ( path != NULL )
      {
      vtkProp *prop = path->GetFirstNode()->GetProp();
      if (/* prop == this->ConeActor || prop == this->LineActor ||
           prop == this->ConeActor2 || prop == this->LineActor2*/
		   prop == this->m_RotationalHandle)
        {
        this->m_State = medOpMMLContourWidget::Rotating;
        this->HighlightNormal(1);

		// begin r-op
		this->m_Operation = medOpMMLContourWidget::Rotation;
        }
     /* else
	  if (prop == this->CenterHandle)
        {
        this->State = medOpMMLContourWidget::Centralising;
        this->HighlightNormal(1);
        }*/
	  else
        {
        this->m_State = medOpMMLContourWidget::Moving;
        this->HighlightPlane(1);

		if (GetTranslationMode())
		{
			// begin t-op
			this->m_Operation = medOpMMLContourWidget::Translation;
		}
		else
		if (GetCenterMode())
		{
			// begin p-op
			this->m_Operation = medOpMMLContourWidget::Placement;
		}
       }
      }
    else
      {
      this->m_State = medOpMMLContourWidget::Outside;
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
void medOpMMLContourWidget::OnLeftButtonUp()
//----------------------------------------------------------------------------
{
  if ( this->m_State == medOpMMLContourWidget::Outside ||
       this->m_State == medOpMMLContourWidget::Start )
    {
    return;
    }

  // 1. switch off display information
  m_M->GetScaledTextActor1()->VisibilityOff();
  m_M->GetScaledTextActor2()->VisibilityOff();

  // 2. save undo parameters
  double params[5];
  //float flag;

  // slice id
  params[0] = m_M->GetCurrentIdOfSyntheticScans();

  // z level
  params[1] = m_M->GetCurrentZOfSyntheticScans();

  // operation type
  params[2] = m_Operation;

  switch (m_Operation)
  {
    case medOpMMLContourWidget::Placement : //
		                params[3] = m_M->GetPHSpline()->Evaluate(params[1]);
						params[4] = m_M->GetPVSpline()->Evaluate(params[1]);
	break;

	case medOpMMLContourWidget::Translation : //
						params[3] = m_M->GetTHSpline()->Evaluate(params[1]);
						params[4] = m_M->GetTVSpline()->Evaluate(params[1]);
	break;

	case medOpMMLContourWidget::Rotation : //
						params[3] = m_M->GetRASpline()->Evaluate(params[1]);
						params[4] = 0.0;
	break;

	case medOpMMLContourWidget::NorthScale : //
						params[3] = m_M->GetSNSpline()->Evaluate(params[1]);
						params[4] = 0.0;
						/*M->ScalingFlagStack->GetTuple(params[0], &flag);
						if (flag == 0.0)
							flag = 1.0;
						M->ScalingFlagStack->SetTuple(params[0], &flag);*/
						if (!m_M->m_ScalingOccured)
						{
							m_M->m_ScalingOccured = TRUE;
							m_M->m_ScalingOccuredOperationId = GetNextOperationId();

							// grey out twist, h/v translation views

							// redraw curves
							m_CH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_CH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_CH->Render();

							m_CV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_CV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_CV->Render();

							m_T->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_T->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_T->Render();

							m_H->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_H->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_H->Render();

							m_V->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_V->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_V->Render();
						}

	break;

	case medOpMMLContourWidget::SouthScale : //
						params[3] = m_M->GetSSSpline()->Evaluate(params[1]);
						params[4] = 0.0;
						/*M->ScalingFlagStack->GetTuple(params[0], &flag);
						if (flag == 0.0)
							flag = 1.0;
						M->ScalingFlagStack->SetTuple(params[0], &flag);*/
						if (!m_M->m_ScalingOccured)
						{
							m_M->m_ScalingOccured = TRUE;
							m_M->m_ScalingOccuredOperationId = GetNextOperationId();

							// grey out twist, h/v translation views

							// redraw curves
							m_CH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_CH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_CH->Render();

							m_CV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_CV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_CV->Render();

							m_T->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_T->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_T->Render();

							m_H->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_H->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_H->Render();

							m_V->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_V->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_V->Render();
						}
	break;

	case medOpMMLContourWidget::EastScale : //
						params[3] = m_M->GetSESpline()->Evaluate(params[1]);
						params[4] = 0.0;
						/*M->ScalingFlagStack->GetTuple(params[0], &flag);
						if (flag == 0.0)
							flag = 1.0;
						M->ScalingFlagStack->SetTuple(params[0], &flag);*/
						if (!m_M->m_ScalingOccured)
						{
							m_M->m_ScalingOccured = TRUE;
							m_M->m_ScalingOccuredOperationId = GetNextOperationId();

							// grey out twist, h/v translation views

							// redraw curves
							m_CH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_CH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_CH->Render();

							m_CV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_CV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_CV->Render();

							m_T->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_T->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_T->Render();

							m_H->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_H->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_H->Render();

							m_V->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_V->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_V->Render();
						}
	break;

	case medOpMMLContourWidget::WestScale : //
						params[3] = m_M->GetSWSpline()->Evaluate(params[1]);
						params[4] = 0.0;
						/*M->ScalingFlagStack->GetTuple(params[0], &flag);
						if (flag == 0.0)
							flag = 1.0;
						M->ScalingFlagStack->SetTuple(params[0], &flag);*/
						if (!m_M->m_ScalingOccured)
						{
							m_M->m_ScalingOccured = TRUE;
							m_M->m_ScalingOccuredOperationId = GetNextOperationId();

							// grey out twist, h/v translation views

							// redraw curves
							m_CH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_CH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_CH->Render();

							m_CV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_CV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_CV->Render();

							m_T->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_T->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_T->Render();

							m_H->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_H->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_H->Render();

							m_V->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							m_V->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							m_V->Render();
						}
	break;

  }

  // exceeded max ?
  assert(GetNextOperationId() < 2000);

  // save
  m_M->m_OperationsStack->SetTuple(GetNextOperationId(), params);

  // new next op
  SetNextOperationId(GetNextOperationId() + 1);

  this->m_State = medOpMMLContourWidget::Start;
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
void medOpMMLContourWidget::OnMiddleButtonDown()
//----------------------------------------------------------------------------
{
  /*
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
    {
    this->State = medOpMMLContourWidget::Outside;
    return;
    }
  
  // Okay, we can process this. If anything is picked, then we
  // can start pushing the plane.
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
    {
    this->State = medOpMMLContourWidget::Pushing;
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
      this->State = medOpMMLContourWidget::Outside;
      return;
      }
    else
      {
      this->State = medOpMMLContourWidget::Pushing;
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
void medOpMMLContourWidget::OnMiddleButtonUp()
//----------------------------------------------------------------------------
{
  /*
  if ( this->State == medOpMMLContourWidget::Outside ||
       this->State == medOpMMLContourWidget::Start )
    {
    return;
    }

  this->State = medOpMMLContourWidget::Start;
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
void medOpMMLContourWidget::OnRightButtonDown()
//----------------------------------------------------------------------------
{
  /*
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
    {
    this->State = medOpMMLContourWidget::Outside;
    return;
    }
  
  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then pick the bounding box.
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
    {
    this->State = medOpMMLContourWidget::Scaling;
    this->HighlightPlane(1);
    this->HighlightHandle(path->GetFirstNode()->GetProp());
    }
  else //see if we picked the plane or a normal
    {
    this->PlanePicker->Pick(X,Y,0.0,this->CurrentRenderer);
    path = this->PlanePicker->GetPath();
    if ( path == NULL )
      {
      this->State = medOpMMLContourWidget::Outside;
      return;
      }
    else
      {
      this->State = medOpMMLContourWidget::Scaling;
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
void medOpMMLContourWidget::OnRightButtonUp()
//----------------------------------------------------------------------------
{
  /*
  if ( this->State == medOpMMLContourWidget::Outside ||
       this->State == medOpMMLContourWidget::Start )
    {
    return;
    }

  this->State = medOpMMLContourWidget::Start;
  this->HighlightPlane(0);
  this->SizeHandles();
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  this->Interactor->Render();
  */
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::OnMouseMove()
//----------------------------------------------------------------------------
{
  // See whether we're active
  if ( this->m_State == medOpMMLContourWidget::Outside || 
       this->m_State == medOpMMLContourWidget::Start )
    {
    return;
    }
  
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Do different things depending on state
  // Calculations everybody does
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z;

  vtkRenderer *renderer = this->Interactor->FindPokedRenderer(X,Y);
  vtkCamera *camera = renderer->GetActiveCamera();
  if ( !camera )
    {
    return;
    }

  // Compute the two points defining the motion vector
  this->ComputeWorldToDisplay(this->LastPickPosition[0], this->LastPickPosition[1],
                              this->LastPickPosition[2], focalPoint);
  z = focalPoint[2];
  this->ComputeDisplayToWorld(double(this->Interactor->GetLastEventPosition()[0]),
                              double(this->Interactor->GetLastEventPosition()[1]),
                              z, prevPickPoint);
  this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);

  // Process the motion
  if ( this->m_State == medOpMMLContourWidget::Moving )
  {
   // interaction operations
   // using a contour widget
   if (GetScalingMode()) // scaling
   {
    if (this->m_CurrentHandle)
	{
	 // display information on.
	 // off in OnLeftButtonUp()
	 m_M->GetScaledTextActor1()->VisibilityOn();

     if (this->m_CurrentHandle == this->m_Handle[0]) // south scaling
	 {
	  
      this->ScaleSouth(prevPickPoint, pickPoint);
	 }
     else
	 if (this->m_CurrentHandle == this->m_Handle[1]) // east scaling
	 {
      this->ScaleEast(prevPickPoint, pickPoint);
	 }
     else
	 if (this->m_CurrentHandle == this->m_Handle[2]) // west scaling
	 {
      this->ScaleWest(prevPickPoint, pickPoint);
	 }
     else
     if (this->m_CurrentHandle == this->m_Handle[3]) // north scaling
	 {
      this->ScaleNorth(prevPickPoint, pickPoint);
	 }
	}
   }
   else
   if (GetTranslationMode()) // translation
   {
	// display information on.
	// off in OnLeftButtonUp()
	m_M->GetScaledTextActor1()->VisibilityOn(); 
	m_M->GetScaledTextActor2()->VisibilityOn();
	this->Translate(prevPickPoint, pickPoint);
   }
   else
   if (GetCenterMode()) // place
   {
	// display information on.
	// off in OnLeftButtonUp()
	m_M->GetScaledTextActor1()->VisibilityOn(); 
	m_M->GetScaledTextActor2()->VisibilityOn();

	this->Place(prevPickPoint, pickPoint);
   }
  }
  else if ( this->m_State == medOpMMLContourWidget::Scaling )
  {
   this->Scale(prevPickPoint, pickPoint, X, Y);
  }
  else if ( this->m_State == medOpMMLContourWidget::Pushing )
  {
   this->Push(prevPickPoint, pickPoint);
  }
  else if ( this->m_State == medOpMMLContourWidget::Rotating ) // rotation
  {
   // display information on.
   // off in OnLeftButtonUp()
   m_M->GetScaledTextActor1()->VisibilityOn();
   this->Rotate(prevPickPoint, pickPoint);
  }

  // Interact, if desired
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent,NULL);
  
  this->Interactor->Render();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::Scale(double *p1, double *p2, int vtkNotUsed(X), int Y)
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
void medOpMMLContourWidget::Push(double *p1, double *p2)
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
void medOpMMLContourWidget::CreateDefaultProperties()
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
void medOpMMLContourWidget::PlaceWidget(double bds[6])
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
void medOpMMLContourWidget::SizeHandles()
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
void medOpMMLContourWidget::SelectRepresentation()
//----------------------------------------------------------------------------
{
  if ( ! this->CurrentRenderer )
    {
    return;
    }

  if ( this->m_Representation == VTK_PLANE_OFF )
    {
    this->CurrentRenderer->RemoveActor(this->m_PlaneActor);
    }
  else if ( this->m_Representation == VTK_PLANE_OUTLINE )
    {
    this->CurrentRenderer->RemoveActor(this->m_PlaneActor);
    this->CurrentRenderer->AddActor(this->m_PlaneActor);
	
	//this->PlaneOutlineTubes->SetRadius(3.0);
    //this->PlaneOutlineTubes->SetNumberOfSides(6);
	//this->PlaneMapper->SetInput( this->PlaneOutlineTubes->GetOutput());

    this->m_PlaneMapper->SetInput( this->m_PlaneOutline);
	this->m_PlaneActor->GetProperty()->SetRepresentationToWireframe();
    }
  else if ( this->m_Representation == VTK_PLANE_SURFACE )
    {
    this->CurrentRenderer->RemoveActor(this->m_PlaneActor);
    this->CurrentRenderer->AddActor(this->m_PlaneActor);
    this->m_PlaneMapper->SetInput( this->m_PlaneSource->GetOutput() );
    this->m_PlaneActor->GetProperty()->SetRepresentationToSurface();
    }
  else //( this->Representation == VTK_PLANE_WIREFRAME )
    {
    this->CurrentRenderer->RemoveActor(this->m_PlaneActor);
    this->CurrentRenderer->AddActor(this->m_PlaneActor);
    this->m_PlaneMapper->SetInput( this->m_PlaneSource->GetOutput() );
    this->m_PlaneActor->GetProperty()->SetRepresentationToWireframe();
    }
}
//----------------------------------------------------------------------------
/** Description:
 Set/Get the resolution (number of subdivisions) of the plane. */
void medOpMMLContourWidget::SetResolution(int r)
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetXResolution(r); 
  this->m_PlaneSource->SetYResolution(r); 
}
//----------------------------------------------------------------------------
int medOpMMLContourWidget::GetResolution()
//----------------------------------------------------------------------------
{ 
  return this->m_PlaneSource->GetXResolution(); 
}
//----------------------------------------------------------------------------
// Description:
// Set/Get the origin of the plane.
void medOpMMLContourWidget::SetOrigin(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetOrigin(x,y,z);
  this->PositionHandles(0.5, 0.5);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetOrigin(double x[3]) 
//----------------------------------------------------------------------------
{
  this->SetOrigin(x[0], x[1], x[2]);
}
//----------------------------------------------------------------------------
double* medOpMMLContourWidget::GetOrigin() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetOrigin();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetOrigin(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetOrigin(xyz);
}
//----------------------------------------------------------------------------
// Description:
// Set/Get the position of the point defining the first axis of the plane.
void medOpMMLContourWidget::SetPoint1(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetPoint1(x,y,z);
  //this->PositionHandles(0.5, 0.5);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetPoint1(double x[3]) 
//----------------------------------------------------------------------------
{
  this->SetPoint1(x[0], x[1], x[2]);
}
//----------------------------------------------------------------------------
double* medOpMMLContourWidget::GetPoint1() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetPoint1();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetPoint1(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetPoint1(xyz);
}
//----------------------------------------------------------------------------
// Description:
// Set/Get the position of the point defining the second axis of the plane.
void medOpMMLContourWidget::SetPoint2(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetPoint2(x,y,z);
  //this->PositionHandles(0.5, 0.5);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetPoint2(double x[3]) 
//----------------------------------------------------------------------------
{
  this->SetPoint2(x[0], x[1], x[2]);
}
//----------------------------------------------------------------------------
double* medOpMMLContourWidget::GetPoint2() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetPoint2();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetPoint2(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetPoint2(xyz);
}
//----------------------------------------------------------------------------
// Description:
// Set the center of the plane.
void medOpMMLContourWidget::SetCenter(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetCenter(x, y, z);
  this->PositionHandles(0.5, 0.5);
}
//----------------------------------------------------------------------------
// Description:
// Set the center of the plane.
void medOpMMLContourWidget::SetCenter(double c[3]) 
//----------------------------------------------------------------------------
{
  this->SetCenter(c[0], c[1], c[2]);
}
//----------------------------------------------------------------------------
// Description:
// Get the center of the plane.
double* medOpMMLContourWidget::GetCenter() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetCenter();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetCenter(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetCenter(xyz);
}
//----------------------------------------------------------------------------
// Description:
// Set the normal to the plane.
void medOpMMLContourWidget::SetNormal(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->SetNormal(x, y, z);
  this->PositionHandles(0.5, 0.5);
}
//----------------------------------------------------------------------------
// Description:
// Set the normal to the plane.
void medOpMMLContourWidget::SetNormal(double n[3]) 
//----------------------------------------------------------------------------
{
  this->SetNormal(n[0], n[1], n[2]);
}
//----------------------------------------------------------------------------
// Description:
// Get the normal to the plane.
double* medOpMMLContourWidget::GetNormal() 
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource->GetNormal();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetNormal(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->GetNormal(xyz);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetPolyData(vtkPolyData *pd)
//----------------------------------------------------------------------------
{ 
  pd->ShallowCopy(this->m_PlaneSource->GetOutput()); 
}
//----------------------------------------------------------------------------
vtkPolyDataSource *medOpMMLContourWidget::GetPolyDataSource()
//----------------------------------------------------------------------------
{
  return this->m_PlaneSource;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetPlane(vtkPlane *plane)
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
void medOpMMLContourWidget::UpdatePlacement(void)
//----------------------------------------------------------------------------
{
  this->m_PlaneSource->Update();
  this->PositionHandles(0.5, 0.5);
}
//----------------------------------------------------------------------------
// MK BEGIN 26/05/04
// Description:
// Set the motion vector
void medOpMMLContourWidget::SetMotionVector(float x, float y, float z) 
//----------------------------------------------------------------------------
{
  this->m_Motion[0] = x;
  this->m_Motion[1] = y;
  this->m_Motion[2] = z;
}
//----------------------------------------------------------------------------
// Description:
// Set the motion vector.
void medOpMMLContourWidget::SetMotionVector(float m[3]) 
//----------------------------------------------------------------------------
{
  this->m_Motion[0] = m[0];
  this->m_Motion[1] = m[1];
  this->m_Motion[2] = m[2];
}
//----------------------------------------------------------------------------
// Description:
// Get the motion vector
float* medOpMMLContourWidget::GetMotionVector() 
//----------------------------------------------------------------------------
{
  return this->m_Motion;
}
//----------------------------------------------------------------------------
// Description:
// Get the motion vector
void medOpMMLContourWidget::GetMotionVector(float xyz[3]) 
//----------------------------------------------------------------------------
{
  xyz[0] = this->m_Motion[0];
  xyz[1] = this->m_Motion[1];
  xyz[2] = this->m_Motion[2];
}
// MK END
//----------------------------------------------------------------------------
void medOpMMLContourWidget::TranslationModeOn()
//----------------------------------------------------------------------------
{
  this->m_BTranslatingMode = TRUE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::TranslationModeOff()
//----------------------------------------------------------------------------
{
  this->m_BTranslatingMode = FALSE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::CenterModeOn()
//----------------------------------------------------------------------------
{
  //this->CenterHandle->VisibilityOn();
  this->m_BCenterMode = TRUE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::CenterModeOff()
//----------------------------------------------------------------------------
{
  //this->CenterHandle->VisibilityOff();
  this->m_BCenterMode = FALSE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::RotationModeOn()
//----------------------------------------------------------------------------
{
  //this->m_RotationalHandle->VisibilityOn();
  //this->Setm_RotationalHandle(50.0, 0.0, 0.0);
  this->m_BRotatingMode = TRUE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::RotationModeOff()
//----------------------------------------------------------------------------
{
  //this->m_RotationalHandle->VisibilityOff();
  this->m_BRotatingMode = FALSE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScalingModeOn()
//----------------------------------------------------------------------------
{
  this->m_BScalingMode = TRUE;
  //this->ScalingHandlesOn();

  // 
  //this->SetRepresentationToWireframe();
  //this->GetPlaneProperty()->SetOpacity(1.0);
  //this->GetSelectedPlaneProperty()->SetOpacity(1.0);

  //
  //this->PlanePicker->DeletePickList(this->PlaneActor);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScalingModeOff()
//----------------------------------------------------------------------------
{
  this->m_BScalingMode = FALSE;

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
void medOpMMLContourWidget::SetHandle0(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[0]->SetCenter(xyz);
  this->m_HandleGeometry[0]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle0(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[0]->SetCenter(x, y, z);
  this->m_HandleGeometry[0]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle1(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[1]->SetCenter(xyz);
  this->m_HandleGeometry[1]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle1(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[1]->SetCenter(x, y, z);
  this->m_HandleGeometry[1]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle2(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[2]->SetCenter(xyz);
  this->m_HandleGeometry[2]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle2(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[2]->SetCenter(x, y, z);
  this->m_HandleGeometry[2]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle3(double *xyz)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[3]->SetCenter(xyz);
  this->m_HandleGeometry[3]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle3(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->m_HandleGeometry[3]->SetCenter(x, y, z);
  this->m_HandleGeometry[3]->Modified();
}
//----------------------------------------------------------------------------
BOOL medOpMMLContourWidget::GetCenterMode()
//----------------------------------------------------------------------------
{
	return m_BCenterMode;
}
//----------------------------------------------------------------------------
BOOL medOpMMLContourWidget::GetTranslationMode()
//----------------------------------------------------------------------------
{
	return m_BTranslatingMode;
}
//----------------------------------------------------------------------------
BOOL medOpMMLContourWidget::GetRotationMode()
//----------------------------------------------------------------------------
{
	return m_BRotatingMode;
}
//----------------------------------------------------------------------------
BOOL medOpMMLContourWidget::GetScalingMode()
//----------------------------------------------------------------------------
{
	return m_BScalingMode;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetRotationalHandle(double x, double y, double z)
//----------------------------------------------------------------------------
{
	this->m_RotationalHandleGeometry->SetCenter(x, y, z);
	this->m_RotationalHandleGeometry->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetRotationalHandle(double *xyz)
//----------------------------------------------------------------------------
{
	this->m_RotationalHandleGeometry->SetCenter(xyz);
	this->m_RotationalHandleGeometry->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScalingHandlesOn()
//----------------------------------------------------------------------------
{
  for(int i=0; i<4; i++)
  {
    this->m_Handle[i]->VisibilityOn();
  }
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScalingHandlesOff()
//----------------------------------------------------------------------------
{
  for(int i=0; i<4; i++)
  {
    this->m_Handle[i]->VisibilityOff();
  }
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetCenterHandle(double x, double y, double z)
//----------------------------------------------------------------------------
{
	this->m_CenterHandleGeometry->SetCenter(x, y, z);
	this->m_CenterHandleGeometry->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetCenterHandle(double *xyz)
//----------------------------------------------------------------------------
{
	this->m_CenterHandleGeometry->SetCenter(xyz);
	this->m_CenterHandleGeometry->Modified();
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetCenterHandleActor()
//----------------------------------------------------------------------------
{
	return m_CenterHandle;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetRotationalHandleActor()
//----------------------------------------------------------------------------
{
	return m_RotationalHandle;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetNorthScalingHandleActor()
//----------------------------------------------------------------------------
{
	return m_Handle[3];
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetSouthScalingHandleActor()
//----------------------------------------------------------------------------
{
	return m_Handle[0];
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetEastScalingHandleActor()
//----------------------------------------------------------------------------
{
	return m_Handle[1];
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetWestScalingHandleActor()
//----------------------------------------------------------------------------
{
	return m_Handle[2];
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandleRadius(float r)
//----------------------------------------------------------------------------
{
	m_HandleRadius = r;
}
//----------------------------------------------------------------------------
float medOpMMLContourWidget::GetHandleRadius()
//----------------------------------------------------------------------------
{
	return m_HandleRadius;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetRotationHandleVisibility()
//----------------------------------------------------------------------------
{/*
	// get z level
	double z = M->GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	M->GetContourActor()->GetBounds(bounds);

	// set up 2d contour axes first
	double width = M->GetContourAxesLengthScale() * (bounds[1] - bounds[0]) / 2.0;
	double height = M->GetContourAxesLengthScale() * (bounds[3] - bounds[2]) / 2.0;
	
	if (width > 0.0 && height > 0.0)
	{
		// visible
		Getm_RotationalHandleActor()->VisibilityOn();	
	}
	else
	{
		// invisible
		Getm_RotationalHandleActor()->VisibilityOff();
	}*/
	GetRotationalHandleActor()->VisibilityOn();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::RotationHandleOn()
//----------------------------------------------------------------------------
{
	GetRotationalHandleActor()->VisibilityOn();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::RotationHandleOff()
//----------------------------------------------------------------------------
{
	GetRotationalHandleActor()->VisibilityOff();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ComputeBounds(double *b)
//----------------------------------------------------------------------------
{
  m_M->GetContourActor()->GetBounds(b);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ComputeCenter(double *c)
//----------------------------------------------------------------------------
{
  double b[6];

  ComputeBounds(b);

  c[0] = (b[0] + b[1]) / 2.0; // x
  c[1] = (b[2] + b[3]) / 2.0; // y
  c[2] = (b[4] + b[5]) / 2.0; // z
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::UpdateScalingHandles()
//----------------------------------------------------------------------------
{
	// get z level
	double z = m_M->GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	m_M->GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = m_M->GetPHSpline()->Evaluate(z);
	ctrans[1] = m_M->GetPVSpline()->Evaluate(z);
	
	// get twist
	double twist = m_M->GetRASpline()->Evaluate(z);

	// get h/v translation
	double trans[2];
	trans[0] = m_M->GetTHSpline()->Evaluate(z);
	trans[1] = m_M->GetTVSpline()->Evaluate(z);

	// get scaling
	double scale[4];
	scale[0] = m_M->GetSESpline()->Evaluate(z); // east
	scale[1] = m_M->GetSWSpline()->Evaluate(z); // west
	scale[2] = m_M->GetSNSpline()->Evaluate(z); // north
	scale[3] = m_M->GetSSSpline()->Evaluate(z); // south;

	// original bounds
	vtkTransform* Transform = vtkTransform::New();
	Transform->Identity();
	
	double SEBounds[6]; // south east
	m_M->GetSEContourTransformPolyDataFilter()->SetTransform(Transform);
	m_M->GetSEContourTransformPolyDataFilter()->Update();
	m_M->GetSEContourTransformPolyDataFilter()->GetOutput()->GetBounds(SEBounds);
	m_M->UpdateSegmentSouthEastTransform();
	
	double SWBounds[6]; // south west
	m_M->GetSWContourTransformPolyDataFilter()->SetTransform(Transform);
	m_M->GetSWContourTransformPolyDataFilter()->Update();
	m_M->GetSWContourTransformPolyDataFilter()->GetOutput()->GetBounds(SWBounds);
	m_M->UpdateSegmentSouthWestTransform();
	
	double NEBounds[6]; // north east
	m_M->GetNEContourTransformPolyDataFilter()->SetTransform(Transform);
	m_M->GetNEContourTransformPolyDataFilter()->Update();
	m_M->GetNEContourTransformPolyDataFilter()->GetOutput()->GetBounds(NEBounds);
	m_M->UpdateSegmentNorthEastTransform();
	
	double NWBounds[6]; // north west
	m_M->GetNWContourTransformPolyDataFilter()->SetTransform(Transform);
	m_M->GetNWContourTransformPolyDataFilter()->Update();
	m_M->GetNWContourTransformPolyDataFilter()->GetOutput()->GetBounds(NWBounds);
	m_M->UpdateSegmentNorthWestTransform();
	Transform->Delete();

	// original handles
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

	// transformed handles
	// north - red
	vtkTransform* h3Transf = vtkTransform::New();
	h3Transf->Identity();

	// r operation
	h3Transf->Translate(center[0], center[1], 0.0); // axes origin
	h3Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	h3Transf->RotateZ(twist); // r operation
	h3Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	h3Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
  
	// s operation
	h3Transf->Translate(center[0], center[1], 0.0); // axes origin
	h3Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	h3Transf->Scale(1.0, scale[2], 1.0); // s operation
	h3Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	h3Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin

	// t operation
	h3Transf->Translate(trans[0], trans[1], 0.0);
  
	// p operation
	h3Transf->Translate(ctrans[0], ctrans[1], 0.0);

	h3Transf->TransformPoint(h3, h3);
	h3Transf->Delete();

	// set
	SetHandle3(h3[0], h3[1], h3[2]);


	// south - green			  
	vtkTransform* h0Transf = vtkTransform::New();
	h0Transf->Identity();
	// r operation
	h0Transf->Translate(center[0], center[1], 0.0); // axes origin
	h0Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	h0Transf->RotateZ(twist); // r operation
	h0Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	h0Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
  
	// s operation
	h0Transf->Translate(center[0], center[1], 0.0); // axes origin
	h0Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	h0Transf->Scale(1.0, scale[3], 1.0); // s operation
	h0Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	h0Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin

	// t operation
	h0Transf->Translate(trans[0], trans[1], 0.0);
  
	// p operation
	h0Transf->Translate(ctrans[0], ctrans[1], 0.0);
	h0Transf->TransformPoint(h0, h0);
	h0Transf->Delete();

	// set
	SetHandle0(h0[0], h0[1], h0[2]);

	
	// east - blue
	vtkTransform* h1Transf = vtkTransform::New();
	h1Transf->Identity();
	// r operation
	h1Transf->Translate(center[0], center[1], 0.0); // axes origin
	h1Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	h1Transf->RotateZ(twist); // r operation
	h1Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	h1Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
  
	// s operation
	h1Transf->Translate(center[0], center[1], 0.0); // axes origin
	h1Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	h1Transf->Scale(scale[0], 1.0, 1.0); // s operation
	h1Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	h1Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin

	// t operation
	h1Transf->Translate(trans[0], trans[1], 0.0);
  
	// p operation
	h1Transf->Translate(ctrans[0], ctrans[1], 0.0);
	h1Transf->TransformPoint(h1, h1);
	h1Transf->Delete();

	// set
	SetHandle1(h1[0], h1[1], h1[2]);

	
	// west - magenta
  h2[0] = std::max(NWBounds[0], SWBounds[0]);
	h2[1] = center[1] - trans[1];
	h2[2] = 0.0;
	vtkTransform* h2Transf = vtkTransform::New();
	h2Transf->Identity();
	
	// r operation
	h2Transf->Translate(center[0], center[1], 0.0); // axes origin
	h2Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	h2Transf->RotateZ(twist); // r operation
	h2Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	h2Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
  
	// s operation
	h2Transf->Translate(center[0], center[1], 0.0); // axes origin
	h2Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	h2Transf->Scale(scale[1], 1.0, 1.0); // s operation
	h2Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	h2Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin

	// t operation
	h2Transf->Translate(trans[0], trans[1], 0.0);
  
	// p operation
	h2Transf->Translate(ctrans[0], ctrans[1], 0.0);
	h2Transf->TransformPoint(h2, h2);
	h2Transf->Delete();

	// set
	SetHandle2(h2[0], h2[1], h2[2]);			
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::UpdateRotationHandle()
//----------------------------------------------------------------------------
{
	// get z level
	double z = m_M->GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	m_M->GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = m_M->GetPHSpline()->Evaluate(z);
	ctrans[1] = m_M->GetPVSpline()->Evaluate(z);

	SetRotationalHandle(center[0] + ctrans[0], center[1] + ctrans[1], 0.0);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::UpdateWidgetTransform()
//----------------------------------------------------------------------------
{
	// get z level
	double z = m_M->GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	m_M->GetContourActor()->GetBounds(bounds);

	// set up 2d contour axes first
	//double width = M->GetContourAxesLengthScale() * (bounds[1] - bounds[0]) / 2.0;
	//double height = M->GetContourAxesLengthScale() * (bounds[3] - bounds[2]) / 2.0;
	
	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z 

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = m_M->GetPHSpline()->Evaluate(z);
	ctrans[1] = m_M->GetPVSpline()->Evaluate(z);

	// get twist
	double twist = m_M->GetRASpline()->Evaluate(z);

	// get h/v translation
	double trans[2];
	trans[0] = m_M->GetTHSpline()->Evaluate(z);
	trans[1] = m_M->GetTVSpline()->Evaluate(z);

	// get scaling
	double scale[4];
	scale[0] = m_M->GetSESpline()->Evaluate(z); // east
	scale[1] = m_M->GetSWSpline()->Evaluate(z); // west
	scale[2] = m_M->GetSNSpline()->Evaluate(z); // north
	scale[3] = m_M->GetSSSpline()->Evaluate(z); // south;

	// original
	double Origin[3];
	double Point1[3];
	double Point2[3];
	if (center[0] == 0.0 && center[1] == 0.0)
	{
		m_M->GetPositiveXAxisActor()->VisibilityOn();
		m_M->GetNegativeXAxisActor()->VisibilityOn();
		m_M->GetPositiveYAxisActor()->VisibilityOn();
		m_M->GetNegativeYAxisActor()->VisibilityOn();

		Point1[0] = 10.0;
		Point1[1] = -10.0;
		Point1[2] = 0.0;
		Point2[0] = -10.0;
		Point2[1] = 10.0;
		Point2[2] = 0.0;
		Origin[0] = Point2[0];
		Origin[1] = Point1[1];
		Origin[2] = 0.0;
	}
	else
	{
		m_M->GetPositiveXAxisActor()->VisibilityOff();
		m_M->GetNegativeXAxisActor()->VisibilityOff();
		m_M->GetPositiveYAxisActor()->VisibilityOff();
		m_M->GetNegativeYAxisActor()->VisibilityOff();
		Point1[0] = bounds[1];
		Point1[1] = bounds[2];
		Point1[2] = 0.0;
		Point2[0] = bounds[0];
		Point2[1] = bounds[3];
		Point2[2] = 0.0;
		Origin[0] = Point2[0];
		Origin[1] = Point1[1];
		Origin[2] = 0.0;
	}

	// transformed origin
	vtkTransform *OrTransf = vtkTransform::New();
	OrTransf->Identity();
	OrTransf->Translate(center[0], center[1], 0.0); // axes origin
	OrTransf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	OrTransf->RotateZ(twist); // r operation
	OrTransf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	OrTransf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	OrTransf->Translate(center[0], center[1], 0.0); // axes origin
	OrTransf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	OrTransf->Scale(scale[1], scale[3], 1.0); // s operation
	OrTransf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	OrTransf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	OrTransf->Translate(trans[0], trans[1], 0.0);
	OrTransf->Translate(ctrans[0], ctrans[1], 0.0);
	OrTransf->TransformPoint(Origin, Origin);
	OrTransf->Delete();

	// transformed point 1
	vtkTransform *P1Transf = vtkTransform::New();
	P1Transf->Identity();
	P1Transf->Translate(center[0], center[1], 0.0); // axes origin
	P1Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	P1Transf->RotateZ(twist); // r operation
	P1Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	P1Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	P1Transf->Translate(center[0], center[1], 0.0); // axes origin
	P1Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	P1Transf->Scale(scale[0], scale[3], 1.0); // s operation
	P1Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	P1Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	P1Transf->Translate(trans[0], trans[1], 0.0);
	P1Transf->Translate(ctrans[0], ctrans[1], 0.0);
	P1Transf->TransformPoint(Point1, Point1);
	P1Transf->Delete();

	// transformed point 2
	vtkTransform *P2Transf = vtkTransform::New();
	P2Transf->Identity();
	P2Transf->Translate(center[0], center[1], 0.0); // axes origin
	P2Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	P2Transf->RotateZ(twist); // r operation
	P2Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	P2Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	P2Transf->Translate(center[0], center[1], 0.0); // axes origin
	P2Transf->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	P2Transf->Scale(scale[1], scale[2], 1.0); // s operation
	P2Transf->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	P2Transf->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	P2Transf->Translate(trans[0], trans[1], 0.0);
	P2Transf->Translate(ctrans[0], ctrans[1], 0.0);
	P2Transf->TransformPoint(Point2, Point2);
	P2Transf->Delete();
	
	//
	SetOrigin(Origin);
	SetPoint1(Point1);
	SetPoint2(Point2);
	m_PlaneSource->Update();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScaleNorth(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  float v[3];
  v[0] = p2[0] - p1[0]; 
  v[1] = p2[1] - p1[1]; 
  v[2] = 0.0; // no change along z

  // get current slice id
  double s = m_M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = m_M->GetCurrentZOfSyntheticScans();

  // get center h translation
  double chtrans = m_M->GetPHSpline()->Evaluate(z);

  // get center v translation
  double cvtrans = m_M->GetPVSpline()->Evaluate(z);
  
  // get twist
  double twist = m_M->GetRASpline()->Evaluate(z);

  // get h translation
  double htrans = m_M->GetTHSpline()->Evaluate(z);

  // get v translation
  double vtrans = m_M->GetTVSpline()->Evaluate(z);
  
  // get east scaling
  double escale = m_M->GetSESpline()->Evaluate(z);

  // get west scaling
  double wscale = m_M->GetSWSpline()->Evaluate(z);

  // get north scaling
  double nscale = m_M->GetSNSpline()->Evaluate(z);

  // get south scaling
  double sscale = m_M->GetSSSpline()->Evaluate(z);

  // rotated y unit vector
  float y[3];
  y[0] = 0.0; y[1] = 1.0; y[2] = 0.0;
  vtkTransform* TransformY = vtkTransform::New();
  TransformY->Identity();
  TransformY->RotateZ(twist);
  TransformY->TransformPoint(y, y);
  TransformY->Delete();

  // projection on rotated y axis
  vtkMath* Math = vtkMath::New();
  double yprojection = Math->Dot(v, y);
  Math->Delete();
 
  if (yprojection == 0.0) // no motion
    return;

  // overall scale
  float scale = yprojection * 0.05;
  scale = nscale + scale;
  
  // exceeded max limits?
  if (scale > m_N->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < m_N->GetMinY())
    return;// display information vtrans
  m_M->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update n spline
  // display values
  m_N->RemovePoint(s);
  m_N->AddPoint(s, scale);
  m_N->Render();

  // actual values
  m_M->GetSNSpline()->RemovePoint(z);
  m_M->GetSNSpline()->AddPoint(z, scale);

  // update widget
  UpdateWidgetTransform();
  
  // 
  UpdateScalingHandles();

  // update north east segment
  m_M->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_M->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_M->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_M->UpdateSegmentSouthWestTransform();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScaleSouth(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  float v[3];
  v[0] = p2[0] - p1[0]; 
  v[1] = p2[1] - p1[1]; 
  v[2] = 0.0; // no change along z

  // get current slice id
  double s = m_M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = m_M->GetCurrentZOfSyntheticScans();

  // get center h translation
  double chtrans = m_M->GetPHSpline()->Evaluate(z);

  // get center v translation
  double cvtrans = m_M->GetPVSpline()->Evaluate(z);
  
  // get twist
  double twist = m_M->GetRASpline()->Evaluate(z);

  // get h translation
  double htrans = m_M->GetTHSpline()->Evaluate(z);

  // get v translation
  double vtrans = m_M->GetTVSpline()->Evaluate(z);
  
  // get east scaling
  double escale = m_M->GetSESpline()->Evaluate(z);

  // get west scaling
  double wscale = m_M->GetSWSpline()->Evaluate(z);

  // get north scaling
  double nscale = m_M->GetSNSpline()->Evaluate(z);

  // get south scaling
  double sscale = m_M->GetSSSpline()->Evaluate(z);

  // rotated y unit vector
  float y[3];
  y[0] = 0.0; y[1] = 1.0; y[2] = 0.0;
  vtkTransform* TransformY = vtkTransform::New();
  TransformY->Identity();
  TransformY->RotateZ(twist);
  TransformY->TransformPoint(y, y);
  TransformY->Delete();

  // projection on rotated y axis
  vtkMath* Math = vtkMath::New();
  double yprojection = Math->Dot(v, y);
  Math->Delete();
 
  if (yprojection == 0.0) // no motion
    return;

  // overall scale
  float scale = -1.0 * yprojection *   0.05;
  scale = sscale + scale;
  
  // exceeded max limits?
  if (scale > m_S->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < m_S->GetMinY())
    return;

  // display information vtrans
  m_M->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update s spline
  // display values
  m_S->RemovePoint(s);
  m_S->AddPoint(s, scale);
  m_S->Render();

  // actual values
  m_M->GetSSSpline()->RemovePoint(z);
  m_M->GetSSSpline()->AddPoint(z, scale);

  // update widget
  UpdateWidgetTransform();
  
  // 
  UpdateScalingHandles();

  // update north east segment
  m_M->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_M->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_M->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_M->UpdateSegmentSouthWestTransform();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScaleEast(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  float v[3];
  v[0] = p2[0] - p1[0]; 
  v[1] = p2[1] - p1[1]; 
  v[2] = 0.0; // no change along z

  // get current slice id
  double s = m_M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = m_M->GetCurrentZOfSyntheticScans();

  // get center h translation
  double chtrans = m_M->GetPHSpline()->Evaluate(z);

  // get center v translation
  double cvtrans = m_M->GetPVSpline()->Evaluate(z);
  
  // get twist
  double twist = m_M->GetRASpline()->Evaluate(z);

  // get h translation
  double htrans = m_M->GetTHSpline()->Evaluate(z);

  // get v translation
  double vtrans = m_M->GetTVSpline()->Evaluate(z);
  
  // get east scaling
  double escale = m_M->GetSESpline()->Evaluate(z);

  // get west scaling
  double wscale = m_M->GetSWSpline()->Evaluate(z);

  // get north scaling
  double nscale = m_M->GetSNSpline()->Evaluate(z);

  // get south scaling
  double sscale = m_M->GetSSSpline()->Evaluate(z);

  // rotated x unit vector
  float x[3];
  x[0] = 1.0; x[1] = 0.0; x[2] = 0.0;
  vtkTransform* TransformX = vtkTransform::New();
  TransformX->Identity();
  TransformX->RotateZ(twist);
  TransformX->TransformPoint(x, x);
  TransformX->Delete();

  // projection on rotated x axis
  vtkMath* Math = vtkMath::New();
  double xprojection = Math->Dot(v, x);
  Math->Delete();

  if (xprojection == 0.0) // no motion
    return;

  // overall scale
  float scale = xprojection * 0.05;
  scale = escale + scale;
  
  // exceeded max limits?
  if (scale > m_E->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < m_E->GetMinY())
    return;

  // display information vtrans
  m_M->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update e spline
  // display values
  m_E->RemovePoint(s);
  m_E->AddPoint(s, scale);
  m_E->Render();

  // actual values
  m_M->GetSESpline()->RemovePoint(z);
  m_M->GetSESpline()->AddPoint(z, scale);

  // update widget
  UpdateWidgetTransform();
  
  // 
  UpdateScalingHandles();

  // update north east segment
  m_M->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_M->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_M->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_M->UpdateSegmentSouthWestTransform();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScaleWest(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // get vector of motion
  float v[3];
  v[0] = p2[0] - p1[0]; 
  v[1] = p2[1] - p1[1]; 
  v[2] = 0.0; // no change along z

  // get current slice id
  double s = m_M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = m_M->GetCurrentZOfSyntheticScans();

  // get center h translation
  double chtrans = m_M->GetPHSpline()->Evaluate(z);

  // get center v translation
  double cvtrans = m_M->GetPVSpline()->Evaluate(z);
  
  // get twist
  double twist = m_M->GetRASpline()->Evaluate(z);

  // get h translation
  double htrans = m_M->GetTHSpline()->Evaluate(z);

  // get v translation
  double vtrans = m_M->GetTVSpline()->Evaluate(z);
  
  // get east scaling
  double escale = m_M->GetSESpline()->Evaluate(z);

  // get west scaling
  double wscale = m_M->GetSWSpline()->Evaluate(z);

  // get north scaling
  double nscale = m_M->GetSNSpline()->Evaluate(z);

  // get south scaling
  double sscale = m_M->GetSSSpline()->Evaluate(z);

  // rotated x unit vector
  float x[3];
  x[0] = 1.0; x[1] = 0.0; x[2] = 0.0;
  vtkTransform* TransformX = vtkTransform::New();
  TransformX->Identity();
  TransformX->RotateZ(twist);
  TransformX->TransformPoint(x, x);
  TransformX->Delete();

  // projection on rotated x axis
  vtkMath* Math = vtkMath::New();
  double xprojection = Math->Dot(v, x);
  Math->Delete();

  if (xprojection == 0.0) // no motion
    return;

  // overall scale
  float scale = -1.0 * xprojection * 0.05;
  scale = wscale + scale;
  
  // exceeded max limits?
  if (scale > m_W->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < m_W->GetMinY())
    return;

  // display information vtrans
  m_M->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update w spline
  // display values
  m_W->RemovePoint(s);
  m_W->AddPoint(s, scale);
  m_W->Render();

  // actual values
  m_M->GetSWSpline()->RemovePoint(z);
  m_M->GetSWSpline()->AddPoint(z, scale);

  // update widget
  UpdateWidgetTransform();
  
  // 
  UpdateScalingHandles();

  // update north east segment
  m_M->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_M->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_M->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_M->UpdateSegmentSouthWestTransform();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::Place(double *p1, double *p2)
//----------------------------------------------------------------------------
{
   // get vector of motion
  double m[3];
  m[0] = p2[0] - p1[0];
  m[1] = p2[1] - p1[1];
  m[2] = 0.0; // no change along z

  // get current slice id
  double s = m_M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = m_M->GetCurrentZOfSyntheticScans();

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = m_M->GetPHSpline()->Evaluate(z);
  ctrans[1] = m_M->GetPVSpline()->Evaluate(z);

  // horizontal/vertical displacement
  double h = m[0];
  double v = m[1];

  // exceeded max limits?
  if ((ctrans[0] + h) > m_CH->GetMaxY())
    return;
  if ((ctrans[1] + v) > m_CV->GetMaxY())
	return;

  // exceeded min limits?
  if ((ctrans[0] + h) < m_CH->GetMinY())
	return;
  if ((ctrans[1] + v) < m_CV->GetMinY())
	return;

  // display information vtrans
  m_M->SetText(1, ctrans[1] + v, 3, 0); // text actor 1
  
  // display information htrans
  m_M->SetText(2, ctrans[0] + h, 3, 0); // text actor 2

  // update ch spline
  // display values
  m_CH->RemovePoint(s);
  m_CH->AddPoint(s, ctrans[0] + h);
  m_CH->Render();

  // actual values
  m_M->GetPHSpline()->RemovePoint(z);
  m_M->GetPHSpline()->AddPoint(z, ctrans[0] + h);
  
  // update cv spline
  // display values
  m_CV->RemovePoint(s);
  m_CV->AddPoint(s, ctrans[1] + v);
  m_CV->Render();
 
  // actual values
  m_M->GetPVSpline()->RemovePoint(z);
  m_M->GetPVSpline()->AddPoint(z, ctrans[1] + v);

  // update widget
  UpdateWidgetTransform();
  
  // update north east segment
  m_M->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_M->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_M->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_M->UpdateSegmentSouthWestTransform();

  // update contour axes
  m_M->UpdateContourAxesTransform();

  // update global axes
  m_M->UpdateGlobalAxesTransform();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::Translate(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  float x[3]; // x axis unit vector
  x[0] = 1.0;
  x[1] = 0.0;
  x[2] = 0.0;

  float y[3]; // y axis unit vector
  y[0] = 0.0;
  y[1] = 1.0;
  y[2] = 0.0;

  // get vector of motion
  float m[3];
  m[0] = p2[0] - p1[0]; 
  m[1] = p2[1] - p1[1]; 
  m[2] = 0.0; // no change along z
  
  // get current slice id
  double s = m_M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = m_M->GetCurrentZOfSyntheticScans();

  // get twist
  double twist = m_M->GetRASpline()->Evaluate(z);

  // get h/v translation
  double trans[2];
  trans[0] = m_M->GetTHSpline()->Evaluate(z);
  trans[1] = m_M->GetTVSpline()->Evaluate(z);

  //
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();
  Transform->RotateZ(twist);
  Transform->TransformPoint(x, x);
  Transform->TransformPoint(y, y);
  Transform->Delete();

  vtkMath* Math = vtkMath::New();
  double h = Math->Dot(m, x); // projection on rotated x axis
  double v = Math->Dot(m, y); // projection on rotated y axis
  Math->Delete();

  // exceeded max limits?
  if ((trans[0] + h) > m_H->GetMaxY())
    return;
  if ((trans[1] + v) > m_V->GetMaxY())
	return;

  // exceeded min limits?
  if ((trans[0] + h) < m_H->GetMinY())
	return;
  if ((trans[1] + v) < m_V->GetMinY())
	return;

  // display information vtrans
  m_M->SetText(1, trans[1] + v, 3, 0); // text actor 1
  
  // display information htrans
  m_M->SetText(2, trans[0] + h, 3, 0); // text actor 2

  // update h spline
  // display values
  m_H->RemovePoint(s);
  m_H->AddPoint(s, trans[0] + h);
  m_H->Render();

  // actual values
  m_M->GetTHSpline()->RemovePoint(z);
  m_M->GetTHSpline()->AddPoint(z, trans[0] + h);
  
  // update v spline
  // display values
  m_V->RemovePoint(s);
  m_V->AddPoint(s, trans[1] + v);
  m_V->Render();
  
  // actual values
  m_M->GetTVSpline()->RemovePoint(z);
  m_M->GetTVSpline()->AddPoint(z, trans[1] + v);

  // update vertical cuts
  m_M->UpdateSegmentCuttingPlanes();

  // update widget
  UpdateWidgetTransform();
  
  // update north east segment
  m_M->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_M->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_M->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_M->UpdateSegmentSouthWestTransform();

  // update contour axes
  m_M->UpdateContourAxesTransform();

  // update global axes
  m_M->UpdateGlobalAxesTransform();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::Rotate(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // motion vector (world space)
  double v[3];
  v[0] = p2[0] - p1[0];
  v[1] = p2[1] - p1[1];
  v[2] = p2[2] - p1[2]; 
  
  // get current slice id
  double s = m_M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = m_M->GetCurrentZOfSyntheticScans();

  // get twist
  double twist = m_M->GetRASpline()->Evaluate(z);

  // rotation angle
  double theta;
  theta = v[1]; // y displacement

  // exceeded max limit?
  if ((twist + theta) > m_T->GetMaxY())
    return;

  // exceeded min limit?
  if ((twist + theta) < m_T->GetMinY())
	return;

  // display information
  m_M->SetText(1, twist + theta, 2, 0);

  // update twist spline
  // display values
  m_T->RemovePoint(s);
  m_T->AddPoint(s, twist + theta);
  m_T->Render();

  // actual values
  m_M->GetRASpline()->RemovePoint(z);
  m_M->GetRASpline()->AddPoint(z, twist + theta);

  // update widget
  UpdateWidgetTransform();
  
  // update north east segment
  m_M->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_M->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_M->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_M->UpdateSegmentSouthWestTransform();

  // update contour axes
  m_M->UpdateContourAxesTransform();

  // update global axes
  m_M->UpdateGlobalAxesTransform();
}
//----------------------------------------------------------------------------
int medOpMMLContourWidget::GetNextOperationId()
//----------------------------------------------------------------------------
{
  return m_OperationID;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetNextOperationId(int n)
//----------------------------------------------------------------------------
{
  m_OperationID = n;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetModel(medOpMMLModelView *Model)
//----------------------------------------------------------------------------
{
	m_M = Model;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetPH(medOpMMLParameterView *PH)
//----------------------------------------------------------------------------
{
	m_CH = PH;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetPV(medOpMMLParameterView *PV)
//----------------------------------------------------------------------------
{
	m_CV = PV;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetTH(medOpMMLParameterView *TH)
//----------------------------------------------------------------------------
{
	m_H = TH;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetTV(medOpMMLParameterView *TV)
//----------------------------------------------------------------------------
{
	m_V = TV;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetRA(medOpMMLParameterView *RA)
//----------------------------------------------------------------------------
{
	m_T = RA;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetSN(medOpMMLParameterView *SN)
//----------------------------------------------------------------------------
{
	m_N = SN;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetSS(medOpMMLParameterView *SS)
//----------------------------------------------------------------------------
{
	m_S = SS;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetSE(medOpMMLParameterView *SE)
//----------------------------------------------------------------------------
{
	m_E = SE;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetSW(medOpMMLParameterView *SW)
//----------------------------------------------------------------------------
{
	m_W = SW;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetRepresentation(int representation)
//----------------------------------------------------------------------------
{
  if (this->m_Representation != (representation<VTK_PLANE_OFF?VTK_PLANE_OFF:(representation>VTK_PLANE_SURFACE?VTK_PLANE_SURFACE:representation))) \
  {
    this->m_Representation = (representation<VTK_PLANE_OFF?VTK_PLANE_OFF:(representation>VTK_PLANE_SURFACE?VTK_PLANE_SURFACE:representation)); \
    this->Modified(); \
  }
}
