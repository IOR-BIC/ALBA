/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolator
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDataPipeInterpolator_h
#define __albaDataPipeInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDataPipe.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaVMEItem;

/** superclass for computing the VME output data out of the internal VME-Items.
  albaDataPipeInterpolator is a data pipe producing as output a dataset interpolated
  against the ones stored in the array associated to the VME itself (only for VMEs
  with data array like albaVMEGeneric). The class also provides the mechanism to 
  obtain the 3D bounds for any time.
  @sa mflDataPipe albaVMEGeneric
*/
class ALBA_EXPORT albaDataPipeInterpolator : public albaDataPipe
{
public:
  albaAbstractTypeMacro(albaDataPipeInterpolator,albaDataPipe);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(albaVME *vme);

  void OnEvent(albaEventBase *alba_event);

  /**
    This function makes the current bounds to be updated. It's optimized
    to not require data updating, so that data bounds can be evaluated 
    without loading all the data (dataset bounds are replicated in the
    VME item data structure)*/
  virtual void UpdateBounds();

  /**
  Get the output of the interpolator item*/
  albaVMEItem *GetCurrentItem() {return m_CurrentItem;}

  /**
  Set the current time. Overidden to allow the output not to change whem 
  not necessary, e.g. constant data (in time) being interpolated should not
  produce a change in the output data when time is changed.*/
  virtual void SetTimeStamp(albaTimeStamp time);

  /**
  Get the MTime: this is the bit of magic that makes everything work.*/
  virtual vtkMTimeType GetMTime();

protected:
  albaDataPipeInterpolator();
  virtual ~albaDataPipeInterpolator();

  virtual void PreExecute()=0;
  virtual void Execute()=0;

  /** This function is called internally to updated the output data according
  interpolator rules. It should be reimplemented in sub-classes.*/
  virtual void InternalItemUpdate();

  /** Internally used to set the current item member variable*/
  void SetCurrentItem(albaVMEItem *data);

  /** Internally used update the current item variable to a new value (only if necessary)*/
  void UpdateCurrentItem(albaVMEItem *item);

  /** Set a flag making the interpolator to release data of unnecessary items */
  void SetReleaseDataFlag(bool flag);
  void ReleaseDataFlagOn();
  void ReleaseDataFlagOff();

  albaVMEItem    *m_CurrentItem; ///< the item currently selected for the current time

  albaVMEItem    *m_OldItem;     ///< the item previously selected

  albaTimeStamp  m_OldTimeStamp; ///< previous time

  albaMTime      m_UpdateTime;   ///< the modification time of last data update

  bool          m_ReleaseDataFlag; ///< this flag forces the interpolator to release unnecessary data (default == false)

private:
  albaDataPipeInterpolator(const albaDataPipeInterpolator&); // Not implemented
  void operator=(const albaDataPipeInterpolator&); // Not implemented
  
};

#endif /* __albaDataPipeInterpolator_h */
 
