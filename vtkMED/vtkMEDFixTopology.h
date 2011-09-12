/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDFixTopology.h,v $
Language:  C++
Date:      $Date: 2011-09-12 15:43:33 $
Version:   $Revision: 1.1.2.3 $
Authors:   Fuli Wu
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __vtkMEDFixTopology_h
#define __vtkMEDFixTopology_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkMEDConfigure.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkPolyDataToPolyDataFilter.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// vtkMEDFixTopology class 
//----------------------------------------------------------------------------
/**
class name: vtkMEDFixTopology
This class is a filter which use vtkMEDPoissonSurfaceReconstruction class for fixing the topology.
*/
class VTK_vtkMED_EXPORT vtkMEDFixTopology : public vtkPolyDataToPolyDataFilter
{
  public:
    /** create instance of the class*/
    static vtkMEDFixTopology *New();
    /** RTTI macro*/
    vtkTypeRevisionMacro(vtkMEDFixTopology,vtkPolyDataToPolyDataFilter);
    /** print information*/
    void PrintSelf(ostream& os, vtkIndent indent);
  
  protected:
    /** constructor */
    vtkMEDFixTopology();
    /** destructor */
    ~vtkMEDFixTopology();

    /** execute the filter*/
    void Execute();

  private:
    /** copy constructor not implemented*/
    vtkMEDFixTopology(const vtkMEDFixTopology&);
    /** operator= not implemented*/
    void operator=(const vtkMEDFixTopology&);
};

#endif