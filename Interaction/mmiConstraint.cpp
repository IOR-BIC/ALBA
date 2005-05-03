/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiConstraint.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:36 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mmiConstraint.h"
#include "vtkObjectFactory.h"

#include "mflSmartPointer.h"
#include "mflMatrixPipeDirectCinematic.h"
#include "vtkMatrixToLinearTransform.h"
#include "vtkDoubleArray.h"
#include <assert.h>

//------------------------------------------------------------------------------
mmiConstraint::mmiConstraint()
//------------------------------------------------------------------------------
{
  RefSys = new mafRefSys;
  Reset();
}

//------------------------------------------------------------------------------
mmiConstraint::~mmiConstraint()
//------------------------------------------------------------------------------
{
  delete RefSys; RefSys = NULL;
}

//----------------------------------------------------------------------------
mmiConstraint::mmiConstraint(const mmiConstraint& source)
//----------------------------------------------------------------------------
{
  RefSys = new mafRefSys;
  Reset();
  *RefSys=*(source.RefSys);

  for (int j = 0; j < 3; j++)
  {
    ConstraintModality[j] = source.ConstraintModality[j];
    LowerBound[j] = source.LowerBound[j];
    Min[j] = source.Min[j];
    Max[j] = source.Max[j];  
    SnapArray[j]=source.SnapArray[j];
  }
}

//----------------------------------------------------------------------------
void mmiConstraint::Reset()
//------------------------------------------------------------------------------
{
  for (int j = 0; j < 3; j++)
  {
    ConstraintModality[j] = FREE;
    LowerBound[j] = 0;
    Min[j] = Max[j] = Step[j] = 0;  
    SnapArray[j]=NULL;
  }

  RefSys->Reset();
}

//----------------------------------------------------------------------------
void mmiConstraint::SetRefSys(mafRefSys *ref_sys)
//----------------------------------------------------------------------------
{
  RefSys->DeepCopy(ref_sys);
}

//----------------------------------------------------------------------------
void mmiConstraint::SetRefSys(mafRefSys &ref_sys)
//----------------------------------------------------------------------------
{
  *RefSys=ref_sys;
}

//----------------------------------------------------------------------------
void mmiConstraint::SetConstraintModality(int constraintModalityOnX, int constraintModalityOnY, int constraintModalityOnZ)
//----------------------------------------------------------------------------
{
  ConstraintModality[X] = constraintModalityOnX;
  ConstraintModality[Y] = constraintModalityOnY;
  ConstraintModality[Z] = constraintModalityOnZ; 
}

//----------------------------------------------------------------------------
void mmiConstraint::SetConstraintModality(int axis, int constraintModality)
//------------------------------------------------------------------------------
{
  ConstraintModality[axis] = constraintModality;
}

//----------------------------------------------------------------------------
int mmiConstraint::GetConstraintModality(int axis)
//------------------------------------------------------------------------------
{
  return ConstraintModality[axis];
}
  
//----------------------------------------------------------------------------
void mmiConstraint::SetLowerBound(int axis, double lbound)
//------------------------------------------------------------------------------
{
  LowerBound[axis] = lbound;
}

//----------------------------------------------------------------------------
double mmiConstraint::GetLowerBound(int axis)
//------------------------------------------------------------------------------
{
  return LowerBound[axis];
}

//----------------------------------------------------------------------------  
void mmiConstraint::SetUpperBound(int axis, double ubound)
//------------------------------------------------------------------------------
{
  UpperBound[axis] = ubound;
} 

//----------------------------------------------------------------------------    
double mmiConstraint::GetUpperBound(int axis)
//------------------------------------------------------------------------------
{
  return UpperBound[axis];
}

//----------------------------------------------------------------------------
void mmiConstraint::SetMin(int axis, double min)
//------------------------------------------------------------------------------
{
  Min[axis] = min;
}

//----------------------------------------------------------------------------
double mmiConstraint::GetMin(int axis)
//------------------------------------------------------------------------------
{
  return Min[axis];
}

//---------------------------------------------------------------------------- 
void mmiConstraint::SetMax(int axis, double max)
//------------------------------------------------------------------------------
{
  Max[axis] = max;
}

//----------------------------------------------------------------------------      
double mmiConstraint::GetMax(int axis)
//------------------------------------------------------------------------------
{
  return Max[axis];
}

//---------------------------------------------------------------------------- 
void mmiConstraint::SetStep(int axis, double step)
//------------------------------------------------------------------------------
{
  Step[axis] = step;
} 

//----------------------------------------------------------------------------      
double mmiConstraint::GetStep(int axis)
//------------------------------------------------------------------------------
{
  return Step[axis];
}

//----------------------------------------------------------------------------      
void mmiConstraint::SetSnapArray(int axis, vtkDoubleArray *array)
//------------------------------------------------------------------------------
{
  assert( array );
  assert( axis>=X && axis <=Z );
  SnapArray[axis]=array;
}   

//----------------------------------------------------------------------------      
vtkDoubleArray *mmiConstraint::GetSnapArray(int axis)
//------------------------------------------------------------------------------
{
  assert( axis>=X && axis <=Z );
  return SnapArray[axis];
}

//----------------------------------------------------------------------------
int mmiConstraint::GetNumberOfDOF()
//------------------------------------------------------------------------------
{
  int ndof = 0;
  for (int j = 0; j < 3; j++)
  {
    if (ConstraintModality[j] != LOCK)
    {
      ndof++;
    }
  }
  return ndof;
}

//----------------------------------------------------------------------------
int mmiConstraint::GetConstraintAxis()
//------------------------------------------------------------------------------
{
  if (this->GetNumberOfDOF() == 1)
  {
    for (int j = 0; j < 3; j++)
    {
      if (ConstraintModality[j] != LOCK)
      {
        return j;
      }
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
void mmiConstraint::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  // To be completed
  int j;

  os << indent << "ConstraintModality:\n";
  os << indent << indent;
  for (j = 0; j < 3; j++) 
  {
    os << ConstraintModality[j] << " ";
  }
  os << "\n";
}


//----------------------------------------------------------------------------
void mmiConstraint::SetBounds(int axis, double *bounds)
//----------------------------------------------------------------------------
{
  this->SetLowerBound(axis, bounds[0]);
  this->SetUpperBound(axis, bounds[1]);
}


//----------------------------------------------------------------------------
void mmiConstraint::GetBounds(int axis, double *bounds)
//----------------------------------------------------------------------------
{
  bounds[0] = this->GetLowerBound(axis);
  bounds[1] = this->GetUpperBound(axis);
}

//----------------------------------------------------------------------------
void mmiConstraint::SetSnapStep(int axis, double min,double max, double step)
//----------------------------------------------------------------------------
{
  this->SetMin(axis, min);
  this->SetMax(axis, max);
  this->SetStep(axis, step);
}

//----------------------------------------------------------------------------
int mmiConstraint::GetConstraintPlane()
//----------------------------------------------------------------------------
{
  const int AXIS_BLOCKED = 0;
  const int AXIS_ALLOWED = 1;
  
  if (this->GetNumberOfDOF()  == 2)
  {
    int axesStatus[3] = {AXIS_BLOCKED, AXIS_BLOCKED, AXIS_BLOCKED};
    for (int axisID = 0; axisID < 3; axisID++)
    {
      if (ConstraintModality[axisID] != LOCK)
      {
        axesStatus[axisID] = AXIS_ALLOWED;
      }   
    }

    if (axesStatus[0] == AXIS_ALLOWED && axesStatus[1] == AXIS_ALLOWED)
    {
      return XY;
    }
    else if (axesStatus[0] == AXIS_ALLOWED && axesStatus[2] == AXIS_ALLOWED)
    {
      return XZ;
    }
    else if (axesStatus[1] == AXIS_ALLOWED && axesStatus[2] == AXIS_ALLOWED)
    {
      return YZ;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
int mmiConstraint::GetConstraintPlaneAxes(int& axis1, int& axis2)
//----------------------------------------------------------------------------
{
  const int AXIS_BLOCKED = 0;
  const int AXIS_ALLOWED = 1;
  
  if (this->GetNumberOfDOF() == 2)
  {
    int axesStatus[3] = {AXIS_BLOCKED, AXIS_BLOCKED, AXIS_BLOCKED};
    for (int axisID = 0; axisID < 3; axisID++)
    {
      if (ConstraintModality[axisID] != LOCK)
      {
        axesStatus[axisID] = AXIS_ALLOWED;
      }   
    }

    if (axesStatus[0] == AXIS_ALLOWED && axesStatus[1] == AXIS_ALLOWED)
    {
      axis1 = X;
      axis2 = Y;
    }
    else if (axesStatus[0] == AXIS_ALLOWED && axesStatus[2] == AXIS_ALLOWED)
    {
      axis1 = X;
      axis2 = Z;
    }
    else if (axesStatus[1] == AXIS_ALLOWED && axesStatus[2] == AXIS_ALLOWED)
    {
      axis1 = Y;
      axis2 = Z;
    }
    return 0;
  }
  else 
  {
    return -1;
  }
}