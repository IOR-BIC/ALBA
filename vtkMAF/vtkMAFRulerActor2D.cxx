/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFRulerActor2D.cxx,v $
  Language:  C++
  Date:      $Date: 2008-07-24 14:46:54 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/

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
#include "vtkIdType.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"

#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkPolyDataMapper2D.h"

vtkCxxRevisionMacro(vtkMAFRulerActor2D, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkMAFRulerActor2D);
//------------------------------------------------------------------------------
vtkMAFRulerActor2D::vtkMAFRulerActor2D()
//------------------------------------------------------------------------------
{
  margin   = 30; 
  shortTickLen = 5;
  midTickLen   = 10;
  longTickLen  = 15;
  DesiredTickSpacing = 20;
  ntick    = 800; // enough for a tick spacing = 15 on a 1600x1200 screen
  ScaleFactor = 1.0;

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
  x_index = 0;
  y_index = 1;

  SetLegend("");
  RulerCreate();

  // 'this' is an actor and he is unhappy without a mapper
  vtkLineSource *a = vtkLineSource::New();
  a->SetPoint1(10,10,10);
  a->SetPoint1(10,10,10);
  vtkPolyDataMapper2D *a_pdm = vtkPolyDataMapper2D::New();
  a_pdm->SetInput(a->GetOutput());
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
  
  rwWidth  = ren->GetRenderWindow()->GetSize()[0];
  rwHeight = ren->GetRenderWindow()->GetSize()[1];

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
void vtkMAFRulerActor2D::RulerCreate()
//----------------------------------------------------------------------------
{
	Points = vtkPoints::New();
	Points->SetNumberOfPoints(4*ntick);
  
  vtkCellArray  *tick_cell = vtkCellArray::New(); 
  tick_cell->Allocate(tick_cell->EstimateSize(2*ntick,2));

  double m0 = margin;
  double m1 = margin - 2;

  int i, id=0, cp[2];
  for(i=0; i<ntick; i++)
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
  ScaleLabel = vtkTextActor::New();
	ScaleLabel->GetProperty()->SetColor(1,0,0);
  ScaleLabel->GetTextProperty()->AntiAliasingOff();
  ScaleLabel->GetTextProperty()->SetFontSize(12);
  ScaleLabel->GetTextProperty()->SetFontFamilyToArial();
  ScaleLabel->GetTextProperty()->SetJustificationToLeft();
	ScaleLabel->ScaledTextOff();
	ScaleLabel->SetDisplayPosition(Position[0] + margin + 4,Position[1] + margin + 4);
	ScaleLabel->SetInput("");

  HorizontalAxesLabel = vtkTextActor::New();
  HorizontalAxesLabel->GetProperty()->SetColor(1,0,0);
  HorizontalAxesLabel->GetTextProperty()->AntiAliasingOff();
  HorizontalAxesLabel->GetTextProperty()->SetFontSize(12);
  HorizontalAxesLabel->GetTextProperty()->SetFontFamilyToArial();
  HorizontalAxesLabel->GetTextProperty()->SetJustificationToRight();
  HorizontalAxesLabel->ScaledTextOff();
  HorizontalAxesLabel->SetDisplayPosition(Position[0],Position[1]);
  HorizontalAxesLabel->SetInput("");

  VerticalAxesLabel = vtkTextActor::New();
  VerticalAxesLabel->GetProperty()->SetColor(1,0,0);
  VerticalAxesLabel->GetTextProperty()->AntiAliasingOff();
  VerticalAxesLabel->GetTextProperty()->SetFontSize(12);
  VerticalAxesLabel->GetTextProperty()->SetFontFamilyToArial();
  VerticalAxesLabel->GetTextProperty()->SetJustificationToLeft();
  VerticalAxesLabel->ScaledTextOff();
  VerticalAxesLabel->SetDisplayPosition(Position[0],Position[1]);
  VerticalAxesLabel->SetInput("");

  for(i=0; i<NUM_LAB; i++)
  {
    Labx[i] = vtkTextActor::New();
    Labx[i]->GetProperty()->SetColor(1,1,1);
    Labx[i]->GetTextProperty()->AntiAliasingOff();
    Labx[i]->GetTextProperty()->SetFontSize(12);
    Labx[i]->GetTextProperty()->SetFontFamilyToArial();
    Labx[i]->GetTextProperty()->SetJustificationToCentered();
    Labx[i]->GetTextProperty()->SetVerticalJustificationToTop();
    Labx[i]->ScaledTextOff();
    Labx[i]->SetDisplayPosition(Position[0],Position[1]);
    Labx[i]->SetInput("");


    Laby[i] = vtkTextActor::New();
    Laby[i]->GetProperty()->SetColor(1,1,1);
    Laby[i]->GetTextProperty()->AntiAliasingOff();
    Laby[i]->GetTextProperty()->SetFontSize(12);
    Laby[i]->GetTextProperty()->SetFontFamilyToArial();
  //Laby[i]->GetTextProperty()->SetJustificationToRight();
    Laby[i]->GetTextProperty()->SetJustificationToLeft();
    Laby[i]->GetTextProperty()->SetVerticalJustificationToCentered();
    Laby[i]->ScaledTextOff();
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
int vtkMAFRulerActor2D::round(double val)
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
  return  TickSpacing * round( val / TickSpacing );
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

    Position[0] = PositionDisplay[0] - margin;
    Position[1] = PositionDisplay[1] - margin;
    Position[2] = PositionDisplay[2];
  }

  double p[4],p0[4],p1[4];

  ren->SetDisplayPoint(Position[0] + margin, Position[1] + margin,0);
  ren->DisplayToWorld();
  ren->GetWorldPoint(p);

  ren->SetDisplayPoint(Position[0], Position[1] ,0);
  ren->DisplayToWorld();
  ren->GetWorldPoint(p0);

  ren->SetDisplayPoint(Position[0]+rwWidth, Position[1]+rwHeight, 0);
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
    w0X =  p0[x_index];
    w0Y =  p0[y_index];
    w1X =  p1[x_index];
    w1Y =  p1[y_index];
    wpX =   p[x_index];
    wpY =   p[y_index];
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

  double w2dX = rwWidth /(w1X-w0X), w2dY = rwHeight/(w1Y-w0Y);  // world to display
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
  for(i=0; i<ntick; i++)
  {
    double wx = worldFirstTickX + i * worldTickSpacingX;
    double wy = worldFirstTickY + i * worldTickSpacingY;
    double dx = (wx - w0X ) * w2dX;  
    double dy = (wy - w0Y ) * w2dY;
    if( dx < rwWidth  - margin ) dx_max = dx;
    if( dy < rwHeight - margin ) dy_max = dy;
  }

  double t0;              // tick begin
  double t1x, t1y;                 // tick end
  double ta; // short tick end 
  double tb;   // mid   tick end 
  double tc;  // long  tick end 

  if(false == InverseTicks)
  {
    t0 = margin;              // tick begin  
    ta = margin-shortTickLen; // short tick end 
    tb = margin-midTickLen;   // mid   tick end 
    tc = margin-longTickLen;  // long  tick end 
  }
  else
  {
    t0 = margin-longTickLen;              // tick begin
    ta = margin-longTickLen+shortTickLen; // short tick end 
    tb = margin-longTickLen+midTickLen;   // mid   tick end 
    tc = margin-longTickLen+longTickLen;  // long  tick end 
  }
  
  int id=0;

  // Update Axis and Ticks Points
  for(i=0; i<ntick; i++)
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

    if( dx > rwWidth  - margin ) { dx = dx_max ; t1x = t0; } // discard tick
    if( dy > rwHeight - margin ) { dy = dy_max ; t1y = t0; } 

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
   
    if( dx < rwWidth  - margin ) sprintf(labxtex, "%g",wx ); else sprintf(labxtex, "" );
    if( dy < rwHeight - margin ) sprintf(labytex, "%g",wy ); else sprintf(labytex, "" );

    Labx[i]->SetInput(labxtex);

    Labx[i]->SetDisplayPosition(Position[0]+dx , Position[1]+ margin + longTickLen  );
    Labx[i]->Modified();
/*
    Labx[i]->SetDisplayPosition(Position[0]+dx , Position[1]+margin - longTickLen -2 );
    Labx[i]->Modified();
*/

    Laby[i]->SetInput(labytex);
    Laby[i]->SetDisplayPosition(Position[0]+margin +4, Position[1]+dy );
    Laby[i]->Modified();
  }


  if (GlobalAxes) 
  {
    char *letter[] = {"x","y","z"};
    char *sign = (w1X-w0X > 0) ? (char *)" " : (char *)"-";

    char caption[100];
    sprintf(caption, "%s%s %s", sign,  letter[x_index], Legend);
    HorizontalAxesLabel->SetInput(caption);
    HorizontalAxesLabel->SetDisplayPosition(Position[0]+rwWidth - margin, Position[1]+margin + 4);

    sign = (w1Y-w0Y > 0) ? (char *)" " : (char *)"-";
    sprintf(caption, "%s%s %s", sign, letter[y_index], Legend);
    VerticalAxesLabel->SetInput(caption);
    VerticalAxesLabel->SetDisplayPosition( Position[0]+margin, Position[1]+rwHeight - margin/2);
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