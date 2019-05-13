/*=========================================================================

 Program: MAF2
 Module: mafVMEVolumeGray
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEVolumeGray_h
#define __mafVMEVolumeGray_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkVolumeData;

/** mafVMEVolumeGray - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEVolumeGray is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMEVolumeGray : public mafVMEVolume
{
public:
  mafTypeMacro(mafVMEVolumeGray,mafVMEVolume);

  enum VOLUME_GRAY_WIDGET_ID
  {
    ID_VOLUME_TRANSFER_FUNCTION = Superclass::ID_LAST,
    ID_LAST
  };

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** return the right type of output */  
  mafVMEOutputVolume *GetVolumeOutput() {return (mafVMEOutputVolume *)GetOutput();}

  /** return the right type of output */  
  virtual mafVMEOutput *GetOutput();

	/** private to avoid calling by external classes */
	virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode = MAF_VME_COPY_DATA);

protected:
  mafVMEVolumeGray();
  virtual ~mafVMEVolumeGray();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

private:
  mafVMEVolumeGray(const mafVMEVolumeGray&); // Not implemented
  void operator=(const mafVMEVolumeGray&); // Not implemented
  
};

#endif
