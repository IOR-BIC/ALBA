/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLODActor
 Authors: Paolo Quadrani & Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <assert.h>
#include <math.h>
#include <GL/gl.h>

#include "albaLODActor.h"

#include "vtkMapper.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderer.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkPointSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkMath.h"
#include "vtkCamera.h"

vtkStandardNewMacro(albaLODActor);

//------------------------------------------------------------------------
albaLODActor::albaLODActor()
//------------------------------------------------------------------------
{
  m_EnableFading = 1;
  m_EnableHighThreshold = 1;
  m_FlagDimension = 10;
  m_PixelThreshold = 20;

  m_FlagShape = vtkPointSource::New();
  m_FlagShape->SetCenter(0 , 0 , 0);
  m_FlagShape->SetNumberOfPoints(1);
  m_FlagShape->SetRadius(0.000000001);
  m_FlagShape->SetDistribution(1);

  m_FlagMapper = vtkPolyDataMapper::New();
  m_FlagMapper->SetInputConnection(m_FlagShape->GetOutputPort());
  m_FlagMapper->SetScalarVisibility(0);

  m_FlagActor = vtkActor::New();
  m_FlagActor->SetMapper(m_FlagMapper);
  m_FlagActor->GetProperty()->SetAmbient(1);
  m_FlagActor->GetProperty()->SetAmbientColor(1 , 0 , 0);
  m_FlagActor->GetProperty()->SetDiffuse(1);
  m_FlagActor->GetProperty()->SetDiffuseColor(1 , 0 , 0);
  m_FlagActor->GetProperty()->SetPointSize(10);  //SIL. 17-jul-2006 : not working ? - we call glPointSize directly
  m_FlagActor->GetProperty()->SetInterpolation(0);
  m_FlagActor->GetProperty()->SetRepresentationToPoints();
  m_FlagActor->SetVisibility(1);
}
//------------------------------------------------------------------------
albaLODActor::~albaLODActor()
//------------------------------------------------------------------------
{
  m_FlagShape->Delete();
  m_FlagMapper->Delete();
  m_FlagActor->Delete();
}
// Actual actor render method.
//------------------------------------------------------------------------
/*void albaLODActor::Render(vtkRenderer *ren, vtkMapper *mapper)
//------------------------------------------------------------------------
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  vtkDataSet *ds = mapper->GetInput();
  if( ds == NULL) return;
  
  double *bounds = ds->GetBounds();
  assert(bounds);
 
  double o[4], p1[4], p2[4], p3[4];
  double d[3], d1[3], d2[3], d3[3];
  double sz, szx, szy, szz;

  o[0]  = bounds[0];
  o[1]  = bounds[2];
  o[2]  = bounds[4];
  o[3]  = 1;

  p1[0] = bounds[1];
  p1[1] = bounds[2];
  p1[2] = bounds[4];
  p1[3] = 1;
  
  p2[0] = bounds[0];
  p2[1] = bounds[3];
  p2[2] = bounds[4];
  p2[3] = 1;
  
  p3[0] = bounds[0];
  p3[1] = bounds[2];
  p3[2] = bounds[5];
  p3[3] = 1;

  ren->SetWorldPoint(o);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(d);

  ren->SetWorldPoint(p1);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(d1);

  ren->SetWorldPoint(p2);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(d2);

  ren->SetWorldPoint(p3);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(d3);

  double size_vect[3];
  int w,h, mindim, maxdim;
  w = ren->GetRenderWindow()->GetSize()[0];
  h = ren->GetRenderWindow()->GetSize()[1];
  size_vect[0] = w;
  size_vect[1] = h;
  size_vect[2] = 0.0;
  mindim = w>h ? w : h;
  maxdim = (int)(vtkMath::Norm(size_vect) * 1.5);
  
  szx = sqrt( vtkMath::Distance2BetweenPoints(o,d1) );
  szy = sqrt( vtkMath::Distance2BetweenPoints(o,d2) );
  szz = sqrt( vtkMath::Distance2BetweenPoints(o,d3) );

  if (szx > szy)
  {
    sz = szx > szz ? szx : szz;
  }
  else
  {
    sz = szy > szz ? szy : szz;
  }

  if (EnableFading)
  {
    // fade the actor by changing its opacity
    if (sz >= mindim && sz <= maxdim)
    {
      double opacity = 1 - (sz - mindim) / (double)(maxdim - mindim);
      opacity = (opacity < 0 ) ? 0.0 : opacity;

      glEnable(GL_BLEND);
      glEnable(GL_LIGHTING);
      glEnable(GL_DEPTH_TEST);

      glShadeModel(GL_SMOOTH);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
      float ambientColor4[4], diffuseColor4[4], specularColor4[4];
      glGetMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor4);
      glGetMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor4);
      glGetMaterialfv(GL_FRONT, GL_SPECULAR, specularColor4);
      ambientColor4[3] = opacity;
      diffuseColor4[3] = opacity;
      specularColor4[3] = opacity;
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  ambientColor4);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  diffuseColor4);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor4);
    }
  }
  if( sz < PixelThreshold)
  {
    // Assign the actor's transformation matrix to the FlagActor 
    // to put this one at the same position of the actor
    FlagActor->PokeMatrix(GetMatrix());

    glPointSize( FlagDimension );
    FlagActor->Render(ren,FlagMapper); 
    glPointSize( 1 );
  }
  else if (sz > maxdim)
  {
    // no draw at all
  }
  else
  {
    vtkOpenGLActor::Render(ren, mapper);
  }
  
  glPopAttrib();
}
*/
//------------------------------------------------------------------------
void albaLODActor::Render(vtkRenderer *ren, vtkMapper *mapper)
//------------------------------------------------------------------------
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  vtkDataSet *ds = mapper->GetInput();
  if( ds == NULL) return;

  double *bounds = ds->GetBounds();
  assert(bounds);

  double p1[4], p2[4];
  double d1[3], d2[3];
  double sz;

  p1[0]  = bounds[0];
  p1[1]  = bounds[2];
  p1[2]  = bounds[4];
  p1[3]  = 1;

  p2[0] = bounds[1];
  p2[1] = bounds[3];
  p2[2] = bounds[5];
  p2[3] = 1;

  ren->SetWorldPoint(p1);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(d1);

  ren->SetWorldPoint(p2);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(d2);

  double size_vect[3];
  int w,h, mindim, maxdim;

  w = ren->GetRenderWindow()->GetSize()[0];
  h = ren->GetRenderWindow()->GetSize()[1];
  size_vect[0] = w;
  size_vect[1] = h;
  size_vect[2] = 0.0;
  mindim = w>h ? w : h;
  maxdim = (int)(vtkMath::Norm(size_vect) * 2.0);

  sz = sqrt( vtkMath::Distance2BetweenPoints(d1,d2) );

  if (m_EnableFading)
  {
    // fade the actor by changing its opacity
    if (sz >= mindim && sz <= maxdim && m_EnableHighThreshold)
    {
      double opacity = 1 - (sz - mindim) / (double)(maxdim - mindim);
      opacity = (opacity < 0 ) ? 0.0 : opacity;

      glEnable(GL_BLEND);
      glEnable(GL_LIGHTING);
      glEnable(GL_DEPTH_TEST);

      glShadeModel(GL_SMOOTH);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
      float ambientColor4[4], diffuseColor4[4], specularColor4[4];
      glGetMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor4);
      glGetMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor4);
      glGetMaterialfv(GL_FRONT, GL_SPECULAR, specularColor4);
      ambientColor4[3] = opacity;
      diffuseColor4[3] = opacity;
      specularColor4[3] = opacity;
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  ambientColor4);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  diffuseColor4);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor4);
    }
  }
  if( sz < m_PixelThreshold)
  {
    // Assign the actor's transformation matrix to the m_FlagActor 
    // to put this one at the same position of the actor
    m_FlagActor->PokeMatrix(GetMatrix());

    glPointSize( m_FlagDimension );
    m_FlagActor->Render(ren,m_FlagMapper); 
    glPointSize( 1 );
  }
  else if (sz > maxdim && m_EnableHighThreshold)
  {
    // no draw at all
  }
  else
  {
    vtkOpenGLActor::Render(ren, mapper);
  }
  glPopAttrib();
}
//------------------------------------------------------------------------
void albaLODActor::SetPixelThreshold(int pixelThreshold)
//------------------------------------------------------------------------
{
  int min = 1;
  int max = VTK_INT_MAX;
  if (this->m_PixelThreshold != (pixelThreshold<min?min:(pixelThreshold>max?max:pixelThreshold)))
  {
    this->m_PixelThreshold = (pixelThreshold<min?min:(pixelThreshold>max?max:pixelThreshold));
    this->Modified();
  }
}
//------------------------------------------------------------------------
void albaLODActor::SetFlagDimension(int flagDimension)
//------------------------------------------------------------------------
{
  int min = 1;
  int max = VTK_INT_MAX;
  if (this->m_FlagDimension != (flagDimension<min?min:(flagDimension>max?max:flagDimension)))
  {
    this->m_FlagDimension = (flagDimension<min?min:(flagDimension>max?max:flagDimension));
    this->Modified();
  }
}
//------------------------------------------------------------------------
void albaLODActor::SetEnableFading(int enableFading)
//------------------------------------------------------------------------
{
  int min = 0;
  int max = 1;
  if (this->m_EnableFading != (enableFading<min?min:(enableFading>max?max:enableFading)))
  {
    this->m_EnableFading = (enableFading<min?min:(enableFading>max?max:enableFading));
    this->Modified();
  }
}
//------------------------------------------------------------------------
void albaLODActor::SetEnableHighThreshold(int enableHighThreshold)
//------------------------------------------------------------------------
{
  int min = 0;
  int max = 1;
  if (this->m_EnableHighThreshold != (enableHighThreshold<min?min:(enableHighThreshold>max?max:enableHighThreshold)))
  {
    this->m_EnableHighThreshold = (enableHighThreshold<min?min:(enableHighThreshold>max?max:enableHighThreshold));
    this->Modified();
  }
}
