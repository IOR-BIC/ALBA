/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutput
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutput_h
#define __albaVMEOutput_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObject.h"
#include "albaSmartPointer.h"
#include "albaString.h"
#include "albaOBB.h"
#include "albaDecl.h" // for MINID
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class albaMatrix;
#ifdef ALBA_EXPORTS
#include "albaTransformBase.h"
#else
class albaTransformBase;
#endif
class albaOBB;
class albaVMEIterator;
class albaGUI;
#ifdef ALBA_USE_VTK
class vtkDataSet;
#endif //ALBA_USE_VTK

#ifdef ALBA_EXPORTS
template class ALBA_EXPORT albaAutoPointer<albaTransformBase>;
#endif

/** albaVMEOutput - the output data structure of a VME node.
  albaVMEOutput is the output produced by a VME node.

  @todo
  - add a GetITKMesh
  - add a GetITKImage
  - try to remove the restore of old data_pipe timestamp inside GetDataBounds()
  */
class ALBA_EXPORT albaVMEOutput : public albaObject
{
public:
  albaVMEOutput();
  virtual ~albaVMEOutput();

  albaAbstractTypeMacro(albaVMEOutput,albaObject);

  /** return the VME connected to this object */
  albaVME *GetVME() const {return m_VME;}

  /** internally used to set the VME connected to this object */
  void SetVME(albaVME *vme) {m_VME=vme;} 

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** Return the VME pose, this function queries the MatrixPipe for producing a matrix */
  void GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,albaTimeStamp t=-1) const;
  /** Return the VME pose */
  void GetPose(double xyz[3],double rxyz[3],albaTimeStamp t=-1) const;
  /** Return the VME pose */
  virtual void GetMatrix(albaMatrix &matrix,albaTimeStamp t=-1) const;
  /** Return the VME pose matrix for the current time */
  albaMatrix *GetMatrix() const;
  /** Return the transform generating the pose matrix of the VME */ 
  virtual albaTransformBase *GetTransform() const;
  
  /** 
    Used by source VME to set internal pointer to Transform: do not use this method directly,
    use instead albaVME::SetMatrix() */
  virtual void SetTransform(albaTransformBase *trans);

  /** Get the global pose matrix of this VME for the given time "t".*/
  void GetAbsMatrix(albaMatrix &matrix,albaTimeStamp t=-1) const;
  /** Get the global pose matrix of this VME for the given time "t".*/
  albaMatrix *GetAbsMatrix() const;
  /** return the transform representing the Abs pose (typically the VME's AbsMatrix pipe) */
  virtual albaTransformBase * GetAbsTransform() const;
  
  /** Get the global pose of this VME for the given time "t".*/
  void GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,albaTimeStamp t=-1) const;
  /** Get the global pose of this VME for the given time "t".*/
  void GetAbsPose(double xyz[3],double rxyz[3],albaTimeStamp t=-1) const;
  
#ifdef ALBA_USE_VTK
  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkDataSet *GetVTKData();
#endif

  /**
    Return the DataType to be produced as output. This can be used to avoid calling
    update for checking about data type directly in the output dataset and thus
    without actually loading the data from file. */
  virtual const char *GetDataType() const {return m_DataType;}

  /**
    Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();
    
  /**
    Return the TimeBounds of the whole subtree, i.e recurse the GetLocalTimeBounds()
    function over all the VME in the subtree.*/
  void GetTimeBounds(albaTimeStamp tbounds[2]) const;

  /**
    Get TimeBounds for this VME. TimeBounds interval is defined by the minimum
    and maximum time stamps against the MatrixVector and VMEItems time stamps.
    If only the pose time stamps are required use the albaMatrixVector::GetTimeBounds()
    function. For the time bounds of the VME items only use the albaVMEGenericAbstract::GetItemsTimesList()*/
  void GetLocalTimeBounds(albaTimeStamp tbounds[2]) const;
  
  /**
    Extract the 4D bounds for this VME only, i.e. the space bounds along all the time range
    for this VME. */
  void GetVME4DBounds(double bounds[6]) const;
  void GetVME4DBounds(albaOBB &bounds) const;

  /**
    Extract the 4D bounds for all the subtree starting a this VME, i.e. the space bounds 
    along all the time for the VME in the subtree.*/
  void Get4DBounds(double bounds[6]) const;
  void Get4DBounds(albaOBB &bounds) const;
  
  /**
    Return the space bound of the subtree for the current time, taking in consideration
    the current data bounds and the current pose matrix*/
  void GetBounds(double bounds[6]) const;
  virtual void GetBounds(albaOBB &bounds,albaTimeStamp t=-1, albaVMEIterator *iter=NULL) const;
  
  /**
    Return the space bound of the VME (not the subtree) for the current time, taking in consideration 
    the current data bounds and the current pose matrix.*/
  void GetVMEBounds(double bounds[6]) const;
  virtual void GetVMEBounds(albaOBB &bounds,albaTimeStamp t=-1, albaVMEIterator *iter=NULL) const;
  
  /**
    Return the local space bound (in local coordinates) of the VME (not the whole subtree) for the 
    current time, taking in consideration the current data bounds and the current pose matrix.*/
  void GetVMELocalBounds(double bounds[6]) const;
  virtual void GetVMELocalBounds(albaOBB &bounds,albaTimeStamp t=-1, albaVMEIterator *iter=NULL) const;

  /** return the time for which this output was computed */
  albaTimeStamp GetTimeStamp() const;
  
  /** 
    Used by source VME to set internal bounds structure: never use this method directly. */
  void SetBounds(const albaOBB &bounds);

  /** IDs for the GUI */
  enum VMEOUTPUT_WIDGET_ID
  {
    ID_FIRST = MINID,
    ID_LAST
  };

  /** create and return the GUI for changing the node parameters */
  albaGUI *GetGui();

  /** destroy the Gui */
  void DeleteGui();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual albaGUI  *CreateGui();

  /** retrieve bounds of the output data not considering the VME pose matrix and the visibility. */
  virtual void GetDataBounds(albaOBB &bounds,albaTimeStamp t) const;

  albaVME *                  m_VME;      ///< pointer to source VME
  albaString                 m_DataType; ///< the type of data stored in object expressed as a string
  albaOBB                    m_Bounds;   ///< bounds of the output data (i.e. for current time)
  albaGUI                   *m_Gui;      ///< user interface

  albaAutoPointer<albaTransformBase> m_Transform; ///< the transform generating the output pose matrix

private:
  albaVMEOutput(const albaVMEOutput&); // Not implemented
  void operator=(const albaVMEOutput&); // Not implemented
};

#endif
