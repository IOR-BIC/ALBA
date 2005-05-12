/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataInterpolator.h,v $
  Language:  C++
  Date:      $Date: 2005-05-12 16:22:02 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDataInterpolator_h
#define __mafDataInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataPipe.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVMEItem;

/** superclass for computing the VME output data out of the internal VME-Items.
  mafDataInterpolator is a data pipe producing as output a dataset interpolated
  against the ones stored in the array associated to the VME itself (only for VMEs
  with data array like mafVMEGeneric). The class also provides the mechanism to 
  obtain the 3D bounds for any time.
  @sa mflDataPipe mafVMEGeneric
*/
class MAF_EXPORT mafDataInterpolator:public mafDataPipe
{
public:
  mafAbstractTypeMacro(mafDataInterpolator,mafDataPipe);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(mafVME *vme);

  /**
    This function makes the current bounds to be updated. It's optimized
    to not require data updating, so that data bounds can be evaluated 
    without loading all the data (dataset bounds are replicated in the
    VME item data structure)*/
  virtual void UpdateBounds();

  /**
  Get the output of the interpolator item*/
  mafVMEItem *GetCurrentItem() {return m_CurrentItem;}

  /**
  Set the current time. Overidden to allow the output not to change whem 
  not necessary, e.g. constant data (in time) being interpolated should not
  produce a change in the output data when time is changed.*/
  virtual void SetTimeStamp(mafTimeStamp time);

  /**
  Get the MTime: this is the bit of magic that makes everything work.*/
  virtual unsigned long GetMTime();

protected:
  mafDataInterpolator();
  virtual ~mafDataInterpolator();

  virtual void PreExecute()=0;
  virtual void Execute()=0;

  /**
  This function is called internally to updated the output data according
  interpolator rules. It should be reimplemented in sub-classes.*/
  virtual void InternalItemUpdate();

  /** Internally used to set the current item memeber variable*/
  void SetCurrentItem(mafVMEItem *data);

  /** Internally used update the current item variable to a new value (only if necessary)*/
  void UpdateCurrentItem(mafVMEItem *item);

  /** Set a flag making the interpolator to release data of unnecessary items */
  void SetReleaseDataFlag(bool flag);
  void ReleaseDataFlagOn();
  void ReleaseDataFlagOff();

  mafVMEItem    *m_CurrentItem; ///< the item currently selected for the current time

  mafVMEItem    *m_OldItem;     ///< the item previously selected

  mafTimeStamp  m_OldTimeStamp; ///< previous time

  mafMTime      m_UpdateTime;   ///< the modification time of last data update

  bool          m_ReleaseDataFlag; ///< this flag forces the interpolator to release unnecessary data (default == false)

private:
  mafDataInterpolator(const mafDataInterpolator&); // Not implemented
  void operator=(const mafDataInterpolator&); // Not implemented
  
};

#endif /* __mafDataInterpolator_h */
 
