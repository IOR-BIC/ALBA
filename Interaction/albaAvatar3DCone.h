/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar3DCone
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAvatar3DCone_h
#define __albaAvatar3DCone_h

#include "albaAvatar3D.h"

class albaMatrix;
class vtkConeSource;
class vtkAxes;
class vtkPolyDataMapper;
class vtkActor;
class vtkAssembly;


/** Create an avatar with a 3D Cone
  Simple implementation of avatar being represented in the scene as a Cone
  @sa albaAvatar3D
*/
class ALBA_EXPORT albaAvatar3DCone : public albaAvatar3D
{
public:
  albaTypeMacro(albaAvatar3DCone,albaAvatar3D);
 
  /** pick in the scene with this avatar, give the avatar pose */
  virtual int Pick(albaMatrix &tracker_pose);

protected:
  albaAvatar3DCone();
  virtual ~albaAvatar3DCone();

  vtkConeSource     *m_ConeCursor; 
  vtkAxes           *m_CursorAxes; 
  vtkPolyDataMapper *m_CursorMapper;
  vtkActor          *m_CursorActor;
  vtkPolyDataMapper *m_CursorAxesMapper;
  vtkActor          *m_CursorAxesActor;

private:
  albaAvatar3DCone(const albaAvatar3DCone&);  // Not implemented.
  void operator=(const albaAvatar3DCone&);  // Not implemented.
};

#endif 
