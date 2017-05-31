/*=========================================================================

 Program: MAF2
 Module: vtkMAFRulerActor2D
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFRulerActor2D.h"

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
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"

#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkPolyDataMapper2D.h"

vtkStandardNewMacro(vtkMAFRulerActor2D);
//------------------------------------------------------------------------------
vtkMAFRulerActor2D::vtkMAFRulerActor2D()
//------------------------------------------------------------------------------
{
  Margin   = 30; 
  ShortTickLen = 5;
  MidTickLen   = 10;
  LongTickLen  = 15;
  DesiredTickSpacing = 20;
  Ntick    = 800; // enough for a tick spacing = 15 on a 1600x1200 screen
  ScaleFactor = 1.0;

  XOffSet = -2;
  YOffSet =  4;

  PositionWorld[0] = PositionWorld[1] = PositionWorld[2] = 0;
  Position[0] = Position[1] = Position[2] = 0;
  PositionDisplay[0] = PositionDisplay[1] = PositionDisplay[2] = 0;
  AttachPositionFlag = false;

  ScaleLabelVisibility  = true;
  AxesLabelVisibility   = true;
  AxesVisibility        = false;
  TickVisibility        = true;
  GlobalAxes            = true;
  InverseTicks          = false;

  Legend = NULL;
  Axis = Tick = ScaleLabel = NULL;
  Points = NULL;
  Xindex = 0;
  Yindex = 1;

	Text[0] = "x";
	Text[1] = "y";
	Text[2] = "z";

  SetLegend("");
  RulerCreate();

  // 'this' is an actor and he is unhappy without a mapper
  vtkLineSource *a = vtkLineSource::New();
  a->SetPoint1(10,10,10);
  a->SetPoint1(10,10,10);
  vtkPolyDataMapper2D *a_pdm = vtkPolyDataMapper2D::New();
  a_pdm->SetInputConnection(a->GetOutputPort());
  this->SetMapper(a_pdm); 
  this->PickableOff();
  a->Delete();
  a_pdm->Delete();
}
//------------------------------------------------------------------------------
vtkMAFRulerActor2D::~vtkMAFRulerActor2D()
//------------------------------------------------------------------------------
{
  if(Axis)   Axis->Delete();
  if(Tick)   Tick->Delete();
  if(ScaleLabel)  ScaleLabel->Delete();
  if(HorizontalAxesLabel) HorizontalAxesLabel->Delete();
  if(VerticalAxesLabel) VerticalAxesLabel->Delete();

  // Added by Losi 10.19.2009 to elimnate leaks
  if(Points)  Points->Delete();

  for(int i = 0; i < NUM_LAB; i++)
  {
    if(Labx[i]) Labx[i]->Delete();
    if(Laby[i]) Laby[i]->Delete();
  }
  if(this->Legend)  delete [] this->Legend; 
  // 
}
//------------------------------------------------------------------------------
void vtkMAFRulerActor2D::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkMAFRulerActor2D::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);
  
  RwWidth  = ren->GetRenderWindow()->GetSize()[0];
  RwHeight = ren->GetRenderWindow()->GetSize()[1];

  vtkCamera *cam = ren->GetActiveCamera();
  if(!cam->GetParallelProjection()) return 0;

  if(!CheckProjectionPlane(cam) && GlobalAxes) return 0;
  RulerUpdate(cam, ren);
  this->Modified();

  if (AxesVisibility)       Axis->RenderOverlay(viewport);
  if (TickVisibility)       Tick->RenderOverlay(viewport);
  if (ScaleLabelVisibility) ScaleLabel->RenderOverlay(viewport);
  if (AxesLabelVisibility && GlobalAxes)  HorizontalAxesLabel->RenderOverlay(viewport);
  if (AxesLabelVisibility && GlobalAxes)  VerticalAxesLabel->RenderOverlay(viewport);

  for(int i=0; i<NUM_LAB; i++)
  {
    Labx[i]->RenderOverlay(viewport);
    Laby[i]->RenderOverlay(viewport);
  }
  return 1;
}
//----------------------------------------------------------------------------
int vtkMAFRulerActor2D::RenderOpaqueGeometry(vtkViewport *viewport)
//----------------------------------------------------------------------------
{
  if (ScaleLabelVisibility) ScaleLabel->RenderOpaqueGeometry(viewport);
  if (AxesLabelVisibility && GlobalAxes) HorizontalAxesLabel->RenderOpaqueGeometry(viewport);
  if (AxesLabelVisibility && GlobalAxes) VerticalAxesLabel->RenderOpaqueGeometry(viewport);

  for(int i=0; i<NUM_LAB; i++)
  {
    Labx[i]->RenderOpaqueGeometry(viewport);
    Laby[i]->RenderOpaqueGeometry(viewport);
  }

  return 0;
}
//----------------------------------------------------------------------------
bool vtkMAFRulerActor2D::CheckProjectionPlane(vtkCamera *cam)
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
      Xindex = 2;
      Yindex = 1;
    }
    else if (abs(vtkMath::Dot(vu,z_axes)) > threshold)
    {
      Xindex = 1;
      Yindex = 2;
    }
    else
      return false;
  }
  else if(abs(vtkMath::Dot(vpn,y_axes)) > threshold)
  {
    if (abs(vtkMath::Dot(vu,x_axes)) > threshold)
    {
      Xindex = 2;
      Yindex = 0;
    }
    else if (abs(vtkMath::Dot(vu,z_axes)) > threshold)
    {
      Xindex = 0;
      Yindex = 2;
    }
    else
      return false;
  }
  else if(abs(vtkMath::Dot(vpn,z_axes)) > threshold)
  {
    if (abs(vtkMath::Dot(vu,y_axes)) > threshold)
    {
      Xindex = 0;
      Yindex = 1;
    }
    else if (abs(vtkMath::Dot(vu,x_axes)) > threshold)
    {
      Xindex = 1;
      Yindex = 0;
    }
    else
      return false;
  }
  else
    return false;

  return true;
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::RulerCreate()
//----------------------------------------------------------------------------
{
	Points = vtkPoints::New();
	Points->SetNumberOfPoints(4*Ntick);
  
  vtkCellArray  *tick_cell = vtkCellArray::New(); 
  tick_cell->Allocate(tick_cell->EstimateSize(2*Ntick,2));

  double m0 = Margin;
  double m1 = Margin - 2;

  int i, id=0, cp[2];
  for(i=0; i<Ntick; i++)
	{
			Points->SetPoint(id,     Position[0]+i, Position[1]+m0,	0);
      Points->SetPoint(id+1,   Position[0]+i, Position[1]+m1, 0);
      Points->SetPoint(id+2,   Position[0]+m0, Position[1]+i, 0);
      Points->SetPoint(id+3,   Position[0]+m1, Position[1]+i, 0);

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
  tick_pdm->SetInputData(tick_pd);

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
  cp[0]=0, cp[1]=4*(Ntick-1);
  axis_cell->InsertNextCell(2,cp);    
  cp[0]=2, cp[1]=4*(Ntick-1)+2;
  axis_cell->InsertNextCell(2,cp);    

  vtkPolyData *axis_pd = vtkPolyData::New();
  axis_pd->SetPoints(Points);
  axis_pd->SetLines(axis_cell);

  vtkPolyDataMapper2D *axis_pdm = vtkPolyDataMapper2D::New();
  axis_pdm->SetInputData(axis_pd);

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

  // Added by Losi 10.19.2009 to elimnate leaks
  axis_p->Delete();
  //

  //Label //////////////////////////
  ScaleLabel = vtkTextActor::New();
	ScaleLabel->GetProperty()->SetColor(1,0,0);
  ScaleLabel->GetTextProperty()->SetFontSize(12);
  ScaleLabel->GetTextProperty()->SetFontFamilyToArial();
  ScaleLabel->GetTextProperty()->SetJustificationToLeft();
	ScaleLabel->SetTextScaleModeToNone();
	ScaleLabel->SetDisplayPosition(Position[0] + Margin + 4,Position[1] + Margin + 4);
	ScaleLabel->SetInput("");

  HorizontalAxesLabel = vtkTextActor::New();
  HorizontalAxesLabel->GetProperty()->SetColor(1,0,0);
  HorizontalAxesLabel->GetTextProperty()->SetFontSize(12);
  HorizontalAxesLabel->GetTextProperty()->SetFontFamilyToArial();
  HorizontalAxesLabel->GetTextProperty()->SetJustificationToRight();
  HorizontalAxesLabel->SetTextScaleModeToNone();
  HorizontalAxesLabel->SetDisplayPosition(Position[0],Position[1]);
  HorizontalAxesLabel->SetInput("");

  VerticalAxesLabel = vtkTextActor::New();
  VerticalAxesLabel->GetProperty()->SetColor(1,0,0);
  VerticalAxesLabel->GetTextProperty()->SetFontSize(12);
  VerticalAxesLabel->GetTextProperty()->SetFontFamilyToArial();
  VerticalAxesLabel->GetTextProperty()->SetJustificationToLeft();
  VerticalAxesLabel->SetTextScaleModeToNone();
  VerticalAxesLabel->SetDisplayPosition(Position[0],Position[1]);
  VerticalAxesLabel->SetInput("");

  for(i=0; i<NUM_LAB; i++)
  {
    Labx[i] = vtkTextActor::New();
    Labx[i]->GetProperty()->SetColor(1,1,1);
    Labx[i]->GetTextProperty()->SetFontSize(12);
    Labx[i]->GetTextProperty()->SetFontFamilyToArial();
    Labx[i]->GetTextProperty()->SetJustificationToCentered();
    Labx[i]->GetTextProperty()->SetVerticalJustificationToTop();
    Labx[i]->SetTextScaleModeToNone();
    Labx[i]->SetDisplayPosition(Position[0],Position[1]);
    Labx[i]->SetInput("");


    Laby[i] = vtkTextActor::New();
    Laby[i]->GetProperty()->SetColor(1,1,1);
    Laby[i]->GetTextProperty()->SetFontSize(12);
    Laby[i]->GetTextProperty()->SetFontFamilyToArial();
  //Laby[i]->GetTextProperty()->SetJustificationToRight();
    Laby[i]->GetTextProperty()->SetJustificationToLeft();
    Laby[i]->GetTextProperty()->SetVerticalJustificationToCentered();
    Laby[i]->SetTextScaleModeToNone();
    Laby[i]->SetDisplayPosition(Position[0],Position[1]);
    Laby[i]->SetInput("");
  }
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::SetColor(double r,double g,double b)
//----------------------------------------------------------------------------
{
	Axis->GetProperty()->SetColor(r,g,b);
  Tick->GetProperty()->SetColor(r,g,b);
  ScaleLabel->GetProperty()->SetColor(r,g,b);
  HorizontalAxesLabel->GetProperty()->SetColor(r,g,b);
  VerticalAxesLabel->GetProperty()->SetColor(r,g,b);

}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::SetScaleFactor(double factor)
//----------------------------------------------------------------------------
{
  if (factor != 0) 
  {
    ScaleFactor = factor;
    Modified();
  }
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::SetLegend(const char *legend)
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
    Modified();
  }
  else
  {
    SetLegend(" ");
    Modified();
  }
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::DecomposeValue(double val, int *sign, double *mantissa, int *exponent)
//----------------------------------------------------------------------------
{
  // val == 0  ---> mantissa = 0
  // val != 0  ---> mantissa = 1.....1.9999
  
  *sign = (val > 0) ? 1 : -1;

  if (val == 0) 
  {
    *mantissa = 0;
    *exponent = 0;
    return;
  }

  val = fabs(val);
  *exponent = (int)(log10(val));
  *mantissa = val/pow(10.0f,*exponent);  // base in pow must be float

  if( *mantissa <1 ) 
  {
    (*mantissa) *= 10; 
    (*exponent) --;     // must use parenthesys !!
  }
}
//----------------------------------------------------------------------------
double vtkMAFRulerActor2D::RicomposeValue(int sign, double mantissa, int exponent)
//----------------------------------------------------------------------------
{
  return sign*mantissa*pow(10.0f,exponent); // base in pow must be float
}
//----------------------------------------------------------------------------
double vtkMAFRulerActor2D::GetTickSpacing(double val)
//----------------------------------------------------------------------------
{
  if( val == 0 ) return 0;
  int sign, exponent;
  double mantissa;
  DecomposeValue(val,&sign,&mantissa,&exponent);

  /*
  if(mantissa<2)       // 1.00 ... 1.99  -> 1    // -1.99 ... -1.00  -> -1
    mantissa=1;
  else if(mantissa<5)  // 2.00 ... 4.99  -> 2    // -4.99 ... -2.00  -> -2
    mantissa=2;
  else                 // 5.00 ... 9.99  -> 5    // -9.99 ... -5.00  -> -5
    mantissa=5;
  */
  /*
  if(mantissa<5)
    mantissa=1;
  else 
    mantissa=5;
  */
  mantissa =1;
  return RicomposeValue(sign,mantissa,exponent);
}
//----------------------------------------------------------------------------
double vtkMAFRulerActor2D::GetMidTickSpacing(double val)
//----------------------------------------------------------------------------
{
  if( val == 0 ) return 0;
  int sign, exponent;
  double mantissa;
  DecomposeValue(val,&sign,&mantissa,&exponent);
  /*
  if(mantissa<2)       // 1.00 ... 1.99  -> 1    // -1.99 ... -1.00  -> -1
    mantissa=5;
  else if(mantissa<5)  // 2.00 ... 4.99  -> 2    // -4.99 ... -2.00  -> -2
    mantissa=5;
  else                 // 5.00 ... 9.99  -> 5    // -9.99 ... -5.00  -> -5
    mantissa=10;
  */
  /*
  if(mantissa<5)
    mantissa=5;
  else 
    mantissa=10;
  */
  mantissa =5;
  return RicomposeValue(sign,mantissa,exponent);
}
//----------------------------------------------------------------------------
double vtkMAFRulerActor2D::GetLongTickSpacing(double val)
//----------------------------------------------------------------------------
{
  if( val == 0 ) return 0;
  int sign, exponent;
  double mantissa;
  DecomposeValue(val,&sign,&mantissa,&exponent);
  /*
  if(mantissa<2)       // 1.00 ... 1.99  -> 1    // -1.99 ... -1.00  -> -1
    mantissa=5;
  else if(mantissa<5)  // 2.00 ... 4.99  -> 2    // -4.99 ... -2.00  -> -2
    mantissa=10;
  else                 // 5.00 ... 9.99  -> 5    // -9.99 ... -5.00  -> -5
    mantissa=20;
  */
  /*
  if(mantissa<5)
    mantissa=10;
  else 
    mantissa=20;
  */
  mantissa =10;
  return RicomposeValue(sign,mantissa,exponent);
}
//----------------------------------------------------------------------------
int vtkMAFRulerActor2D::Round(double val)
//----------------------------------------------------------------------------
{
  int sign = ( val >= 0 ) ? 1 : -1 ;
  return sign * (int)( abs(val) + 0.5 );
}
//----------------------------------------------------------------------------
bool vtkMAFRulerActor2D::IsMultiple(double val, double multiplier)
//----------------------------------------------------------------------------
{
  double ratio = val/multiplier; 
  double fractionalPart = ratio - (int)(ratio);
  return fractionalPart == 0;
}
//----------------------------------------------------------------------------
double vtkMAFRulerActor2D::NearestTick(double val, double TickSpacing)
//----------------------------------------------------------------------------
{
  // if tickSpacing > 0
  // return the first multiple of tickSpacing  that is greater than val

  // if tickSpacing < 0
  // return the first multiple of tickSpacing  that is smaller than val

  val = val + TickSpacing / 2.0f; 
  return  TickSpacing * Round( val / TickSpacing );
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::RulerUpdate(vtkCamera *camera, vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  //    given P,P0,P1 in Display coordinate ....
  //
  //              rwWidth
  //    -----------------------p1
  //    |                      |  
  //    |                      |
  //    |        Display       | rwHeight
  //    |                      | 
  //    | p                    |
  //   p0-----------------------
  //
  //    convert them in world coord - resulting in w,w0,w1
  // 
  //             w1x-w0x
  //    -----------------------w1
  //    |                      |  
  //    |                      |
  //    |        World         | w1y-w0y
  //    |                      | 
  //    | w                    |
  //   w0-----------------------
  //
  //   
  //   thus -- conversion from World to Display is
  //   (d0 is = 0) 
  //
  //                   ( w1 - w0 )  
  //   d =    (w-w0) * -----------   =   (w - w0) * w2d;
  //                   ( d1 - d0 )  
  //
  //   and viceversa is
  //
  //   (w - wo) =    d * 1/w2d   =   d * d2w;
  //

  if(true == AttachPositionFlag)
  {
    ren->SetWorldPoint(PositionWorld[0],PositionWorld[1],PositionWorld[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(PositionDisplay);

    Position[0] = PositionDisplay[0] - Margin;
    Position[1] = PositionDisplay[1] - Margin;
    Position[2] = PositionDisplay[2];
  }

  double p[4],p0[4],p1[4];

  ren->SetDisplayPoint(Position[0] + Margin, Position[1] + Margin,0);
  ren->DisplayToWorld();
  ren->GetWorldPoint(p);

  ren->SetDisplayPoint(Position[0], Position[1] ,0);
  ren->DisplayToWorld();
  ren->GetWorldPoint(p0);

  ren->SetDisplayPoint(Position[0]+RwWidth, Position[1]+RwHeight, 0);
  ren->DisplayToWorld();
  ren->GetWorldPoint(p1);

  int i;
  for(i=0; i<3; i++)
  {
    p[i]  /= ScaleFactor; 
    p0[i] /= ScaleFactor; 
    p1[i] /= ScaleFactor; 
  }

  double w0X, w0Y, w1X, w1Y, wpX, wpY, vx[3], vy[3];
  if (GlobalAxes) 
  {
    w0X =  p0[Xindex];
    w0Y =  p0[Yindex];
    w1X =  p1[Xindex];
    w1Y =  p1[Yindex];
    wpX =   p[Xindex];
    wpY =   p[Yindex];
  }
  else
  {
    double vpn[3];
    camera->GetViewUp(vy);
    camera->GetViewPlaneNormal(vpn);
    vtkMath::Cross(vy,vpn,vx);
    w0X = vtkMath::Dot(vx,p0);
    w0Y = vtkMath::Dot(vy,p0);
    w1X = vtkMath::Dot(vx,p1);
    w1Y = vtkMath::Dot(vy,p1);
    wpX = vtkMath::Dot(vx,p);
    wpY = vtkMath::Dot(vy,p);
  }

  double w2dX = RwWidth /(w1X-w0X), w2dY = RwHeight/(w1Y-w0Y);  // world to display
  double d2wX = 1/w2dX,             d2wY = 1/w2dY;              // display to world along Y
  
  // determine tick spacing
  double desiredWorldTickSpacingX = DesiredTickSpacing * d2wX;
  double worldTickSpacingX        = GetTickSpacing( desiredWorldTickSpacingX );  
  double worldMidTickSpacingX     = GetMidTickSpacing( desiredWorldTickSpacingX );  
  double worldLongTickSpacingX    = GetLongTickSpacing( desiredWorldTickSpacingX );  
  double worldFirstTickX          = NearestTick( wpX, worldTickSpacingX );
  double worldFirstLongTickX      = NearestTick( wpX, worldLongTickSpacingX );

  double desiredWorldTickSpacingY = DesiredTickSpacing * d2wY;
  double worldTickSpacingY        = GetTickSpacing( desiredWorldTickSpacingY );  
  double worldMidTickSpacingY     = GetMidTickSpacing( desiredWorldTickSpacingY );  
  double worldLongTickSpacingY    = GetLongTickSpacing( desiredWorldTickSpacingY );  
  double worldFirstTickY          = NearestTick( wpY, worldTickSpacingY );
  double worldFirstLongTickY      = NearestTick( wpY, worldLongTickSpacingY );

  // find last tick pos
  double dx_max;
  double dy_max;
  for(i=0; i<Ntick; i++)
  {
    double wx = worldFirstTickX + i * worldTickSpacingX;
    double wy = worldFirstTickY + i * worldTickSpacingY;
    double dx = (wx - w0X ) * w2dX;  
    double dy = (wy - w0Y ) * w2dY;
    if( dx < RwWidth  - Margin ) dx_max = dx;
    if( dy < RwHeight - Margin ) dy_max = dy;
  }

  double t0;              // tick begin
  double t1x, t1y;                 // tick end
  double ta; // short tick end 
  double tb;   // mid   tick end 
  double tc;  // long  tick end 

  if(false == InverseTicks)
  {
    t0 = Margin;              // tick begin  
    ta = Margin-ShortTickLen; // short tick end 
    tb = Margin-MidTickLen;   // mid   tick end 
    tc = Margin-LongTickLen;  // long  tick end 
  }
  else
  {
    t0 = Margin-LongTickLen;              // tick begin
    ta = Margin-LongTickLen+ShortTickLen; // short tick end 
    tb = Margin-LongTickLen+MidTickLen;   // mid   tick end 
    tc = Margin-LongTickLen+LongTickLen;  // long  tick end 
  }
  
  int id=0;

  // Update Axis and Ticks Points
  for(i=0; i<Ntick; i++)
  {
    double wx = worldFirstTickX + i * worldTickSpacingX;
    double wy = worldFirstTickY + i * worldTickSpacingY;

    // decide length of tick
    if     ( IsMultiple( wx, worldLongTickSpacingX ) ) t1x = tc; 
    else if( IsMultiple( wx, worldMidTickSpacingX  ) ) t1x = tb; 
    else t1x = ta; 

    if     ( IsMultiple( wy, worldLongTickSpacingY ) ) t1y = tc; 
    else if( IsMultiple( wy, worldMidTickSpacingY  ) ) t1y = tb; 
    else t1y = ta; 


    double dx = (wx - w0X ) * w2dX;  // bring to Display coords
    double dy = (wy - w0Y ) * w2dY;

    if( dx > RwWidth  - Margin ) { dx = dx_max ; t1x = t0; } // discard tick
    if( dy > RwHeight - Margin ) { dy = dy_max ; t1y = t0; } 

    Points->SetPoint(id++, Position[0]+dx,  Position[1]+t0,   0);
    Points->SetPoint(id++, Position[0]+dx,	Position[1]+t1x,  0);
    Points->SetPoint(id++, Position[0]+t0,  Position[1]+dy,   0);
    Points->SetPoint(id++, Position[0]+t1y, Position[1]+dy,   0);
 }
  
  char labxtex[50];
  char labytex[50];
  for(i=0; i<NUM_LAB; i++)
  {
    double wx = worldFirstLongTickX + i * worldLongTickSpacingX;
    double wy = worldFirstLongTickY + i * worldLongTickSpacingY;
    
    double dx = (wx - w0X ) * w2dX;  // bring to Display coords
    double dy = (wy - w0Y ) * w2dY;
   
    if( dx < RwWidth  - Margin ) sprintf(labxtex, "%g",wx ); else sprintf(labxtex, "" );
    if( dy < RwHeight - Margin ) sprintf(labytex, "%g",wy ); else sprintf(labytex, "" );

    
    Labx[i]->SetInput(labxtex);

    Labx[i]->SetDisplayPosition(Position[0]+dx , Position[1]+ Margin + LongTickLen  );
    Labx[i]->Modified();
/*
    Labx[i]->SetDisplayPosition(Position[0]+dx , Position[1]+margin - longTickLen -2 );
    Labx[i]->Modified();
*/


	  Labx[i]->SetDisplayPosition(Position[0]+dx , Position[1]+ Margin - LongTickLen + XOffSet );
	  Labx[i]->Modified();
/*
	  Labx[i]->SetDisplayPosition(Position[0]+dx , Position[1]+margin - longTickLen -2 );
	  Labx[i]->Modified();
*/

	  Laby[i]->SetInput(labytex);
	  Laby[i]->SetDisplayPosition(Position[0]+ Margin +YOffSet, Position[1]+dy );
	  Laby[i]->Modified();
    
    
  }


  if (GlobalAxes) 
  {
    char *sign = (w1X-w0X > 0) ? (char *)" " : (char *)"-";

    char caption[100];
    sprintf(caption, "%s%s %s", sign,  Text[Xindex], Legend);
    HorizontalAxesLabel->SetInput(caption);
    HorizontalAxesLabel->SetDisplayPosition(RwWidth - (Margin>0?Margin:-Margin) , (Margin>0?Margin:-Margin) + 4);

    sign = (w1Y-w0Y > 0) ? (char *)" " : (char *)"-";
    sprintf(caption, "%s%s %s", sign, Text[Yindex], Legend);
    VerticalAxesLabel->SetInput(caption);
    VerticalAxesLabel->SetDisplayPosition( (Margin>0?Margin:-Margin), RwHeight - (Margin>0?Margin:-Margin)/2);
  }

  char lab[50];
  sprintf(lab,"%g %s", abs( worldTickSpacingX ), Legend);
  ScaleLabel->SetInput(lab);
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::SetAttachPosition(double position[3])
//----------------------------------------------------------------------------
{
  PositionWorld[0] = position[0];
  PositionWorld[1] = position[1];
  PositionWorld[2] = position[2];
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::SetAttachPositionFlag(bool value)
//----------------------------------------------------------------------------
{
  AttachPositionFlag = value;
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2D::ChangeRulerMarginsAndLengths(int marginArg, int shortTickLenArg, int midTickLenArg, int longTickLenArg, int xOffSetArg, int yOffSetArg)
//----------------------------------------------------------------------------
{
  Margin   = marginArg; 
  ShortTickLen = shortTickLenArg;
  MidTickLen   = midTickLenArg;
  LongTickLen  = longTickLenArg;

  XOffSet = xOffSetArg;
  YOffSet = yOffSetArg;
}