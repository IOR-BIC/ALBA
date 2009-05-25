/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrixInterpolator.h,v $
  Language:  C++
  Date:      $Date: 2009-05-25 14:45:39 $
  Version:   $Revision: 1.5.24.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafMatrixInterpolator_h
#define __mafMatrixInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafMatrixPipe.h"
#include "mafSmartPointer.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafMatrix;

/** Class Name: mafMatrixInterpolator.
  mafMatrixInterpolator is used for computing the position of VME in terms 
  of matrix that is an element of vector of items.
  mafMatrixInterpolator is a matrix pipe producing as output a matrix interpolated
  against the ones stored in the array associated to the VME itself (only for VMEs
  with matrix array like mafVMEGenericAbstract). 

*/
class MAF_EXPORT mafMatrixInterpolator:public mafMatrixPipe
{
public:
  /** type macro for RTTI and instance creation.*/
  mafTypeMacro(mafMatrixInterpolator,mafMatrixPipe);

  /** This matrix pipe accepts only VME's with internal matrix Array. */
  virtual bool Accept(mafVME *vme);

  /**  Get the output of the interpolator item. */
  mafMatrix *GetCurrentItem() {return m_CurrentItem;}

  /**
  Set the current time. Overidden to allow the output not to change when 
  not necessary, e.g. constant pose (in time) being interpolated should not
  produce a change in the output data when time is changed.*/
  virtual void SetTimeStamp(mafTimeStamp time);

  /**
  Get the MTime: this is the bit of magic that makes everything work.*/
  virtual unsigned long GetMTime();

  /** Force update of the pipe.*/
  virtual void Update();

protected:
  /** constructor.*/
  mafMatrixInterpolator();
  /** destructor.*/
  virtual ~mafMatrixInterpolator();

  //virtual void PreExecute(){};
  //virtual void Execute(){};

  /**
  This function is called internally to updated the output data according
  interpolator rules. It should be reimplemented in sub-classes.*/
  virtual void InternalItemUpdate();

  /** update the output matrix. */
  virtual void InternalUpdate();

  /** Set the current matrix of the pipe. */
  void SetCurrentItem(mafMatrix *data);

  /** request the update of the current matrix of the pipe. */
  void UpdateCurrentItem(mafMatrix *item);

  mafAutoPointer<mafMatrix>    m_CurrentItem; ///< the item currently selected for the current time

  mafAutoPointer<mafMatrix>    m_OldItem;     ///< the item previously selected

  mafTimeStamp  m_OldTimeStamp; ///< previous time
private:
  /** copy constructor not implemented. */
  mafMatrixInterpolator(const mafMatrixInterpolator&);
  /** assignment operator not implemeted. */
  void operator=(const mafMatrixInterpolator&); // Not implemented
  
};

#endif /* __mafMatrixInterpolator_h */
 
