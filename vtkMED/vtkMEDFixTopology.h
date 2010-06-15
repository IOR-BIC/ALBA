/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDFixTopology.h,v $
Language:  C++
Date:      $Date: 2010-06-15 13:56:33 $
Version:   $Revision: 1.1.2.1 $
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
class VTK_GRAPHICS_EXPORT vtkMEDFixTopology : public vtkPolyDataToPolyDataFilter
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