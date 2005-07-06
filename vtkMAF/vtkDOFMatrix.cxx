 /////////////////////////////////////////////////////////////////////////////
// Name:        vtkDOFMatrix.cxx
//
// Project:     MULTIMOD (LAL services)
// Author:      Stefano Perticoni
// Date:        20/11/2003
/////////////////////////////////////////////////////////////////////////////
#include "vtkDOFMatrix.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include <stdlib.h>
#include <math.h>

vtkCxxRevisionMacro(vtkDOFMatrix, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkDOFMatrix);

//----------------------------------------------------------------------------
vtkDOFMatrix::vtkDOFMatrix()
{
this->Reset();
}

//----------------------------------------------------------------------------
vtkDOFMatrix::~vtkDOFMatrix()
{

}


//----------------------------------------------------------------------------
void vtkDOFMatrix::DeepCopy(vtkDOFMatrix *source)
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
void vtkDOFMatrix::Reset()
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
void vtkDOFMatrix::SetState(int mtype, int axis, int state)
{
DOFMatrix[mtype][axis].state = state;
}

//----------------------------------------------------------------------------
int vtkDOFMatrix::GetState(int mtype, int axis)
{
return DOFMatrix[mtype][axis].state;
}
  
//----------------------------------------------------------------------------
void vtkDOFMatrix::SetLowerBound(int mtype, int axis, double lbound)
{
DOFMatrix[mtype][axis].lowb = lbound;
}

//----------------------------------------------------------------------------
double vtkDOFMatrix::GetLowerBound(int mtype, int axis)
{
return DOFMatrix[mtype][axis].lowb;
}

//----------------------------------------------------------------------------  
void vtkDOFMatrix::SetUpperBound(int mtype, int axis, double ubound)
{
DOFMatrix[mtype][axis].upb = ubound;
} 

//----------------------------------------------------------------------------    
double vtkDOFMatrix::GetUpperBound(int mtype, int axis)
{
return DOFMatrix[mtype][axis].upb;
}

//----------------------------------------------------------------------------
void vtkDOFMatrix::SetMin(int mtype, int axis, double min)
{
DOFMatrix[mtype][axis].min = min;
}

//----------------------------------------------------------------------------
double vtkDOFMatrix::GetMin(int mtype, int axis)
{
return DOFMatrix[mtype][axis].min;
}

//---------------------------------------------------------------------------- 
void vtkDOFMatrix::SetMax(int mtype, int axis, double max)
{
DOFMatrix[mtype][axis].max = max;
}

//----------------------------------------------------------------------------      
double vtkDOFMatrix::GetMax(int mtype, int axis)
{
return DOFMatrix[mtype][axis].max;
}

//---------------------------------------------------------------------------- 
void vtkDOFMatrix::SetStep(int mtype, int axis, double step)
{
DOFMatrix[mtype][axis].step = step;
} 

//----------------------------------------------------------------------------      
double vtkDOFMatrix::GetStep(int mtype, int axis)
{
return DOFMatrix[mtype][axis].step;
}

//----------------------------------------------------------------------------      
void vtkDOFMatrix::SetArray(int mtype, int axis, vtkDoubleArray *array)
{
DOFMatrix[mtype][axis].farray = array;
}   

//----------------------------------------------------------------------------      
vtkDoubleArray *vtkDOFMatrix::GetArray(int mtype, int axis)
{
return DOFMatrix[mtype][axis].farray;
}

//----------------------------------------------------------------------------
int vtkDOFMatrix::GetDOFNumber(int transform)
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
int vtkDOFMatrix::GetConstrainAxis(int transform)
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
int vtkDOFMatrix::GetConstrainPlane(int transform)
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
void vtkDOFMatrix::PrintSelf(ostream& os, vtkIndent indent)
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
