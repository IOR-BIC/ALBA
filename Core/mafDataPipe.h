/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataPipe.h,v $
  Language:  C++
  Date:      $Date: 2005-03-11 10:10:46 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafDataPipe_h
#define __mafDataPipe_h

#include "mafObject.h"
#include "mafTimeStamped.h"
#include "mafEventSender.h"
#include "mafOBB.h"

//----------------------------------------------------------------------------
//  forward declarations
//----------------------------------------------------------------------------
class mafVMEItem;
class mafVME;

/** abstract class for process objects producing data as output of a VME.
  mafDataPipe is the base class for process objects producing data as output 
  of a VME. This basic class does not define the kind of output data. The class
  also implements the mechanism to compute the 3D bounds
  at any time...
 
  @sa mafInterpolator
 
  @todo
  - rewrite comments
  - modify the GetCurrentBounds to make it call UpdateCurentBounds explicitly!
  - rewrite GetOutput()
  - reerite UpdateBounds()
*/
class MAF_EXPORT mafDataPipe:public mafObject, public mafTimeStamped, public mafEventSender
{
public:
  mafDataPipe();
  virtual ~mafDataPipe();

  mafTypeMacro(mafDataPipe,mafObject);

  /**
    This function makes the current bounds to be updated. It's optimized
    to not require real data updating, i.e. data bounds are be evaluated 
    without actually loading the data, but reading dataset bounds from 
    VME item meta-data structure */
  virtual void UpdateBounds();
 
  /** 
    Return the current dataset bounds, updated by UpdateBounds() and
    Update() when CurrentTime is changed. The Update() functions
    must be called before calling this function, otherwise the bounds
    are invalid.*/
  mafOBB *GetBounds() {return &m_CurrentBounds;}
  

  /** Set/Get the current time */
  void SetCurrentTime(mafTimeStamp t);
  mafTimeStamp GetCurrentTime();

  /** return modification time */
  virtual unsigned long GetMTime();

  /** This function returns true if the VME is accepted by this Pipe. */
  virtual bool Accept(mafVME *vme) {return vme!=NULL;}

  /**
    Make a copy of this pipe, also copying all parameters. This equivalente to
    NewInstance + DeepCopy.
    BEWARE: the returned object has reference counter already set to 0. This avoid the 
    to do an extra Delete(), but requires to Register it before passing it
    to other objects. */
  mafDataPipe *MakeACopy();

  /**
    Copy from another pipe, the function return VTK_ERROR if the specied pipe 
    is not compatible. */
  virtual int DeepCopy(mafDataPipe *pipe);

  /** Set/Get the VME to be used as input for this PIPE */
  int SetVME(mafVME *vme);
  mafVME *GetVME() {return m_VME;}

  /** enable/disable the dependence of this data pipe from VME's pose matrix */
  void SetDependOnPose(int flag) {m_DependOnPose=flag;Modified();}
  int GetDependOnPose() {return m_DependOnPose;}

  /** enable/disable the dependence of this data pipe from VME's ABS pose matrix */
  void SetDependOnAbsPose(int flag) {m_DependOnAbsPose=flag;Modified();}
  int GetDependOnAbsPose() {return m_DependOnAbsPose;}

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs);
  
protected:
  /** function called before of data pipe execution */
  virtual void PreExecute() {}

  /** function called to updated the data pipe output */
  virtual void Exectute() {}

  mafOBB        m_CurrentBounds;///< bounds of the output data
  mafTimeStamp  m_CurrentTime;  ///< time for which data is computed
  mafVME        *m_VME;         ///< pointer to the VME for which output is computed

  int           m_DependOnPose;
  int           m_DependOnAbsPose;

private:
  mafDataPipe(const mafDataPipe&);   //Not implemented
  void operator=(const mafDataPipe&);   //Not implemented  
};

#endif /* __mafDataPipe_h */
 
