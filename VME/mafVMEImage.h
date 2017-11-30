/*=========================================================================

 Program: MAF2
 Module: mafVMEImage
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEImage_h
#define __mafVMEImage_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGeneric.h"
#include "mafVMEOutputImage.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkImageData;
class mmaMaterial;

/** mafVMEImage - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEImage is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMEImage : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMEImage,mafVMEGeneric);

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
    virtual int SetData(vtkImageData *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

    /** return the right type of output */  
    mafVMEOutputImage *GetImageOutput() {return (mafVMEOutputImage *)GetOutput();}

    /** return the right type of output */  
    virtual mafVMEOutput *GetOutput();

		/** Return pointer to material attribute. */
		mmaMaterial *GetMaterial();

    /** Return the suggested pipe-typename for the visualization of this vme */
    virtual mafString GetVisualPipe() {return mafString("mafPipeImage3D");};

		wxBitmap GetImageAsBitmap();

    /** return icon */
    static char** GetIcon();
protected:
  mafVMEImage();
  virtual ~mafVMEImage();

private:
  mafVMEImage(const mafVMEImage&); // Not implemented
  void operator=(const mafVMEImage&); // Not implemented

	/** used to initialize and create the material attribute if not yet present */
	virtual int InternalInitialize();
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};

#endif
