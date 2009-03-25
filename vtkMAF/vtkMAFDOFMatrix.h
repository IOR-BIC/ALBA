/*=======================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFDOFMatrix.h,v $
  Language:  C++
  Date:      $Date: 2009-03-25 15:01:56 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni (perticoni@tecno.ior.it)
  Project:   Multimod Project (http://www.ior.it/multimod/)

=========================================================================
Copyright (c) 2001 B3C - BioComputing Compentence Centre 
(http://www.cineca.it/B3C/)

Redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the 
following conditions are met: 

* Redistributions of source code must retain the above 
copyright notice, this list of conditions and the following 
disclaimer.

* Redistributions in binary form must reproduce the above 
copyright notice, this list of conditions and the following 
disclaimer in the documentation and/or other materials 
provided with the distribution.

* Neither name of the Multimod Consortium or its partner 
institutions nor the names of any contributors may be used 
to endorse or promote products derived from this software 
without specific prior written permission.

* Modified source versions must be plainly marked as such, 
and must not be misrepresented
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THIS SOFTWARE IS INTENDED FOR EDUCATIONAL AND 
INVESTIGATIONAL PURPOSES ONLY. THUS, NO MEDICAL DECISION 
SHOULD BE BASED ON RESULTS PROVIDED BY THE SOFTWARE. UNDER 
NO CIRCUMSTANCES SHALL SOFTWARE BE USED AS A CLINICAL 
DIAGNOSTIC TOOL.

=========================================================================*/
// .NAME vtkMAFDOFMatrix 
// .SECTION Description
// 
// vtkMAFDOFMatrix holds informations on constrains to be applied on an
// actor during interaction. Information for the single dof
// (ie an element of the internal DOFMatrix[3][3]) is stored in 
// the element struct. The single dof can have different states:
// LOCK:       no interaction is allowed  
// FREE:       free movement 
// BOUNDS:     free movement in a given interval
// SNAP_STEP:  snap by a given step
// SNAP_ARRAY: snap on a given array of positions
//
// This state is registered in the state variable of element struct. 
//               
//                DOFMatrix[3][3]
// ----------------------------------------------
// |                                             |
// | t                        axis               |
// | r               -------------------------   |
// | a               |  X        Y       Z       |
// | n    -----------|------------------------   |
// | s t  |TRANSLATE |[0,0]    [0,1]   [0,2]     |
// | f y  |ROTATE    |[1,0]    [1,1]   [1,2]     |
// | o p  |SCALE     |[2,0]    [2,1]   [2,2]     |
// | r e                                         |
// | m                                           |
// |                                             |
// | DOFMatrix[i][j] = element                   |
// -----------------------------------------------     
// 
// Default values for all elements is LOCK
//
// .SECTION See Also
//  mafISAGeneric.h
// 
// .SECTION ToDo
//

#ifndef __vtkMAFDOFMatrix_h
#define __vtkMAFDOFMatrix_h

#include "vtkObject.h"
#include "vtkMAFConfigure.h"

class vtkDoubleArray;

class VTK_vtkMAF_EXPORT vtkMAFDOFMatrix : public vtkObject
{
  
public:

   // Manage the state of the constrain 
  enum 
   {
      LOCK = 0,
      FREE,
      BOUNDS,
      SNAP_STEP,
      SNAP_ARRAY,
   };
   
   // Type of movement (DOFMatrix rows)
   enum 
   {
      TRANSLATE = 0,
      ROTATE,
      SCALE,    
   };     
   
   // Axis name (DOFMatrix columns)
   enum 
   {
      X = 0,
      Y,
      Z,    
   };

   // Plane name
   enum
   {
      XY = 0,
      XZ,
      YZ,
   };
   
  
  /**
  Construct a vtkMAFDOFMatrix */
  static vtkMAFDOFMatrix *New();

  vtkTypeRevisionMacro(vtkMAFDOFMatrix,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  /**
  DeepCopy the source matrix into the target. The target is the object
  invoking the DeepCopy method*/
  void DeepCopy(vtkMAFDOFMatrix *source); 
    
  /**
  All dof matrix element states are set to LOCK, all values of the element
  struct are set to 0 and pointers to snap_array are set to NULL.*/
  void Reset();
   
  /**
  Set the state of the constrain for the given dof; allowed state are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY.*/
  void SetState(int transformType, int axis, int state);

  /** 
  Get the state of the constrain for the given dof*/
  int GetState(int transformType, int axis);

  /**
  Set the lower bound for bound constrain type for the given dof*/
  void SetLowerBound(int transformType, int axis, double lbound);

  /**
  Get the lower bound for bound constrain type for the given dof*/
  double GetLowerBound(int transformType, int axis);

  /**
  Set the upper bound for bound constrain type for the given dof*/
  void SetUpperBound(int transformType, int axis, double ubound);

  /**
  Get the upper bound for bound constrain type for the given dof*/
  double GetUpperBound(int transformType, int axis);

  /** 
  Set the min value for snap step constrain type for the given dof*/
  void SetMin(int transformType, int axis, double min);
      
  /**      
  Get the min value for snap step constrain type for the given dof*/
  double GetMin(int transformType, int axis);

  /**
  Set the max value for snap step constrain type for the given dof*/
  void SetMax(int transformType, int axis, double min);
      
  /** 
  Get the max value for snap step constrain type for the given dof*/
  double GetMax(int transformType, int axis);
    
  /** 
  Set the step value for snap step constrain type for the given dof*/
  void SetStep(int transformType, int axis, double step);
      
  /**      
  Get the step value for snap step constrain type for the given dof*/
  double GetStep(int transformType, int axis);
 
  /**   
  Set the array which contains the allowed positions for snap.
  The array is not copied, only the pointer is stored*/
  void SetArray(int transformType, int axis, vtkDoubleArray *array);
       
  /**      
  Get the array which contains the allowed positions for snap.*/
  vtkDoubleArray *GetArray(int transformType, int axis);

  /**
  Get the number of axis on which movement is allowed (ie unlocked
  axis) for the given transform type (ROTATE, TRANSLATE or SCALE)*/
  int GetDOFNumber(int transformType);
  
  /**
  Return constrain axis when there is only one degree of freedom for the
  given transform type. If there is more than one dof returns -1.*/
  int GetConstrainAxis(int transformType);  
    
  /**
  Return the constrain plane ie an integer from the enum {XY= 0, XZ, YZ} 
  when there are two degree of freedom for the given transform type. Returns 
  -1 if dof number is different from 2.*/
  int GetConstrainPlane(int transformType);

protected:
  vtkMAFDOFMatrix();
  ~vtkMAFDOFMatrix();
  
  // The single element of the internal dof matrix.
  // Holds informations for the different states of the constrain
  typedef struct 
  {
    int state;
 
    // Free movment with bounds
    double lowb, upb;

    // Snap with step parameters
    double min, max, step;

     // Snap with array
     vtkDoubleArray *farray;
  } element;   
 
  // element access
  element GetElement(int transformType, int axis)  {return DOFMatrix[transformType][axis];}

  // The DOF Matrix
  element DOFMatrix[3][3];

private:
  
  vtkMAFDOFMatrix(const vtkMAFDOFMatrix&);  // Not implemented
  void operator= (const vtkMAFDOFMatrix&);  // Not implemented
};
#endif
