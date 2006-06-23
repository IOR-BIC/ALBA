/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataPipe.h,v $
  Language:  C++
  Date:      $Date: 2006-06-23 08:09:42 $
  Version:   $Revision: 1.11 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDataPipe_h
#define __mafDataPipe_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafReferenceCounted.h"
#include "mafObserver.h"
#include "mafTimeStamped.h"
#include "mafOBB.h"

//----------------------------------------------------------------------------
//  forward declarations
//----------------------------------------------------------------------------
class mafVMEItem;
class mafVME;

#ifdef MAF_USE_VTK
  class vtkDataSet;
#endif

/** abstract class for process objects producing data as output of a VME.
  mafDataPipe is the base class for process objects producing data as output 
  of a VME. This basic class does not define the kind of output data. The class
  also implements the mechanism to compute the 3D bounds
  at any time...
 
  @sa mafDataInterpolator
 
  @todo
  - rewrite comments
  - modify the GetCurrentBounds to make it call UpdateCurentBounds explicitly!
  - rewrite GetOutput()
  - reerite UpdateBounds()
*/
class MAF_EXPORT mafDataPipe:public mafReferenceCounted, public mafTimeStamped, public mafObserver
{
public:
  mafDataPipe();
  virtual ~mafDataPipe();

  mafTypeMacro(mafDataPipe,mafReferenceCounted);

  /**
    This function makes the current bounds to be updated. It should be optimized
    to not require data loading, i.e. data bounds to be evaluated 
    without actually loading the data, but reading dataset bounds from 
    VME item meta-data structure (@sa mafDataInterpolator)*/
  virtual void UpdateBounds() {}
 
  /** update the output of the data pipe */
  virtual void Update() {}

  /**
    Return the current dataset bounds, updated by UpdateBounds() and
    Update() when CurrentTime is changed. The Update() functions
    must be called before calling this function, otherwise the bounds
    are invalid.*/
  mafOBB *GetBounds() {return &m_Bounds;}

#ifdef MAF_USE_VTK
  /**
    Return a VTK dataset corresponding to the current time.*/
  virtual vtkDataSet *GetVTKData() {return NULL;}
#endif

  /** Set/Get the current time */
  void SetTimeStamp(mafTimeStamp t);
  mafTimeStamp GetTimeStamp() {return m_CurrentTime;}

  /** return modification time */
  virtual unsigned long GetMTime();

  /** This function returns true if the VME is accepted by this Pipe. */
  virtual bool Accept(mafVME *vme) {return vme!=NULL;}

  /**
    Make a copy of this pipe, also copying all parameters. This is equivalent to
    NewInstance + DeepCopy.
    BEWARE: the returned object has reference counter already set to 0. This avoid the 
    to do an extra Delete(), but requires to Register it before passing it
    to other objects. */
  mafDataPipe *MakeACopy();

  /**
    Copy from another pipe, the function return VTK_ERROR if the specifyed pipe 
    is not compatible. */
  virtual int DeepCopy(mafDataPipe *pipe);

  /** Set/Get the VME to be used as input for this PIPE */
  int SetVME(mafVME *vme);
  mafVME *GetVME() {return m_VME;}

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

  void OnEvent(mafEventBase *maf_event);
  
protected:
  /** function called before of data pipe execution */
  virtual void PreExecute();

  /** function called to updated the data pipe output */
  virtual void Execute();

  mafOBB        m_Bounds;///< bounds of the output data
  mafTimeStamp  m_CurrentTime;  ///< time for which data is computed
  mafVME        *m_VME;         ///< pointer to the VME for which output is computed
  mafMTime      m_PreExecuteTime;///< use to reduce the number of time PreExecute is called

  int           m_DependOnPose;
  int           m_DependOnAbsPose;
  int           m_DependOnVMETime;

private:
  mafDataPipe(const mafDataPipe&);   //Not implemented
  void operator=(const mafDataPipe&);   //Not implemented  
};
#endif /* __mafDataPipe_h */
