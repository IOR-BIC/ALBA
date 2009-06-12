/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeTensorField.h,v $ 
  Language: C++ 
  Date: $Date: 2009-06-12 16:34:48 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeTensorField_h__
#define medPipeTensorField_h__

#include "mafPipe.h"

class mafGUI;


/** General class for tensor fields, contains useful stuff  */
class medPipeTensorField : public mafPipe
{
public:
  mafAbstractTypeMacro(medPipeTensorField, mafPipe);

protected:
  bool m_bCreateVTKPipeAlways;      ///<true, if the VTK pipe should be created always, no matter, if VME contains tensor
  int m_TensorFieldIndex;           ///<index of tensor field to be processed
  int m_ScalarFieldIndex;           ///<index of scalar field to be used (e.g. for colouring)

public:	
  medPipeTensorField();  

public:
  /** Creates the VTK rendering pipeline for tensor fields. 
  Calls ComputeDefaultParameters, CreateVTKPipe and UpdateVTKPipe
  in this order. CreateVTKPipe and UpdateVTKPipe is called only,
  if there is any tensor field in the input VME. */
  /*virtual*/ void Create(mafSceneNode *n);  

  /** Gets the current status of m_bCreateVTKPipeAlways. */
  inline bool GetCreateVTKPipeAlways() {
    return m_bCreateVTKPipeAlways;
  }

  /** Sets a new status of m_bCreateVTKPipeAlways.
  N.B. it has only effect when called before Create. */
  inline void SetCreateVTKPipeAlways(bool bCreate = true) {
    m_bCreateVTKPipeAlways = bCreate;
  }

protected:  
  /** Default radius, etc. should be calculated in this method, 
  i.e., inherited classes should always override this method. 
  The default implementation is to update VME*/
  virtual void ComputeDefaultParameters();

  /** Constructs the VTK pipeline with default options. */
  virtual void CreateVTKPipe() = 0;

  /** Updates VTK pipeline, when something changes (e.g. radius). */
  virtual void UpdateVTKPipe() = 0;

  /** Returns name of tensor field at the the specified index.  
  Returns NULL, if it fails to find the appropriate field.*/
  inline const char* GetTensorFieldName(int nIndex) {
    return GetFieldName(nIndex, true);
  }

  /** Returns name of scalar field at the the specified index.  
  Returns NULL, if it fails to find the appropriate field.*/
  inline const char* GetScalarFieldName(int nIndex) {
    return GetFieldName(nIndex, false);    
  }

  /** 
  Returns the name of field (scalar or tensors depending on
  bTensors parameter) at the specified index. 
  The routine returns NULL, if it cannot find appropriate field. */
  const char* GetFieldName(int nIndex, bool bTensors = true);

  /** Returns index of tensor field with the specified name.  
  Returns -1, if it fails to find the appropriate field.*/
  inline int GetTensorFieldIndex(const char* szName = NULL) {
    return GetFieldIndex(NULL, true);
  }

  /** Returns index of tensor field with the specified name.  
  Returns -1, if it fails to find the appropriate field.*/
  inline int GetScalarFieldIndex(const char* szName = NULL) {
    return GetFieldIndex(NULL, false);    
  }

  /** 
  Returns the index of specified field (scalar or tensors depending on
  bTensors parameter). If it cannot be found, the index of currently active
  (scalar or tensor) field is returned.
  The routine returns -1, if it cannot find appropriate field. */
  int GetFieldIndex(const char* szName = NULL, bool bTensors = true);
  
  /** Returns the number of available tensors. */
  inline int GetNumberOfTensors() {
    return GetNumberOfFields(true);
  }

  /** Returns the number of available scalars. */
  inline int GetNumberOfScalars() {
    return GetNumberOfFields(false);
  }

  /** Returns the number of available scalars/tensors. */
  int GetNumberOfFields(bool bTensors = true);  

  /** Populates the combo box by names of scalar/tensor fields */
  void PopulateCombo(wxComboBox* combo, bool bTensors);
};
#endif // medPipeTensorField_h__