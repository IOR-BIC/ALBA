/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDFixTopology.h,v $
Language:  C++
Date:      $Date: 2011-05-26 08:33:31 $
Version:   $Revision: 1.1.2.2 $
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
class VTK_vtkMED_EXPORT vtkMEDFixTopology : public vtkPolyDataToPolyDataFilter
{
  public:
    static vtkMEDFixTopology *New();

    vtkTypeRevisionMacro(vtkMEDFixTopology,vtkPolyDataToPolyDataFilter);
    void PrintSelf(ostream& os, vtkIndent indent);
  
  protected:
    vtkMEDFixTopology();
    ~vtkMEDFixTopology();

    void Execute();

  private:
    vtkMEDFixTopology(const vtkMEDFixTopology&);  // Not implemented.
    void operator=(const vtkMEDFixTopology&);  // Not implemented. 
};

#endif