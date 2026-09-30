/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAAnnotatedCubeActor.cpp,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAAnnotatedCubeActor.h"

#include "vtkActor.h"
#include "vtkAppendPolyData.h"
#include "vtkAssembly.h"
#include "vtkCubeSource.h"
#include "vtkFeatureEdges.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkPropCollection.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkVectorText.h"
#include "vcl_cassert.h"

vtkStandardNewMacro(vtkALBAAnnotatedCubeActor);

//-------------------------------------------------------------------------
vtkALBAAnnotatedCubeActor::vtkALBAAnnotatedCubeActor()
{
  this->FaceTextScale  = 0.5;
  this->XPlusFaceText  = NULL;
  this->XMinusFaceText = NULL;
  this->YPlusFaceText  = NULL;
  this->YMinusFaceText = NULL;
  this->ZPlusFaceText  = NULL;
  this->ZMinusFaceText = NULL;

  this->Assembly = vtkAssembly::New();

  this->CubeSource = vtkCubeSource::New();
  this->CubeSource->SetBounds(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
  this->CubeSource->SetCenter(0, 0, 0);

  vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
  this->CubeActor = vtkActor::New();
  cubeMapper->SetInputConnection( this->CubeSource->GetOutputPort() );
  this->CubeActor->SetMapper( cubeMapper );
  cubeMapper->Delete();

  this->Assembly->AddPart( this->CubeActor );

  vtkProperty* prop = this->CubeActor->GetProperty();
  prop->SetRepresentationToSurface();
  prop->SetColor(1, 1, 1);
  prop->SetLineWidth(1);

  this->SetXPlusFaceText ( "L" );
  this->SetXMinusFaceText( "R" );
  this->SetYPlusFaceText ( "P" );
  this->SetYMinusFaceText( "A" );
  this->SetZPlusFaceText ( "S" );
  this->SetZMinusFaceText( "I" );

  this->XPlusFaceVectorText  = vtkVectorText::New();
  this->XMinusFaceVectorText = vtkVectorText::New();
  this->YPlusFaceVectorText  = vtkVectorText::New();
  this->YMinusFaceVectorText = vtkVectorText::New();
  this->ZPlusFaceVectorText  = vtkVectorText::New();
  this->ZMinusFaceVectorText = vtkVectorText::New();

  vtkPolyDataMapper *xplusMapper  = vtkPolyDataMapper::New();
  vtkPolyDataMapper *xminusMapper = vtkPolyDataMapper::New();
  vtkPolyDataMapper *yplusMapper  = vtkPolyDataMapper::New();
  vtkPolyDataMapper *yminusMapper = vtkPolyDataMapper::New();
  vtkPolyDataMapper *zplusMapper  = vtkPolyDataMapper::New();
  vtkPolyDataMapper *zminusMapper = vtkPolyDataMapper::New();

  xplusMapper->SetInputConnection ( this->XPlusFaceVectorText->GetOutputPort() );
  xminusMapper->SetInputConnection( this->XMinusFaceVectorText->GetOutputPort() );
  yplusMapper->SetInputConnection ( this->YPlusFaceVectorText->GetOutputPort() );
  yminusMapper->SetInputConnection( this->YMinusFaceVectorText->GetOutputPort() );
  zplusMapper->SetInputConnection ( this->ZPlusFaceVectorText->GetOutputPort() );
  zminusMapper->SetInputConnection( this->ZMinusFaceVectorText->GetOutputPort() );

  this->XPlusFaceActor  = vtkActor::New();
  this->XMinusFaceActor = vtkActor::New();
  this->YPlusFaceActor  = vtkActor::New();
  this->YMinusFaceActor = vtkActor::New();
  this->ZPlusFaceActor  = vtkActor::New();
  this->ZMinusFaceActor = vtkActor::New();

  this->XPlusFaceActor-> SetMapper( xplusMapper );
  this->XMinusFaceActor->SetMapper( xminusMapper );
  this->YPlusFaceActor-> SetMapper( yplusMapper );
  this->YMinusFaceActor->SetMapper( yminusMapper );
  this->ZPlusFaceActor-> SetMapper( zplusMapper );
  this->ZMinusFaceActor->SetMapper( zminusMapper );

  xplusMapper->Delete();
  xminusMapper->Delete();
  yplusMapper->Delete();
  yminusMapper->Delete();
  zplusMapper->Delete();
  zminusMapper->Delete();

  this->Assembly->AddPart( this->XPlusFaceActor );
  this->Assembly->AddPart( this->XMinusFaceActor );
  this->Assembly->AddPart( this->YPlusFaceActor );
  this->Assembly->AddPart( this->YMinusFaceActor );
  this->Assembly->AddPart( this->ZPlusFaceActor );
  this->Assembly->AddPart( this->ZMinusFaceActor );

  prop = this->XPlusFaceActor->GetProperty();
  prop->SetColor(1, 1, 1);
  prop->SetDiffuse(0);
  prop->SetAmbient(1);
  prop->BackfaceCullingOn();
  this->XMinusFaceActor->GetProperty()->DeepCopy( prop );
  this->YPlusFaceActor-> GetProperty()->DeepCopy( prop );
  this->YMinusFaceActor->GetProperty()->DeepCopy( prop );
  this->ZPlusFaceActor-> GetProperty()->DeepCopy( prop );
  this->ZMinusFaceActor->GetProperty()->DeepCopy( prop );

  this->AppendTextEdges = vtkAppendPolyData::New();
  this->AppendTextEdges->UserManagedInputsOn();
  this->AppendTextEdges->SetNumberOfInputs(6);

  for (int i = 0; i < 6; i++)
    {
    vtkPolyData *edges = vtkPolyData::New();
    this->AppendTextEdges->SetInputDataByNumber(i,edges);
    edges->Delete();
    }

  this->ExtractTextEdges = vtkFeatureEdges::New();
  this->ExtractTextEdges->BoundaryEdgesOn();
  this->ExtractTextEdges->ColoringOff();
  this->ExtractTextEdges->SetInputConnection( this->AppendTextEdges->GetOutputPort() );

  vtkPolyDataMapper* edgesMapper = vtkPolyDataMapper::New();
  edgesMapper->SetInputConnection( this->ExtractTextEdges->GetOutputPort() );

  this->TextEdgesActor = vtkActor::New();
  this->TextEdgesActor->SetMapper( edgesMapper );
  edgesMapper->Delete();

  this->Assembly->AddPart( this->TextEdgesActor );

  prop = this->TextEdgesActor->GetProperty();
  prop->SetRepresentationToWireframe();
  prop->SetColor(1,0.5,0);
  prop->SetDiffuse(0);
  prop->SetAmbient(1);
  prop->SetLineWidth(1);

  this->TransformFilter = vtkTransformFilter::New();
  this->Transform = vtkTransform::New();
  this->TransformFilter->SetTransform( this->Transform );

  this->XFaceTextRotation = 0.0;
  this->YFaceTextRotation = 0.0;
  this->ZFaceTextRotation = 0.0;

  this->UpdateProps();
}

//-------------------------------------------------------------------------
vtkALBAAnnotatedCubeActor::~vtkALBAAnnotatedCubeActor()
{
  this->CubeSource->Delete();
  this->CubeActor->Delete();

  this->SetXPlusFaceText ( NULL );
  this->SetXMinusFaceText( NULL );
  this->SetYPlusFaceText ( NULL );
  this->SetYMinusFaceText( NULL );
  this->SetZPlusFaceText ( NULL );
  this->SetZMinusFaceText( NULL );

  this->XPlusFaceVectorText->Delete();
  this->XMinusFaceVectorText->Delete();
  this->YPlusFaceVectorText->Delete();
  this->YMinusFaceVectorText->Delete();
  this->ZPlusFaceVectorText->Delete();
  this->ZMinusFaceVectorText->Delete();

  this->XPlusFaceActor->Delete();
  this->XMinusFaceActor->Delete();
  this->YPlusFaceActor->Delete();
  this->YMinusFaceActor->Delete();
  this->ZPlusFaceActor->Delete();
  this->ZMinusFaceActor->Delete();

  this->AppendTextEdges->Delete();
  this->ExtractTextEdges->Delete();
  this->TextEdgesActor->Delete();

  this->TransformFilter->Delete();
  this->Transform->Delete();

  this->Assembly->Delete();
}

//-------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::SetTextEdgesVisibility(int vis)
{
  this->TextEdgesActor->SetVisibility(vis);
  this->Assembly->Modified();
}

//-------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::SetCubeVisibility(int vis)
{
  this->CubeActor->SetVisibility(vis);
  this->Assembly->Modified();
}

//-------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::SetFaceTextVisibility(int vis)
{
  this->XPlusFaceActor->SetVisibility(vis);
  this->XMinusFaceActor->SetVisibility(vis);
  this->YPlusFaceActor->SetVisibility(vis);
  this->YMinusFaceActor->SetVisibility(vis);
  this->ZPlusFaceActor->SetVisibility(vis);
  this->ZMinusFaceActor->SetVisibility(vis);
  this->Assembly->Modified();
}

//-------------------------------------------------------------------------
int vtkALBAAnnotatedCubeActor::GetTextEdgesVisibility()
{
  return this->TextEdgesActor->GetVisibility();
}

//-------------------------------------------------------------------------
int vtkALBAAnnotatedCubeActor::GetCubeVisibility()
{
  return this->CubeActor->GetVisibility();
}

//-------------------------------------------------------------------------
int vtkALBAAnnotatedCubeActor::GetFaceTextVisibility()
{
 // either they are all visible or not, so one response will do
  return this->XPlusFaceActor->GetVisibility();
}

//-------------------------------------------------------------------------
// Shallow copy of a vtkAnnotatedCubeActor.
void vtkALBAAnnotatedCubeActor::ShallowCopy(vtkProp *prop)
{
  vtkALBAAnnotatedCubeActor *a = vtkALBAAnnotatedCubeActor::SafeDownCast(prop);
  if ( a != NULL )
    {
    this->SetXPlusFaceText( a->GetXPlusFaceText() );
    this->SetXMinusFaceText( a->GetXMinusFaceText() );
    this->SetYPlusFaceText( a->GetYPlusFaceText() );
    this->SetYMinusFaceText( a->GetYMinusFaceText() );
    this->SetZPlusFaceText( a->GetZPlusFaceText() );
    this->SetZMinusFaceText( a->GetZMinusFaceText() );
    this->SetFaceTextScale( a->GetFaceTextScale() );
    }

  // Now do superclass
  this->vtkProp3D::ShallowCopy(prop);
}

//-------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::GetActors(vtkPropCollection *ac)
{
  this->Assembly->GetActors( ac );
}

//-------------------------------------------------------------------------
int vtkALBAAnnotatedCubeActor::RenderOpaqueGeometry(vtkViewport *vp)
{
  this->UpdateProps();

  return this->Assembly->RenderOpaqueGeometry(vp);
}

//-----------------------------------------------------------------------------
int vtkALBAAnnotatedCubeActor::RenderTranslucentPolygonalGeometry(vtkViewport *vp)
{
  this->UpdateProps();

  //return this->Assembly->RenderTranslucentPolygonalGeometry( vp );

  assert(false);
  return false;
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkALBAAnnotatedCubeActor::HasTranslucentPolygonalGeometry()
{
  this->UpdateProps();

  //return this->Assembly->HasTranslucentPolygonalGeometry();
  assert(false);
  return false;
}

//-----------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::ReleaseGraphicsResources(vtkWindow *win)
{
  this->Assembly->ReleaseGraphicsResources( win );
}

//-------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::GetBounds(double bounds[6])
{
  this->Assembly->GetBounds( bounds );
}

//-------------------------------------------------------------------------
// Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
double *vtkALBAAnnotatedCubeActor::GetBounds()
{
  return this->Assembly->GetBounds( );
}

//-------------------------------------------------------------------------
vtkMTimeType vtkALBAAnnotatedCubeActor::GetMTime()
{
  return this->Assembly->GetMTime();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAAnnotatedCubeActor::GetXPlusFaceProperty()
{
  return this->XPlusFaceActor->GetProperty();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAAnnotatedCubeActor::GetXMinusFaceProperty()
{
  return this->XMinusFaceActor->GetProperty();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAAnnotatedCubeActor::GetYPlusFaceProperty()
{
  return this->YPlusFaceActor->GetProperty();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAAnnotatedCubeActor::GetYMinusFaceProperty()
{
  return this->YMinusFaceActor->GetProperty();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAAnnotatedCubeActor::GetZPlusFaceProperty()
{
  return this->ZPlusFaceActor->GetProperty();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAAnnotatedCubeActor::GetZMinusFaceProperty()
{
  return this->ZMinusFaceActor->GetProperty();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAAnnotatedCubeActor::GetCubeProperty()
{
  return this->CubeActor->GetProperty();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAAnnotatedCubeActor::GetTextEdgesProperty()
{
  return this->TextEdgesActor->GetProperty();
}

//-------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::SetFaceTextScale(double scale)
{
  if ( this->FaceTextScale == scale )
    {
    return;
    }
  this->FaceTextScale = scale;
  this->UpdateProps();
}

//-------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::UpdateProps()
{
  this->XPlusFaceVectorText-> SetText( this->XPlusFaceText );
  this->XMinusFaceVectorText->SetText( this->XMinusFaceText );
  this->YPlusFaceVectorText-> SetText( this->YPlusFaceText );
  this->YMinusFaceVectorText->SetText( this->YMinusFaceText );
  this->ZPlusFaceVectorText-> SetText( this->ZPlusFaceText );
  this->ZMinusFaceVectorText->SetText( this->ZMinusFaceText );

  vtkProperty* prop = this->CubeActor->GetProperty();

  // Place the text slightly offset from the cube face to prevent
  // rendering problems when the cube is in surface render mode.
  double offset = (prop->GetRepresentation() == VTK_SURFACE)? (0.501) : (0.5);

  this->XPlusFaceVectorText->Update();
  double* bounds = this->XPlusFaceVectorText->GetOutput()->GetBounds();
  double cu = -this->FaceTextScale*fabs(0.5*(bounds[0] + bounds[1]));
  double cv = -this->FaceTextScale*fabs(0.5*(bounds[2] + bounds[3]));

  this->XPlusFaceActor->SetScale( this->FaceTextScale );
  this->XPlusFaceActor->SetPosition( offset, cu, cv );
  this->XPlusFaceActor->SetOrientation( 90 , 0, 90 );

  this->XMinusFaceVectorText->Update();
  bounds = this->XMinusFaceVectorText->GetOutput()->GetBounds();
  cu = this->FaceTextScale*fabs(0.5*(bounds[0] + bounds[1]));
  cv = -this->FaceTextScale*fabs(0.5*(bounds[2] + bounds[3]));

  this->XMinusFaceActor->SetScale( this->FaceTextScale );
  this->XMinusFaceActor->SetPosition( -offset, cu, cv );
  this->XMinusFaceActor->SetOrientation( 90 , 0, -90 );

  if ( this->XFaceTextRotation != 0.0 )
    {
    vtkTransform* transform = vtkTransform::New();
    transform->Identity();
    transform->RotateX( this->XFaceTextRotation );
    this->XPlusFaceActor->SetUserTransform( transform );
    this->XMinusFaceActor->SetUserTransform( transform );
    transform->Delete();
    }

  this->YPlusFaceVectorText->Update();
  bounds = this->YPlusFaceVectorText->GetOutput()->GetBounds();
  cu = this->FaceTextScale*0.5*(bounds[0] + bounds[1]);
  cv = -this->FaceTextScale*0.5*(bounds[2] + bounds[3]);

  this->YPlusFaceActor->SetScale( this->FaceTextScale );
  this->YPlusFaceActor->SetPosition( cu, offset, cv );
  this->YPlusFaceActor->SetOrientation( 90, 0, 180 );

  this->YMinusFaceVectorText->Update();
  bounds = this->YMinusFaceVectorText->GetOutput()->GetBounds();
  cu = -this->FaceTextScale*0.5*(bounds[0] + bounds[1]);
  cv = -this->FaceTextScale*0.5*(bounds[2] + bounds[3]);

  this->YMinusFaceActor->SetScale( this->FaceTextScale );
  this->YMinusFaceActor->SetPosition( cu, -offset, cv );
  this->YMinusFaceActor->SetOrientation( 90, 0, 0 );

  if ( this->YFaceTextRotation != 0.0 )
    {
    vtkTransform* transform = vtkTransform::New();
    transform->Identity();
    transform->RotateY( this->YFaceTextRotation );
    this->YPlusFaceActor->SetUserTransform( transform );
    this->YMinusFaceActor->SetUserTransform( transform );
    transform->Delete();
    }

  this->ZPlusFaceVectorText->Update();
  bounds = this->ZPlusFaceVectorText->GetOutput()->GetBounds();
  cu = this->FaceTextScale*0.5*(bounds[0] + bounds[1]);
  cv = -this->FaceTextScale*0.5*(bounds[2] + bounds[3]);

  this->ZPlusFaceActor->SetScale( this->FaceTextScale );
  this->ZPlusFaceActor->SetPosition( cv, cu, offset );
  this->ZPlusFaceActor->SetOrientation( 0, 0, -90 );

  this->ZMinusFaceVectorText->Update();
  bounds = this->ZMinusFaceVectorText->GetOutput()->GetBounds();
  cu = -this->FaceTextScale*0.5*(bounds[0] + bounds[1]);
  cv = -this->FaceTextScale*0.5*(bounds[2] + bounds[3]);

  this->ZMinusFaceActor->SetScale( this->FaceTextScale );
  this->ZMinusFaceActor->SetPosition( cv, cu, -offset );
  this->ZMinusFaceActor->SetOrientation( 180, 0, 90 );

  if ( this->ZFaceTextRotation != 0.0 )
    {
    vtkTransform* transform = vtkTransform::New();
    transform->Identity();
    transform->RotateZ( this->ZFaceTextRotation );
    this->ZPlusFaceActor->SetUserTransform( transform );
    this->ZMinusFaceActor->SetUserTransform( transform );
    transform->Delete();
    }

  this->XPlusFaceActor->ComputeMatrix();
  this->TransformFilter->SetInputConnection( this->XPlusFaceVectorText->GetOutputPort() );
  this->Transform->SetMatrix( this->XPlusFaceActor->GetMatrix() );
  this->TransformFilter->Update();
  vtkPolyData* edges = this->AppendTextEdges->GetInput( 0 );
  edges->CopyStructure( this->TransformFilter->GetOutput() );

  this->XMinusFaceActor->ComputeMatrix();
  this->TransformFilter->SetInputConnection( this->XMinusFaceVectorText->GetOutputPort() );
  this->Transform->SetMatrix( this->XMinusFaceActor->GetMatrix() );
  this->TransformFilter->Update();
  edges = this->AppendTextEdges->GetInput( 1 );
  edges->CopyStructure( this->TransformFilter->GetOutput() );

  this->YPlusFaceActor->ComputeMatrix();
  this->TransformFilter->SetInputConnection( this->YPlusFaceVectorText->GetOutputPort() );
  this->Transform->SetMatrix( this->YPlusFaceActor->GetMatrix() );
  this->TransformFilter->Update();
  edges = this->AppendTextEdges->GetInput( 2 );
  edges->CopyStructure( this->TransformFilter->GetOutput() );

  this->YMinusFaceActor->ComputeMatrix();
  this->TransformFilter->SetInputConnection( this->YMinusFaceVectorText->GetOutputPort() );
  this->Transform->SetMatrix( this->YMinusFaceActor->GetMatrix() );
  this->TransformFilter->Update();
  edges = this->AppendTextEdges->GetInput( 3 );
  edges->CopyStructure( this->TransformFilter->GetOutput() );

  this->ZPlusFaceActor->ComputeMatrix();
  this->TransformFilter->SetInputConnection( this->ZPlusFaceVectorText->GetOutputPort() );
  this->Transform->SetMatrix( this->ZPlusFaceActor->GetMatrix() );
  this->TransformFilter->Update();
  edges = this->AppendTextEdges->GetInput( 4 );
  edges->CopyStructure(this->TransformFilter->GetOutput());

  this->ZMinusFaceActor->ComputeMatrix();
  this->TransformFilter->SetInputConnection( this->ZMinusFaceVectorText->GetOutputPort() );
  this->Transform->SetMatrix( this->ZMinusFaceActor->GetMatrix() );
  this->TransformFilter->Update();
  edges = this->AppendTextEdges->GetInput( 5 );
  edges->CopyStructure( this->TransformFilter->GetOutput() );
}

//-------------------------------------------------------------------------
void vtkALBAAnnotatedCubeActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "XPlusFaceText: " << (this->XPlusFaceText ?
                                         this->XPlusFaceText : "(none)")
     << endl;

  os << indent << "XMinusFaceText: " << (this->XMinusFaceText ?
                                         this->XMinusFaceText : "(none)")
     << endl;

  os << indent << "YPlusFaceText: " << (this->YPlusFaceText ?
                                         this->YPlusFaceText : "(none)")
     << endl;

  os << indent << "YMinusFaceText: " << (this->YMinusFaceText ?
                                         this->YMinusFaceText : "(none)")
     << endl;

  os << indent << "ZPlusFaceText: " << (this->ZPlusFaceText ?
                                         this->ZPlusFaceText : "(none)")
     << endl;

  os << indent << "ZMinusFaceText: " << (this->ZMinusFaceText ?
                                         this->ZMinusFaceText : "(none)")
     << endl;

  os << indent << "FaceTextScale: " << this->FaceTextScale << endl;

  os << indent << "XFaceTextRotation: " << this->XFaceTextRotation << endl;

  os << indent << "YFaceTextRotation: " << this->YFaceTextRotation << endl;

  os << indent << "ZFaceTextRotation: " << this->ZFaceTextRotation << endl;
}

