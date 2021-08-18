/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEScalarMatrix
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEScalarMatrix_h
#define __albaVMEScalarMatrix_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEGenericAbstract.h"
#include "vnl/vnl_matrix.h"

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_VNL_MATRIX(ALBA_EXPORT, double);
#endif

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMEOutputScalarMatrix;
class vtkDataSet;

/** albaVMEScalarMatrix */
class ALBA_EXPORT albaVMEScalarMatrix : public albaVMEGenericAbstract
{
public:
  albaTypeMacro(albaVMEScalarMatrix,albaVMEGenericAbstract);

  void OnEvent(albaEventBase *alba_event);

  enum SCALAR_ARRAY_ORIENTATION
  {
    ROWS = 0,
    COLUMNS
  };
  
  enum SCALAR_ID_TYPE
  {
    USE_TIME = 0,
    USE_PROGRESS_NUMBER,
    USE_SCALAR
  };

  enum SCALAR_WIDGET_ID
  {
    ID_SCALAR_FOR_X = Superclass::ID_LAST,
    ID_TYPE_FOR_X,
    ID_SCALAR_FOR_Y,
    ID_TYPE_FOR_Y,
    ID_SCALAR_FOR_Z,
    ID_TYPE_FOR_Z,
    ID_ACTIVE_SCALAR,
    ID_LAST
  };

  /** Set the data for the given timestamp. 
  This function automatically creates a VMEItem for the data to be stored.
  Return ALBA_OK if succeeded, ALBA_ERROR if they kind of data is not accepted by
  this type of VME. */
  virtual int SetData(vnl_matrix<double> &data, albaTimeStamp t);

  /** Set the time for this VME.
  It updates also the vtk representation for the scalar data.*/
  void SetTimeStamp(albaTimeStamp t);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another albaVMEScalarMatrix into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another albaVMEScalarMatrix. */
  virtual bool Equals(albaVME *vme);

  /** Assign an active scalar to the geometry; by default (-1) no scalar is associated to the geometry.*/
  void SetActiveScalarOnGeometry(int scalar = -1);

  /** Return the active scalar id assigned to the geometrical representation of the scalar data.*/
  int GetActiveScalarOnGeometry();

  /** Set/Get the scalar array orientation inside the scalar matrix: ROWS or COLUMNS.*/
  void SetScalarArrayOrientation(int o = ROWS);

  /** Set/Get the scalar array orientation inside the scalar matrix: ROWS or COLUMNS.*/
  int GetScalarArrayOrientation() {return m_ScalarArrayOrientationInMatrix;};

  /** Set/Get which scalar ID has to be used as X coordinate for the VTK Representation.*/
  void SetScalarIdForXCoordinate(int id);

  /** Set/Get which scalar ID has to be used as X coordinate for the VTK Representation.*/
  int GetScalarIdForXCoordinate();

  /** Set/Get the type of elements that will be put inside X coordinate: timestamp, progrs numbers or a scalar component.*/
  void SetTypeForXCoordinates(int t);

  /** Set/Get the type of elements that will be put inside X coordinate: timestamp, progrs numbers or a scalar component.*/
  int GetTypeForXCoordinates() {return m_Xtype;};
  
  /** Set/Get which scalar ID has to be used as Y coordinate for the VTK Representation.*/
  void SetScalarIdForYCoordinate(int id);
  
  /** Set/Get which scalar ID has to be used as Y coordinate for the VTK Representation.*/
  int GetScalarIdForYCoordinate();

  /** Set/Get the type of elements that will be put inside Y coordinate: timestamp, progrs numbers or a scalar component.*/
  void SetTypeForYCoordinates(int t);

  /** Set/Get the type of elements that will be put inside Y coordinate: timestamp, progrs numbers or a scalar component.*/
  int GetTypeForYCoordinates() {return m_Ytype;};

  /** Set/Get which scalar ID has to be used as Z coordinate for the VTK Representation.*/
  void SetScalarIdForZCoordinate(int id);

  /** Set/Get which scalar ID has to be used as Z coordinate for the VTK Representation.*/
  int GetScalarIdForZCoordinate();

  /** Set/Get the type of elements that will be put inside Z coordinate: timestamp, progrs numbers or a scalar component.*/
  void SetTypeForZCoordinates(int t);

  /** Set/Get the type of elements that will be put inside Z coordinate: timestamp, progrs numbers or a scalar component.*/
  int GetTypeForZCoordinates() {return m_Ztype;};

  /** return the right type of output.*/  
  albaVMEOutputScalarMatrix *GetScalarOutput() {return (albaVMEOutputScalarMatrix *)GetOutput();}

  /** return the right type of output */  
  virtual albaVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeScalarMatrix");};

protected:
  albaVMEScalarMatrix();
  virtual ~albaVMEScalarMatrix();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  void InternalPreUpdate();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  /** Enable/Disable widgets according to the corresponding selected type.*/
  void EnableWidgets();

  int m_XID; ///< Scalar id or timestamp (if -1) associated with the X coordinate of the VTK representation;
  int m_YID; ///< Scalar id or timestamp (if -1) associated with the Y coordinate of the VTK representation;
  int m_ZID; ///< Scalar id or timestamp (if -1) associated with the Z coordinate of the VTK representation;

  int m_Xtype;
  int m_Ytype;
  int m_Ztype;

  int m_ActiveScalar; ///< Active scalar associated to the geometry.
  int m_ScalarArrayOrientationInMatrix; ///< Say if the scalars are stored in rws or in columns.

private:
  albaVMEScalarMatrix(const albaVMEScalarMatrix&); // Not implemented
  void operator=(const albaVMEScalarMatrix&); // Not implemented
};
#endif
