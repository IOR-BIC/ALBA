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
#include "vtkPolyDataToPolyDataFilter.h"

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
class ALBA_EXPORT vtkALBAFixTopology : public vtkPolyDataToPolyDataFilter
{
  public:
    /** create instance of the class*/
    static vtkALBAFixTopology *New();
    /** RTTI macro*/
    vtkTypeRevisionMacro(vtkALBAFixTopology,vtkPolyDataToPolyDataFilter);
    /** print information*/
    void PrintSelf(ostream& os, vtkIndent indent);
  
  protected:
    /** constructor */
    vtkALBAFixTopology();
    /** destructor */
    ~vtkALBAFixTopology();

    /** execute the filter*/
    void Execute();

  private:
    /** copy constructor not implemented*/
    vtkALBAFixTopology(const vtkALBAFixTopology&);
    /** operator= not implemented*/
    void operator=(const vtkALBAFixTopology&);
};

#endif