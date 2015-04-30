/*=========================================================================

 Program: MAF2
 Module: vtkMAFDummyRectilinearGridToRectilinearGridFilter
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
