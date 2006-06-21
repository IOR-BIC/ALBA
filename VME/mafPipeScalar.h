/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-06-21 15:24:04 $
  Version:   $Revision: 1.1 $
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
class vtkWarpScalar;
class vtkLineSource;
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
    ID_ACTIVE_SCALAR = Superclass::ID_LAST,
    ID_LAST
  };

protected:
  int m_ActiveScalar; ///<Active scalar visualized
  int m_Order;  ///<Order of how are stored the scalar values: 0 = Rows, 1 = Columns

  vtkLineSource     *m_LineSource;
  vtkWarpScalar     *m_WrapScalar;
  vtkPolyDataMapper *m_Mapper;
  vtkActor          *m_Actor;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeScalar_H__
