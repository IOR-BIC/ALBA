/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar3DCone.h,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:55 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAvatar3DCone_h
#define __mafAvatar3DCone_h

#ifdef __GNUG__
    #pragma interface "mafAvatar3DCone.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafAvatar3D.h"

class vtkConeSource;
class vtkAxes;
class vtkPolyDataMapper;
class vtkActor;
class vtkAssembly;


/** Create an avatar with a 3D Cone
  Simple implementation of avatar being represented in the scene as a Cone
  @sa mafAvatar3D
*/
class mafAvatar3DCone : public mafAvatar3D
{
public:
  vtkTypeMacro(mafAvatar3DCone,mafAvatar3D);
  static mafAvatar3DCone *New();
  static const char *GetTypeName() {return "mafAvatar3DCone";}
  static vtkObject *NewObjectInstance() {return new mafAvatar3DCone;}

  /** pick in the scene with this avatar, give the avatar pose */
  virtual int Pick(mflMatrix *pose=NULL);

protected:
  mafAvatar3DCone();
  virtual ~mafAvatar3DCone();

  vtkConeSource     *ConeCursor; 
  vtkAxes           *CursorAxes; 
  vtkPolyDataMapper *CursorMapper;
  vtkActor          *CursorActor;
  vtkPolyDataMapper *CursorAxesMapper;
  vtkActor          *CursorAxesActor;

private:
  mafAvatar3DCone(const mafAvatar3DCone&);  // Not implemented.
  void operator=(const mafAvatar3DCone&);  // Not implemented.
};

#endif 
