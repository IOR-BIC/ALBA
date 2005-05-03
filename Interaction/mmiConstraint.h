/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiConstraint.h,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:36 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiConstraint_h
#define __mmiConstraint_h

#include "mafRefSys.h"
#include "mflSmartPointer.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class mflMatrixPipeDirectCinematic;
class vtkDoubleArray;

//----------------------------------------------------------------------------

/** class representing a constraint for the interaction
  This class is used to store constraint modality parameters and owns a reference system 
  used for constrained interaction. 
  The stored reference system is used as pivot reference system ie:

  - during translation with snap the RefSys position is the point to be snapped in vme
  local coordinates system

  - during rotation the RefSys is the rotation reference system ie its position is the 
  centre of rotation.
 
  
  @todo
  - write complete documentation
  - write a debugging Print function
  - complete PrintSelf function

*/
class mmiConstraint
{
public:
  mmiConstraint();
  virtual ~mmiConstraint();

  /** Copy constructor to allow object copy */
  mmiConstraint(const mmiConstraint& source);

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
  mafRefSys *GetRefSys() {return RefSys;}

  /** Copy the contents of another RefSys into the contraint one*/
  void SetRefSys(mafRefSys *ref_sys);
  void SetRefSys(mafRefSys &ref_sys);
   
  //----------------------------------------------------------------------------

  /** Debug printing of internal data */
  void PrintSelf(ostream& os, vtkIndent indent);

  void DeepCopy(const mmiConstraint *source) { *this=*source;}

protected:

  /** the constraint reference system*/
  mafRefSys *RefSys;
  
  /** Register constraint modality for each axis*/
  int ConstraintModality[3];

  /** Bounds modality parameters*/
  double LowerBound[3];
  double UpperBound[3];

  /** Snap step modality parameters*/
  double Min[3];
  double Max[3];
  double Step[3];

  /** Snap array modality*/
  mflAutoPointer<vtkDoubleArray> SnapArray[3];
  
};

#endif 
