/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVolumeGray.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:06:00 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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

  /** Set Volume data. */
  virtual int SetData(vtkImageData *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
  /** Set Volume data. */
  virtual int SetData(vtkRectilinearGrid *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** return the right type of output */  
  mafVMEOutputVolume *GetVolumeOutput() {return (mafVMEOutputVolume *)GetOutput();}

  /** return the right type of output */  
  virtual mafVMEOutput *GetOutput();

protected:
  mafVMEVolumeGray();
  virtual ~mafVMEVolumeGray();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

private:
  mafVMEVolumeGray(const mafVMEVolumeGray&); // Not implemented
  void operator=(const mafVMEVolumeGray&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};

#endif
