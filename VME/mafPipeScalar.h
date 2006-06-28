/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-06-28 16:34:18 $
  Version:   $Revision: 1.2 $
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
    ID_SCALAR_FACTOR,
    ID_LAST
  };

protected:
  int m_ActiveScalar; ///<Active scalar visualized
  int m_Order;  ///<Order of how are stored the scalar values: 0 = Rows, 1 = Columns
  double m_ScalarFactor;

  vtkLineSource     *m_LineSource;
  vtkWarpScalar     *m_WarpScalar;
  vtkPolyDataMapper *m_Mapper;
  vtkActor          *m_Actor;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeScalar_H__
