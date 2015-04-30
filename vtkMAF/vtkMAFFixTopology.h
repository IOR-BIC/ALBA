/*=========================================================================

 Program: MAF2
 Module: vtkMAFFixTopology
 Authors: Fuli Wu
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFFixTopology_h
#define __vtkMAFFixTopology_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafConfigure.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkPolyDataToPolyDataFilter.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// vtkMAFFixTopology class 
//----------------------------------------------------------------------------
/**
class name: vtkMAFFixTopology
This class is a filter which use vtkMAFPoissonSurfaceReconstruction class for fixing the topology.
*/
class MAF_EXPORT vtkMAFFixTopology : public vtkPolyDataToPolyDataFilter
{
  public:
    /** create instance of the class*/
    static vtkMAFFixTopology *New();
    /** RTTI macro*/
    vtkTypeRevisionMacro(vtkMAFFixTopology,vtkPolyDataToPolyDataFilter);
    /** print information*/
    void PrintSelf(ostream& os, vtkIndent indent);
  
  protected:
    /** constructor */
    vtkMAFFixTopology();
    /** destructor */
    ~vtkMAFFixTopology();

    /** execute the filter*/
    void Execute();

  private:
    /** copy constructor not implemented*/
    vtkMAFFixTopology(const vtkMAFFixTopology&);
    /** operator= not implemented*/
    void operator=(const vtkMAFFixTopology&);
};

#endif