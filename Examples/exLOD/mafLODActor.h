/*=========================================================================

 Program: MAF2
 Module: mafLODActor
 Authors: Paolo Quadrani & Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafLODActor_h
#define __mafLODActor_h

#include "vtkOpenGLActor.h"
class vtkPointSource;
class vtkPolyDataMapper;
class vtkActor;


class mafLODActor : public vtkOpenGLActor
{
protected:
  
public:
  static mafLODActor *New();
  vtkTypeRevisionMacro(mafLODActor,vtkOpenGLActor);

  // Description:
  void Render(vtkRenderer *ren, vtkMapper *mapper);
  
protected:
  mafLODActor();
  ~mafLODActor();

private:
  mafLODActor(const mafLODActor&);  // Not implemented.
  void operator=(const mafLODActor&);  // Not implemented.

  vtkPointSource *PS;
  vtkPolyDataMapper *PDM;
  vtkActor *A;

};

#endif

