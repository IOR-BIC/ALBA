/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFDummyRectilinearGridToRectilinearGridFilter.h,v $
Language:  C++
Date:      $Date: 2009-09-02 12:32:06 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_DUMMYRECTILINEARGRIDTORECTILINEARGRIDFILTER_H__
#define __CPP_UNIT_DUMMYRECTILINEARGRIDTORECTILINEARGRIDFILTER_H__

#include "vtkMAFRectilinearGridToRectilinearGridFilter.h"

class vtkMAFDummyRectilinearGridToRectilinearGridFilter : public vtkMAFRectilinearGridToRectilinearGridFilter
{
  vtkTypeRevisionMacro(vtkMAFDummyRectilinearGridToRectilinearGridFilter,vtkMAFRectilinearGridToRectilinearGridFilter);

protected:
  void operator=(const vtkMAFDummyRectilinearGridToRectilinearGridFilter&) {};
  vtkMAFDummyRectilinearGridToRectilinearGridFilter(const vtkMAFDummyRectilinearGridToRectilinearGridFilter&) {};

};

#endif
