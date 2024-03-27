/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBASimpleRulerActor2D
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBASimpleRulerActor2D.h"

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

vtkStandardNewMacro(vtkALBASimpleRulerActor2D);
//------------------------------------------------------------------------------
vtkALBASimpleRulerActor2D::vtkALBASimpleRulerActor2D()
//------------------------------------------------------------------------------
{
  Margin   = 15; 
  ShortTickLen = 5;
  LongTickLen  = 10;
  DesiredTickSpacing = 15;
  Ntick    = 800; // enough for a tick spacing = 15 on a 1600x1200 screen
  ScaleFactor = 1.0;

	XOffSet = -2;
	YOffSet =  4;

  ScaleLabelVisibility  = true;
  AxesLabelVisibility   = true;
  AxesVisibility        = true;
  TickVisibility        = true;
	FixedTickVisibility   = false;
  CenterAxes            = true;
  GlobalAxes            = true;

  InverseTicks          = true;
	Position[0] = Position[1] = Position[2] = 0;
	PositionDisplay[0] = PositionDisplay[1] = PositionDisplay[2] = 0;
	PositionWorld[0] = PositionWorld[1] = PositionWorld[2] = 0;
	AttachPositionFlag = false;

  Legend = NULL;
  Axis = Tick = ScaleLabel = NULL;
  Points = NULL;
  XIndex = 0;
  YIndex = 1;

  FixedTickPoints = NULL;
  FixedTickActor = NULL;
  FixedTickLabel = NULL;

	RulerCreate();

  this->PickableOff();
}
//------------------------------------------------------------------------------
vtkALBASimpleRulerActor2D::~vtkALBASimpleRulerActor2D()
//------------------------------------------------------------------------------
{
  if(Points) Points->Delete();
  if(Axis)   Axis->Delete();
  if(Tick)   Tick->Delete();
  if(ScaleLabel)  ScaleLabel->Delete();
  if(HorizontalAxesLabel) HorizontalAxesLabel->Delete();
  if(VerticalAxesLabel) VerticalAxesLabel->Delete();
  if (this->Legend) 
  { 
    delete [] this->Legend; 
  }

  if (FixedTickPoints) FixedTickPoints->Delete();
  if (FixedTickActor) FixedTickActor->Delete();
  if (FixedTickLabel) FixedTickLabel->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkALBASimpleRulerActor2D::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

	RwWidth = ren->GetRenderWindow()->GetSize()[0];
	RwHeight = ren->GetRenderWindow()->GetSize()[1];

	vtkCamera *cam = ren->GetActiveCamera();
	if (!cam->GetParallelProjection()) return 0;

	if (!CheckProjectionPlane(cam) && GlobalAxes) return 0;
	RulerUpdate(cam, ren);
	this->Modified();

	if (AxesVisibility)       Axis->RenderOverlay(viewport);
	if (TickVisibility)       Tick->RenderOverlay(viewport);
	if (ScaleLabelVisibility) ScaleLabel->RenderOverlay(viewport);
	if (AxesLabelVisibility && GlobalAxes)  HorizontalAxesLabel->RenderOverlay(viewport);
	if (AxesLabelVisibility && GlobalAxes)  VerticalAxesLabel->RenderOverlay(viewport);

	if (FixedTickVisibility) FixedTickActor->RenderOverlay(viewport);
	if (FixedTickVisibility) FixedTickLabel->RenderOverlay(viewport);

	return 1;
}
//----------------------------------------------------------------------------
int vtkALBASimpleRulerActor2D::RenderOpaqueGeometry(vtkViewport *viewport)
//----------------------------------------------------------------------------
{
  if (ScaleLabelVisibility)ScaleLabel->RenderOpaqueGeometry(viewport);
  if (AxesLabelVisibility && GlobalAxes) HorizontalAxesLabel->RenderOpaqueGeometry(viewport);
  if (AxesLabelVisibility && GlobalAxes) VerticalAxesLabel->RenderOpaqueGeometry(viewport);

		if (FixedTickVisibility) FixedTickLabel->RenderOpaqueGeometry(viewport);

  return 0;
}
//----------------------------------------------------------------------------
bool vtkALBASimpleRulerActor2D::CheckProjectionPlane(vtkCamera *cam)
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
      XIndex = 2;
      YIndex = 1;
    }
    else if (abs(vtkMath::Dot(vu,z_axes)) > threshold)
    {
      XIndex = 1;
      YIndex = 2;
    }
    else
      return false;
  }
  else if(abs(vtkMath::Dot(vpn,y_axes)) > threshold)
  {
    if (abs(vtkMath::Dot(vu,x_axes)) > threshold)
    {
      XIndex = 2;
      YIndex = 0;
    }
    else if (abs(vtkMath::Dot(vu,z_axes)) > threshold)
    {
      XIndex = 0;
      YIndex = 2;
    }
    else
      return false;
  }
  else if(abs(vtkMath::Dot(vpn,z_axes)) > threshold)
  {
    if (abs(vtkMath::Dot(vu,y_axes)) > threshold)
    {
      XIndex = 0;
      YIndex = 1;
    }
    else if (abs(vtkMath::Dot(vu,x_axes)) > threshold)
    {
      XIndex = 1;
      YIndex = 0;
    }
    else
      return false;
  }
  else
    return false;

  return true;
}
//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::RulerCreate()
//----------------------------------------------------------------------------
{
	Points = vtkPoints::New();
	Points->SetNumberOfPoints(4*Ntick);
  
  vtkCellArray  *tick_cell = vtkCellArray::New(); 
  tick_cell->Allocate(tick_cell->EstimateSize(2*Ntick,2));

  double m0 = Margin;
  double m1 = Margin - 2;

	vtkIdType i, id=0, cp[2];
  for(i=0; i<Ntick; i++)
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
  axis_p->Delete();

		//Label //////////////////////////
  ScaleLabel = vtkTextActor::New();
	ScaleLabel->GetProperty()->SetColor(1,0,0);
  ScaleLabel->GetTextProperty()->SetFontSize(12);
  ScaleLabel->GetTextProperty()->SetFontFamilyToArial();
  ScaleLabel->GetTextProperty()->SetJustificationToLeft();
	ScaleLabel->SetTextScaleModeToNone();
	ScaleLabel->SetDisplayPosition(Position[0] + Margin + 4, Position[1] + Margin + 4);
	ScaleLabel->SetInput("");

  HorizontalAxesLabel = vtkTextActor::New();
  HorizontalAxesLabel->GetProperty()->SetColor(1,0,0);
  HorizontalAxesLabel->GetTextProperty()->SetFontSize(12);
  HorizontalAxesLabel->GetTextProperty()->SetFontFamilyToArial();
  HorizontalAxesLabel->GetTextProperty()->SetJustificationToRight();
  HorizontalAxesLabel->SetTextScaleModeToNone();
	HorizontalAxesLabel->SetDisplayPosition(Position[0], Position[1]);
  HorizontalAxesLabel->SetInput("");

  VerticalAxesLabel = vtkTextActor::New();
  VerticalAxesLabel->GetProperty()->SetColor(1,0,0);
  VerticalAxesLabel->GetTextProperty()->SetFontSize(12);
  VerticalAxesLabel->GetTextProperty()->SetFontFamilyToArial();
  VerticalAxesLabel->GetTextProperty()->SetJustificationToLeft();
  VerticalAxesLabel->SetTextScaleModeToNone();
  VerticalAxesLabel->SetDisplayPosition(Position[0],Position[1]);
  VerticalAxesLabel->SetInput("");

	// Fixed Tick
	CreateFixedTick();
}

//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::CreateFixedTick()
{
	int nSegments = 3;
	FixedTickPoints = vtkPoints::New();
	FixedTickPoints->SetNumberOfPoints(nSegments * 2);

	vtkCellArray  *fixedTick_CellArray = vtkCellArray::New();
	fixedTick_CellArray->Allocate(fixedTick_CellArray->EstimateSize(nSegments, 2));

	vtkIdType id = 0;
	vtkIdType cp[2];

	for (int i = 0; i < nSegments; i++)
	{
		FixedTickPoints->SetPoint(id, Margin, i, 0);
		FixedTickPoints->SetPoint(id + 1, Margin, i, 0);

		cp[0] = id, cp[1] = id + 1;
		fixedTick_CellArray->InsertNextCell(2, cp);

		id += 2;
	}

	vtkPolyData *fixedTick_PolyData = vtkPolyData::New();
	fixedTick_PolyData->SetPoints(FixedTickPoints);
	fixedTick_PolyData->SetLines(fixedTick_CellArray);

	vtkPolyDataMapper2D *fixedTick_PolyDataMapper = vtkPolyDataMapper2D::New();
	fixedTick_PolyDataMapper->SetInputData(fixedTick_PolyData);

	vtkProperty2D *fixedTick_Property2D = vtkProperty2D::New();
	fixedTick_Property2D->SetColor(1, 0, 0);
	fixedTick_Property2D->SetLineWidth(1);

	FixedTickActor = vtkActor2D::New();
	FixedTickActor->SetMapper(fixedTick_PolyDataMapper);
	FixedTickActor->SetProperty(fixedTick_Property2D);
	FixedTickActor->SetPosition(0, 0);

	fixedTick_CellArray->Delete();
	fixedTick_PolyData->Delete();
	fixedTick_PolyDataMapper->Delete();
	fixedTick_Property2D->Delete();

	//Label //////////////////////////
	FixedTickLabel = vtkTextActor::New();
	FixedTickLabel->GetProperty()->SetColor(1, 0, 0);
	FixedTickLabel->GetTextProperty()->SetFontSize(12);
	FixedTickLabel->GetTextProperty()->SetFontFamilyToArial();
	FixedTickLabel->GetTextProperty()->SetJustificationToCentered();
	FixedTickLabel->SetTextScaleModeToNone();
	FixedTickLabel->SetDisplayPosition(Position[0] + Margin + 4, Position[1] + RwHeight - Margin);
	FixedTickLabel->SetInput("Cm");
}

//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::SetColor(double r,double g,double b)
//----------------------------------------------------------------------------
{
	 Axis->GetProperty()->SetColor(r,g,b);
  Tick->GetProperty()->SetColor(r,g,b);
  ScaleLabel->GetProperty()->SetColor(r,g,b);
  HorizontalAxesLabel->GetProperty()->SetColor(r,g,b);
  VerticalAxesLabel->GetProperty()->SetColor(r,g,b);

	FixedTickActor->GetProperty()->SetColor(r, g, b);
	FixedTickLabel->GetProperty()->SetColor(r, g, b);
}
//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::SetScaleFactor(double factor)
//----------------------------------------------------------------------------
{
  if (factor != 0) 
  {
    ScaleFactor = factor;
    Modified();
  }
}
//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::SetLegend(const char *legend)
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
void vtkALBASimpleRulerActor2D::CenterAxesOnScreen(bool center)
//----------------------------------------------------------------------------
{
  CenterAxes = center;
  this->Modified();
}
//----------------------------------------------------------------------------
double vtkALBASimpleRulerActor2D::RicomposeValue(int sign, double mantissa, int exponent)
//----------------------------------------------------------------------------
{
  return sign*mantissa*pow(10.0f,exponent); // base in pow must be float
}
//----------------------------------------------------------------------------
int vtkALBASimpleRulerActor2D::Round(double val)
//----------------------------------------------------------------------------
{
  int sign = ( val >= 0 ) ? 1 : -1 ;
  return sign * (int)( abs(val) + 0.5 );
}
//----------------------------------------------------------------------------
bool vtkALBASimpleRulerActor2D::IsMultiple(double val, double multiplier)
//----------------------------------------------------------------------------
{
  double ratio = val/multiplier; 
  double fractionalPart = ratio - (int)(ratio);
  return fractionalPart == 0;
}
//----------------------------------------------------------------------------
double vtkALBASimpleRulerActor2D::NearestTick(double val, double TickSpacing)
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
void vtkALBASimpleRulerActor2D::DecomposeValue(double val, int *sign, double *mantissa, int *exponent)
//----------------------------------------------------------------------------
{
  // val == 0  ---> mantissa = 0
  // val != 0  ---> mantissa = 1.....1.9999

  *sign = (val >= 0) ? 1 : -1;

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
    (*mantissa) *= 10; // ( e ) matters !!
    (*exponent)--;     // ( e ) matters !!
  }
}
//----------------------------------------------------------------------------
double vtkALBASimpleRulerActor2D::GetTickSpacing(double val)
//----------------------------------------------------------------------------
{
  if( val == 0 ) return 0;

  int sign, exponent;
  double mantissa;
  DecomposeValue(val,&sign,&mantissa,&exponent);


  if(mantissa<2)       // 1.00 ... 1.99  -> 1    // -1.99 ... -1.00  -> -1
    mantissa=1;
  else if(mantissa<5)  // 2.00 ... 4.99  -> 2    // -4.99 ... -2.00  -> -2
    mantissa=2;
  else                 // 5.00 ... 9.99  -> 5    // -9.99 ... -5.00  -> -5
    mantissa=5;

  return RicomposeValue(sign,mantissa,exponent);
}
//----------------------------------------------------------------------------
double vtkALBASimpleRulerActor2D::GetLongTickSpacing(double val)
//----------------------------------------------------------------------------
{
  if( val == 0 ) return 0;
  
  int sign, exponent;
  double mantissa;
  DecomposeValue(val,&sign,&mantissa,&exponent);
  
  mantissa=10;

  return RicomposeValue(sign,mantissa,exponent);
}
//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::RulerUpdate(vtkCamera *camera, vtkRenderer *ren)
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

	if (true == AttachPositionFlag)
	{
		ren->SetWorldPoint(PositionWorld[0], PositionWorld[1], PositionWorld[2], 1.);
		ren->WorldToDisplay();
		ren->GetDisplayPoint(PositionDisplay);

		Position[0] = PositionDisplay[0] - Margin;
		Position[1] = PositionDisplay[1] - Margin;
		Position[2] = PositionDisplay[2];
	}

	double p[4], p0[4], p1[4];

	ren->SetDisplayPoint(Position[0] + Margin, Position[1] + Margin, 0);
	ren->DisplayToWorld();
	ren->GetWorldPoint(p);

	ren->SetDisplayPoint(Position[0], Position[1], 0);
	ren->DisplayToWorld();
	ren->GetWorldPoint(p0);

	ren->SetDisplayPoint(Position[0] + RwWidth, Position[1] + RwHeight, 0);
	ren->DisplayToWorld();
	ren->GetWorldPoint(p1);

	int i;
	for (i = 0; i < 3; i++)
	{
		p[i] /= ScaleFactor;
		p0[i] /= ScaleFactor;
		p1[i] /= ScaleFactor;
	}

	double w0X, w0Y, w1X, w1Y, wpX, wpY, vx[3], vy[3];
	if (GlobalAxes)
	{
		w0X = p0[XIndex];
		w0Y = p0[YIndex];
		w1X = p1[XIndex];
		w1Y = p1[YIndex];
		wpX = p[XIndex];
		wpY = p[YIndex];
	}
	else
	{
		double vpn[3];
		camera->GetViewUp(vy);
		camera->GetViewPlaneNormal(vpn);
		vtkMath::Cross(vy, vpn, vx);
		w0X = vtkMath::Dot(vx, p0);
		w0Y = vtkMath::Dot(vy, p0);
		w1X = vtkMath::Dot(vx, p1);
		w1Y = vtkMath::Dot(vy, p1);
		wpX = vtkMath::Dot(vx, p);
		wpY = vtkMath::Dot(vy, p);
	}

	double w2dX = RwWidth / (w1X - w0X), w2dY = RwHeight / (w1Y - w0Y);  // world to display
	double d2wX = 1 / w2dX, d2wY = 1 / w2dY;              // display to world along Y

	// determine tick spacing
	double desiredWorldTickSpacingX = DesiredTickSpacing * d2wX;
	double worldTickSpacingX = GetTickSpacing(desiredWorldTickSpacingX);
	double worldLongTickSpacingX = GetLongTickSpacing(desiredWorldTickSpacingX);
	double worldFirstTickX = NearestTick(wpX, worldTickSpacingX);
	double worldFirstLongTickX = NearestTick(wpX, worldLongTickSpacingX);

	double desiredWorldTickSpacingY = DesiredTickSpacing * d2wY;
	double worldTickSpacingY = GetTickSpacing(desiredWorldTickSpacingY);
	double worldLongTickSpacingY = GetLongTickSpacing(desiredWorldTickSpacingY);
	double worldFirstTickY = NearestTick(wpY, worldTickSpacingY);
	double worldFirstLongTickY = NearestTick(wpY, worldLongTickSpacingY);

	// find last tick pos
	double dx_max;
	double dy_max;
	for (i = 0; i < Ntick; i++)
	{
		double wx = worldFirstTickX + i * worldTickSpacingX;
		double wy = worldFirstTickY + i * worldTickSpacingY;
		double dx = (wx - w0X) * w2dX;
		double dy = (wy - w0Y) * w2dY;
		if (dx < RwWidth - Margin) dx_max = dx;
		if (dy < RwHeight - Margin) dy_max = dy;
	}

	double axesOffsetX = (CenterAxes) ? RwWidth / 2 - Margin : ren->GetOrigin()[0];
	double axesOffsetY = (CenterAxes) ? RwHeight / 2 - Margin : ren->GetOrigin()[1];


	double t0;              // tick begin
	double t1x, t1y; // tick end 
	double ta; // short tick end 
	double tc;  // long  tick end 

	if (false == InverseTicks)
	{
		t0 = Margin;              // tick begin
		ta = Margin - ShortTickLen; // short tick end 
		tc = Margin - LongTickLen;  // long  tick end 
	}
	else
	{
		t0 = Margin - LongTickLen;              // tick begin
		ta = Margin - LongTickLen + ShortTickLen; // short tick end 
		tc = Margin - LongTickLen + LongTickLen;  // long  tick end 
	}


	int id = 0;
	double fixedSpacing = 0.0;

	// Update Axis and Ticks Points
	for (i = 0; i < Ntick; i++)
	{
		double wx = worldFirstTickX + i * worldTickSpacingX;
		double wy = worldFirstTickY + i * worldTickSpacingY;

		// decide length of tick
		if (IsMultiple(wx, worldLongTickSpacingX)) t1x = tc; else t1x = ta;

		if (IsMultiple(wy, worldLongTickSpacingY)) t1y = tc; else t1y = ta;

		double dx = (wx - w0X) * w2dX;  // bring to Display coordinates
		double dy = (wy - w0Y) * w2dY;

		if (dx > RwWidth - Margin) { dx = dx_max; t1x = t0; } // discard tick
		if (dy > RwHeight - Margin) { dy = dy_max; t1y = t0; }

		if (i == 0)
		{
			fixedSpacing = dx;
		}

		if (i == 1)
		{
			fixedSpacing = dx - fixedSpacing;
		}

		Points->SetPoint(id++, Position[0] + dx, Position[1] + t0 + axesOffsetY, 0);
		Points->SetPoint(id++, Position[0] + dx, Position[1] + t1x + axesOffsetY, 0);
		Points->SetPoint(id++, Position[0] + t0 + axesOffsetX, Position[1] + dy, 0);
		Points->SetPoint(id++, Position[0] + t1y + axesOffsetX, Position[1] + dy, 0);
	}

	if (FixedTickPoints)
	{
		double pos1X = Position[0] + 100.0;
		double pos2X = pos1X + fixedSpacing *(10.0 / abs(worldTickSpacingX));
		double posY = Position[1] + RwHeight - Margin;

		FixedTickLabel->SetInput("1 Cm");

		if (pos2X / (RwWidth / 2) > 1)
		{
			pos2X = pos1X + (fixedSpacing *(10.0 / abs(worldTickSpacingX))/10.0);
			FixedTickLabel->SetInput("1 mm");
		}

		double offSet = 0.0;

		FixedTickLabel->SetDisplayPosition(pos1X + offSet + (pos2X - pos1X) / 2, posY + 5.0);

		FixedTickPoints->SetPoint(0, pos1X + offSet, posY, 0); // p1
		FixedTickPoints->SetPoint(1, pos2X + offSet, posY, 0); // p2

		FixedTickPoints->SetPoint(2, pos1X + offSet, posY - 10.0, 0); //  
		FixedTickPoints->SetPoint(3, pos1X + offSet, posY + 10.0, 0); //

		FixedTickPoints->SetPoint(4, pos2X + offSet, posY - 10.0, 0);
		FixedTickPoints->SetPoint(5, pos2X + offSet, posY + 10.0, 0);
	}

	if (GlobalAxes)
	{
		char *alab[] = { "x","y","z","-x","-y","-z" };
		int direction = (w1X - w0X > 0) ? 0 : 3;
		HorizontalAxesLabel->SetInput(alab[XIndex + direction]);
		HorizontalAxesLabel->SetDisplayPosition(Position[0] + RwWidth - Margin, Position[1] + axesOffsetY + Margin + 4);

		direction = (w1Y - w0Y > 0) ? 0 : 3;
		VerticalAxesLabel->SetInput(alab[YIndex + direction]);
		VerticalAxesLabel->SetDisplayPosition(Position[0] + axesOffsetX + Margin + 4, Position[1] + RwHeight - Margin);
	}

	char lab[50];
	sprintf(lab, "%g %s", abs(worldTickSpacingX), Legend);
	ScaleLabel->SetInput(lab);
	ScaleLabel->SetDisplayPosition(Position[0] + axesOffsetX + Margin + 4, Position[1] + axesOffsetY + Margin + 4);
}
//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::SetAttachPosition(double position[3])
//----------------------------------------------------------------------------
{
	PositionWorld[0] = position[0];
	PositionWorld[1] = position[1];
	PositionWorld[2] = position[2];
}
//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::SetAttachPositionFlag(bool value)
//----------------------------------------------------------------------------
{
	AttachPositionFlag = value;
}
//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2D::ChangeRulerMarginsAndLengths(int marginArg, int shortTickLenArg, int midTickLenArg, int longTickLenArg, int xOffSetArg, int yOffSetArg)
//----------------------------------------------------------------------------
{
	Margin = marginArg;
	ShortTickLen = shortTickLenArg;
	//midTickLen   = midTickLenArg;
	LongTickLen = longTickLenArg;

	XOffSet = xOffSetArg;
	YOffSet = yOffSetArg;
}

