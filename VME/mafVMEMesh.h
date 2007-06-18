/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEMesh.h,v $
  Language:  C++
  Date:      $Date: 2007-06-18 13:09:09 $
  Version:   $Revision: 1.4 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEMesh_h
#define __mafVMEMesh_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGeneric.h"
#include "mafVMEOutputMesh.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkUnstructuredGrid;
class mmaMaterial;

/** mafVMEMesh - A vme that can accept vtkUnstructuredGrid 
dataset only. it's used to store finite element analysis
data*/
class MAF_EXPORT mafVMEMesh : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMEMesh,mafVMEGeneric);

  /** Set data for the give timestamp. 
  This function automatically creates a VMEItem for the data to be stored. 
  The optional parameter specify if data must be copied, referenced, or detached from original source. 
  Default is to copy data which creates a new data set copying the original one.
  Referencing means the data is kept connected to its original source, and
  detaching means disconnect is like referencing but also disconnect the data
  from the source.
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. Accepted data is vtkUnstructuredGrid */
  virtual int SetData(vtkUnstructuredGrid *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** return the output casted to mafVMEOutputMesh*/
  mafVMEOutputMesh *GetUnstructuredGridOutput() {return mafVMEOutputMesh::SafeDownCast(GetOutput());}

  /** return the output */
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeMesh");};

	/** used to initialize and create the material attribute if not yet present */
	virtual int InternalInitialize();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

protected:
  mafVMEMesh();
  virtual ~mafVMEMesh();

private:
  mafVMEMesh(const mafVMEMesh&); // Not implemented
  void operator=(const mafVMEMesh&); // Not implemented
  
};
#endif
