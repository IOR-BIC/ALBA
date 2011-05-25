/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeScalarMatrix.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:52:18 $
  Version:   $Revision: 1.3.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeScalarMatrix_H__
#define __mafPipeScalarMatrix_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkCubeAxesActor2D;
class vtkActor;

//----------------------------------------------------------------------------
// mafPipeScalarMatrix :
//----------------------------------------------------------------------------
class MAF_EXPORT mafPipeScalarMatrix : public mafPipe
{
public:
  mafTypeMacro(mafPipeScalarMatrix,mafPipe);

               mafPipeScalarMatrix();
  virtual     ~mafPipeScalarMatrix ();

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
  vtkCubeAxesActor2D *m_CubeAxes;
  vtkActor *m_Actor;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  virtual mafGUI  *CreateGui();
};  
#endif // __mafPipeScalar_H__
