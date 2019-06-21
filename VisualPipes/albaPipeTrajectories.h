/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeTrajectories
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeTrajectories_H__
#define __albaPipeTrajectories_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkPolyData;
class vtkSphereSource;
class albaVMELandmark;
class vtkAppendPolyData;
class albaMatrixVector;


//----------------------------------------------------------------------------
// albaPipeTrajectories :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeTrajectories : public albaPipe
{
public:
  albaTypeMacro(albaPipeTrajectories,albaPipe);

  albaPipeTrajectories();
  virtual     ~albaPipeTrajectories ();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  virtual void Create(albaSceneNode *n);
  virtual void Select(bool select); 

  /** Set trajectories interval */
  void SetInterval(int interval) {m_Interval = interval;};

  /**Function to update trajectory */
  void UpdateProperty(bool fromTag = false);


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
  albaVMELandmark         *m_Landmark;

  std::vector<albaTimeStamp> m_TimeVector;
  albaMatrixVector *m_MatrixVector;

  int m_Interval;
  
  virtual albaGUI  *CreateGui();

  
};  
#endif // __albaPipeTrajectories_H__
