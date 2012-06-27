/*=========================================================================

 Program: MAF2
 Module: mafAvatar3DCone
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafAvatar3DCone_h
#define __mafAvatar3DCone_h

#include "mafAvatar3D.h"

class mafMatrix;
class vtkConeSource;
class vtkAxes;
class vtkPolyDataMapper;
class vtkActor;
class vtkAssembly;


/** Create an avatar with a 3D Cone
  Simple implementation of avatar being represented in the scene as a Cone
  @sa mafAvatar3D
*/
class MAF_EXPORT mafAvatar3DCone : public mafAvatar3D
{
public:
  mafTypeMacro(mafAvatar3DCone,mafAvatar3D);
 
  /** pick in the scene with this avatar, give the avatar pose */
  virtual int Pick(mafMatrix &tracker_pose);

protected:
  mafAvatar3DCone();
  virtual ~mafAvatar3DCone();

  vtkConeSource     *m_ConeCursor; 
  vtkAxes           *m_CursorAxes; 
  vtkPolyDataMapper *m_CursorMapper;
  vtkActor          *m_CursorActor;
  vtkPolyDataMapper *m_CursorAxesMapper;
  vtkActor          *m_CursorAxesActor;

private:
  mafAvatar3DCone(const mafAvatar3DCone&);  // Not implemented.
  void operator=(const mafAvatar3DCone&);  // Not implemented.
};

#endif 
