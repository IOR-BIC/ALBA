/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalarMatrix.h,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:22:57 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEScalarMatrix_h
#define __mafVMEScalarMatrix_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGenericAbstract.h"
#include "vnl/vnl_matrix.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEOutputScalarMatrix;
class vtkDataSet;

/** mafVMEScalarMatrix */
class MAF_EXPORT mafVMEScalarMatrix : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMEScalarMatrix,mafVMEGenericAbstract);

  void OnEvent(mafEventBase *maf_event);

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
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. */
  virtual int SetData(vnl_matrix<double> &data, mafTimeStamp t);

  /** Set the time for this VME.
  It updates also the vtk representation for the scalar data.*/
  void SetTimeStamp(mafTimeStamp t);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another mafVMEScalarMatrix into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another mafVMEScalarMatrix. */
  virtual bool Equals(mafVME *vme);

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
  mafVMEOutputScalarMatrix *GetScalarOutput() {return (mafVMEOutputScalarMatrix *)GetOutput();}

  /** return the right type of output */  
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeScalarMatrix");};

protected:
  mafVMEScalarMatrix();
  virtual ~mafVMEScalarMatrix();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  void InternalPreUpdate();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

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
  mafVMEScalarMatrix(const mafVMEScalarMatrix&); // Not implemented
  void operator=(const mafVMEScalarMatrix&); // Not implemented
};
#endif
