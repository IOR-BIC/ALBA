/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkGridActor.h,v $
  Language:  C++
  Date:      $Date: 2005-11-23 18:10:33 $
  Version:   $Revision: 1.1 $
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

#ifndef __vtkGridActor_h
#define __vtkGridActor_h

#include "vtkMAFConfigure.h"

#include "vtkActor.h"
#include "vtkTextActor.h"

class vtkViewport;
class vtkCamera;
//-----------------------------------------------------------------------------
// constants:
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
class VTK_vtkMAF_EXPORT vtkGridActor : public vtkActor
//-----------------------------------------------------------------------------
{
 public:
  vtkTypeRevisionMacro(vtkGridActor,vtkActor);
  void PrintSelf(ostream& os, vtkIndent indent);
  static	vtkGridActor *New();

					 void			SetGridNormal   (int normal_id = GRID_Z);
					 void			SetGridPosition (double position);
					 void			SetGridColor    (double r,double g,double b);
	 vtkTextActor		 *GetLabelActor() {return Label;};

  virtual  void 		AdjustClippingRange(vtkViewport *viewport);
  virtual  int			RenderOpaqueGeometry(vtkViewport *viewport);
  virtual  int			RenderTranslucentGeometry(vtkViewport *viewport);

protected:
										vtkGridActor();
									 ~vtkGridActor();

					 void			GridCreate ();
           void			GridUpdate(vtkCamera *camera);
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
  // hide the two parameter Render() method from the user and the compiler.
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  vtkGridActor(const vtkGridActor&);  	// Not implemented.
  void operator=(const vtkGridActor&);  // Not implemented.
};
#endif
