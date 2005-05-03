/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGeneric6DOF.h,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:37 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiGeneric6DOF_h
#define __mmiGeneric6DOF_h

#ifdef __GNUG__
    #pragma interface "mmiGeneric6DOF.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mmi6DOF.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafEventListener;

/** implements 6DOF move of objects in the scene.
  This class implements a 6DOF move of objects in the scene. The interaction
  modality is a direct manipulation, where movement of the tracker are mapped
  into movements of the object. More details to be written...
*/
class mmiGeneric6DOF : public mmi6DOF
{
public:
  static mmiGeneric6DOF *New();
  vtkTypeMacro(mmiGeneric6DOF,mmi6DOF);

  /**  Process events coming from tracker */
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

  /** manage move events */
  virtual void OnMove(mflEventInteraction *e);

  /** 
    Enable/Disable differential moving. If enable this interactor moves 
    the object by applying a differential transformation to its original 
    pose, i.e. the original pose is retained and only the delta transform
    is applied to it.
    Default is false. */
  vtkSetMacro(DifferentialMoving,int);
  vtkGetMacro(DifferentialMoving,int); ///< @sa SetDifferentialMoving()
  vtkBooleanMacro(DifferentialMoving,int); ///< @sa SetDifferentialMoving()

protected:
  mmiGeneric6DOF();
  virtual ~mmiGeneric6DOF();

  /** Update the transform */
  void Update();

  mflTransform        *OutputTransform; 
  double              OutputOrientation[3]; ///< orientation of the output matrix
  double              OutputPosition[3]; ///< position of the output matrix
  double              PivotPosition[3]; ///< position of the selected object at start of interaction

  int                 DifferentialMoving;

private:
  mmiGeneric6DOF(const mmiGeneric6DOF&);  // Not implemented.
  void operator=(const mmiGeneric6DOF&);  // Not implemented.
};

#endif 
