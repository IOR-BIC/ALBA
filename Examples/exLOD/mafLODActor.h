/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLODActor.h,v $
  Language:  C++
  Date:      $Date: 2006-07-17 14:58:36 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani & Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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

