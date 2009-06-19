/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFGridActor.h,v $
  Language:  C++
  Date:      $Date: 2009-06-19 12:48:08 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2002/2003 
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.
=========================================================================*/

#ifndef __vtkMAFGridActor_h
#define __vtkMAFGridActor_h

#define DEFAULT_GRID_COLOR 0.5

//-----------------------------------------------------------------------------
// Includes:
//-----------------------------------------------------------------------------
#include "vtkMAFConfigure.h"
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
class VTK_vtkMAF_EXPORT vtkMAFGridActor : public vtkActor
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
