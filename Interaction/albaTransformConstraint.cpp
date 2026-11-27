/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformConstraint
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// include this first to avoid compilation errors in wxWindows
#include "albaDefines.h"

#include "albaTransformConstraint.h"
#include "albaAbsMatrixPipe.h"
#include "albaIndent.h"
#include "albaRefSys.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaTransformConstraint::albaTransformConstraint()
//------------------------------------------------------------------------------
{
  m_RefSys = new albaRefSys;
  Reset();
}

//------------------------------------------------------------------------------
albaTransformConstraint::~albaTransformConstraint()
//------------------------------------------------------------------------------
{
  delete m_RefSys; m_RefSys = NULL;
}

//----------------------------------------------------------------------------
albaTransformConstraint::albaTransformConstraint(const albaTransformConstraint& source)
//----------------------------------------------------------------------------
{
  m_RefSys = new albaRefSys;
  Reset();
  *this = source;
}

//----------------------------------------------------------------------------
albaTransformConstraint &albaTransformConstraint::operator=(const albaTransformConstraint& source)
//----------------------------------------------------------------------------
{
  // each constraint owns its ref sys: the contents are copied, not the pointer
  *m_RefSys = *(source.m_RefSys);

  for (int j = 0; j < 3; j++)
  {
    m_ConstraintModality[j] = source.m_ConstraintModality[j];
    m_LowerBound[j] = source.m_LowerBound[j];
    m_UpperBound[j] = source.m_UpperBound[j];
    m_Min[j] = source.m_Min[j];
    m_Max[j] = source.m_Max[j];
    m_Step[j] = source.m_Step[j];
    m_SnapArray[j] = source.m_SnapArray[j];
  }
  return *this;
}

//----------------------------------------------------------------------------
void albaTransformConstraint::Reset()
//------------------------------------------------------------------------------
{
  for (int j = 0; j < 3; j++)
  {
    m_ConstraintModality[j] = FREE;
    m_LowerBound[j] = m_UpperBound[j] = 0;
    m_Min[j] = m_Max[j] = m_Step[j] = 0;  
    m_SnapArray[j]=NULL;
  }

  m_RefSys->Reset();
}

//----------------------------------------------------------------------------
void albaTransformConstraint::SetRefSys(albaRefSys *ref_sys)
//----------------------------------------------------------------------------
{
  m_RefSys->DeepCopy(ref_sys);
}

//----------------------------------------------------------------------------
void albaTransformConstraint::SetRefSys(albaRefSys &ref_sys)
//----------------------------------------------------------------------------
{
  *m_RefSys=ref_sys;
}

//----------------------------------------------------------------------------
void albaTransformConstraint::SetConstraintModality(int constraintModalityOnX, int constraintModalityOnY, int constraintModalityOnZ)
//----------------------------------------------------------------------------
{
  m_ConstraintModality[X] = constraintModalityOnX;
  m_ConstraintModality[Y] = constraintModalityOnY;
  m_ConstraintModality[Z] = constraintModalityOnZ; 
}

//----------------------------------------------------------------------------
void albaTransformConstraint::SetConstraintModality(int axis, int constraintModality)
//------------------------------------------------------------------------------
{
  m_ConstraintModality[axis] = constraintModality;
}

//----------------------------------------------------------------------------
int albaTransformConstraint::GetConstraintModality(int axis)
//------------------------------------------------------------------------------
{
  return m_ConstraintModality[axis];
}
  
//----------------------------------------------------------------------------
void albaTransformConstraint::SetLowerBound(int axis, double lbound)
//------------------------------------------------------------------------------
{
  m_LowerBound[axis] = lbound;
}

//----------------------------------------------------------------------------
double albaTransformConstraint::GetLowerBound(int axis)
//------------------------------------------------------------------------------
{
  return m_LowerBound[axis];
}

//----------------------------------------------------------------------------  
void albaTransformConstraint::SetUpperBound(int axis, double ubound)
//------------------------------------------------------------------------------
{
  m_UpperBound[axis] = ubound;
} 

//----------------------------------------------------------------------------    
double albaTransformConstraint::GetUpperBound(int axis)
//------------------------------------------------------------------------------
{
  return m_UpperBound[axis];
}

//----------------------------------------------------------------------------
void albaTransformConstraint::SetMin(int axis, double min)
//------------------------------------------------------------------------------
{
  m_Min[axis] = min;
}

//----------------------------------------------------------------------------
double albaTransformConstraint::GetMin(int axis)
//------------------------------------------------------------------------------
{
  return m_Min[axis];
}

//---------------------------------------------------------------------------- 
void albaTransformConstraint::SetMax(int axis, double max)
//------------------------------------------------------------------------------
{
  m_Max[axis] = max;
}

//----------------------------------------------------------------------------      
double albaTransformConstraint::GetMax(int axis)
//------------------------------------------------------------------------------
{
  return m_Max[axis];
}

//---------------------------------------------------------------------------- 
void albaTransformConstraint::SetStep(int axis, double step)
//------------------------------------------------------------------------------
{
  m_Step[axis] = step;
} 

//----------------------------------------------------------------------------      
double albaTransformConstraint::GetStep(int axis)
//------------------------------------------------------------------------------
{
  return m_Step[axis];
}

//----------------------------------------------------------------------------      
void albaTransformConstraint::SetSnapArray(int axis, vtkDoubleArray *array)
//------------------------------------------------------------------------------
{
  assert( array );
  assert( axis>=X && axis <=Z );
  m_SnapArray[axis]=array;
}   

//----------------------------------------------------------------------------      
vtkDoubleArray *albaTransformConstraint::GetSnapArray(int axis)
//------------------------------------------------------------------------------
{
  assert( axis>=X && axis <=Z );
  return m_SnapArray[axis];
}

//----------------------------------------------------------------------------
int albaTransformConstraint::GetNumberOfDOF()
//------------------------------------------------------------------------------
{
  int ndof = 0;
  for (int j = 0; j < 3; j++)
  {
    if (m_ConstraintModality[j] != LOCK)
    {
      ndof++;
    }
  }
  return ndof;
}

//----------------------------------------------------------------------------
int albaTransformConstraint::GetConstraintAxis()
//------------------------------------------------------------------------------
{
  if (this->GetNumberOfDOF() == 1)
  {
    for (int j = 0; j < 3; j++)
    {
      if (m_ConstraintModality[j] != LOCK)
      {
        return j;
      }
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
void albaTransformConstraint::Print(std::ostream& os, const int tabs)
//----------------------------------------------------------------------------
{
  // To be completed
  int j;
  albaIndent indent(tabs);

  os << indent << "m_ConstraintModality:\n";
  os << indent << indent;
  for (j = 0; j < 3; j++) 
  {
    os << m_ConstraintModality[j] << " ";
  }
  os << "\n";
}


//----------------------------------------------------------------------------
void albaTransformConstraint::SetBounds(int axis, double *bounds)
//----------------------------------------------------------------------------
{
  this->SetLowerBound(axis, bounds[0]);
  this->SetUpperBound(axis, bounds[1]);
}


//----------------------------------------------------------------------------
void albaTransformConstraint::GetBounds(int axis, double *bounds)
//----------------------------------------------------------------------------
{
  bounds[0] = this->GetLowerBound(axis);
  bounds[1] = this->GetUpperBound(axis);
}

//----------------------------------------------------------------------------
void albaTransformConstraint::SetSnapStep(int axis, double min,double max, double step)
//----------------------------------------------------------------------------
{
  this->SetMin(axis, min);
  this->SetMax(axis, max);
  this->SetStep(axis, step);
}

//----------------------------------------------------------------------------
int albaTransformConstraint::GetConstraintPlane()
//----------------------------------------------------------------------------
{
  const int AXIS_BLOCKED = 0;
  const int AXIS_ALLOWED = 1;
  
  if (this->GetNumberOfDOF()  == 2)
  {
    int axesStatus[3] = {AXIS_BLOCKED, AXIS_BLOCKED, AXIS_BLOCKED};
    for (int axisID = 0; axisID < 3; axisID++)
    {
      if (m_ConstraintModality[axisID] != LOCK)
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
albaRefSys* albaTransformConstraint::GetRefSys()
//----------------------------------------------------------------------------
{
  return m_RefSys;
}
//----------------------------------------------------------------------------
int albaTransformConstraint::GetConstraintPlaneAxes(int& axis1, int& axis2)
//----------------------------------------------------------------------------
{
  const int AXIS_BLOCKED = 0;
  const int AXIS_ALLOWED = 1;
  
  if (this->GetNumberOfDOF() == 2)
  {
    int axesStatus[3] = {AXIS_BLOCKED, AXIS_BLOCKED, AXIS_BLOCKED};
    for (int axisID = 0; axisID < 3; axisID++)
    {
      if (m_ConstraintModality[axisID] != LOCK)
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
