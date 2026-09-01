 /////////////////////////////////////////////////////////////////////////////
// Name:        vtkALBADOFMatrix.cxx
//
// Project:     MULTIMOD (LAL services)
// Author:      Stefano Perticoni
// Date:        20/11/2003
/////////////////////////////////////////////////////////////////////////////
#include "vtkALBADOFMatrix.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include <stdlib.h>
#include <math.h>

vtkStandardNewMacro(vtkALBADOFMatrix);

//----------------------------------------------------------------------------
vtkALBADOFMatrix::vtkALBADOFMatrix()
{
this->Reset();
}

//----------------------------------------------------------------------------
vtkALBADOFMatrix::~vtkALBADOFMatrix()
{

}


//----------------------------------------------------------------------------
void vtkALBADOFMatrix::DeepCopy(vtkALBADOFMatrix *source)
{
  int i, j;

  if (source)
  {
  for (i = 0; i < 3; i++)
    {
    for (j = 0; j < 3; j++)
      {
      this->DOFMatrix[i][j] = source->GetElement(i, j);
      }
    }
  }
}

//----------------------------------------------------------------------------
void vtkALBADOFMatrix::Reset()
{
 
  int i,j;

  element temp;
  temp.state = LOCK;
  temp.lowb = temp.upb = 0;
  temp.min = temp.max = temp.step = 0;  
  temp.farray = NULL;

  for (i = 0; i < 3; i++)
    {
    for (j = 0; j < 3; j++)
      {
      this->DOFMatrix[i][j] = temp;
      }
    }
}

//----------------------------------------------------------------------------
void vtkALBADOFMatrix::SetState(int mtype, int axis, int state)
{
DOFMatrix[mtype][axis].state = state;
}

//----------------------------------------------------------------------------
int vtkALBADOFMatrix::GetState(int mtype, int axis)
{
return DOFMatrix[mtype][axis].state;
}
  
//----------------------------------------------------------------------------
void vtkALBADOFMatrix::SetLowerBound(int mtype, int axis, double lbound)
{
DOFMatrix[mtype][axis].lowb = lbound;
}

//----------------------------------------------------------------------------
double vtkALBADOFMatrix::GetLowerBound(int mtype, int axis)
{
return DOFMatrix[mtype][axis].lowb;
}

//----------------------------------------------------------------------------  
void vtkALBADOFMatrix::SetUpperBound(int mtype, int axis, double ubound)
{
DOFMatrix[mtype][axis].upb = ubound;
} 

//----------------------------------------------------------------------------    
double vtkALBADOFMatrix::GetUpperBound(int mtype, int axis)
{
return DOFMatrix[mtype][axis].upb;
}

//----------------------------------------------------------------------------
void vtkALBADOFMatrix::SetMin(int mtype, int axis, double min)
{
DOFMatrix[mtype][axis].min = min;
}

//----------------------------------------------------------------------------
double vtkALBADOFMatrix::GetMin(int mtype, int axis)
{
return DOFMatrix[mtype][axis].min;
}

//---------------------------------------------------------------------------- 
void vtkALBADOFMatrix::SetMax(int mtype, int axis, double max)
{
DOFMatrix[mtype][axis].max = max;
}

//----------------------------------------------------------------------------      
double vtkALBADOFMatrix::GetMax(int mtype, int axis)
{
return DOFMatrix[mtype][axis].max;
}

//---------------------------------------------------------------------------- 
void vtkALBADOFMatrix::SetStep(int mtype, int axis, double step)
{
DOFMatrix[mtype][axis].step = step;
} 

//----------------------------------------------------------------------------      
double vtkALBADOFMatrix::GetStep(int mtype, int axis)
{
return DOFMatrix[mtype][axis].step;
}

//----------------------------------------------------------------------------      
void vtkALBADOFMatrix::SetArray(int mtype, int axis, vtkDoubleArray *array)
{
DOFMatrix[mtype][axis].farray = array;
}   

//----------------------------------------------------------------------------      
vtkDoubleArray *vtkALBADOFMatrix::GetArray(int mtype, int axis)
{
return DOFMatrix[mtype][axis].farray;
}

//----------------------------------------------------------------------------
int vtkALBADOFMatrix::GetDOFNumber(int transform)
{
int ndof = 0;
for (int j = 0; j < 3; j++)
{
  if (DOFMatrix[transform][j].state != LOCK)
  {
    ndof++;
  }
}
return ndof;
}

//----------------------------------------------------------------------------
int vtkALBADOFMatrix::GetConstrainAxis(int transform)
{
  if (this->GetDOFNumber(transform) == 1)
  {
    for (int j = 0; j < 3; j++)
    {
      if (DOFMatrix[transform][j].state != LOCK)
      {
        return j;
      }
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
int vtkALBADOFMatrix::GetConstrainPlane(int transform)
{
  const int AXIS_BLOCKED = 0;
  const int AXIS_ALLOWED = 1;
  
  if (this->GetDOFNumber(transform) == 2)
  {
    int axesStatus[3] = {AXIS_BLOCKED, AXIS_BLOCKED, AXIS_BLOCKED};
    for (int j = 0; j < 3; j++)
    {
      if (DOFMatrix[transform][j].state != LOCK)
      {
        axesStatus[j] = AXIS_ALLOWED;
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
void vtkALBADOFMatrix::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  int i, j;

  os << indent << "State:\n";
  for (i = 0; i < 3; i++) 
    {
    os << indent << indent;
    for (j = 0; j < 3; j++) 
      {
      os << this->DOFMatrix[i][j].state << " ";
      }
    os << "\n";
    }
}
