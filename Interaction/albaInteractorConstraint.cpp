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

// include this first to avoid compilation errors in wxWindows
#include "albaDefines.h"

#include "albaInteractorConstraint.h"
#include "albaAbsMatrixPipe.h"
#include "albaIndent.h"
#include "albaRefSys.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaInteractorConstraint::albaInteractorConstraint()
//------------------------------------------------------------------------------
{
  m_RefSys = new albaRefSys;
  Reset();
}

//------------------------------------------------------------------------------
albaInteractorConstraint::~albaInteractorConstraint()
//------------------------------------------------------------------------------
{
  delete m_RefSys; m_RefSys = NULL;
}

//----------------------------------------------------------------------------
albaInteractorConstraint::albaInteractorConstraint(const albaInteractorConstraint& source)
//----------------------------------------------------------------------------
{
  m_RefSys = new albaRefSys;
  Reset();
  *m_RefSys=*(source.m_RefSys);

  for (int j = 0; j < 3; j++)
  {
    m_ConstraintModality[j] = source.m_ConstraintModality[j];
    m_LowerBound[j] = source.m_LowerBound[j];
    m_Min[j] = source.m_Min[j];
    m_Max[j] = source.m_Max[j];  
    m_SnapArray[j]=source.m_SnapArray[j];
  }
}

//----------------------------------------------------------------------------
void albaInteractorConstraint::Reset()
//------------------------------------------------------------------------------
{
  for (int j = 0; j < 3; j++)
  {
    m_ConstraintModality[j] = FREE;
    m_LowerBound[j] = 0;
    m_Min[j] = m_Max[j] = m_Step[j] = 0;  
    m_SnapArray[j]=NULL;
  }

  m_RefSys->Reset();
}

//----------------------------------------------------------------------------
void albaInteractorConstraint::SetRefSys(albaRefSys *ref_sys)
//----------------------------------------------------------------------------
{
  m_RefSys->DeepCopy(ref_sys);
}

//----------------------------------------------------------------------------
void albaInteractorConstraint::SetRefSys(albaRefSys &ref_sys)
//----------------------------------------------------------------------------
{
  *m_RefSys=ref_sys;
}

//----------------------------------------------------------------------------
void albaInteractorConstraint::SetConstraintModality(int constraintModalityOnX, int constraintModalityOnY, int constraintModalityOnZ)
//----------------------------------------------------------------------------
{
  m_ConstraintModality[X] = constraintModalityOnX;
  m_ConstraintModality[Y] = constraintModalityOnY;
  m_ConstraintModality[Z] = constraintModalityOnZ; 
}

//----------------------------------------------------------------------------
void albaInteractorConstraint::SetConstraintModality(int axis, int constraintModality)
//------------------------------------------------------------------------------
{
  m_ConstraintModality[axis] = constraintModality;
}

//----------------------------------------------------------------------------
int albaInteractorConstraint::GetConstraintModality(int axis)
//------------------------------------------------------------------------------
{
  return m_ConstraintModality[axis];
}
  
//----------------------------------------------------------------------------
void albaInteractorConstraint::SetLowerBound(int axis, double lbound)
//------------------------------------------------------------------------------
{
  m_LowerBound[axis] = lbound;
}

//----------------------------------------------------------------------------
double albaInteractorConstraint::GetLowerBound(int axis)
//------------------------------------------------------------------------------
{
  return m_LowerBound[axis];
}

//----------------------------------------------------------------------------  
void albaInteractorConstraint::SetUpperBound(int axis, double ubound)
//------------------------------------------------------------------------------
{
  m_UpperBound[axis] = ubound;
} 

//----------------------------------------------------------------------------    
double albaInteractorConstraint::GetUpperBound(int axis)
//------------------------------------------------------------------------------
{
  return m_UpperBound[axis];
}

//----------------------------------------------------------------------------
void albaInteractorConstraint::SetMin(int axis, double min)
//------------------------------------------------------------------------------
{
  m_Min[axis] = min;
}

//----------------------------------------------------------------------------
double albaInteractorConstraint::GetMin(int axis)
//------------------------------------------------------------------------------
{
  return m_Min[axis];
}

//---------------------------------------------------------------------------- 
void albaInteractorConstraint::SetMax(int axis, double max)
//------------------------------------------------------------------------------
{
  m_Max[axis] = max;
}

//----------------------------------------------------------------------------      
double albaInteractorConstraint::GetMax(int axis)
//------------------------------------------------------------------------------
{
  return m_Max[axis];
}

//---------------------------------------------------------------------------- 
void albaInteractorConstraint::SetStep(int axis, double step)
//------------------------------------------------------------------------------
{
  m_Step[axis] = step;
} 

//----------------------------------------------------------------------------      
double albaInteractorConstraint::GetStep(int axis)
//------------------------------------------------------------------------------
{
  return m_Step[axis];
}

//----------------------------------------------------------------------------      
void albaInteractorConstraint::SetSnapArray(int axis, vtkDoubleArray *array)
//------------------------------------------------------------------------------
{
  assert( array );
  assert( axis>=X && axis <=Z );
  m_SnapArray[axis]=array;
}   

//----------------------------------------------------------------------------      
vtkDoubleArray *albaInteractorConstraint::GetSnapArray(int axis)
//------------------------------------------------------------------------------
{
  assert( axis>=X && axis <=Z );
  return m_SnapArray[axis];
}

//----------------------------------------------------------------------------
int albaInteractorConstraint::GetNumberOfDOF()
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
int albaInteractorConstraint::GetConstraintAxis()
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
void albaInteractorConstraint::Print(std::ostream& os, const int tabs)
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
void albaInteractorConstraint::SetBounds(int axis, double *bounds)
//----------------------------------------------------------------------------
{
  this->SetLowerBound(axis, bounds[0]);
  this->SetUpperBound(axis, bounds[1]);
}


//----------------------------------------------------------------------------
void albaInteractorConstraint::GetBounds(int axis, double *bounds)
//----------------------------------------------------------------------------
{
  bounds[0] = this->GetLowerBound(axis);
  bounds[1] = this->GetUpperBound(axis);
}

//----------------------------------------------------------------------------
void albaInteractorConstraint::SetSnapStep(int axis, double min,double max, double step)
//----------------------------------------------------------------------------
{
  this->SetMin(axis, min);
  this->SetMax(axis, max);
  this->SetStep(axis, step);
}

//----------------------------------------------------------------------------
int albaInteractorConstraint::GetConstraintPlane()
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
albaRefSys* albaInteractorConstraint::GetRefSys()
//----------------------------------------------------------------------------
{
  return m_RefSys;
}
//----------------------------------------------------------------------------
int albaInteractorConstraint::GetConstraintPlaneAxes(int& axis1, int& axis2)
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
