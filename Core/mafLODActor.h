/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLODActor.h,v $
  Language:  C++
  Date:      $Date: 2006-07-20 17:31:47 $
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

  /** Set the dimension in pixels of the FlagActor representing the actor when its dimensions are too small to be rendered.*/
  vtkSetClampMacro(FlagDimension, int, 1, VTK_INT_MAX);
  
  /** Get the dimension in pixels of the FlagActor.*/
  vtkGetMacro(FlagDimension, double);

  /** Set the threshold in pixels to switch the actor's shape to the FlagActor representation. Accepted values are > 1*/
  vtkSetClampMacro(PixelThreshold, int, 1, VTK_INT_MAX);
  
  /** Get the threshold in pixels*/
  vtkGetMacro(PixelThreshold, int);

  /** Enable/Disable fade out and fade in for the actor when it switch between one LOD to another.*/
  vtkSetClampMacro(EnableFading, int, 0, 1);

  /** Enable/Disable fade out and fade in for the actor when it switch between one LOD to another.*/
  vtkBooleanMacro(EnableFading,int);

  /** Return the status of the fading flag.*/
  vtkGetMacro(EnableFading, int);
  
protected:
  mafLODActor();
  ~mafLODActor();

private:
  mafLODActor(const mafLODActor&);  // Not implemented.
  void operator=(const mafLODActor&);  // Not implemented.

  vtkPointSource    *FlagShape;
  vtkPolyDataMapper *FlagMapper;
  vtkActor          *FlagActor;

  int PixelThreshold;
  int FlagDimension;
  int EnableFading;
};
#endif
