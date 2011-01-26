/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeVectorField.h,v $ 
  Language: C++ 
  Date: $Date: 2011-01-26 14:23:02 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeVectorField_h__
#define medPipeVectorField_h__

#include "mafPipe.h"

class mafGUI;


/** General class for vector fields, contains useful stuff  */
class medPipeVectorField : public mafPipe
{
public:
  mafAbstractTypeMacro(medPipeVectorField, mafPipe);

protected:
  bool m_BCreateVTKPipeAlways;      ///<true, if the VTK pipe should be created always, no matter, if VME contains vector
  int m_VectorFieldIndex;           ///<index of vector field to be processed
  int m_ScalarFieldIndex;           ///<index of scalar field to be used (e.g. for colouring)

public:	
  medPipeVectorField();  

public:
  /** Creates the VTK rendering pipeline for vector fields. 
  Calls ComputeDefaultParameters, CreateVTKPipe and UpdateVTKPipe
  in this order. CreateVTKPipe and UpdateVTKPipe is called only,
  if there is any vector field in the input VME. */
  /*virtual*/ void Create(mafSceneNode *n);  

  /** Gets the current status of m_bCreateVTKPipeAlways. */
  inline bool GetCreateVTKPipeAlways() {
    return m_BCreateVTKPipeAlways;
  }

  /** Sets a new status of m_bCreateVTKPipeAlways.
  N.B. it has only effect when called before Create. */
  inline void SetCreateVTKPipeAlways(bool bCreate = true) {
    m_BCreateVTKPipeAlways = bCreate;
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

  /** Returns name of vector field at the the specified index.  
  Returns NULL, if it fails to find the appropriate field.*/
  inline const char* GetVectorFieldName(int nIndex) {
    return GetFieldName(nIndex, true);
  }

  /** Returns name of scalar field at the the specified index.  
  Returns NULL, if it fails to find the appropriate field.*/
  inline const char* GetScalarFieldName(int nIndex) {
    return GetFieldName(nIndex, false);    
  }

  /** 
  Returns the name of field (scalar or vectors depending on
  bVectors parameter) at the specified index. 
  The routine returns NULL, if it cannot find appropriate field. */
  virtual const char* GetFieldName(int nIndex, bool bVectors = true);

  /** Returns index of vector field with the specified name.  
  Returns -1, if it fails to find the appropriate field.*/
  inline int GetVectorFieldIndex(const char* szName = NULL) {
    return GetFieldIndex(NULL, true);
  }

  /** Returns index of vector field with the specified name.  
  Returns -1, if it fails to find the appropriate field.*/
  inline int GetScalarFieldIndex(const char* szName = NULL) {
    return GetFieldIndex(NULL, false);    
  }

  /** 
  Returns the index of specified field (scalar or vectors depending on
  bVectors parameter). If it cannot be found, the index of currently active
  (scalar or vector) field is returned.
  The routine returns -1, if it cannot find appropriate field. */
  virtual int GetFieldIndex(const char* szName = NULL, bool bVectors = true);
  
  /** Returns the number of available vectors. */
  inline int GetNumberOfVectors() {
    return GetNumberOfFields(true);
  }

  /** Returns the number of available scalars. */
  inline int GetNumberOfScalars() {
    return GetNumberOfFields(false);
  }

  /** Returns the number of available scalars/vectors. */
  virtual int GetNumberOfFields(bool bVectors = true);  

  /** Populates the combo box by names of scalar/vector fields */
  virtual void PopulateCombo(wxComboBox* combo, bool bVectors);
};
#endif // medPipeVectorField_h__