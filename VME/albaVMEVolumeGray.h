/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolumeGray
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEVolumeGray_h
#define __albaVMEVolumeGray_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEVolume.h"
#include "albaVMEOutputVolume.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkVolumeData;

/** albaVMEVolumeGray - a VME featuring an internal array for matrices and VTK datasets.
  albaVMEVolumeGray is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa albaVME albaMatrixVector albaDataVector  */
class ALBA_EXPORT albaVMEVolumeGray : public albaVMEVolume
{
public:
  albaTypeMacro(albaVMEVolumeGray,albaVMEVolume);

  enum VOLUME_GRAY_WIDGET_ID
  {
    ID_VOLUME_TRANSFER_FUNCTION = Superclass::ID_LAST,
    ID_LAST
  };

  /** Precess events coming from other objects */ 
  virtual void OnEvent(albaEventBase *alba_event);

  /** return the right type of output */  
  albaVMEOutputVolume *GetVolumeOutput() {return (albaVMEOutputVolume *)GetOutput();}

  /** return the right type of output */  
  virtual albaVMEOutput *GetOutput();

	/** private to avoid calling by external classes */
	virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode = ALBA_VME_COPY_DATA);

protected:
  albaVMEVolumeGray();
  virtual ~albaVMEVolumeGray();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

private:
  albaVMEVolumeGray(const albaVMEVolumeGray&); // Not implemented
  void operator=(const albaVMEVolumeGray&); // Not implemented
  
};

#endif
