/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkRulerActor2D.cxx,v $
  Language:  C++
  Date:      $Date: 2006-03-02 07:51:12 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/

#include "vtkRulerActor2D.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include "vtkLineSource.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkIdType.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"

#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkPolyDataMapper2D.h"

vtkCxxRevisionMacro(vtkRulerActor2D, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkRulerActor2D);
//------------------------------------------------------------------------------
vtkRulerActor2D::vtkRulerActor2D()
//------------------------------------------------------------------------------
{
  Axis = Tick = Label = NULL;
  Points = NULL;

  margin   = 15; 
  tickLen  = 5;
  ntick    = 400; // enough for a tick spacing = 15 on a 1600x1200 screen
  ScaleFactor = 1.0;
  x_index = 0;
  y_index = 1;
  
  Legend = NULL;

  ScaleLabelVisibility  = true;
  AxesLabelVisibility   = true;

	RulerCreate();

  // 'this' is an actor and he is unhappy without a mapper
	this->PickableOff();
  vtkLineSource *a = vtkLineSource::New();
  a->SetPoint1(10,10,10);
  a->SetPoint1(10,10,10);
  vtkPolyDataMapper2D *a_pdm = vtkPolyDataMapper2D::New();
  a_pdm->SetInput(a->GetOutput());
  this->SetMapper(a_pdm); 
  a->Delete();
  a_pdm->Delete();
}
//------------------------------------------------------------------------------
vtkRulerActor2D::~vtkRulerActor2D()
//------------------------------------------------------------------------------
{
  if(Axis)  Axis->Delete();
  if(Tick)  Tick->Delete();
  if(Label) Label->Delete();
  if(Label1) Label1->Delete();
  if(Label2) Label2->Delete();
}
//------------------------------------------------------------------------------
void vtkRulerActor2D::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkRulerActor2D::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);
  
  rwWidth  = ren->GetRenderWindow()->GetSize()[0];
  rwHeight = ren->GetRenderWindow()->GetSize()[1];

  vtkCamera *cam = ren->GetActiveCamera();
  if(!cam->GetParallelProjection()) return 0;

  if(!CheckProjectionPlane(cam)) return 0;
  RulerUpdate(cam, ren);
  this->Modified();

  Axis->RenderOverlay(viewport);
  Tick->RenderOverlay(viewport);
  
  if (ScaleLabelVisibility) Label->RenderOverlay(viewport);
  if (AxesLabelVisibility) Label1->RenderOverlay(viewport);
  if (AxesLabelVisibility) Label2->RenderOverlay(viewport);

  return 1;
}
//----------------------------------------------------------------------------
bool vtkRulerActor2D::CheckProjectionPlane(vtkCamera *cam)
//----------------------------------------------------------------------------
{
  double vu[3], vpn[3], threshold;
  double x_axes[3] = {1,0,0};
  double y_axes[3] = {0,1,0};
  double z_axes[3] = {0,0,1};
  threshold = .99;
  cam->GetViewUp(vu);
  cam->GetViewPlaneNormal(vpn);

  if(abs(vtkMath::Dot(vpn,x_axes)) > threshold)
  {
    if (abs(vtkMath::Dot(vu,y_axes)) > threshold)
    {
      x_index = 2;
      y_index = 1;
    }
    else if (abs(vtkMath::Dot(vu,z_axes)) > threshold)
    {
      x_index = 1;
      y_index = 2;
    }
    else
      return false;
  }
  else if(abs(vtkMath::Dot(vpn,y_axes)) > threshold)
  {
    if (abs(vtkMath::Dot(vu,x_axes)) > threshold)
    {
      x_index = 2;
      y_index = 0;
    }
    else if (abs(vtkMath::Dot(vu,z_axes)) > threshold)
    {
      x_index = 0;
      y_index = 2;
    }
    else
      return false;
  }
  else if(abs(vtkMath::Dot(vpn,z_axes)) > threshold)
  {
    if (abs(vtkMath::Dot(vu,y_axes)) > threshold)
    {
      x_index = 0;
      y_index = 1;
    }
    else if (abs(vtkMath::Dot(vu,x_axes)) > threshold)
    {
      x_index = 1;
      y_index = 0;
    }
    else
      return false;
  }
  else
    return false;

  return true;
}
//----------------------------------------------------------------------------
void vtkRulerActor2D::RulerCreate()
//----------------------------------------------------------------------------
{
	Points = vtkPoints::New();
	Points->SetNumberOfPoints(4*ntick);
  
  vtkCellArray  *tick_cell = vtkCellArray::New(); 
  tick_cell->Allocate(tick_cell->EstimateSize(2*ntick,2));

  double m0 = margin;
  double m1 = margin-tickLen;

  int i, id=0, cp[2];
  for(i=0; i<ntick; i++)
	{
			Points->SetPoint(id,     i, m0,	0);
      Points->SetPoint(id+1,   i, m1, 0);
      Points->SetPoint(id+2,   m0, i, 0);
      Points->SetPoint(id+3,   m1, i, 0);

      cp[0]=id, cp[1]=id+1;
			tick_cell->InsertNextCell(2,cp);    
      cp[0]=id+2, cp[1]=id+3;
			tick_cell->InsertNextCell(2,cp);

      id +=4;    
	}
  vtkPolyData *tick_pd = vtkPolyData::New();
	tick_pd->SetPoints(Points);
	tick_pd->SetLines(tick_cell);

  vtkPolyDataMapper2D *tick_pdm = vtkPolyDataMapper2D::New();
  tick_pdm->SetInput(tick_pd);

  vtkProperty2D *tick_p = vtkProperty2D::New();
  tick_p->SetColor(1,0,0);
  tick_p->SetLineWidth(1);

  Tick = vtkActor2D::New();
  Tick->SetMapper(tick_pdm);
	Tick->SetProperty(tick_p);
  Tick->SetPosition(0,0);

  tick_cell->Delete();
  tick_pd->Delete();
	tick_pdm->Delete();
  tick_p->Delete();

  // Axis ///////////////////
  vtkCellArray *axis_cell = vtkCellArray::New(); 
  axis_cell->Allocate(axis_cell->EstimateSize(2,2));
  cp[0]=0, cp[1]=4*(ntick-1);
  axis_cell->InsertNextCell(2,cp);    
  cp[0]=2, cp[1]=4*(ntick-1)+2;
  axis_cell->InsertNextCell(2,cp);    

  vtkPolyData *axis_pd = vtkPolyData::New();
  axis_pd->SetPoints(Points);
  axis_pd->SetLines(axis_cell);

  vtkPolyDataMapper2D *axis_pdm = vtkPolyDataMapper2D::New();
  axis_pdm->SetInput(axis_pd);

  vtkProperty2D *axis_p = vtkProperty2D::New();
  axis_p->SetColor(1,0.0,0.0);
  axis_p->SetLineWidth(2);

  Axis = vtkActor2D::New();
  Axis->SetMapper(axis_pdm);
  Axis->SetProperty(axis_p);
  Axis->SetPosition(0,0);
	
  axis_cell->Delete();
  axis_pd->Delete();
  axis_pdm->Delete();

  //Label //////////////////////////
  Label = vtkTextActor::New();
	Label->GetProperty()->SetColor(1,0,0);
  Label->GetTextProperty()->AntiAliasingOff();
  Label->GetTextProperty()->SetFontSize(12);
  Label->GetTextProperty()->SetFontFamilyToArial();
  Label->GetTextProperty()->SetJustificationToLeft();
	Label->ScaledTextOff();
	Label->SetDisplayPosition(margin + 4,margin + 4);
	Label->SetInput("");

  Label1 = vtkTextActor::New();
  Label1->GetProperty()->SetColor(1,0,0);
  Label1->GetTextProperty()->AntiAliasingOff();
  Label1->GetTextProperty()->SetFontSize(12);
  Label1->GetTextProperty()->SetFontFamilyToArial();
  Label1->GetTextProperty()->SetJustificationToRight();
  Label1->ScaledTextOff();
  Label1->SetDisplayPosition(0,0);
  Label1->SetInput("");

  Label2 = vtkTextActor::New();
  Label2->GetProperty()->SetColor(1,0,0);
  Label2->GetTextProperty()->AntiAliasingOff();
  Label2->GetTextProperty()->SetFontSize(12);
  Label2->GetTextProperty()->SetFontFamilyToArial();
  Label2->GetTextProperty()->SetJustificationToLeft();
  Label2->ScaledTextOff();
  Label2->SetDisplayPosition(0,0);
  Label2->SetInput("");
}
//----------------------------------------------------------------------------
double vtkRulerActor2D::Round(double val)
//----------------------------------------------------------------------------
{
	if (val == 0) 
    return 1;

	int sign = (val > 0) ? 1 : -1;
	val = fabs(val);

	int    exponent = (int)(log10(val));
	double mantissa = val/pow(10.0,exponent);   // note 10.0 --- .0 is relevant

  if(mantissa <1) // fix for val< 1
	{
     mantissa *= 10;
		 exponent --;
	}
  
  if(mantissa<2) 
		mantissa=1;
  else if(mantissa<5)
	  mantissa=2;
  else 
	  mantissa=5;
	return sign*mantissa*pow(10.0,exponent);  // note 10.0 --- .0 is relevant
}
//----------------------------------------------------------------------------
double vtkRulerActor2D::Round2(double val)
//----------------------------------------------------------------------------
{
  if (val == 0) 
    return 1;

  int sign = (val > 0) ? 1 : -1;
  val = fabs(val);

  int    exponent = (int)(log10(val));
  double mantissa = val/pow(10.0,exponent);   // note 10.0 --- .0 is relevant

  if(mantissa <1) // fix for val< 1
  {
    mantissa *= 10;
    exponent --;
  }

  if(mantissa<2) 
    mantissa=5;
  else if(mantissa<5)
    mantissa=10;
  else 
    mantissa=10;
  return sign*mantissa*pow(10.0,exponent);  // note 10.0 --- .0 is relevant
}
//----------------------------------------------------------------------------
void vtkRulerActor2D::SetColor(double r,double g,double b)
//----------------------------------------------------------------------------
{
	Axis->GetProperty()->SetColor(r,g,b);
  Tick->GetProperty()->SetColor(r,g,b);
  Label->GetProperty()->SetColor(r,g,b);
}
//----------------------------------------------------------------------------
void vtkRulerActor2D::RulerUpdate(vtkCamera *camera, vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  double p[4],p0[4],p1[4];

  ren->SetDisplayPoint(margin,margin,0);
  ren->DisplayToWorld();
  ren->GetWorldPoint(p);

  ren->SetDisplayPoint(0,0,0);
  ren->DisplayToWorld();
  ren->GetWorldPoint(p0);

  ren->SetDisplayPoint(rwWidth, rwHeight, 0);
  ren->DisplayToWorld();
  ren->GetWorldPoint(p1);

  for(int i=0; i<3; i++)
  {
    p[i] /= ScaleFactor; 
    p0[i] /= ScaleFactor; 
    p1[i] /= ScaleFactor; 
  }

  double wDeltaX   = (p1[x_index]-p0[x_index]);
  double w2dx = rwWidth / wDeltaX;    // display to word - for x axis
  double d2wx = 1 / w2dx;             // word to display - for x axis  

  double wDeltaY   = (p1[y_index]-p0[y_index]);   
  double w2dy = rwHeight / wDeltaY;   // display to word - for y axis
  double d2wy = 1 / w2dy;             // word to display - for y axis  

  int signDeltaX = (wDeltaX >= 0) ? 1 : -1;
  int signDeltaY = (wDeltaY >= 0) ? 1 : -1;

  char *alab[] = {"x","y","z","-x","-y","-z"};
  Label1->SetInput(alab[(signDeltaX > 0) ? x_index  : x_index + 3]);
  Label2->SetInput(alab[(signDeltaY > 0) ? y_index  : y_index + 3]);
  Label1->SetDisplayPosition(rwWidth - margin , margin + 4);
  Label2->SetDisplayPosition(margin + 4, rwHeight - margin );

  double desiredDisplayTickSpacing = 10;
  double desiredWorldTickSpacing = desiredDisplayTickSpacing * d2wx;

  double worldTickSpacingABS = Round( desiredWorldTickSpacing );  // unique for both axis 
  double worldTickSpacingX = worldTickSpacingABS * signDeltaX;         // unique for both axis 
  double worldTickSpacingY = worldTickSpacingABS * signDeltaY;         // unique for both axis 
  double worldFirstTickX = (worldTickSpacingX) * (int)(p[x_index] / worldTickSpacingX);
  double worldFirstTickY = (worldTickSpacingY) * (int)(p[y_index] / worldTickSpacingY);

  double worldTickSpacing2ABS = Round2( desiredWorldTickSpacing ); // unique for both axis 
  double worldTickSpacing2X = worldTickSpacing2ABS * signDeltaX;
  double worldTickSpacing2Y = worldTickSpacing2ABS * signDeltaY;

  int urk = (int)(worldTickSpacing2ABS / worldTickSpacingABS);
  double worldFirstTick2X = (worldTickSpacing2X) * (int)(p[x_index] / worldTickSpacing2X);
  double worldFirstTick2Y = (worldTickSpacing2Y) * (int)(p[y_index] / worldTickSpacing2Y);
  int IndexOfFirstLongTickX = (int)((worldFirstTick2X - worldFirstTickX) / worldTickSpacingX);
  int IndexOfFirstLongTickY = (int)((worldFirstTick2Y - worldFirstTickY) / worldTickSpacingY);

  double dispTickSpacingX = worldTickSpacingX * w2dx;
  double dispTickSpacingY = worldTickSpacingY * w2dy;
  double dispFirstTickX  = (worldFirstTickX - p0[x_index]) * w2dx;
  double dispFirstTickY  = (worldFirstTickY - p0[y_index]) * w2dy;
  while(dispFirstTickX < margin) dispFirstTickX += dispTickSpacingX;
  while(dispFirstTickY < margin) dispFirstTickY += dispTickSpacingY;

  char lab[50];
  sprintf(lab,"%g %s",worldTickSpacingABS,Legend);
  Label->SetInput(lab);

  double m0 = margin;
  double ma = margin-tickLen;
  double mb = margin-tickLen-8;
  double m1x,m1y;
  int id=0;

  // this is to avoid the Axis to go outside the render-window
  int max_i_x = (rwWidth  - dispTickSpacingX - dispFirstTickX ) / dispTickSpacingX;
  double max_x = dispFirstTickX + max_i_x * dispTickSpacingX;
  int max_i_y = (rwHeight - dispTickSpacingY - dispFirstTickY) / dispTickSpacingY;
  double max_y = dispFirstTickY + max_i_y * dispTickSpacingY;

  // Update Axis and Ticks Points
  for(int i=0; i<ntick; i++)
  {
    m1x = m1y = ma;
    if( (i-IndexOfFirstLongTickX) % urk == 0 ) m1x = mb;
    if( (i-IndexOfFirstLongTickY) % urk == 0 ) m1y = mb;

    double x = dispFirstTickX + i*dispTickSpacingX;
    double y = dispFirstTickY + i*dispTickSpacingY;
    
    if( x > max_x ) {x = max_x; m1x = m0; }
    if( y > max_y ) {y = max_y; m1y = m0; }

    Points->SetPoint(id,   x,   m0,  0);
    Points->SetPoint(id+1, x,	  m1x, 0);
    Points->SetPoint(id+2, m0,  y,   0);
    Points->SetPoint(id+3, m1y, y,   0);

    id +=4;    
  }
}
//----------------------------------------------------------------------------
void vtkRulerActor2D::SetScaleFactor(double factor)
//----------------------------------------------------------------------------
{
  if (factor != 0)
  {
    ScaleFactor = factor;
  }
}
//----------------------------------------------------------------------------
int vtkRulerActor2D::RenderOpaqueGeometry(vtkViewport *viewport)
//----------------------------------------------------------------------------
{
  if (ScaleLabelVisibility) Label->RenderOpaqueGeometry(viewport);
  if (AxesLabelVisibility) Label1->RenderOpaqueGeometry(viewport);
  if (AxesLabelVisibility) Label2->RenderOpaqueGeometry(viewport);
  return 0;
}
//----------------------------------------------------------------------------
void vtkRulerActor2D::SetLegend(const char *legend)
//----------------------------------------------------------------------------
{
  if ( this->Legend && legend && (!strcmp(this->Legend,legend))) 
  {
    return;
  }
  if (this->Legend) 
  { 
    delete [] this->Legend; 
  }  
  if (legend)
  {
    this->Legend = new char[strlen(legend)+1];
    strcpy(this->Legend,legend);
  }
  else
  {
    this->Legend = NULL;
  }
}