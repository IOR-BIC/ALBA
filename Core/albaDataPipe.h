/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipe
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDataPipe_h
#define __albaDataPipe_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaReferenceCounted.h"
#include "albaObserver.h"
#include "albaTimeStamped.h"
#include "albaOBB.h"

//----------------------------------------------------------------------------
//  forward declarations
//----------------------------------------------------------------------------
class albaVMEItem;
class albaVME;

#ifdef ALBA_USE_VTK
  class vtkDataSet;
#endif

/** abstract class for process objects producing data as output of a VME.
  albaDataPipe is the base class for process objects producing data as output 
  of a VME. This basic class does not define the kind of output data. The class
  also implements the mechanism to compute the 3D bounds
  at any time...
 
  @sa albaDataPipeInterpolator
 
  @todo
  - rewrite comments
  - modify the GetCurrentBounds to make it call UpdateCurentBounds explicitly!
  - rewrite GetOutput()
  - reerite UpdateBounds()
*/
class ALBA_EXPORT albaDataPipe:public albaReferenceCounted, public albaTimeStamped, public albaObserver
{
public:
  albaDataPipe();
  virtual ~albaDataPipe();

  albaTypeMacro(albaDataPipe,albaReferenceCounted);

  /**
    This function makes the current bounds to be updated. It should be optimized
    to not require data loading, i.e. data bounds to be evaluated 
    without actually loading the data, but reading dataset bounds from 
    VME item meta-data structure (@sa albaDataPipeInterpolator)*/
  virtual void UpdateBounds() {}
 
  /** update the output of the data pipe */
  virtual void Update() {}

  /**
    Return the current dataset bounds, updated by UpdateBounds() and
    Update() when CurrentTime is changed. The Update() functions
    must be called before calling this function, otherwise the bounds
    are invalid.*/
  albaOBB *GetBounds() {return &m_Bounds;}

#ifdef ALBA_USE_VTK
  /**
    Return a VTK dataset corresponding to the current time.*/
  virtual vtkDataSet *GetVTKData() {return NULL;}
#endif

  /** Set/Get the current time */
  void SetTimeStamp(albaTimeStamp t);
  albaTimeStamp GetTimeStamp() {return m_CurrentTime;}

  /** return modification time */
  virtual vtkMTimeType GetMTime();

  /** This function returns true if the VME is accepted by this Pipe. */
  virtual bool Accept(albaVME *vme) {return vme!=NULL;}

  /**
    Make a copy of this pipe, also copying all parameters. This is equivalent to
    NewInstance + DeepCopy.
    BEWARE: the returned object has reference counter already set to 0. This avoid the 
    to do an extra Delete(), but requires to Register it before passing it
    to other objects. */
  albaDataPipe *MakeACopy();

  /**
    Copy from another pipe, the function return VTK_ERROR if the specified pipe 
    is not compatible. */
  virtual int DeepCopy(albaDataPipe *pipe);

  /** Set/Get the VME to be used as input for this PIPE */
  int SetVME(albaVME *vme);
  albaVME *GetVME() {return m_VME;}

  /** enable/disable the dependence of this data pipe from VME's pose matrix */
  void SetDependOnPose(int flag) {m_DependOnPose=flag;Modified();}
  int GetDependOnPose() {return m_DependOnPose;}

  /** enable/disable the dependence of this data pipe from VME's pose matrix */
  void SetDependOnVMETime(int flag) {m_DependOnVMETime=flag;Modified();}
  int GetDependOnVMETime() {return m_DependOnVMETime;}

  /** enable/disable the dependence of this data pipe from VME's ABS pose matrix */
  void SetDependOnAbsPose(int flag) {m_DependOnAbsPose=flag;Modified();}
  int GetDependOnAbsPose() {return m_DependOnAbsPose;}

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  virtual void OnEvent(albaEventBase *alba_event);
  
protected:
  /** function called before of data pipe execution */
  virtual void PreExecute();

  /** function called to updated the data pipe output */
  virtual void Execute();

  albaOBB        m_Bounds;///< bounds of the output data
  albaTimeStamp  m_CurrentTime;  ///< time for which data is computed
  albaVME        *m_VME;         ///< pointer to the VME for which output is computed
  albaMTime      m_PreExecuteTime;///< use to reduce the number of time PreExecute is called

  int           m_DependOnPose;
  int           m_DependOnAbsPose;
  int           m_DependOnVMETime;

private:
  albaDataPipe(const albaDataPipe&);   //Not implemented
  void operator=(const albaDataPipe&);   //Not implemented  
};
#endif /* __albaDataPipe_h */
