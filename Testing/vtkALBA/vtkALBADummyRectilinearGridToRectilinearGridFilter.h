/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADummyRectilinearGridToRectilinearGridFilter
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_DUMMYRECTILINEARGRIDTORECTILINEARGRIDFILTER_H__
#define __CPP_UNIT_DUMMYRECTILINEARGRIDTORECTILINEARGRIDFILTER_H__

#include "vtkALBARectilinearGridToRectilinearGridFilter.h"

class vtkALBADummyRectilinearGridToRectilinearGridFilter : public vtkALBARectilinearGridToRectilinearGridFilter
{
  vtkTypeRevisionMacro(vtkALBADummyRectilinearGridToRectilinearGridFilter,vtkALBARectilinearGridToRectilinearGridFilter);

protected:
  void operator=(const vtkALBADummyRectilinearGridToRectilinearGridFilter&) {};
  vtkALBADummyRectilinearGridToRectilinearGridFilter(const vtkALBADummyRectilinearGridToRectilinearGridFilter&) {};

};

#endif
