/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractor6DOFMove.h,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:46:39 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafInteractor6DOFMove_h
#define __mafInteractor6DOFMove_h

#ifdef __GNUG__
    #pragma interface "mafInteractor6DOFMove.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafInteractor6DOF.h"

/** implements 6DOF move of objects in the scene.
  This class implements a 6DOF move of objects in the scene. The interaction
  modality is a direct manipulation, where movement of the tracker are mapped
  into movements of the object. More details to be written...
*/
class mafInteractor6DOFMove : public mafInteractor6DOF
{
public:
  static mafInteractor6DOFMove *New();
  vtkTypeMacro(mafInteractor6DOFMove,mafInteractor6DOF);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mmdTracker *tracker,mflMatrix *pose=NULL);

  /**  Process events coming from tracker */
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

  /** 
    Enable/Disable differential moving. If enable this interactor moves 
    the object by applying a differential transformation to its original 
    pose, i.e. the original pose is retained and only the delta transform
    is applied to it.
    Default is false.
  */
  vtkSetMacro(DifferentialMoving,int);
  /** @sa SetDifferentialMoving() */
  vtkGetMacro(DifferentialMoving,int);
  /** @sa SetDifferentialMoving() */
  vtkBooleanMacro(DifferentialMoving,int);

  /** Set the matrix storing the object pose */
  void SetObjectPoseMatrix(mflMatrix *matrix);
  /** Get the matrix storing the object pose */
  mflMatrix *GetObjectPoseMatrix();
protected:
  mafInteractor6DOFMove();
  virtual ~mafInteractor6DOFMove();

  void Update();

  //move object with differential transform
  //void OnTrackerMove();

  mflTransform      *StartPropTransform;
  mflMatrix         *OutputObjectPoseMatrix;
  mflMatrix         *ObjectPoseMatrix;

  double               ObjectOrientation[3]; ///< orientation of the object at last Update
  double               ObjectPosition[3]; ///< position of the object at last Update
  float                StartObjPosition[3]; ///< position of the selected object at start of interaction
  float                StartObjOrientation[3]; ///< orientation of the selected object at start of interaction
  //double               StartObjScale[3]; ///< scale of the selected object at start of interaction

   
  int DifferentialMoving;

  //vtkCamera *CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  vtkTimeStamp  UpdateTime; ///< Timestamp of the last update of the tracker to world transformation 

private:
  mafInteractor6DOFMove(const mafInteractor6DOFMove&);  // Not implemented.
  void operator=(const mafInteractor6DOFMove&);  // Not implemented.
};

#endif 
