/////////////////////////////////////////////////////////////////////////////
// Name:        vtkWeightedLandmarkTransform.h
//
// Project:     MULTIMOD 
// Author:      Stefania Paperini
// Date:        6-5-2004
/////////////////////////////////////////////////////////////////////////////

#ifndef __vtkWeightedLandmarkTransform_h
#define __vtkWeightedLandmarkTransform_h


#include "vtkLandmarkTransform.h"


//----------------------------------------------------------------------------
// vtkWeightedLandmarkTransform :
//----------------------------------------------------------------------------
/**
 */  
class vtkWeightedLandmarkTransform : public vtkLandmarkTransform
{
public:
  static vtkWeightedLandmarkTransform *New();
  vtkTypeRevisionMacro(vtkWeightedLandmarkTransform,vtkLandmarkTransform);

  /**  Set the vector of weights.*/
  void SetWeights(double	*w, int number);
     
protected:
  vtkWeightedLandmarkTransform();
  ~vtkWeightedLandmarkTransform();

  /** Update the matrix from the quaternion using the vector of weights.*/  
  void InternalUpdate();  

  /** vector of weights.*/
  double *Weight;

private:
  vtkWeightedLandmarkTransform(const vtkWeightedLandmarkTransform&);  // Not implemented.
  void operator=(const vtkWeightedLandmarkTransform&);  // Not implemented.
};
 

#endif
