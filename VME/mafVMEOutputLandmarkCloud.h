/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputLandmarkCloud.h,v $
  Language:  C++
  Date:      $Date: 2006-01-10 16:13:09 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEOutputLandmarkCloud_h
#define __mafVMEOutputLandmarkCloud_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputPointSet.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mmaMaterial;

/** NULL output for VME node with a VTK image output data.
  mafVMEOutputLandmarkCloud is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputLandmarkCloud : public mafVMEOutputPointSet
{
public:
  mafVMEOutputLandmarkCloud();
  virtual ~mafVMEOutputLandmarkCloud();

  mafTypeMacro(mafVMEOutputLandmarkCloud,mafVMEOutputPointSet);

  /**
  Return a VTK dataset corresponding to the current time. This is
  the output of the DataPipe currently attached to the VME.
  Usually the output is a  "smart copy" of one of the dataset in 
  the DataArray. In some cases it can be NULL, e.g. in case the number
  of stored Items is 0. Also special VME could not support VTK dataset output.
  An event is rised when the output data changes to allow attached classes to 
  update their input.*/
  virtual vtkDataSet *GetVTKData();

  /** Redefined to update gui.*/
  virtual void Update();

protected:
  mmgGui *CreateGui();
  mafString m_NumLandmarks;

private:
  mafVMEOutputLandmarkCloud(const mafVMEOutputLandmarkCloud&); // Not implemented
  void operator=(const mafVMEOutputLandmarkCloud&); // Not implemented
};

#endif
