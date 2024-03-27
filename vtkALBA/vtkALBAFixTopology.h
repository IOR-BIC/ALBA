/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixTopology
 Authors: Fuli Wu
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAFixTopology_h
#define __vtkALBAFixTopology_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaConfigure.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkPolyDataAlgorithm.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// vtkALBAFixTopology class 
//----------------------------------------------------------------------------
/**
class name: vtkALBAFixTopology
This class is a filter which use vtkALBAPoissonSurfaceReconstruction class for fixing the topology.
*/
class ALBA_EXPORT vtkALBAFixTopology : public vtkPolyDataAlgorithm
{
  public:
    /** create instance of the class*/
    static vtkALBAFixTopology *New();
    /** RTTI macro*/
    vtkTypeMacro(vtkALBAFixTopology,vtkPolyDataAlgorithm);
    /** print information*/
    void PrintSelf(ostream& os, vtkIndent indent);
  
  protected:
    /** constructor */
    vtkALBAFixTopology();
    /** destructor */
    ~vtkALBAFixTopology();

    /** execute the filter*/
    int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  private:
    /** copy constructor not implemented*/
    vtkALBAFixTopology(const vtkALBAFixTopology&);
    /** operator= not implemented*/
    void operator=(const vtkALBAFixTopology&);
};

#endif