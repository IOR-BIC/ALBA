/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi6DOFCameraMove.h,v $
  Language:  C++
  Date:      $Date: 2005-05-21 07:55:51 $
  Version:   $Revision: 1.2 $
  Authors:   Michele Diegoli & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmi6DOFCameraMove_h
#define __mmi6DOFCameraMove_h

#include "mmi6DOF.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCamera;

/** implements 6DOF move of camera in the scene.
  This class implements a 6DOF move of the renderer camera in the scene. The interaction
  modality is a direct manipulation, where movement of the tracker are mapped
  into movements of the camera. More details to be written...
*/
class mmi6DOFCameraMove : public mmi6DOF
{
public:
  mafTypeMacro(mmi6DOFCameraMove,mmi6DOF);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mmdTracker *tracker,mafMatrix *pose=NULL);

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

protected:
  mmi6DOFCameraMove();
  virtual ~mmi6DOFCameraMove();

  virtual void Update();

  //double               m_ObjectOrientation[3]; ///< orientation of the object at last Update
  //double               m_ObjectPosition[3]; ///< position of the object at last Update
  double               m_StartCameraPosition[4]; ///< position of the camera at start of interaction
  double               m_StartFocalPoint[4]; ///< focal point of the camera at start of interaction
  double               m_StartViewUp[4]; ///< viewUp of the camera at start of interaction
  double               m_StartOrientation[4];
  double               m_OldZ;

//  bool                 m_ActiveCameraToCurrentCameraFlag;
  vtkCamera*           m_CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  mafMTime             m_UpdateTime; ///< Timestamp of the last update of the tracker to world transformation 

private:
  mmi6DOFCameraMove(const mmi6DOFCameraMove&);  // Not implemented.
  void operator=(const mmi6DOFCameraMove&);  // Not implemented.
};

#endif 
