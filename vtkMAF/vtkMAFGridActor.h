/*=========================================================================

 Program: MAF2
 Module: vtkMAFGridActor
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFGridActor_h
#define __vtkMAFGridActor_h

#define DEFAULT_GRID_COLOR 0.5

//-----------------------------------------------------------------------------
// Includes:
//-----------------------------------------------------------------------------
#include "mafConfigure.h"
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
class MAF_EXPORT vtkMAFGridActor : public vtkActor
//-----------------------------------------------------------------------------
{
 public:
  /** RTTI macro. */
  vtkTypeRevisionMacro(vtkMAFGridActor,vtkActor);
  /** Print information regarding the status of the object. */
  void PrintSelf(ostream& os, vtkIndent indent);
  /** Create an instance of the object. */
  static	vtkMAFGridActor *New();
  
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
  vtkMAFGridActor();
  /** destructor. */
  ~vtkMAFGridActor();

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
  vtkMAFGridActor(const vtkMAFGridActor&);  	// Not implemented.
  /** Assign Operator Not implemented. */
  void operator=(const vtkMAFGridActor&);  // Not implemented.
};
#endif
