/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrixInterpolator.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:16:32 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafMatrixInterpolator_h
#define __mafMatrixInterpolator_h

#include "mafMatrixPipe.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafMatrix;

/** superclass for computing the VME output data out of the internal VME-Items.
  mafMatrixInterpolator is a data pipe producing as output a dataset interpolated
  against the ones stored in the array associated to the VME itself (only for VMEs
  with data array like mafVMEGeneric). The class also provides the mechanism to 
  obtain the 3D bounds for any time.

  @sa mflDataPipe mafVMEGeneric
  
  @todo
 
*/
class MAF_EXPORT mafMatrixInterpolator:public mafMatrixPipe
{
public:
  mafTypeMacro(mafMatrixInterpolator,mafMatrixPipe);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(mafVME *vme);

  /**  Get the output of the interpolator item*/
  mafMatrix *GetCurrentItem() {return m_CurrentItem;}

  /**
  Set the current time. Overidden to allow the output not to change whem 
  not necessary, e.g. constant data (in time) being interpolated should not
  produce a change in the output data when time is changed.*/
  virtual void SetCurrentTime(mafTimeStamp time);

  /**
  Get the MTime: this is the bit of magic that makes everything work.*/
  virtual unsigned long GetMTime();

  virtual void Update();

protected:
  mafMatrixInterpolator();
  virtual ~mafMatrixInterpolator();

  //virtual void PreExecute(){};
  //virtual void Execute(){};

  /**
  This function is called internally to updated the output data according
  interpolator rules. It should be reimplemented in sub-classes.*/
  virtual void InternalItemUpdate();

  /** update the output matrix */
  virtual void InternalUpdate();

  void SetCurrentItem(mafMatrix *data);
  void UpdateCurrentItem(mafMatrix *item);

  mafMatrix    *m_CurrentItem; ///< the item currently selected for the current time

  mafMatrix    *m_OldItem;     ///< the item previously selected

  mafTimeStamp  m_OldTimeStamp; ///< previous time

  mafMTime      m_UpdateTime;   ///< the modification time of last data update

private:
  mafMatrixInterpolator(const mafMatrixInterpolator&); // Not implemented
  void operator=(const mafMatrixInterpolator&); // Not implemented
  
};

#endif /* __mafMatrixInterpolator_h */
 
