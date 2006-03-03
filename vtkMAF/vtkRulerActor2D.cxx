/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkRulerActor2D.cxx,v $
  Language:  C++
  Date:      $Date: 2006-03-03 15:53:44 $
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
  margin   = 30; 
  shortTickLen = 5;
  midTickLen   = 10;
  longTickLen  = 15;
  DesiredTickSpacing = 60;
  ntick    = 800; // enough for a tick spacing = 15 on a 1600x1200 screen
  ScaleFactor = 1.0;

  ScaleLabelVisibility  = true;
  AxesLabelVisibility   = true;
  AxesVisibility        = false;
  TickVisibility        = true;

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
vtkRulerActor2D::~vtkRulerActor2D()
//------------------------------------------------------------------------------
{
  if(Axis)   Axis->Delete();
  if(Tick)   Tick->Delete();
  if(ScaleLabel)  ScaleLabel->Delete();
  if(HorizontalAxeLabel) HorizontalAxeLabel->Delete();
  if(VerticalAxeLabel) VerticalAxeLabel->Delete();
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

  if (AxesVisibility)       Axis->  RenderOverlay(viewport);
  if (TickVisibility)       Tick->  RenderOverlay(viewport);
  if (ScaleLabelVisibility) ScaleLabel-> RenderOverlay(viewport);
  if (AxesLabelVisibility)  HorizontalAxeLabel->RenderOverlay(viewport);
  if (AxesLabelVisibility)  VerticalAxeLabel->RenderOverlay(viewport);

  for(int i=0; i<NUM_LAB; i++)
  {
    Labx[i]->RenderOverlay(viewport);
    Laby[i]->RenderOverlay(viewport);
  }
  return 1;
}
//----------------------------------------------------------------------------
int vtkRulerActor2D::RenderOpaqueGeometry(vtkViewport *viewport)
//----------------------------------------------------------------------------
{
  if (ScaleLabelVisibility) ScaleLabel->RenderOpaqueGeometry(viewport);
  if (AxesLabelVisibility) HorizontalAxeLabel->RenderOpaqueGeometry(viewport);
  if (AxesLabelVisibility) VerticalAxeLabel->RenderOpaqueGeometry(viewport);

  for(int i=0; i<NUM_LAB; i++)
  {
    Labx[i]->RenderOpaqueGeometry(viewport);
    Laby[i]->RenderOpaqueGeometry(viewport);
  }

  return 0;
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
  double m1 = margin - 2;

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
  ScaleLabel = vtkTextActor::New();
	ScaleLabel->GetProperty()->SetColor(1,0,0);
  ScaleLabel->GetTextProperty()->AntiAliasingOff();
  ScaleLabel->GetTextProperty()->SetFontSize(12);
  ScaleLabel->GetTextProperty()->SetFontFamilyToArial();
  ScaleLabel->GetTextProperty()->SetJustificationToLeft();
	ScaleLabel->ScaledTextOff();
	ScaleLabel->SetDisplayPosition(margin + 4,margin + 4);
	ScaleLabel->SetInput("");

  HorizontalAxeLabel = vtkTextActor::New();
  HorizontalAxeLabel->GetProperty()->SetColor(1,0,0);
  HorizontalAxeLabel->GetTextProperty()->AntiAliasingOff();
  HorizontalAxeLabel->GetTextProperty()->SetFontSize(12);
  HorizontalAxeLabel->GetTextProperty()->SetFontFamilyToArial();
  HorizontalAxeLabel->GetTextProperty()->SetJustificationToRight();
  HorizontalAxeLabel->ScaledTextOff();
  HorizontalAxeLabel->SetDisplayPosition(0,0);
  HorizontalAxeLabel->SetInput("");

  VerticalAxeLabel = vtkTextActor::New();
  VerticalAxeLabel->GetProperty()->SetColor(1,0,0);
  VerticalAxeLabel->GetTextProperty()->AntiAliasingOff();
  VerticalAxeLabel->GetTextProperty()->SetFontSize(12);
  VerticalAxeLabel->GetTextProperty()->SetFontFamilyToArial();
  VerticalAxeLabel->GetTextProperty()->SetJustificationToLeft();
  VerticalAxeLabel->ScaledTextOff();
  VerticalAxeLabel->SetDisplayPosition(0,0);
  VerticalAxeLabel->SetInput("");

  for(int i=0; i<NUM_LAB; i++)
  {
    Labx[i] = vtkTextActor::New();
    Labx[i]->GetProperty()->SetColor(1,1,1);
    Labx[i]->GetTextProperty()->AntiAliasingOff();
    Labx[i]->GetTextProperty()->SetFontSize(12);
    Labx[i]->GetTextProperty()->SetFontFamilyToArial();
    Labx[i]->GetTextProperty()->SetJustificationToCentered();
    Labx[i]->GetTextProperty()->SetVerticalJustificationToTop();
    Labx[i]->ScaledTextOff();
    Labx[i]->SetDisplayPosition(0,0);
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
    Laby[i]->SetDisplayPosition(0,0);
    Laby[i]->SetInput("");
  }
}
//----------------------------------------------------------------------------
void vtkRulerActor2D::SetColor(double r,double g,double b)
//----------------------------------------------------------------------------
{
	Axis->GetProperty()->SetColor(r,g,b);
  Tick->GetProperty()->SetColor(r,g,b);
  ScaleLabel->GetProperty()->SetColor(r,g,b);
  HorizontalAxeLabel->GetProperty()->SetColor(r,g,b);
  VerticalAxeLabel->GetProperty()->SetColor(r,g,b);

}
//----------------------------------------------------------------------------
void vtkRulerActor2D::SetScaleFactor(double factor)
//----------------------------------------------------------------------------
{
  if (factor != 0) ScaleFactor = factor;
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
    SetLegend(" ");
    //this->Legend = NULL;
  }
}
//----------------------------------------------------------------------------
void vtkRulerActor2D::DecomposeValue(double val, int *sign, double *mantissa, int *exponent)
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
double vtkRulerActor2D::RicomposeValue(int sign, double mantissa, int exponent)
//----------------------------------------------------------------------------
{
  return sign*mantissa*pow(10.0f,exponent); // base in pow must be float
}
//----------------------------------------------------------------------------
double vtkRulerActor2D::GetTickSpacing(double val)
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
double vtkRulerActor2D::GetMidTickSpacing(double val)
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
double vtkRulerActor2D::GetLongTickSpacing(double val)
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
int vtkRulerActor2D::round(double val)
//----------------------------------------------------------------------------
{
  int sign = ( val >= 0 ) ? 1 : -1 ;
  return sign * (int)( abs(val) + 0.5 );
}
//----------------------------------------------------------------------------
bool vtkRulerActor2D::IsMultiple(double val, double multiplier)
//----------------------------------------------------------------------------
{
  double ratio = val/multiplier; 
  double fractionalPart = ratio - (int)(ratio);
  return fractionalPart == 0;
}
//----------------------------------------------------------------------------
double vtkRulerActor2D::NearestTick(double val, double TickSpacing)
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
void vtkRulerActor2D::RulerUpdate(vtkCamera *camera, vtkRenderer *ren)
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
    p[i]  /= ScaleFactor; 
    p0[i] /= ScaleFactor; 
    p1[i] /= ScaleFactor; 
  }

  double w0X =  p0[x_index], w0Y =  p0[y_index];
  double w1X =  p1[x_index], w1Y =  p1[y_index];
  double wpX =   p[x_index], wpY =   p[y_index];

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

  //double worldLastTickPoseX = NearestTick(  (rwWidth - margin) *d2wX, worldTickSpacingX );
  //double dx0 = (worldLastTickPoseX - w0X ) * w2dX;
  //double worldLastTickPoseY = NearestTick(  (rwHeight - margin) *d2wY, worldTickSpacingY );
  //double dy0 = (worldLastTickPoseX - w0Y ) * w2dY;
  
  //double dx0 = rwWidth - margin;//(worldFirstTickX - w0X ) * w2dX; // --- discarded tick are drawn here
  //double dy0 = rwHeight- margin;//(worldFirstTickY - w0Y ) * w2dY; // 

  //double dx0 = NearestTick( rwWidth - margin, worldTickSpacingX * w2dX );
  //double dy0 = NearestTick( rwHeight - margin, worldTickSpacingY * w2dY );


  // find last tick pos
  double dx_max;
  double dy_max;
  for(int i=0; i<ntick; i++)
  {
    double wx = worldFirstTickX + i * worldTickSpacingX;
    double wy = worldFirstTickY + i * worldTickSpacingY;
    double dx = (wx - w0X ) * w2dX;  
    double dy = (wy - w0Y ) * w2dY;
    if( dx < rwWidth  - margin ) dx_max = dx;
    if( dy < rwHeight - margin ) dy_max = dy;
  }

  double t0 = margin;              // tick begin
  double t1x, t1y;                 // tick end
  double ta = margin-shortTickLen; // short tick end 
  double tb = margin-midTickLen;   // mid   tick end 
  double tc = margin-longTickLen;  // long  tick end 
  int id=0;

  // Update Axis and Ticks Points
  for(int i=0; i<ntick; i++)
  {
    double wx = worldFirstTickX + i * worldTickSpacingX;
    double wy = worldFirstTickY + i * worldTickSpacingY;

    // decide lenght of tick
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

    Points->SetPoint(id++, dx,  t0,   0);
    Points->SetPoint(id++, dx,	t1x,  0);
    Points->SetPoint(id++, t0,  dy,   0);
    Points->SetPoint(id++, t1y, dy,   0);
 }
  
  char labxtex[50];
  char labytex[50];
  for(int i=0; i<NUM_LAB; i++)
  {
    double wx = worldFirstLongTickX + i * worldLongTickSpacingX;
    double wy = worldFirstLongTickY + i * worldLongTickSpacingY;
    
    double dx = (wx - w0X ) * w2dX;  // bring to Display coords
    double dy = (wy - w0Y ) * w2dY;
   
    if( dx < rwWidth  - margin ) sprintf(labxtex, "%g",wx ); else sprintf(labxtex, "" );
    if( dy < rwHeight - margin ) sprintf(labytex, "%g",wy ); else sprintf(labytex, "" );

    Labx[i]->SetInput(labxtex);
    Labx[i]->SetDisplayPosition(dx , margin - longTickLen -2 );

    Laby[i]->SetInput(labytex);
  //Laby[i]->SetDisplayPosition(margin - longTickLen -2, dy );
    Laby[i]->SetDisplayPosition(margin +4, dy );
  }


  char *letter[] = {"x","y","z"};
  char *sign = (w1X-w0X > 0) ? " " : "-";
  char caption[100];
  sprintf(caption, "%s%s %s", sign ,  letter[x_index],   Legend );
  HorizontalAxeLabel->SetInput(caption);
  HorizontalAxeLabel->SetDisplayPosition(rwWidth - margin, margin + 4);

  sign = (w1Y-w0Y > 0) ? " " : "-";
  sprintf(caption, "%s%s %s", sign,    letter[y_index],   Legend );
  VerticalAxeLabel->SetInput(caption);
  VerticalAxeLabel->SetDisplayPosition( margin , rwHeight - margin/2);

  char lab[50];
  sprintf(lab,"%g %s", abs( worldTickSpacingX ) ,Legend);
  ScaleLabel->SetInput(lab);

}


