/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAGridActor
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAGridActor_h
#define __vtkALBAGridActor_h

#define DEFAULT_GRID_COLOR 0.5

//-----------------------------------------------------------------------------
// Includes:
//-----------------------------------------------------------------------------
#include "albaConfigure.h"
#include "vtkActor.h"
#include "vtkTextActor.h"

//-----------------------------------------------------------------------------
// Forward declarations:
//-----------------------------------------------------------------------------
class vtkViewport;
class vtkCamera;

//-----------------------------------------------------------------------------
// Constants:
//-----------------------------------------------------------------------------
enum GRID_ACTOR_ORIENTATION
{ 
  GRID_X = 0,
  GRID_Y,
  GRID_Z 
};

/**
this class create a grid to be used to represent one of the planes z=k, y=k, z=k.
The plane may be specified using SetGridNormal() 
and k may be specified with SetGridPosition().

To work correctly this actor must be hidden when ComputeVisiblePropBounds 
is called to Reset the Camera, and must be Shown when ComputeVisiblePropBounds 
is called to compute ClippingRange.
(sorry)
*/
//-----------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAGridActor : public vtkActor
//-----------------------------------------------------------------------------
{
 public:
  /** RTTI macro. */
  vtkTypeMacro(vtkALBAGridActor,vtkActor);
  /** Print information regarding the status of the object. */
  void PrintSelf(ostream& os, vtkIndent indent);
  /** Create an instance of the object. */
  static	vtkALBAGridActor *New();
  
  /** Set the normal of the grid.*/
	void			SetGridNormal   (int normal_id = GRID_Z);
  /** Set the position of the grid regarding along the normal.*/
	void			SetGridPosition (double position);
  /** Set grid color .*/
	void			SetGridColor    (double r,double g,double b);
  /** Retrieve the label actor that is initialized with grid scale value.*/
	vtkTextActor		 *GetLabelActor() {return Label;};

  /** Adjust the clipping range (this method is empty).*/
  virtual  void 		AdjustClippingRange(vtkViewport *viewport);
  /** Method is intended for rendering Opaque Geometry.*/
  virtual  int			RenderOpaqueGeometry(vtkViewport *viewport);
  /** Method is intended for rendering Translucent Geometry. */
  virtual  int			RenderTranslucentGeometry(vtkViewport *viewport);

protected:
  /** constructor. */
  vtkALBAGridActor();
  /** destructor. */
  ~vtkALBAGridActor();

  /** Create an instance of the grid that is a polydata.*/
	void			GridCreate ();
  /** Update grid position, label , scale.*/
  void			GridUpdate(vtkCamera *camera);
  /** Round a double value. Used to calculate the grid scale*/
  double		Round(double val);

	int 							GridSize;
	float 						GridScale;
	int 							GridNormal;
	double 						GridPosition;
  vtkActor				 *Grid;  
  vtkActor				 *Axis1; 
  vtkActor				 *Axis2; 
	vtkTextActor     *Label; 

private:
  /** Hide the two parameter Render() method from the user and the compiler.*/
  virtual void Render(vtkRenderer *, vtkMapper *) {};
  /** Copy Constructor Not implemented. */
  vtkALBAGridActor(const vtkALBAGridActor&);  	// Not implemented.
  /** Assign Operator Not implemented. */
  void operator=(const vtkALBAGridActor&);  // Not implemented.
};
#endif
