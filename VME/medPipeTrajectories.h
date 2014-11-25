/*=========================================================================

 Program: MAF2Medical
 Module: medPipeTrajectories
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medPipeTrajectories_H__
#define __medPipeTrajectories_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
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
class MAF_EXPORT medPipeTrajectories : public mafPipe
{
public:
  mafTypeMacro(medPipeTrajectories,mafPipe);

  medPipeTrajectories();
  virtual     ~medPipeTrajectories ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n);
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
  mafVMELandmark         *m_Landmark;

  std::vector<mafTimeStamp> m_TimeVector;
  mafMatrixVector *m_MatrixVector;

  int m_Interval;
  
  virtual mafGUI  *CreateGui();

  
};  
#endif // __mafPipeTrajectories_H__
