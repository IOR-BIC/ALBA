/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractorConstraint.cpp,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:46:39 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

// include this first to avoid compilation errors in wxWindows
#include "mafDefines.h"

#include "mafInteractorConstraint.h"
#include "mafAbsMatrixPipe.h"
#include "mafIndent.h"

#include "vtkDoubleArray.h"
#include <assert.h>

//------------------------------------------------------------------------------
mafInteractorConstraint::mafInteractorConstraint()
//------------------------------------------------------------------------------
{
  m_RefSys = new mafRefSys;
  Reset();
}

//------------------------------------------------------------------------------
mafInteractorConstraint::~mafInteractorConstraint()
//------------------------------------------------------------------------------
{
  delete m_RefSys; m_RefSys = NULL;
}

//----------------------------------------------------------------------------
mafInteractorConstraint::mafInteractorConstraint(const mafInteractorConstraint& source)
//----------------------------------------------------------------------------
{
  m_RefSys = new mafRefSys;
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
void mafInteractorConstraint::Reset()
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
void mafInteractorConstraint::SetRefSys(mafRefSys *ref_sys)
//----------------------------------------------------------------------------
{
  m_RefSys->DeepCopy(ref_sys);
}

//----------------------------------------------------------------------------
void mafInteractorConstraint::SetRefSys(mafRefSys &ref_sys)
//----------------------------------------------------------------------------
{
  *m_RefSys=ref_sys;
}

//----------------------------------------------------------------------------
void mafInteractorConstraint::SetConstraintModality(int constraintModalityOnX, int constraintModalityOnY, int constraintModalityOnZ)
//----------------------------------------------------------------------------
{
  m_ConstraintModality[X] = constraintModalityOnX;
  m_ConstraintModality[Y] = constraintModalityOnY;
  m_ConstraintModality[Z] = constraintModalityOnZ; 
}

//----------------------------------------------------------------------------
void mafInteractorConstraint::SetConstraintModality(int axis, int constraintModality)
//------------------------------------------------------------------------------
{
  m_ConstraintModality[axis] = constraintModality;
}

//----------------------------------------------------------------------------
int mafInteractorConstraint::GetConstraintModality(int axis)
//------------------------------------------------------------------------------
{
  return m_ConstraintModality[axis];
}
  
//----------------------------------------------------------------------------
void mafInteractorConstraint::SetLowerBound(int axis, double lbound)
//------------------------------------------------------------------------------
{
  m_LowerBound[axis] = lbound;
}

//----------------------------------------------------------------------------
double mafInteractorConstraint::GetLowerBound(int axis)
//------------------------------------------------------------------------------
{
  return m_LowerBound[axis];
}

//----------------------------------------------------------------------------  
void mafInteractorConstraint::SetUpperBound(int axis, double ubound)
//------------------------------------------------------------------------------
{
  m_UpperBound[axis] = ubound;
} 

//----------------------------------------------------------------------------    
double mafInteractorConstraint::GetUpperBound(int axis)
//------------------------------------------------------------------------------
{
  return m_UpperBound[axis];
}

//----------------------------------------------------------------------------
void mafInteractorConstraint::SetMin(int axis, double min)
//------------------------------------------------------------------------------
{
  m_Min[axis] = min;
}

//----------------------------------------------------------------------------
double mafInteractorConstraint::GetMin(int axis)
//------------------------------------------------------------------------------
{
  return m_Min[axis];
}

//---------------------------------------------------------------------------- 
void mafInteractorConstraint::SetMax(int axis, double max)
//------------------------------------------------------------------------------
{
  m_Max[axis] = max;
}

//----------------------------------------------------------------------------      
double mafInteractorConstraint::GetMax(int axis)
//------------------------------------------------------------------------------
{
  return m_Max[axis];
}

//---------------------------------------------------------------------------- 
void mafInteractorConstraint::SetStep(int axis, double step)
//------------------------------------------------------------------------------
{
  m_Step[axis] = step;
} 

//----------------------------------------------------------------------------      
double mafInteractorConstraint::GetStep(int axis)
//------------------------------------------------------------------------------
{
  return m_Step[axis];
}

//----------------------------------------------------------------------------      
void mafInteractorConstraint::SetSnapArray(int axis, vtkDoubleArray *array)
//------------------------------------------------------------------------------
{
  assert( array );
  assert( axis>=X && axis <=Z );
  m_SnapArray[axis]=array;
}   

//----------------------------------------------------------------------------      
vtkDoubleArray *mafInteractorConstraint::GetSnapArray(int axis)
//------------------------------------------------------------------------------
{
  assert( axis>=X && axis <=Z );
  return m_SnapArray[axis];
}

//----------------------------------------------------------------------------
int mafInteractorConstraint::GetNumberOfDOF()
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
int mafInteractorConstraint::GetConstraintAxis()
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
void mafInteractorConstraint::Print(std::ostream& os, const int tabs)
//----------------------------------------------------------------------------
{
  // To be completed
  int j;
  mafIndent indent(tabs);

  os << indent << "m_ConstraintModality:\n";
  os << indent << indent;
  for (j = 0; j < 3; j++) 
  {
    os << m_ConstraintModality[j] << " ";
  }
  os << "\n";
}


//----------------------------------------------------------------------------
void mafInteractorConstraint::SetBounds(int axis, double *bounds)
//----------------------------------------------------------------------------
{
  this->SetLowerBound(axis, bounds[0]);
  this->SetUpperBound(axis, bounds[1]);
}


//----------------------------------------------------------------------------
void mafInteractorConstraint::GetBounds(int axis, double *bounds)
//----------------------------------------------------------------------------
{
  bounds[0] = this->GetLowerBound(axis);
  bounds[1] = this->GetUpperBound(axis);
}

//----------------------------------------------------------------------------
void mafInteractorConstraint::SetSnapStep(int axis, double min,double max, double step)
//----------------------------------------------------------------------------
{
  this->SetMin(axis, min);
  this->SetMax(axis, max);
  this->SetStep(axis, step);
}

//----------------------------------------------------------------------------
int mafInteractorConstraint::GetConstraintPlane()
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
int mafInteractorConstraint::GetConstraintPlaneAxes(int& axis1, int& axis2)
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
