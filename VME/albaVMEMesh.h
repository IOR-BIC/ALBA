/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMesh
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVMEMesh_h
#define __albaVMEMesh_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEGeneric.h"
#include "albaVMEOutputMesh.h"
#include "vtkIntArray.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkUnstructuredGrid;
class mmaMaterial;

/** albaVMEMesh - A vme that can accept vtkUnstructuredGrid 
dataset only. it's used to store finite element analysis
data*/
class ALBA_EXPORT albaVMEMesh : public albaVMEGeneric
{
public:
  albaTypeMacro(albaVMEMesh,albaVMEGeneric);

  /** Set data for the give timestamp. 
  This function automatically creates a VMEItem for the data to be stored. 
  The optional parameter specify if data must be copied, referenced, or detached from original source. 
  Default is to copy data which creates a new data set copying the original one.
  Referencing means the data is kept connected to its original source, and
  detaching means disconnect is like referencing but also disconnect the data
  from the source.
  Return ALBA_OK if succeeded, ALBA_ERROR if they kind of data is not accepted by
  this type of VME. Accepted data is vtkUnstructuredGrid */
  virtual int SetData(vtkUnstructuredGrid *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);
  virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

  /** return the output casted to albaVMEOutputMesh*/
  albaVMEOutputMesh *GetUnstructuredGridOutput() {return albaVMEOutputMesh::SafeDownCast(GetOutput());}

  /** return the Node IDs Array*/
  vtkIntArray *GetNodesIDArray();
	/** return the Node IDs Array*/
	static vtkIntArray *GetNodesIDArray(vtkUnstructuredGrid *inputUGrid);
			
  /** return the Element IDs Array*/
  vtkIntArray *GetElementsIDArray();
	/** return the Element IDs Array*/
	static vtkIntArray *GetElementsIDArray(vtkUnstructuredGrid *inputUGrid);

	/** return the Element Types Array*/
	vtkIntArray *GetElementsTypeArray();
	/** return the Element Types Array*/
	static vtkIntArray *GetElementsTypeArray(vtkUnstructuredGrid *inputUGrid);

	/** return the Element Reals Array*/
	vtkIntArray *GetElementsRealArray();
	/** return the Element Reals Array*/
	static vtkIntArray *GetElementsRealArray(vtkUnstructuredGrid *inputUGrid);

	
  /** return the output */
  virtual albaVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeMesh");};

	/** used to initialize and create the material attribute if not yet present */
	virtual int InternalInitialize();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

protected:
  albaVMEMesh();
  virtual ~albaVMEMesh();

	/** Return the array with the name given. Returns NULL is array not found */
	vtkIntArray *GetIntCellArray(const char *arrayName, const char *arrayName2);

	/** Return the array with the name given. Returns NULL is array not found */
	static vtkIntArray *GetIntCellArray(vtkUnstructuredGrid *inputUGrid, const char *arrayName, const char *arrayName2);

private:
  albaVMEMesh(const albaVMEMesh&); // Not implemented
  void operator=(const albaVMEMesh&); // Not implemented
  
};
#endif
