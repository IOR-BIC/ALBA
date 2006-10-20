/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-10-20 08:51:27 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeScalar_H__
#define __mafPipeScalar_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkGlyph3D;
class vtkPolyDataMapper;
class vtkActor;

//----------------------------------------------------------------------------
// mafPipeScalar :
//----------------------------------------------------------------------------
class mafPipeScalar : public mafPipe
{
public:
  mafTypeMacro(mafPipeScalar,mafPipe);

               mafPipeScalar();
  virtual     ~mafPipeScalar ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_SCALAR_WIDGET_ID
  {
    ID_RADIUS = Superclass::ID_LAST,
    ID_LAST
  };

protected:
  vtkSphereSource   *m_Sphere;
  vtkGlyph3D        *m_Glyph;
  vtkPolyDataMapper *m_Mapper;
  vtkActor          *m_Actor;

  double m_Radius;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeScalar_H__
