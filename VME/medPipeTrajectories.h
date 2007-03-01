/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeTrajectories.h,v $
  Language:  C++
  Date:      $Date: 2007-03-01 09:07:21 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeTrajectories_H__
#define __medPipeTrajectories_H__

#include "mafPipe.h"
#include "mafEvent.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkPolyData;
class vtkSphereSource;
class mafVMELandmark;
class vtkAppendPolyData;
class mafMatrixVector;


//----------------------------------------------------------------------------
// medPipeTrajectories :
//----------------------------------------------------------------------------
class medPipeTrajectories : public mafPipe
{
public:
  mafTypeMacro(medPipeTrajectories,mafPipe);

  medPipeTrajectories();
  virtual     ~medPipeTrajectories ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 



  /** IDs for the GUI */
  enum PIPE_POLYLINE_WIDGET_ID
  {
    ID_INTERVAL = Superclass::ID_LAST,
        ID_LAST
  };

  
protected:
  vtkAppendPolyData      *m_Traj;
  vtkSphereSource        *m_Sphere;
  vtkPolyDataMapper	     *m_Mapper;
  vtkActor               *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
  vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  vtkActor               *m_OutlineActor;
  mafVMELandmark         *m_Landmark;

  std::vector<mafTimeStamp> m_TimeVector;
  mafMatrixVector *m_MatrixVector;

  int m_Interval;
  
  virtual mmgGui  *CreateGui();

  /**Function to update trajectory */
  void UpdateProperty(bool fromTag = false);
};  
#endif // __mafPipeTrajectories_H__
