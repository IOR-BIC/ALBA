/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLODActor.cpp,v $
  Language:  C++
  Date:      $Date: 2006-07-17 14:58:36 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani & Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <assert.h>
#include <math.h>
#include <GL/gl.h>

#include "mafLODActor.h"

#include "vtkMapper.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderer.h"
#include "vtkProperty.h"

#include "vtkPointSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkMath.h"

vtkCxxRevisionMacro(mafLODActor, "$Revision: 1.1 $");
vtkStandardNewMacro(mafLODActor);

//------------------------------------------------------------------------
mafLODActor::mafLODActor()
//------------------------------------------------------------------------
{
  PS = vtkPointSource::New();
  PS->SetCenter(0 , 0 , 0);
  PS->SetNumberOfPoints(1);
  PS->SetRadius(0.000000001);
  PS->SetDistribution(1);

  PDM = vtkPolyDataMapper::New();
  PDM->SetInput(PS->GetOutput());
  PDM->SetScalarVisibility(0);

  A = vtkActor::New();
  A->SetMapper(PDM);
  A->GetProperty()->SetAmbient(1);
  A->GetProperty()->SetAmbientColor(1 , 0 , 0);
  A->GetProperty()->SetDiffuse(1);
  A->GetProperty()->SetDiffuseColor(1 , 0 , 0);
  A->GetProperty()->SetPointSize(10);  //SIL. 17-jul-2006 : not working ? - we call glPointSize directly
  A->GetProperty()->SetInterpolation(0);
  A->GetProperty()->SetRepresentationToPoints();
  A->SetPosition(0 , 0 , 0);
  A->SetVisibility(1);
}
//------------------------------------------------------------------------
mafLODActor::~mafLODActor()
//------------------------------------------------------------------------
{
  PS->Delete();
  PDM->Delete();
  A->Delete();
}
// Actual actor render method.
//------------------------------------------------------------------------
void mafLODActor::Render(vtkRenderer *ren, vtkMapper *mapper)
//------------------------------------------------------------------------
{
  //risalire al dato
  vtkDataSet *ds = mapper->GetInput();
  if( ds == NULL) return;
  
  double *bounds = ds->GetBounds();
  assert(bounds);
 
  double p1[4],p2[4],d1[4],d2[4],sz;
  p1[0]=bounds[0];
  p1[1]=bounds[2];
  p1[2]=bounds[4];
  p1[3]=1;
  
  p2[0]=bounds[1];
  p2[1]=bounds[3];
  p2[2]=bounds[5];
  p2[3]=1;

  ren->SetWorldPoint(p1);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(d1);
  
  ren->SetWorldPoint(p2);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(d2);
  
  sz = sqrt( vtkMath::Distance2BetweenPoints(d1,d2) );

  if( sz < 50)
  {
    A->SetPosition(ds->GetCenter()); 

    //GLfloat size = 4;
    glPointSize( 4 );

    A->Render(ren,PDM); 

    //size = 1;
    glPointSize( 1 );

    //vtkOpenGLActor::Render(ren, PDM);
  }
  else
  {
    vtkOpenGLActor::Render(ren, mapper);
  }
}




