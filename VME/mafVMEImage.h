/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEImage.h,v $
  Language:  C++
  Date:      $Date: 2005-04-19 08:27:01 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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

    /** return icon */
    static char** GetIcon();
protected:
  mafVMEImage();
  virtual ~mafVMEImage();

private:
  mafVMEImage(const mafVMEImage&); // Not implemented
  void operator=(const mafVMEImage&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};

#endif
