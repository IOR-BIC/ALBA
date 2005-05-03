/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi6DOFCameraMove.h,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:35 $
  Version:   $Revision: 1.1 $
  Authors:   Michele Diegoli & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmi6DOFCameraMove_h
#define __mmi6DOFCameraMove_h

#ifdef __GNUG__
    #pragma interface "mmi6DOFCameraMove.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mmi6DOF.h"

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
  static mmi6DOFCameraMove *New();
  vtkTypeMacro(mmi6DOFCameraMove,mmi6DOF);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mmdTracker *tracker,mflMatrix *pose=NULL);

  /**  Process events coming from tracker */
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

protected:
  mmi6DOFCameraMove();
  virtual ~mmi6DOFCameraMove();

  virtual void Update();

  //double               ObjectOrientation[3]; ///< orientation of the object at last Update
  //double               ObjectPosition[3]; ///< position of the object at last Update
  double               StartCameraPosition[4]; ///< position of the camera at start of interaction
  double               StartFocalPoint[4]; ///< focal point of the camera at start of interaction
  double               StartViewUp[4]; ///< viewUp of the camera at start of interaction
  double			   StartOrientation[4];
  double			   OldZ;

  bool active_camera_to_Current_Camera_flag;
  vtkCamera *CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  vtkTimeStamp  UpdateTime; ///< Timestamp of the last update of the tracker to world transformation 

private:
  mmi6DOFCameraMove(const mmi6DOFCameraMove&);  // Not implemented.
  void operator=(const mmi6DOFCameraMove&);  // Not implemented.
};

#endif 
