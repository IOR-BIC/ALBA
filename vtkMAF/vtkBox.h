/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkBox.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

Some modifications by Simone Brazzale in order to make it work
under MAF (www.openmaf.org)

=========================================================================*/
// .NAME vtkBox - implicit function for a bounding box
// .SECTION Description
// vtkBox computes the implicit function and/or gradient for a axis-aligned
// bounding box. (The superclasses transform can be used to modify this
// orientation.) Each side of the box is orthogonal to all other sides
// meeting along shared edges and all faces are orthogonal to the x-y-z
// coordinate axes.  (If you wish to orient this box differently, recall that
// the superclass vtkImplicitFunction supports a transformation matrix.)
// vtkCube is a concrete implementation of vtkImplicitFunction.
// 
// .SECTION See Also
// vtkCubeSource vtkImplicitFunction

#ifndef __vtkBox_h
#define __vtkBox_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkMAFConfigure.h"
#include "vtkImplicitFunction.h"
#include "vtkMAFConfigure.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class vtkBoundingBox;
/**
class name: vtkBox.
Optimized box ,which intersect method taken from Graphic Gems vol I.
*/
class VTK_vtkMAF_EXPORT vtkBox : public vtkImplicitFunction
{
public:
  /** RTTI macro */
  vtkTypeRevisionMacro(vtkBox,vtkImplicitFunction);
  /** print information */
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  /**Construct box with center at (0,0,0) and each side of length 1.0. */
  static vtkBox *New();

  // Description
  /**Evaluate box defined by the two points (pMin,pMax).*/
  double EvaluateFunction(double x[3]);
  /**Evaluate box defined by the two points (pMin,pMax).*/
  double EvaluateFunction(double x, double y, double z)
    {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); }

  // Description
  /**Evaluate the gradient of the box.*/
  void EvaluateGradient(double x[3], double n[3]);

  // Description:
  /**Set the bounding box*/
  void SetXMin(double p[3]);
  /**Set the bounding box*/
  void SetXMin(double x, double y, double z);
  /**Get the bounding box*/
  void GetXMin(double p[3]);
  /**Get the bounding box*/
  void GetXMin(double &x, double &y, double &z);

  /**Set the bounding box*/
  void SetXMax(double p[3]);
  /**Set the bounding box*/
  void SetXMax(double x, double y, double z);
  /**Get the bounding box*/
  void GetXMax(double p[3]);
  /**Get the bounding box*/
  void GetXMax(double &x, double &y, double &z);

  /** set bounds */
  void SetBounds(double xMin, double xMax,
                 double yMin, double yMax,
                 double zMin, double zMax);
  /** set bounds */
  void SetBounds(double bounds[6]);
  /** retrieve  bounds */
  void GetBounds(double &xMin, double &xMax,
                 double &yMin, double &yMax,
                 double &zMin, double &zMax);
  /**retrieve  bounds  C style*/
  void GetBounds(double bounds[6]);
  /** retrieve  bounds */
  double *GetBounds();

  // Description:
  /**A special method that allows union set operation on bounding boxes.
     Start with a SetBounds(). Subsequent AddBounds() methods are union set
     operations on the original bounds. Retrieve the final bounds with a 
     GetBounds() method.*/
  void AddBounds(double bounds[6]);

  // Description:
  /**Bounding box intersection modified from Graphics Gems Vol I. The method
    returns a non-zero value if the bounding box is hit. Origin[3] starts
    the ray, dir[3] is the vector components of the ray in the x-y-z
    directions, coord[3] is the location of hit, and t is the parametric
    coordinate along line. (Notes: the intersection ray dir[3] is NOT
    normalized.  Valid intersections will only occur between 0<=t<=1.)*/
  static char IntersectBox(double bounds[6], double origin[3], double dir[3], 
                           double coord[3], double& t);

protected:
  /** object constructor.*/
  vtkBox();
  /** destructor constructor.*/
  ~vtkBox();

  vtkBoundingBox *BBox; 
  double Bounds[6]; //supports the GetBounds() method

private:
  /**copy constructor not implemented */
  vtkBox(const vtkBox&);
  /** operator =  not implemented */
  void operator=(const vtkBox&);  // Not implemented.
};



inline void vtkBox::SetXMin(double p[3]) 
{
  this->SetXMin(p[0], p[1], p[2]);
}

inline void vtkBox::SetXMax(double p[3]) 
{
  this->SetXMax(p[0], p[1], p[2]);
}


#endif


