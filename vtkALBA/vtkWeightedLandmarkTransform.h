/////////////////////////////////////////////////////////////////////////////
// Name:        vtkWeightedLandmarkTransform.h
//
// Project:     MULTIMOD 
// Author:      Stefania Paperini
// Date:        6-5-2004
/////////////////////////////////////////////////////////////////////////////

#ifndef __vtkWeightedLandmarkTransform_h
#define __vtkWeightedLandmarkTransform_h

#include "albaConfigure.h"
#include "vtkLandmarkTransform.h"


//----------------------------------------------------------------------------
// vtkWeightedLandmarkTransform :
//----------------------------------------------------------------------------
/**
    class name: vtkWeightedLandmarkTransform
    It handle landmark transform using weights which be associated to each landmark.
 */  
class ALBA_EXPORT vtkWeightedLandmarkTransform : public vtkLandmarkTransform
{
public:
  /** construct the object */
  static vtkWeightedLandmarkTransform *New();
  /** RTTI macro */
  vtkTypeMacro(vtkWeightedLandmarkTransform,vtkLandmarkTransform);

  /**  Set the vector of weights.*/
  void SetWeights(double	*w, int number);
     
protected:
  /** object constructor */
  vtkWeightedLandmarkTransform();
  /** object destructor */
  ~vtkWeightedLandmarkTransform();

  /** Update the matrix from the quaternion using the vector of weights.*/  
  void InternalUpdate();  

  /** vector of weights.*/
  double *Weight;

private:
  /** copy constructor not implemented */
  vtkWeightedLandmarkTransform(const vtkWeightedLandmarkTransform&);
  /** overload equal operator not implemented */
  void operator=(const vtkWeightedLandmarkTransform&);
};
 

#endif
