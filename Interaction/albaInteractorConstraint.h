/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorConstraint
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractorConstraint_h
#define __albaInteractorConstraint_h

#include "vtkALBASmartPointer.h"
#include "vtkDoubleArray.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaRefSys;

//----------------------------------------------------------------------------

/** class representing a constraint for the interaction
  This class is used to store constraint modality parameters and owns a reference system 
  used for constrained interaction. 
  The stored reference system is used as pivot reference system ie:

  - during translation with snap the m_RefSys position is the point to be snapped in vme
  local coordinates system

  - during rotation the RefSys is the rotation reference system ie its position is the 
  centre of rotation.
 
  
  @todo
  - write complete documentation
  - write a debugging Print function
  - complete PrintSelf function

*/
class ALBA_EXPORT albaInteractorConstraint
{
public:
  albaInteractorConstraint();
  virtual ~albaInteractorConstraint();

  /** Copy constructor to allow object copy */
  albaInteractorConstraint(const albaInteractorConstraint& source);

  //----------------------------------------------------------------------------
  // Enums
  //----------------------------------------------------------------------------

  /** Axis enum*/
  enum AXIS
  {
    X = 0,
    Y,
    Z,  
    NUM_AXES,
  };

  /** Plane enum*/
  enum PLANE
  {
    XY = 0,
    XZ,
    YZ,
    NUM_PLANES,
  };

  /** The possible constrained modalities for each axis */
  enum CONSTRAINT_MODALITY
  {
    LOCK = 0,    // <No movement is allowed on/around the given axis
    FREE,        // <Move freely on/around the given axis
    BOUNDS,      // <Bounds constraint
    SNAP_STEP,   // <Snap on a given interval by a given step
    SNAP_ARRAY,  // <Snap on values provided in a given double array
  };

  //----------------------------------------------------------------------------
  // Constraint modality settings
  //----------------------------------------------------------------------------

  /** Contrain on a specified axis, plane or unconstrained;
  allowed constraints are  LOCK, FREE, SNAP_STEP, SNAP_ARRAY
  For example to allow free movement only along X axis use SetConstraintAxis(FREE, LOCK, LOCK).
  */
  void SetConstraintModality(int constraintModalityOnX, int constraintModalityOnY, int constraintModalityOnZ); 

  /**
  Set the constraint modality for the given axis; allowed constraint modality are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY.*/
  void SetConstraintModality(int axis, int constrainModality);

  /** Get the constraint modality for the given axis*/
  int GetConstraintModality(int axis);

  /**
  Get the number of axis on which movement is allowed (ie unlocked
  axis). If DOF==3 the movement is unconstrained.
  */
  int GetNumberOfDOF();
  
  /**
  Return constraint axis when there is only one degree of freedom for the
  given movement type. If there is more than one dof returns -1.
  */
  int GetConstraintAxis();   

  /**
  Return the constraint plane ie an integer from the enum {XY= 0, XZ, YZ} 
  when there are two degree of freedom for the given movement type. Returns 
  -1 if dof number is different from 2.*/
  int GetConstraintPlane();

  /**
  Return the constraint plane axes when there are two degree of freedom for the given 
  movement type. Returns -1 if dof number is different from 2, 0 if dof number is 2.*/
  int GetConstraintPlaneAxes(int& axis1, int& axis2);

  /**
  Constraint modality is set to LOCK for all axes, all values of the element
  struct are set to 0 and pointers to snap_array are set to NULL.*/
  void Reset();
    
  //----------------------------------------------------------------------------
  // BOUNDS modality settings
  //----------------------------------------------------------------------------

  /** Set the lower bound for bound constraint type for the given axis*/
  void SetLowerBound(int axis, double lbound);

  /** Get the lower bound for bound constraint type for the given axis*/
  double GetLowerBound(int axis);

  /** Set the upper bound for bound constraint type for the given axis*/
  void SetUpperBound(int axis, double ubound);

  /** Get the upper bound for bound constraint type for the given axis*/
  double GetUpperBound(int axis);

  /** Set the bounds interval*/
  void SetBounds(int axis, double *bounds);

   /** Get the bounds interval*/
  void GetBounds(int axis, double *bounds);

  //----------------------------------------------------------------------------
  // SNAP_STEP modality settings
  //----------------------------------------------------------------------------

  /** Set the min value for snap step constraint type for the given axis*/
  void SetMin(int axis, double min);
      
  /** Get the min value for snap step constraint type for the given axis*/
  double GetMin(int axis);

  /** Set the max value for snap step constraint type for the given axis*/
  void SetMax(int axis, double min);
      
  /** Get the max value for snap step constraint type for the given axis*/
  double GetMax(int axis);
  
  /** Set the step value for snap step constraint type for the given axis*/
  void SetStep(int axis, double step);
      
  /** Get the step value for snap step constraint type for the given axis*/
  double GetStep(int axis); 

  /** Set snap step modality parameters*/
  void SetSnapStep(int axis, double min,double max, double step);
  
  //----------------------------------------------------------------------------
  // SNAP_ARRAY modality settings
  //----------------------------------------------------------------------------

  /** Set the array which contains the allowed positions for snap.
  The array is not copied, only the pointer is stored
  */
  void SetSnapArray(int axis, vtkDoubleArray *array);
       
  /** Get the array which contains the allowed positions for snap. */
  vtkDoubleArray *GetSnapArray(int axis);

  //----------------------------------------------------------------------------
  // Reference system settings
  //----------------------------------------------------------------------------
  
  /** Get the constraint reference system, i.e. the refsys to which the
  transform will be constrained also known as Pivot reference system. */
  albaRefSys *GetRefSys();

  /** Copy the contents of another RefSys into the contraint one*/
  void SetRefSys(albaRefSys *ref_sys);
  void SetRefSys(albaRefSys &ref_sys);
   
  //----------------------------------------------------------------------------

  /** Debug printing of internal data */
  void Print(std::ostream& os, const int tabs);

  void DeepCopy(const albaInteractorConstraint *source) { *this=*source;}

protected:

  /** the constraint reference system*/
  albaRefSys *m_RefSys;
  
  /** Register constraint modality for each axis*/
  int m_ConstraintModality[3];

  /** Bounds modality parameters*/
  double m_LowerBound[3];
  double m_UpperBound[3];

  /** Snap step modality parameters*/
  double m_Min[3];
  double m_Max[3];
  double m_Step[3];

  /** Snap array modality*/
  vtkALBAAutoPointer<vtkDoubleArray> m_SnapArray[3];
  
};

#endif 
