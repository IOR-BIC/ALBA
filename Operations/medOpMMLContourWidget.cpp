/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMMLContourWidget.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:48:42 $
  Version:   $Revision: 1.1 $
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


vtkCxxRevisionMacro(medOpMMLContourWidget, "$Revision: 1.1 $");
vtkStandardNewMacro(medOpMMLContourWidget);
vtkCxxSetObjectMacro(medOpMMLContourWidget, PlaneProperty, vtkProperty);

//----------------------------------------------------------------------------
medOpMMLContourWidget::medOpMMLContourWidget() 
: vtkPolyDataSourceWidget()
//----------------------------------------------------------------------------
{
  this->OperationID = 0; // first operation id is 0
  this->m_bCenterMode = FALSE; // center mode off
  this->m_bScalingMode = FALSE; // scaling mode off
  this->m_bRotatingMode = FALSE; // rotating mode off
  this->m_bTranslatingMode = FALSE; // translating mode off
  this->State = medOpMMLContourWidget::Start;
  this->EventCallbackCommand->SetCallback(medOpMMLContourWidget::ProcessEvents);
  
  this->NormalToXAxis = 0;
  this->NormalToYAxis = 0;
  this->NormalToZAxis = 0;
  this->Representation = VTK_PLANE_WIREFRAME;
 
  //
  this->SetHandleRadius(1.5);


  //Build the representation of the widget
  int i;
  // Represent the plane
  this->PlaneSource = vtkPlaneSource::New();
  this->PlaneSource->SetXResolution(4);
  this->PlaneSource->SetYResolution(4);
  this->PlaneOutline = vtkPolyData::New();
  this->PlaneOutlineTubes = vtkTubeFilter::New(); // MK
  this->PlaneOutlineTubes->SetInput(this->PlaneOutline); // Mk
  
  vtkPoints *pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  vtkCellArray *outline = vtkCellArray::New();
  outline->InsertNextCell(4);
  outline->InsertCellPoint(0);
  outline->InsertCellPoint(1);
  outline->InsertCellPoint(2);
  outline->InsertCellPoint(3);
  this->PlaneOutline->SetPoints(pts);
  pts->Delete();
  this->PlaneOutline->SetPolys(outline);
  outline->Delete();
  
  this->PlaneMapper = vtkPolyDataMapper::New();
  this->PlaneMapper->SetInput(this->PlaneSource->GetOutput());
  this->PlaneActor = vtkActor::New();
  this->PlaneActor->SetMapper(this->PlaneMapper);

  // Create the handles
  this->Handle = new vtkActor* [4];
  this->HandleMapper = new vtkPolyDataMapper* [4];
  this->HandleGeometry = new vtkSphereSource* [4];
  for (i=0; i<4; i++)
    {
    this->HandleGeometry[i] = vtkSphereSource::New();
    this->HandleGeometry[i]->SetThetaResolution(16);
    this->HandleGeometry[i]->SetPhiResolution(8);
    this->HandleMapper[i] = vtkPolyDataMapper::New();
    this->HandleMapper[i]->SetInput(this->HandleGeometry[i]->GetOutput());
    this->Handle[i] = vtkActor::New();
    this->Handle[i]->SetMapper(this->HandleMapper[i]);
    }

  // handles off
  for (i=0; i<4; i++)
  {
    this->Handle[i]->VisibilityOff();
  }

  // rotational handle
  this->RotationalHandle = vtkActor::New();
  this->RotationalHandleMapper = vtkPolyDataMapper::New();
  this->RotationalHandleGeometry = vtkSphereSource::New();
  this->RotationalHandleGeometry->SetThetaResolution(16);
  this->RotationalHandleGeometry->SetPhiResolution(8);
  this->RotationalHandleMapper->SetInput(this->RotationalHandleGeometry->GetOutput());
  this->RotationalHandle = vtkActor::New();
  this->RotationalHandle->SetMapper(this->RotationalHandleMapper);

  // rotational handle off
  this->RotationalHandle->VisibilityOff();

  // center handle
  this->CenterHandle = vtkActor::New();
  this->CenterHandleMapper = vtkPolyDataMapper::New();
  this->CenterHandleGeometry = vtkSphereSource::New();
  this->CenterHandleGeometry->SetThetaResolution(16);
  this->CenterHandleGeometry->SetPhiResolution(8);
  this->CenterHandleMapper->SetInput(this->CenterHandleGeometry->GetOutput());
  this->CenterHandle = vtkActor::New();
  this->CenterHandle->SetMapper(this->CenterHandleMapper);

  // center handle off
  this->CenterHandle->VisibilityOff();
   
  // Create the + plane normal
  this->LineSource = vtkLineSource::New();
  this->LineSource->SetResolution(1);
  this->LineMapper = vtkPolyDataMapper::New();
  this->LineMapper->SetInput(this->LineSource->GetOutput());
  this->LineActor = vtkActor::New();
  this->LineActor->SetMapper(this->LineMapper);

  this->ConeSource = vtkConeSource::New();
  this->ConeSource->SetResolution(12);
  this->ConeSource->SetAngle(25.0);
  this->ConeMapper = vtkPolyDataMapper::New();
  this->ConeMapper->SetInput(this->ConeSource->GetOutput());
  this->ConeActor = vtkActor::New();
  this->ConeActor->SetMapper(this->ConeMapper);

  // Create the - plane normal
  this->LineSource2 = vtkLineSource::New();
  this->LineSource2->SetResolution(1);
  this->LineMapper2 = vtkPolyDataMapper::New();
  this->LineMapper2->SetInput(this->LineSource2->GetOutput());
  this->LineActor2 = vtkActor::New();
  this->LineActor2->SetMapper(this->LineMapper2);

  this->ConeSource2 = vtkConeSource::New();
  this->ConeSource2->SetResolution(12);
  this->ConeSource2->SetAngle(25.0);
  this->ConeMapper2 = vtkPolyDataMapper::New();
  this->ConeMapper2->SetInput(this->ConeSource2->GetOutput());
  this->ConeActor2 = vtkActor::New();
  this->ConeActor2->SetMapper(this->ConeMapper2);

  this->Transform = vtkTransform::New();

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
  this->HandlePicker = vtkCellPicker::New();
  this->HandlePicker->SetTolerance(0.001);
  for (i=0; i<4; i++)
    {
    this->HandlePicker->AddPickList(this->Handle[i]);
    }
  this->HandlePicker->PickFromListOn();

  this->PlanePicker = vtkCellPicker::New();
  this->PlanePicker->SetTolerance(0.005); //need some fluff
  this->PlanePicker->AddPickList(this->PlaneActor);
  this->PlanePicker->AddPickList(this->RotationalHandle);
  this->PlanePicker->AddPickList(this->CenterHandle);
  //this->PlanePicker->AddPickList(this->ConeActor);
  //this->PlanePicker->AddPickList(this->LineActor);
  //this->PlanePicker->AddPickList(this->ConeActor2);
  //this->PlanePicker->AddPickList(this->LineActor2);
  this->PlanePicker->PickFromListOn();
  
  this->CurrentHandle = NULL;

  // Set up the initial properties
  this->CreateDefaultProperties();
  
  this->SelectRepresentation();
}
//----------------------------------------------------------------------------
medOpMMLContourWidget::~medOpMMLContourWidget()
//----------------------------------------------------------------------------
{
  this->PlaneActor->Delete();
  this->PlaneMapper->Delete();
  this->PlaneSource->Delete();
  this->PlaneOutline->Delete();

  for (int i=0; i<4; i++)
    {
    this->HandleGeometry[i]->Delete();
    this->HandleMapper[i]->Delete();
    this->Handle[i]->Delete();
    }
  delete [] this->Handle;
  delete [] this->HandleMapper;
  delete [] this->HandleGeometry;
  
  this->ConeActor->Delete();
  this->ConeMapper->Delete();
  this->ConeSource->Delete();

  this->LineActor->Delete();
  this->LineMapper->Delete();
  this->LineSource->Delete();

  this->ConeActor2->Delete();
  this->ConeMapper2->Delete();
  this->ConeSource2->Delete();

  this->LineActor2->Delete();
  this->LineMapper2->Delete();
  this->LineSource2->Delete();

  this->HandlePicker->Delete();
  this->PlanePicker->Delete();

  this->HandleProperty->Delete();
  this->SelectedHandleProperty->Delete();
  this->PlaneProperty->Delete();
  this->SelectedPlaneProperty->Delete();
  
  this->Transform->Delete();
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
    this->CurrentRenderer->AddActor(this->PlaneActor);
    this->PlaneActor->SetProperty(this->PlaneProperty);

    // turn on the handles
    for (int j=0; j<4; j++)
      {
      this->CurrentRenderer->AddActor(this->Handle[j]);
      //this->Handle[j]->SetProperty(this->HandleProperty);
      }

	// colours
	this->Handle[0]->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	this->Handle[3]->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	this->Handle[2]->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
	this->Handle[1]->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue

	this->CurrentRenderer->AddActor(this->RotationalHandle);
	this->RotationalHandle->SetProperty(this->HandleProperty);

	this->CurrentRenderer->AddActor(this->CenterHandle);
	this->CenterHandle->SetProperty(this->HandleProperty);

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
    this->CurrentRenderer->RemoveActor(this->PlaneActor);

    // turn off the handles
    for (int i=0; i<4; i++)
      {
      this->CurrentRenderer->RemoveActor(this->Handle[i]);
      }

	this->CurrentRenderer->RemoveActor(this->RotationalHandle);
	this->CurrentRenderer->RemoveActor(this->CenterHandle);
    // turn off the normal vector
   /* this->CurrentRenderer->RemoveActor(this->LineActor);
    this->CurrentRenderer->RemoveActor(this->ConeActor);
    this->CurrentRenderer->RemoveActor(this->LineActor2);
    this->CurrentRenderer->RemoveActor(this->ConeActor2);*/

    this->CurrentHandle = NULL;
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

  if ( this->HandleProperty )
    {
    os << indent << "Handle Property: " << this->HandleProperty << "\n";
    }
  else
    {
    os << indent << "Handle Property: (none)\n";
    }
  if ( this->SelectedHandleProperty )
    {
    os << indent << "Selected Handle Property: " 
       << this->SelectedHandleProperty << "\n";
    }
  else
    {
    os << indent << "SelectedHandle Property: (none)\n";
    }

  if ( this->PlaneProperty )
    {
    os << indent << "Plane Property: " << this->PlaneProperty << "\n";
    }
  else
    {
    os << indent << "Plane Property: (none)\n";
    }
  if ( this->SelectedPlaneProperty )
    {
    os << indent << "Selected Plane Property: " 
       << this->SelectedPlaneProperty << "\n";
    }
  else
    {
    os << indent << "Selected Plane Property: (none)\n";
    }

  os << indent << "Plane Representation: ";
  if ( this->Representation == VTK_PLANE_WIREFRAME )
    {
    os << "Wireframe\n";
    }
  else if ( this->Representation == VTK_PLANE_SURFACE )
    {
    os << "Surface\n";
    }
  else //( this->Representation == VTK_PLANE_OUTLINE )
    {
    os << "Outline\n";
    }

  os << indent << "Normal To X Axis: " 
     << (this->NormalToXAxis ? "On" : "Off") << "\n";
  os << indent << "Normal To Y Axis: " 
     << (this->NormalToYAxis ? "On" : "Off") << "\n";
  os << indent << "Normal To Z Axis: " 
     << (this->NormalToZAxis ? "On" : "Off") << "\n";

  int res = this->PlaneSource->GetXResolution();
  double *o = this->PlaneSource->GetOrigin();
  double *pt1 = this->PlaneSource->GetPoint1();
  double *pt2 = this->PlaneSource->GetPoint2();

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

  this->RotationalHandleGeometry->SetCenter(RotHandle);

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
  if ( this->CurrentHandle )
    {
    //this->CurrentHandle->SetProperty(this->HandleProperty);
    }

  this->CurrentHandle = (vtkActor *)prop;

  if ( this->CurrentHandle )
    {
    this->ValidPick = 1;
    this->HandlePicker->GetPickPosition(this->LastPickPosition);
    //this->CurrentHandle->SetProperty(this->SelectedHandleProperty);
    for (int i=0; i<4; i++) //find handle
      {
      if ( this->CurrentHandle == this->Handle[i] )
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
    this->PlanePicker->GetPickPosition(this->LastPickPosition);
    this->LineActor->SetProperty(this->SelectedHandleProperty);
    this->ConeActor->SetProperty(this->SelectedHandleProperty);
    this->LineActor2->SetProperty(this->SelectedHandleProperty);
    this->ConeActor2->SetProperty(this->SelectedHandleProperty);
    }
  else
    {
    this->LineActor->SetProperty(this->HandleProperty);
    this->ConeActor->SetProperty(this->HandleProperty);
    this->LineActor2->SetProperty(this->HandleProperty);
    this->ConeActor2->SetProperty(this->HandleProperty);
    }
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::HighlightPlane(int highlight)
//----------------------------------------------------------------------------
{
  if ( highlight )
    {
    this->ValidPick = 1;
    this->PlanePicker->GetPickPosition(this->LastPickPosition);
    this->PlaneActor->SetProperty(this->SelectedPlaneProperty);
    }
  else
    {
    this->PlaneActor->SetProperty(this->PlaneProperty);
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
    this->State = medOpMMLContourWidget::Outside;
    return;
    }
  
  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then try to pick the plane.
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
    {
    this->State = medOpMMLContourWidget::Moving;
    this->HighlightHandle(path->GetFirstNode()->GetProp());

	if (GetScalingMode())
	{
			if (this->CurrentHandle)
			{
				if (this->CurrentHandle == this->Handle[0])
				{
					// begin s-s op
					this->Operation = medOpMMLContourWidget::SouthScale;
				}
				else
				if (this->CurrentHandle == this->Handle[1])
				{
					// begin s-e op
					this->Operation = medOpMMLContourWidget::EastScale;
				}
				else
				if (this->CurrentHandle == this->Handle[2])
				{
					// begin s-w op
					this->Operation = medOpMMLContourWidget::WestScale;
				}
				else
				if (this->CurrentHandle == this->Handle[3])
				{
					// begin s-n op
					this->Operation = medOpMMLContourWidget::NorthScale;
				}
			}
	}

    }
  else
    {
    this->PlanePicker->Pick(X,Y,0.0,this->CurrentRenderer);
    path = this->PlanePicker->GetPath();
    if ( path != NULL )
      {
      vtkProp *prop = path->GetFirstNode()->GetProp();
      if (/* prop == this->ConeActor || prop == this->LineActor ||
           prop == this->ConeActor2 || prop == this->LineActor2*/
		   prop == this->RotationalHandle)
        {
        this->State = medOpMMLContourWidget::Rotating;
        this->HighlightNormal(1);

		// begin r-op
		this->Operation = medOpMMLContourWidget::Rotation;
        }
     /* else
	  if (prop == this->CenterHandle)
        {
        this->State = medOpMMLContourWidget::Centralising;
        this->HighlightNormal(1);
        }*/
	  else
        {
        this->State = medOpMMLContourWidget::Moving;
        this->HighlightPlane(1);

		if (GetTranslationMode())
		{
			// begin t-op
			this->Operation = medOpMMLContourWidget::Translation;
		}
		else
		if (GetCenterMode())
		{
			// begin p-op
			this->Operation = medOpMMLContourWidget::Placement;
		}
       }
      }
    else
      {
      this->State = medOpMMLContourWidget::Outside;
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
  if ( this->State == medOpMMLContourWidget::Outside ||
       this->State == medOpMMLContourWidget::Start )
    {
    return;
    }

  // 1. switch off display information
  M->GetScaledTextActor1()->VisibilityOff();
  M->GetScaledTextActor2()->VisibilityOff();

  // 2. save undo parameters
  double params[5];
  //float flag;

  // slice id
  params[0] = M->GetCurrentIdOfSyntheticScans();

  // z level
  params[1] = M->GetCurrentZOfSyntheticScans();

  // operation type
  params[2] = Operation;

  switch (Operation)
  {
    case medOpMMLContourWidget::Placement : //
		                params[3] = M->GetPHSpline()->Evaluate(params[1]);
						params[4] = M->GetPVSpline()->Evaluate(params[1]);
	break;

	case medOpMMLContourWidget::Translation : //
						params[3] = M->GetTHSpline()->Evaluate(params[1]);
						params[4] = M->GetTVSpline()->Evaluate(params[1]);
	break;

	case medOpMMLContourWidget::Rotation : //
						params[3] = M->GetRASpline()->Evaluate(params[1]);
						params[4] = 0.0;
	break;

	case medOpMMLContourWidget::NorthScale : //
						params[3] = M->GetSNSpline()->Evaluate(params[1]);
						params[4] = 0.0;
						/*M->ScalingFlagStack->GetTuple(params[0], &flag);
						if (flag == 0.0)
							flag = 1.0;
						M->ScalingFlagStack->SetTuple(params[0], &flag);*/
						if (!M->ScalingOccured)
						{
							M->ScalingOccured = TRUE;
							M->ScalingOccuredOperationId = GetNextOperationId();

							// grey out twist, h/v translation views

							// redraw curves
							CH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							CH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							CH->Render();

							CV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							CV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							CV->Render();

							T->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							T->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							T->Render();

							H->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							H->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							H->Render();

							V->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							V->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							V->Render();
						}

	break;

	case medOpMMLContourWidget::SouthScale : //
						params[3] = M->GetSSSpline()->Evaluate(params[1]);
						params[4] = 0.0;
						/*M->ScalingFlagStack->GetTuple(params[0], &flag);
						if (flag == 0.0)
							flag = 1.0;
						M->ScalingFlagStack->SetTuple(params[0], &flag);*/
						if (!M->ScalingOccured)
						{
							M->ScalingOccured = TRUE;
							M->ScalingOccuredOperationId = GetNextOperationId();

							// grey out twist, h/v translation views

							// redraw curves
							CH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							CH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							CH->Render();

							CV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							CV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							CV->Render();

							T->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							T->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							T->Render();

							H->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							H->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							H->Render();

							V->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							V->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							V->Render();
						}
	break;

	case medOpMMLContourWidget::EastScale : //
						params[3] = M->GetSESpline()->Evaluate(params[1]);
						params[4] = 0.0;
						/*M->ScalingFlagStack->GetTuple(params[0], &flag);
						if (flag == 0.0)
							flag = 1.0;
						M->ScalingFlagStack->SetTuple(params[0], &flag);*/
						if (!M->ScalingOccured)
						{
							M->ScalingOccured = TRUE;
							M->ScalingOccuredOperationId = GetNextOperationId();

							// grey out twist, h/v translation views

							// redraw curves
							CH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							CH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							CH->Render();

							CV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							CV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							CV->Render();

							T->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							T->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							T->Render();

							H->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							H->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							H->Render();

							V->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							V->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							V->Render();
						}
	break;

	case medOpMMLContourWidget::WestScale : //
						params[3] = M->GetSWSpline()->Evaluate(params[1]);
						params[4] = 0.0;
						/*M->ScalingFlagStack->GetTuple(params[0], &flag);
						if (flag == 0.0)
							flag = 1.0;
						M->ScalingFlagStack->SetTuple(params[0], &flag);*/
						if (!M->ScalingOccured)
						{
							M->ScalingOccured = TRUE;
							M->ScalingOccuredOperationId = GetNextOperationId();

							// grey out twist, h/v translation views

							// redraw curves
							CH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							CH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							CH->Render();

							CV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							CV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							CV->Render();

							T->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							T->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							T->Render();

							H->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							H->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							H->Render();

							V->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
							V->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
							V->Render();
						}
	break;

  }

  // exceeded max ?
  assert(GetNextOperationId() < 2000);

  // save
  M->OperationsStack->SetTuple(GetNextOperationId(), params);

  // new next op
  SetNextOperationId(GetNextOperationId() + 1);

  this->State = medOpMMLContourWidget::Start;
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
  if ( this->State == medOpMMLContourWidget::Outside || 
       this->State == medOpMMLContourWidget::Start )
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
  if ( this->State == medOpMMLContourWidget::Moving )
  {
   // interaction operations
   // using a contour widget
   if (GetScalingMode()) // scaling
   {
    if (this->CurrentHandle)
	{
	 // display information on.
	 // off in OnLeftButtonUp()
	 M->GetScaledTextActor1()->VisibilityOn();

     if (this->CurrentHandle == this->Handle[0]) // south scaling
	 {
	  
      this->ScaleSouth(prevPickPoint, pickPoint);
	 }
     else
	 if (this->CurrentHandle == this->Handle[1]) // east scaling
	 {
      this->ScaleEast(prevPickPoint, pickPoint);
	 }
     else
	 if (this->CurrentHandle == this->Handle[2]) // west scaling
	 {
      this->ScaleWest(prevPickPoint, pickPoint);
	 }
     else
     if (this->CurrentHandle == this->Handle[3]) // north scaling
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
	M->GetScaledTextActor1()->VisibilityOn(); 
	M->GetScaledTextActor2()->VisibilityOn();
	this->Translate(prevPickPoint, pickPoint);
   }
   else
   if (GetCenterMode()) // place
   {
	// display information on.
	// off in OnLeftButtonUp()
	M->GetScaledTextActor1()->VisibilityOn(); 
	M->GetScaledTextActor2()->VisibilityOn();

	this->Place(prevPickPoint, pickPoint);
   }
  }
  else if ( this->State == medOpMMLContourWidget::Scaling )
  {
   this->Scale(prevPickPoint, pickPoint, X, Y);
  }
  else if ( this->State == medOpMMLContourWidget::Pushing )
  {
   this->Push(prevPickPoint, pickPoint);
  }
  else if ( this->State == medOpMMLContourWidget::Rotating ) // rotation
  {
   // display information on.
   // off in OnLeftButtonUp()
   M->GetScaledTextActor1()->VisibilityOn();
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
  double *o = this->PlaneSource->GetOrigin();
  double *pt1 = this->PlaneSource->GetPoint1();
  double *pt2 = this->PlaneSource->GetPoint2();

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

  this->PlaneSource->SetOrigin(origin);
  this->PlaneSource->SetPoint1(point1);
  this->PlaneSource->SetPoint2(point2);
  this->PlaneSource->Update();

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
  
  this->PlaneSource->Push( vtkMath::Dot(v,this->Normal) );
  this->PlaneSource->Update();
  this->PositionHandles(0.5, 0.5);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::CreateDefaultProperties()
//----------------------------------------------------------------------------
{
  // Handle properties
  this->HandleProperty = vtkProperty::New();
  this->HandleProperty->SetColor(1,1,1);

  this->SelectedHandleProperty = vtkProperty::New();
  this->SelectedHandleProperty->SetColor(1,0,0);

  // Plane properties
  this->PlaneProperty = vtkProperty::New();
  this->PlaneProperty->SetAmbient(1.0);
  this->PlaneProperty->SetAmbientColor(1.0,1.0,1.0);

  this->SelectedPlaneProperty = vtkProperty::New();
  this->SelectRepresentation();
  this->SelectedPlaneProperty->SetAmbient(1.0);
  this->SelectedPlaneProperty->SetAmbientColor(0.5,0.5,0.5);
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
    if ( this->NormalToYAxis )
      {
      this->PlaneSource->SetOrigin(bounds[0],center[1],bounds[4]);
      this->PlaneSource->SetPoint1(bounds[1],center[1],bounds[4]);
      this->PlaneSource->SetPoint2(bounds[0],center[1],bounds[5]);
      }
    else if ( this->NormalToZAxis )
      {
      this->PlaneSource->SetOrigin(bounds[0],bounds[2],center[2]);
      this->PlaneSource->SetPoint1(bounds[1],bounds[2],center[2]);
      this->PlaneSource->SetPoint2(bounds[0],bounds[3],center[2]);
      }
    else //default or x-normal
      {
      this->PlaneSource->SetOrigin(center[0],bounds[2],bounds[4]);
      this->PlaneSource->SetPoint1(center[0],bounds[3],bounds[4]);
      this->PlaneSource->SetPoint2(center[0],bounds[2],bounds[5]);
      }
    }

  this->PlaneSource->Update();

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
    this->PlaneSource->GetOrigin(origin);
    this->PlaneSource->GetPoint1(point1);
    this->PlaneSource->GetPoint2(point2);
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
    this->HandleGeometry[i]->SetRadius(radius);
    }

  this->RotationalHandleGeometry->SetRadius(radius);
  this->CenterHandleGeometry->SetRadius(radius);

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

  if ( this->Representation == VTK_PLANE_OFF )
    {
    this->CurrentRenderer->RemoveActor(this->PlaneActor);
    }
  else if ( this->Representation == VTK_PLANE_OUTLINE )
    {
    this->CurrentRenderer->RemoveActor(this->PlaneActor);
    this->CurrentRenderer->AddActor(this->PlaneActor);
	
	//this->PlaneOutlineTubes->SetRadius(3.0);
    //this->PlaneOutlineTubes->SetNumberOfSides(6);
	//this->PlaneMapper->SetInput( this->PlaneOutlineTubes->GetOutput());

    this->PlaneMapper->SetInput( this->PlaneOutline);
	this->PlaneActor->GetProperty()->SetRepresentationToWireframe();
    }
  else if ( this->Representation == VTK_PLANE_SURFACE )
    {
    this->CurrentRenderer->RemoveActor(this->PlaneActor);
    this->CurrentRenderer->AddActor(this->PlaneActor);
    this->PlaneMapper->SetInput( this->PlaneSource->GetOutput() );
    this->PlaneActor->GetProperty()->SetRepresentationToSurface();
    }
  else //( this->Representation == VTK_PLANE_WIREFRAME )
    {
    this->CurrentRenderer->RemoveActor(this->PlaneActor);
    this->CurrentRenderer->AddActor(this->PlaneActor);
    this->PlaneMapper->SetInput( this->PlaneSource->GetOutput() );
    this->PlaneActor->GetProperty()->SetRepresentationToWireframe();
    }
}
//----------------------------------------------------------------------------
/** Description:
 Set/Get the resolution (number of subdivisions) of the plane. */
void medOpMMLContourWidget::SetResolution(int r)
//----------------------------------------------------------------------------
{
  this->PlaneSource->SetXResolution(r); 
  this->PlaneSource->SetYResolution(r); 
}
//----------------------------------------------------------------------------
int medOpMMLContourWidget::GetResolution()
//----------------------------------------------------------------------------
{ 
  return this->PlaneSource->GetXResolution(); 
}
//----------------------------------------------------------------------------
// Description:
// Set/Get the origin of the plane.
void medOpMMLContourWidget::SetOrigin(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->SetOrigin(x,y,z);
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
  return this->PlaneSource->GetOrigin();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetOrigin(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->GetOrigin(xyz);
}
//----------------------------------------------------------------------------
// Description:
// Set/Get the position of the point defining the first axis of the plane.
void medOpMMLContourWidget::SetPoint1(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->SetPoint1(x,y,z);
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
  return this->PlaneSource->GetPoint1();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetPoint1(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->GetPoint1(xyz);
}
//----------------------------------------------------------------------------
// Description:
// Set/Get the position of the point defining the second axis of the plane.
void medOpMMLContourWidget::SetPoint2(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->SetPoint2(x,y,z);
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
  return this->PlaneSource->GetPoint2();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetPoint2(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->GetPoint2(xyz);
}
//----------------------------------------------------------------------------
// Description:
// Set the center of the plane.
void medOpMMLContourWidget::SetCenter(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->SetCenter(x, y, z);
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
  return this->PlaneSource->GetCenter();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetCenter(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->GetCenter(xyz);
}
//----------------------------------------------------------------------------
// Description:
// Set the normal to the plane.
void medOpMMLContourWidget::SetNormal(double x, double y, double z) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->SetNormal(x, y, z);
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
  return this->PlaneSource->GetNormal();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetNormal(double xyz[3]) 
//----------------------------------------------------------------------------
{
  this->PlaneSource->GetNormal(xyz);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::GetPolyData(vtkPolyData *pd)
//----------------------------------------------------------------------------
{ 
  pd->ShallowCopy(this->PlaneSource->GetOutput()); 
}
//----------------------------------------------------------------------------
vtkPolyDataSource *medOpMMLContourWidget::GetPolyDataSource()
//----------------------------------------------------------------------------
{
  return this->PlaneSource;
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
  this->PlaneSource->Update();
  this->PositionHandles(0.5, 0.5);
}
//----------------------------------------------------------------------------
// MK BEGIN 26/05/04
// Description:
// Set the motion vector
void medOpMMLContourWidget::SetMotionVector(float x, float y, float z) 
//----------------------------------------------------------------------------
{
  this->Motion[0] = x;
  this->Motion[1] = y;
  this->Motion[2] = z;
}
//----------------------------------------------------------------------------
// Description:
// Set the motion vector.
void medOpMMLContourWidget::SetMotionVector(float m[3]) 
//----------------------------------------------------------------------------
{
  this->Motion[0] = m[0];
  this->Motion[1] = m[1];
  this->Motion[2] = m[2];
}
//----------------------------------------------------------------------------
// Description:
// Get the motion vector
float* medOpMMLContourWidget::GetMotionVector() 
//----------------------------------------------------------------------------
{
  return this->Motion;
}
//----------------------------------------------------------------------------
// Description:
// Get the motion vector
void medOpMMLContourWidget::GetMotionVector(float xyz[3]) 
//----------------------------------------------------------------------------
{
  xyz[0] = this->Motion[0];
  xyz[1] = this->Motion[1];
  xyz[2] = this->Motion[2];
}
// MK END
//----------------------------------------------------------------------------
void medOpMMLContourWidget::TranslationModeOn()
//----------------------------------------------------------------------------
{
  this->m_bTranslatingMode = TRUE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::TranslationModeOff()
//----------------------------------------------------------------------------
{
  this->m_bTranslatingMode = FALSE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::CenterModeOn()
//----------------------------------------------------------------------------
{
  //this->CenterHandle->VisibilityOn();
  this->m_bCenterMode = TRUE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::CenterModeOff()
//----------------------------------------------------------------------------
{
  //this->CenterHandle->VisibilityOff();
  this->m_bCenterMode = FALSE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::RotationModeOn()
//----------------------------------------------------------------------------
{
  //this->RotationalHandle->VisibilityOn();
  //this->SetRotationalHandle(50.0, 0.0, 0.0);
  this->m_bRotatingMode = TRUE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::RotationModeOff()
//----------------------------------------------------------------------------
{
  //this->RotationalHandle->VisibilityOff();
  this->m_bRotatingMode = FALSE;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScalingModeOn()
//----------------------------------------------------------------------------
{
  this->m_bScalingMode = TRUE;
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
  this->m_bScalingMode = FALSE;

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
  this->HandleGeometry[0]->SetCenter(xyz);
  this->HandleGeometry[0]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle0(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->HandleGeometry[0]->SetCenter(x, y, z);
  this->HandleGeometry[0]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle1(double *xyz)
//----------------------------------------------------------------------------
{
  this->HandleGeometry[1]->SetCenter(xyz);
  this->HandleGeometry[1]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle1(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->HandleGeometry[1]->SetCenter(x, y, z);
  this->HandleGeometry[1]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle2(double *xyz)
//----------------------------------------------------------------------------
{
  this->HandleGeometry[2]->SetCenter(xyz);
  this->HandleGeometry[2]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle2(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->HandleGeometry[2]->SetCenter(x, y, z);
  this->HandleGeometry[2]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle3(double *xyz)
//----------------------------------------------------------------------------
{
  this->HandleGeometry[3]->SetCenter(xyz);
  this->HandleGeometry[3]->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandle3(double x, double y, double z)
//----------------------------------------------------------------------------
{
  this->HandleGeometry[3]->SetCenter(x, y, z);
  this->HandleGeometry[3]->Modified();
}
//----------------------------------------------------------------------------
BOOL medOpMMLContourWidget::GetCenterMode()
//----------------------------------------------------------------------------
{
	return m_bCenterMode;
}
//----------------------------------------------------------------------------
BOOL medOpMMLContourWidget::GetTranslationMode()
//----------------------------------------------------------------------------
{
	return m_bTranslatingMode;
}
//----------------------------------------------------------------------------
BOOL medOpMMLContourWidget::GetRotationMode()
//----------------------------------------------------------------------------
{
	return m_bRotatingMode;
}
//----------------------------------------------------------------------------
BOOL medOpMMLContourWidget::GetScalingMode()
//----------------------------------------------------------------------------
{
	return m_bScalingMode;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetRotationalHandle(double x, double y, double z)
//----------------------------------------------------------------------------
{
	this->RotationalHandleGeometry->SetCenter(x, y, z);
	this->RotationalHandleGeometry->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetRotationalHandle(double *xyz)
//----------------------------------------------------------------------------
{
	this->RotationalHandleGeometry->SetCenter(xyz);
	this->RotationalHandleGeometry->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScalingHandlesOn()
//----------------------------------------------------------------------------
{
  for(int i=0; i<4; i++)
  {
    this->Handle[i]->VisibilityOn();
  }
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::ScalingHandlesOff()
//----------------------------------------------------------------------------
{
  for(int i=0; i<4; i++)
  {
    this->Handle[i]->VisibilityOff();
  }
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetCenterHandle(double x, double y, double z)
//----------------------------------------------------------------------------
{
	this->CenterHandleGeometry->SetCenter(x, y, z);
	this->CenterHandleGeometry->Modified();
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetCenterHandle(double *xyz)
//----------------------------------------------------------------------------
{
	this->CenterHandleGeometry->SetCenter(xyz);
	this->CenterHandleGeometry->Modified();
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetCenterHandleActor()
//----------------------------------------------------------------------------
{
	return CenterHandle;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetRotationalHandleActor()
//----------------------------------------------------------------------------
{
	return RotationalHandle;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetNorthScalingHandleActor()
//----------------------------------------------------------------------------
{
	return Handle[3];
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetSouthScalingHandleActor()
//----------------------------------------------------------------------------
{
	return Handle[0];
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetEastScalingHandleActor()
//----------------------------------------------------------------------------
{
	return Handle[1];
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLContourWidget::GetWestScalingHandleActor()
//----------------------------------------------------------------------------
{
	return Handle[2];
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetHandleRadius(float r)
//----------------------------------------------------------------------------
{
	HandleRadius = r;
}
//----------------------------------------------------------------------------
float medOpMMLContourWidget::GetHandleRadius()
//----------------------------------------------------------------------------
{
	return HandleRadius;
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
		GetRotationalHandleActor()->VisibilityOn();	
	}
	else
	{
		// invisible
		GetRotationalHandleActor()->VisibilityOff();
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
  M->GetContourActor()->GetBounds(b);
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
	double z = M->GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	M->GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = M->GetPHSpline()->Evaluate(z);
	ctrans[1] = M->GetPVSpline()->Evaluate(z);
	
	// get twist
	double twist = M->GetRASpline()->Evaluate(z);

	// get h/v translation
	double trans[2];
	trans[0] = M->GetTHSpline()->Evaluate(z);
	trans[1] = M->GetTVSpline()->Evaluate(z);

	// get scaling
	double scale[4];
	scale[0] = M->GetSESpline()->Evaluate(z); // east
	scale[1] = M->GetSWSpline()->Evaluate(z); // west
	scale[2] = M->GetSNSpline()->Evaluate(z); // north
	scale[3] = M->GetSSSpline()->Evaluate(z); // south;

	// original bounds
	vtkTransform* Transform = vtkTransform::New();
	Transform->Identity();
	
	double SEBounds[6]; // south east
	M->GetSEContourTransformPolyDataFilter()->SetTransform(Transform);
	M->GetSEContourTransformPolyDataFilter()->Update();
	M->GetSEContourTransformPolyDataFilter()->GetOutput()->GetBounds(SEBounds);
	M->UpdateSegmentSouthEastTransform();
	
	double SWBounds[6]; // south west
	M->GetSWContourTransformPolyDataFilter()->SetTransform(Transform);
	M->GetSWContourTransformPolyDataFilter()->Update();
	M->GetSWContourTransformPolyDataFilter()->GetOutput()->GetBounds(SWBounds);
	M->UpdateSegmentSouthWestTransform();
	
	double NEBounds[6]; // north east
	M->GetNEContourTransformPolyDataFilter()->SetTransform(Transform);
	M->GetNEContourTransformPolyDataFilter()->Update();
	M->GetNEContourTransformPolyDataFilter()->GetOutput()->GetBounds(NEBounds);
	M->UpdateSegmentNorthEastTransform();
	
	double NWBounds[6]; // north west
	M->GetNWContourTransformPolyDataFilter()->SetTransform(Transform);
	M->GetNWContourTransformPolyDataFilter()->Update();
	M->GetNWContourTransformPolyDataFilter()->GetOutput()->GetBounds(NWBounds);
	M->UpdateSegmentNorthWestTransform();
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
	double z = M->GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	M->GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = M->GetPHSpline()->Evaluate(z);
	ctrans[1] = M->GetPVSpline()->Evaluate(z);

	SetRotationalHandle(center[0] + ctrans[0], center[1] + ctrans[1], 0.0);
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::UpdateWidgetTransform()
//----------------------------------------------------------------------------
{
	// get z level
	double z = M->GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	M->GetContourActor()->GetBounds(bounds);

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
	ctrans[0] = M->GetPHSpline()->Evaluate(z);
	ctrans[1] = M->GetPVSpline()->Evaluate(z);

	// get twist
	double twist = M->GetRASpline()->Evaluate(z);

	// get h/v translation
	double trans[2];
	trans[0] = M->GetTHSpline()->Evaluate(z);
	trans[1] = M->GetTVSpline()->Evaluate(z);

	// get scaling
	double scale[4];
	scale[0] = M->GetSESpline()->Evaluate(z); // east
	scale[1] = M->GetSWSpline()->Evaluate(z); // west
	scale[2] = M->GetSNSpline()->Evaluate(z); // north
	scale[3] = M->GetSSSpline()->Evaluate(z); // south;

	// original
	double Origin[3];
	double Point1[3];
	double Point2[3];
	if (center[0] == 0.0 && center[1] == 0.0)
	{
		M->GetPositiveXAxisActor()->VisibilityOn();
		M->GetNegativeXAxisActor()->VisibilityOn();
		M->GetPositiveYAxisActor()->VisibilityOn();
		M->GetNegativeYAxisActor()->VisibilityOn();

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
		M->GetPositiveXAxisActor()->VisibilityOff();
		M->GetNegativeXAxisActor()->VisibilityOff();
		M->GetPositiveYAxisActor()->VisibilityOff();
		M->GetNegativeYAxisActor()->VisibilityOff();
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
	PlaneSource->Update();
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
  double s = M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = M->GetCurrentZOfSyntheticScans();

  // get center h translation
  double chtrans = M->GetPHSpline()->Evaluate(z);

  // get center v translation
  double cvtrans = M->GetPVSpline()->Evaluate(z);
  
  // get twist
  double twist = M->GetRASpline()->Evaluate(z);

  // get h translation
  double htrans = M->GetTHSpline()->Evaluate(z);

  // get v translation
  double vtrans = M->GetTVSpline()->Evaluate(z);
  
  // get east scaling
  double escale = M->GetSESpline()->Evaluate(z);

  // get west scaling
  double wscale = M->GetSWSpline()->Evaluate(z);

  // get north scaling
  double nscale = M->GetSNSpline()->Evaluate(z);

  // get south scaling
  double sscale = M->GetSSSpline()->Evaluate(z);

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
  if (scale > N->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < N->GetMinY())
    return;// display information vtrans
  M->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update n spline
  // display values
  N->RemovePoint(s);
  N->AddPoint(s, scale);
  N->Render();

  // actual values
  M->GetSNSpline()->RemovePoint(z);
  M->GetSNSpline()->AddPoint(z, scale);

  // update widget
  UpdateWidgetTransform();
  
  // 
  UpdateScalingHandles();

  // update north east segment
  M->UpdateSegmentNorthEastTransform();

  // update north west segment
  M->UpdateSegmentNorthWestTransform();

  // update south east segment
  M->UpdateSegmentSouthEastTransform();

  // update south west segment
  M->UpdateSegmentSouthWestTransform();
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
  double s = M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = M->GetCurrentZOfSyntheticScans();

  // get center h translation
  double chtrans = M->GetPHSpline()->Evaluate(z);

  // get center v translation
  double cvtrans = M->GetPVSpline()->Evaluate(z);
  
  // get twist
  double twist = M->GetRASpline()->Evaluate(z);

  // get h translation
  double htrans = M->GetTHSpline()->Evaluate(z);

  // get v translation
  double vtrans = M->GetTVSpline()->Evaluate(z);
  
  // get east scaling
  double escale = M->GetSESpline()->Evaluate(z);

  // get west scaling
  double wscale = M->GetSWSpline()->Evaluate(z);

  // get north scaling
  double nscale = M->GetSNSpline()->Evaluate(z);

  // get south scaling
  double sscale = M->GetSSSpline()->Evaluate(z);

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
  if (scale > S->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < S->GetMinY())
    return;

  // display information vtrans
  M->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update s spline
  // display values
  S->RemovePoint(s);
  S->AddPoint(s, scale);
  S->Render();

  // actual values
  M->GetSSSpline()->RemovePoint(z);
  M->GetSSSpline()->AddPoint(z, scale);

  // update widget
  UpdateWidgetTransform();
  
  // 
  UpdateScalingHandles();

  // update north east segment
  M->UpdateSegmentNorthEastTransform();

  // update north west segment
  M->UpdateSegmentNorthWestTransform();

  // update south east segment
  M->UpdateSegmentSouthEastTransform();

  // update south west segment
  M->UpdateSegmentSouthWestTransform();
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
  double s = M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = M->GetCurrentZOfSyntheticScans();

  // get center h translation
  double chtrans = M->GetPHSpline()->Evaluate(z);

  // get center v translation
  double cvtrans = M->GetPVSpline()->Evaluate(z);
  
  // get twist
  double twist = M->GetRASpline()->Evaluate(z);

  // get h translation
  double htrans = M->GetTHSpline()->Evaluate(z);

  // get v translation
  double vtrans = M->GetTVSpline()->Evaluate(z);
  
  // get east scaling
  double escale = M->GetSESpline()->Evaluate(z);

  // get west scaling
  double wscale = M->GetSWSpline()->Evaluate(z);

  // get north scaling
  double nscale = M->GetSNSpline()->Evaluate(z);

  // get south scaling
  double sscale = M->GetSSSpline()->Evaluate(z);

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
  if (scale > E->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < E->GetMinY())
    return;

  // display information vtrans
  M->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update e spline
  // display values
  E->RemovePoint(s);
  E->AddPoint(s, scale);
  E->Render();

  // actual values
  M->GetSESpline()->RemovePoint(z);
  M->GetSESpline()->AddPoint(z, scale);

  // update widget
  UpdateWidgetTransform();
  
  // 
  UpdateScalingHandles();

  // update north east segment
  M->UpdateSegmentNorthEastTransform();

  // update north west segment
  M->UpdateSegmentNorthWestTransform();

  // update south east segment
  M->UpdateSegmentSouthEastTransform();

  // update south west segment
  M->UpdateSegmentSouthWestTransform();
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
  double s = M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = M->GetCurrentZOfSyntheticScans();

  // get center h translation
  double chtrans = M->GetPHSpline()->Evaluate(z);

  // get center v translation
  double cvtrans = M->GetPVSpline()->Evaluate(z);
  
  // get twist
  double twist = M->GetRASpline()->Evaluate(z);

  // get h translation
  double htrans = M->GetTHSpline()->Evaluate(z);

  // get v translation
  double vtrans = M->GetTVSpline()->Evaluate(z);
  
  // get east scaling
  double escale = M->GetSESpline()->Evaluate(z);

  // get west scaling
  double wscale = M->GetSWSpline()->Evaluate(z);

  // get north scaling
  double nscale = M->GetSNSpline()->Evaluate(z);

  // get south scaling
  double sscale = M->GetSSSpline()->Evaluate(z);

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
  if (scale > W->GetMaxY())
    return;

  // exceeded min limits?
  if (scale < W->GetMinY())
    return;

  // display information vtrans
  M->SetText(1, scale, 1, 1); // text actor 1, scaling flag 1

  // update w spline
  // display values
  W->RemovePoint(s);
  W->AddPoint(s, scale);
  W->Render();

  // actual values
  M->GetSWSpline()->RemovePoint(z);
  M->GetSWSpline()->AddPoint(z, scale);

  // update widget
  UpdateWidgetTransform();
  
  // 
  UpdateScalingHandles();

  // update north east segment
  M->UpdateSegmentNorthEastTransform();

  // update north west segment
  M->UpdateSegmentNorthWestTransform();

  // update south east segment
  M->UpdateSegmentSouthEastTransform();

  // update south west segment
  M->UpdateSegmentSouthWestTransform();
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
  double s = M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = M->GetCurrentZOfSyntheticScans();

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = M->GetPHSpline()->Evaluate(z);
  ctrans[1] = M->GetPVSpline()->Evaluate(z);

  // horizontal/vertical displacement
  double h = m[0];
  double v = m[1];

  // exceeded max limits?
  if ((ctrans[0] + h) > CH->GetMaxY())
    return;
  if ((ctrans[1] + v) > CV->GetMaxY())
	return;

  // exceeded min limits?
  if ((ctrans[0] + h) < CH->GetMinY())
	return;
  if ((ctrans[1] + v) < CV->GetMinY())
	return;

  // display information vtrans
  M->SetText(1, ctrans[1] + v, 3, 0); // text actor 1
  
  // display information htrans
  M->SetText(2, ctrans[0] + h, 3, 0); // text actor 2

  // update ch spline
  // display values
  CH->RemovePoint(s);
  CH->AddPoint(s, ctrans[0] + h);
  CH->Render();

  // actual values
  M->GetPHSpline()->RemovePoint(z);
  M->GetPHSpline()->AddPoint(z, ctrans[0] + h);
  
  // update cv spline
  // display values
  CV->RemovePoint(s);
  CV->AddPoint(s, ctrans[1] + v);
  CV->Render();
 
  // actual values
  M->GetPVSpline()->RemovePoint(z);
  M->GetPVSpline()->AddPoint(z, ctrans[1] + v);

  // update widget
  UpdateWidgetTransform();
  
  // update north east segment
  M->UpdateSegmentNorthEastTransform();

  // update north west segment
  M->UpdateSegmentNorthWestTransform();

  // update south east segment
  M->UpdateSegmentSouthEastTransform();

  // update south west segment
  M->UpdateSegmentSouthWestTransform();

  // update contour axes
  M->UpdateContourAxesTransform();

  // update global axes
  M->UpdateGlobalAxesTransform();
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
  double s = M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = M->GetCurrentZOfSyntheticScans();

  // get twist
  double twist = M->GetRASpline()->Evaluate(z);

  // get h/v translation
  double trans[2];
  trans[0] = M->GetTHSpline()->Evaluate(z);
  trans[1] = M->GetTVSpline()->Evaluate(z);

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
  if ((trans[0] + h) > H->GetMaxY())
    return;
  if ((trans[1] + v) > V->GetMaxY())
	return;

  // exceeded min limits?
  if ((trans[0] + h) < H->GetMinY())
	return;
  if ((trans[1] + v) < V->GetMinY())
	return;

  // display information vtrans
  M->SetText(1, trans[1] + v, 3, 0); // text actor 1
  
  // display information htrans
  M->SetText(2, trans[0] + h, 3, 0); // text actor 2

  // update h spline
  // display values
  H->RemovePoint(s);
  H->AddPoint(s, trans[0] + h);
  H->Render();

  // actual values
  M->GetTHSpline()->RemovePoint(z);
  M->GetTHSpline()->AddPoint(z, trans[0] + h);
  
  // update v spline
  // display values
  V->RemovePoint(s);
  V->AddPoint(s, trans[1] + v);
  V->Render();
  
  // actual values
  M->GetTVSpline()->RemovePoint(z);
  M->GetTVSpline()->AddPoint(z, trans[1] + v);

  // update vertical cuts
  M->UpdateSegmentCuttingPlanes();

  // update widget
  UpdateWidgetTransform();
  
  // update north east segment
  M->UpdateSegmentNorthEastTransform();

  // update north west segment
  M->UpdateSegmentNorthWestTransform();

  // update south east segment
  M->UpdateSegmentSouthEastTransform();

  // update south west segment
  M->UpdateSegmentSouthWestTransform();

  // update contour axes
  M->UpdateContourAxesTransform();

  // update global axes
  M->UpdateGlobalAxesTransform();
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
  double s = M->GetCurrentIdOfSyntheticScans();

  // get z level
  double z = M->GetCurrentZOfSyntheticScans();

  // get twist
  double twist = M->GetRASpline()->Evaluate(z);

  // rotation angle
  double theta;
  theta = v[1]; // y displacement

  // exceeded max limit?
  if ((twist + theta) > T->GetMaxY())
    return;

  // exceeded min limit?
  if ((twist + theta) < T->GetMinY())
	return;

  // display information
  M->SetText(1, twist + theta, 2, 0);

  // update twist spline
  // display values
  T->RemovePoint(s);
  T->AddPoint(s, twist + theta);
  T->Render();

  // actual values
  M->GetRASpline()->RemovePoint(z);
  M->GetRASpline()->AddPoint(z, twist + theta);

  // update widget
  UpdateWidgetTransform();
  
  // update north east segment
  M->UpdateSegmentNorthEastTransform();

  // update north west segment
  M->UpdateSegmentNorthWestTransform();

  // update south east segment
  M->UpdateSegmentSouthEastTransform();

  // update south west segment
  M->UpdateSegmentSouthWestTransform();

  // update contour axes
  M->UpdateContourAxesTransform();

  // update global axes
  M->UpdateGlobalAxesTransform();
}
//----------------------------------------------------------------------------
int medOpMMLContourWidget::GetNextOperationId()
//----------------------------------------------------------------------------
{
  return OperationID;
}
//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetNextOperationId(int n)
//----------------------------------------------------------------------------
{
  OperationID = n;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetModel(medOpMMLModelView *Model)
//----------------------------------------------------------------------------
{
	M = Model;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetPH(medOpMMLParameterView *PH)
//----------------------------------------------------------------------------
{
	CH = PH;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetPV(medOpMMLParameterView *PV)
//----------------------------------------------------------------------------
{
	CV = PV;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetTH(medOpMMLParameterView *TH)
//----------------------------------------------------------------------------
{
	H = TH;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetTV(medOpMMLParameterView *TV)
//----------------------------------------------------------------------------
{
	V = TV;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetRA(medOpMMLParameterView *RA)
//----------------------------------------------------------------------------
{
	T = RA;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetSN(medOpMMLParameterView *SN)
//----------------------------------------------------------------------------
{
	N = SN;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetSS(medOpMMLParameterView *SS)
//----------------------------------------------------------------------------
{
	S = SS;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetSE(medOpMMLParameterView *SE)
//----------------------------------------------------------------------------
{
	E = SE;
}

//----------------------------------------------------------------------------
void medOpMMLContourWidget::SetSW(medOpMMLParameterView *SW)
//----------------------------------------------------------------------------
{
	W = SW;
}
