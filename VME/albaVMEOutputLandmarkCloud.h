/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputLandmarkCloud
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVMEOutputLandmarkCloud_h
#define __albaVMEOutputLandmarkCloud_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutputPointSet.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mmaMaterial;

/** NULL output for VME node with a VTK image output data.
  albaVMEOutputLandmarkCloud is the output produced by a node generating an output
  with a VTK dataset.
*/
class ALBA_EXPORT albaVMEOutputLandmarkCloud : public albaVMEOutputPointSet
{
public:
  albaVMEOutputLandmarkCloud();
  virtual ~albaVMEOutputLandmarkCloud();

  albaTypeMacro(albaVMEOutputLandmarkCloud,albaVMEOutputPointSet);

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

  albaString GetNumberOfLandmarksSTR(){return m_NumLandmarks;};

protected:
  albaGUI *CreateGui();
  albaString m_NumLandmarks;

private:
  albaVMEOutputLandmarkCloud(const albaVMEOutputLandmarkCloud&); // Not implemented
  void operator=(const albaVMEOutputLandmarkCloud&); // Not implemented
};

#endif
