/*=========================================================================

 Program: MAF2
 Module: mafVMEPolyline
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEPolyline_h
#define __mafVMEPolyline_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGeneric.h"
#include "mafVMEOutputPolyline.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkPolyData;
class mmaMaterial;

/** mafVMEPolyline - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEPolyline is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMEPolyline : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMEPolyline,mafVMEGeneric);

  /** 
    Set data for the give timestamp. This function automatically creates a
    a VMEItem for the data to be stored. The optional parameter specify if
    data must be copied, referenced, or detached from original source. Default
    is to copy data which creates a new data set copying the original one.
    Referencing means the data is kept connected to its original source, and
    detaching means disconnect is like referencing but also disconnect the data
    from the source.
    Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
    this type of VME. */
    virtual int SetData(vtkPolyData *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

    /** return the right type of output */  
    mafVMEOutputPolyline *GetPolylineOutput() {return (mafVMEOutputPolyline *)GetOutput();}

    /** return the right type of output */  
    virtual mafVMEOutput *GetOutput();

    /** Return pointer to material attribute. */
    mmaMaterial *GetMaterial();

    /** Return the suggested pipe-typename for the visualization of this vme */
    virtual mafString GetVisualPipe() {return mafString("mafPipePolyline");};

		/** return an xpm-icon that can be used to represent this node */
		static char ** GetIcon();

protected:
  mafVMEPolyline();
  virtual ~mafVMEPolyline();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

private:
  mafVMEPolyline(const mafVMEPolyline&); // Not implemented
  void operator=(const mafVMEPolyline&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};

#endif
