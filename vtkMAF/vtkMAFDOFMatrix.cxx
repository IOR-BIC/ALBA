 /////////////////////////////////////////////////////////////////////////////
// Name:        vtkMAFDOFMatrix.cxx
//
// Project:     MULTIMOD (LAL services)
// Author:      Stefano Perticoni
// Date:        20/11/2003
/////////////////////////////////////////////////////////////////////////////
#include "vtkMAFDOFMatrix.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include <stdlib.h>
#include <math.h>

vtkCxxRevisionMacro(vtkMAFDOFMatrix, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFDOFMatrix);

//----------------------------------------------------------------------------
vtkMAFDOFMatrix::vtkMAFDOFMatrix()
{
this->Reset();
}

//----------------------------------------------------------------------------
vtkMAFDOFMatrix::~vtkMAFDOFMatrix()
{

}


//----------------------------------------------------------------------------
void vtkMAFDOFMatrix::DeepCopy(vtkMAFDOFMatrix *source)
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
void vtkMAFDOFMatrix::Reset()
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
void vtkMAFDOFMatrix::SetState(int mtype, int axis, int state)
{
DOFMatrix[mtype][axis].state = state;
}

//----------------------------------------------------------------------------
int vtkMAFDOFMatrix::GetState(int mtype, int axis)
{
return DOFMatrix[mtype][axis].state;
}
  
//----------------------------------------------------------------------------
void vtkMAFDOFMatrix::SetLowerBound(int mtype, int axis, double lbound)
{
DOFMatrix[mtype][axis].lowb = lbound;
}

//----------------------------------------------------------------------------
double vtkMAFDOFMatrix::GetLowerBound(int mtype, int axis)
{
return DOFMatrix[mtype][axis].lowb;
}

//----------------------------------------------------------------------------  
void vtkMAFDOFMatrix::SetUpperBound(int mtype, int axis, double ubound)
{
DOFMatrix[mtype][axis].upb = ubound;
} 

//----------------------------------------------------------------------------    
double vtkMAFDOFMatrix::GetUpperBound(int mtype, int axis)
{
return DOFMatrix[mtype][axis].upb;
}

//----------------------------------------------------------------------------
void vtkMAFDOFMatrix::SetMin(int mtype, int axis, double min)
{
DOFMatrix[mtype][axis].min = min;
}

//----------------------------------------------------------------------------
double vtkMAFDOFMatrix::GetMin(int mtype, int axis)
{
return DOFMatrix[mtype][axis].min;
}

//---------------------------------------------------------------------------- 
void vtkMAFDOFMatrix::SetMax(int mtype, int axis, double max)
{
DOFMatrix[mtype][axis].max = max;
}

//----------------------------------------------------------------------------      
double vtkMAFDOFMatrix::GetMax(int mtype, int axis)
{
return DOFMatrix[mtype][axis].max;
}

//---------------------------------------------------------------------------- 
void vtkMAFDOFMatrix::SetStep(int mtype, int axis, double step)
{
DOFMatrix[mtype][axis].step = step;
} 

//----------------------------------------------------------------------------      
double vtkMAFDOFMatrix::GetStep(int mtype, int axis)
{
return DOFMatrix[mtype][axis].step;
}

//----------------------------------------------------------------------------      
void vtkMAFDOFMatrix::SetArray(int mtype, int axis, vtkDoubleArray *array)
{
DOFMatrix[mtype][axis].farray = array;
}   

//----------------------------------------------------------------------------      
vtkDoubleArray *vtkMAFDOFMatrix::GetArray(int mtype, int axis)
{
return DOFMatrix[mtype][axis].farray;
}

//----------------------------------------------------------------------------
int vtkMAFDOFMatrix::GetDOFNumber(int transform)
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
int vtkMAFDOFMatrix::GetConstrainAxis(int transform)
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
int vtkMAFDOFMatrix::GetConstrainPlane(int transform)
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
void vtkMAFDOFMatrix::PrintSelf(ostream& os, vtkIndent indent)
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
