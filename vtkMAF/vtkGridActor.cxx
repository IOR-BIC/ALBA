/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkGridActor.cxx,v $
  Language:  C++
  Date:      $Date: 2005-11-23 18:10:32 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/

#include "vtkGridActor.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"

#include "vtkLineSource.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkIdType.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty2D.h"
#include "vtkActor.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"

vtkCxxRevisionMacro(vtkGridActor, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkGridActor);
//------------------------------------------------------------------------------
vtkGridActor::vtkGridActor()
//------------------------------------------------------------------------------
{
	GridNormal	 = GRID_Z;
  GridPosition = 0;
	GridScale    = 1;
  GridSize		 = 50;	
	GridCreate();

	this->PickableOff();
}
//------------------------------------------------------------------------------
vtkGridActor::~vtkGridActor()
//------------------------------------------------------------------------------
{
  if(Grid)  Grid->Delete();
  if(Axis1) Axis1->Delete();
  if(Axis2) Axis2->Delete();
  if(Label) Label->Delete();
}
//------------------------------------------------------------------------------
void vtkGridActor::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkGridActor::RenderOpaqueGeometry(vtkViewport *vp)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(vp);
  vtkCamera *cam = ren->GetActiveCamera();
	if(cam->GetMTime() > this->GetMTime())
	{
	  GridUpdate(cam);
    this->Modified();
  }
	Grid->RenderOpaqueGeometry(vp);
	Axis1->RenderOpaqueGeometry(vp);
	Axis2->RenderOpaqueGeometry(vp);

	return 1;
}
//------------------------------------------------------------------------------
int vtkGridActor::RenderTranslucentGeometry(vtkViewport *vp)
//------------------------------------------------------------------------------
{
  return 0;
}
//------------------------------------------------------------------------------
void vtkGridActor::AdjustClippingRange(vtkViewport *vp)
//------------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkGridActor::GridCreate()
//----------------------------------------------------------------------------
{
	int nline = GridSize+1; //num lines in each direction
	int np = 4*nline;
	int nc = 2*nline;

	vtkPoints *gp = vtkPoints::New();
	gp->SetNumberOfPoints(np);
  
	vtkCellArray  *gc = vtkCellArray::New(); 
  gc->Allocate(gc->EstimateSize(nc,2));
  
	int i, id=0, cp[2];
  for(i=0; i<nline; i++)
	{
			gp->SetPoint(id,   i,				0,				0);
      gp->SetPoint(id+1, i,				GridSize, 0);
      gp->SetPoint(id+2, 0,				i,				0);
      gp->SetPoint(id+3, GridSize,i,				0);
      cp[0]=id, cp[1]=id+1;
			gc->InsertNextCell(2,cp);    
      cp[0]=id+2, cp[1]=id+3;
			gc->InsertNextCell(2,cp);
			id +=4;    
	}
  vtkPolyData *g_grid = vtkPolyData::New();
	g_grid->SetPoints(gp);
	g_grid->SetLines(gc);
  gp->Delete();
  gc->Delete();

  vtkPolyDataMapper *g_pdm = vtkPolyDataMapper::New();
  g_pdm->ImmediateModeRenderingOn();
  g_pdm->SetInput(g_grid);

  vtkProperty *g_p = vtkProperty::New();
  g_p->SetColor(0.5,0.5,0.5);
  g_p->SetAmbient(1);
  g_p->SetDiffuse(0);
  g_p->SetSpecular(0);
  g_p->SetInterpolationToFlat();
  g_p->SetRepresentationToWireframe();

  Grid = vtkActor::New();
  Grid->SetMapper(g_pdm);
  this->SetMapper(g_pdm); //to fix bounds problem 
	Grid->SetProperty(g_p);
  Grid->PickableOff();
 
  g_grid->Delete();
	g_pdm->Delete();
  g_p->Delete();

	// Axis Property///////////////////
  vtkProperty *a_p = vtkProperty::New();
  a_p->SetColor(0.5,0.5,0.5);
  a_p->SetAmbient(1);
  a_p->SetDiffuse(0);
  a_p->SetSpecular(0);
  a_p->SetInterpolationToFlat();
  a_p->SetRepresentationToWireframe();
  a_p->SetLineWidth(3);

	// Axis1 //////////////////////////
  vtkLineSource *a1 = vtkLineSource::New();
	a1->SetPoint1(0,0,0);
	a1->SetPoint2(GridSize,0,0);
  vtkPolyDataMapper *a1_pdm = vtkPolyDataMapper::New();
  a1_pdm->SetInput(a1->GetOutput());
  a1_pdm->ImmediateModeRenderingOn();
  Axis1 = vtkActor::New();
  Axis1->SetMapper(a1_pdm);
  Axis1->SetProperty(a_p);
  Axis1->PickableOff();
	a1->Delete();
	a1_pdm->Delete();

	// Axis2 //////////////////////////
  vtkLineSource *a2 = vtkLineSource::New();
	a2->SetPoint1(0,0,0);
	a2->SetPoint2(0,GridSize,0);
  vtkPolyDataMapper *a2_pdm = vtkPolyDataMapper::New();
  a2_pdm->SetInput(a2->GetOutput());
  a2_pdm->ImmediateModeRenderingOn();
  Axis2 = vtkActor::New();
  Axis2->SetMapper(a2_pdm);
  Axis2->SetProperty(a_p);
  Axis2->PickableOff();
	a2->Delete();
	a2_pdm->Delete();
  a_p->Delete();

  //Label //////////////////////////
  Label = vtkTextActor::New();
	//Label->GetProperty()->SetColor(0.6,0.6,0.6);
	Label->GetProperty()->SetColor(1,1,1);
	Label->ScaledTextOff();
	Label->SetDisplayPosition(5,5);
	Label->SetInput("");
	Label->GetTextProperty()->AntiAliasingOff();
	Label->GetTextProperty()->SetFontSize(12);
	Label->GetTextProperty()->SetFontFamilyToArial();
	//Label->GetTextProperty()->SetJustificationToRight();
	Label->GetTextProperty()->SetJustificationToLeft();
}
//----------------------------------------------------------------------------
double	vtkGridActor::Round(double val)
//----------------------------------------------------------------------------
{
	if (val == 0) 
    return 1;

	int sign = (val > 0) ? 1 : -1;
	val = fabs(val);

	int    exponent = (int)(log10(val));
	double mantissa = val/pow(10,exponent); 
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
	return sign*mantissa*pow(10,exponent);
}
//----------------------------------------------------------------------------
void	vtkGridActor::GridUpdate(vtkCamera *camera)
//----------------------------------------------------------------------------
{
	double cam[3];
	camera->GetPosition(cam);
	double focalp[3];
	camera->GetFocalPoint(focalp);

  double size;
  if(camera->GetParallelProjection())
	{
	  size = 16*(focalp[GridNormal] + camera->GetParallelScale()); //totally heuristic
	}
	else
	{
		double vdist[3];
		vdist[0] = focalp[0]-cam[0];
		vdist[1] = focalp[1]-cam[1];
		vdist[2] = focalp[2]-cam[2];
		double dist = focalp[GridNormal] + vtkMath::Norm(vdist); 
		size = 2*dist*tan(camera->GetViewAngle());
	}
	GridScale = Round(size/GridSize);
	Grid ->SetScale(GridScale);
	Axis1->SetScale(GridScale);
	Axis2->SetScale(GridScale);
 
  double pos[3];
	pos[0] = GridScale *( (int)(focalp[0]/GridScale) -GridSize/2 );
  pos[1] = GridScale *( (int)(focalp[1]/GridScale) -GridSize/2 );
  pos[2] = GridScale *( (int)(focalp[2]/GridScale) -GridSize/2 );
  pos[GridNormal] = GridPosition;
	Grid->SetPosition(pos);

	switch(GridNormal)
	{
    case GRID_X: 
		  Axis1->SetPosition(pos[0],0     ,pos[2]);
		  Axis2->SetPosition(pos[0],pos[1],0     );
    break;
    case GRID_Y: 
		  Axis1->SetPosition(pos[0],pos[1],0     );
		  Axis2->SetPosition(0     ,pos[1],pos[2]);
    break;
    case GRID_Z: 
		  Axis1->SetPosition(pos[0],0     ,pos[2]);
		  Axis2->SetPosition(0     ,pos[1],pos[2]);
    break;
  }
  
	char lab[50];
	sprintf(lab,"%g",GridScale);
  Label->SetInput(lab);

	this->SetPosition(Grid->GetPosition());  //to fix bounds problem 
	this->SetScale(Grid->GetScale());        //to fix bounds problem 
}
//----------------------------------------------------------------------------
void	vtkGridActor::SetGridNormal(int normal_id)
//----------------------------------------------------------------------------
{
	switch(normal_id)
	{
	  case GRID_X:
		  Grid->SetOrientation( 0,-90,  0); 
	  break;
	  case GRID_Y:
		  Grid->SetOrientation(90,  0,  0); 
	  break;
	  case GRID_Z:
		  Grid->SetOrientation( 0,  0,  0); 
	  break;
	  default:
	    return;
	  break;
	}
	GridNormal = normal_id;
	Axis1->SetOrientation(Grid->GetOrientation());
	Axis2->SetOrientation(Grid->GetOrientation());
	this->SetOrientation(Grid->GetOrientation()); //to fix bounds problem 
}
//----------------------------------------------------------------------------
void	vtkGridActor::SetGridPosition(double pos)
//----------------------------------------------------------------------------
{
   GridPosition = pos;
}
//----------------------------------------------------------------------------
void	vtkGridActor::SetGridColor(double r,double g,double b)
//----------------------------------------------------------------------------
{
  Grid->GetProperty()->SetColor(r,g,b);
	Axis1->GetProperty()->SetColor(r,g,b);
	Axis2->GetProperty()->SetColor(r,g,b);
}
