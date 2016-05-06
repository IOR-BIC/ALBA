/*=========================================================================

 Program: MAF2
 Module: mafVMEOutput
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEOutput_h
#define __mafVMEOutput_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "mafSmartPointer.h"
#include "mafString.h"
#include "mafOBB.h"
#include "mafDecl.h" // for MINID
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafMatrix;
#ifdef MAF_EXPORTS
#include "mafTransformBase.h"
#else
class mafTransformBase;
#endif
class mafOBB;
class mafVMEIterator;
class mafGUI;
#ifdef MAF_USE_VTK
class vtkDataSet;
#endif //MAF_USE_VTK

#ifdef MAF_EXPORTS
template class MAF_EXPORT mafAutoPointer<mafTransformBase>;
#endif

/** mafVMEOutput - the output data structure of a VME node.
  mafVMEOutput is the output produced by a VME node.

  @todo
  - add a GetITKMesh
  - add a GetITKImage
  - try to remove the restore of old data_pipe timestamp inside GetDataBounds()
  */
class MAF_EXPORT mafVMEOutput : public mafObject
{
public:
  mafVMEOutput();
  virtual ~mafVMEOutput();

  mafAbstractTypeMacro(mafVMEOutput,mafObject);

  /** return the VME connected to this object */
  mafVME *GetVME() const {return m_VME;}

  /** internally used to set the VME connected to this object */
  void SetVME(mafVME *vme) {m_VME=vme;} 

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** Return the VME pose, this function queries the MatrixPipe for producing a matrix */
  void GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t=-1) const;
  /** Return the VME pose */
  void GetPose(double xyz[3],double rxyz[3],mafTimeStamp t=-1) const;
  /** Return the VME pose */
  virtual void GetMatrix(mafMatrix &matrix,mafTimeStamp t=-1) const;
  /** Return the VME pose matrix for the current time */
  mafMatrix *GetMatrix() const;
  /** Return the transform generating the pose matrix of the VME */ 
  virtual mafTransformBase *GetTransform() const;
  
  /** 
    Used by source VME to set internal pointer to Transform: do not use this method directly,
    use instead mafVME::SetMatrix() */
  virtual void SetTransform(mafTransformBase *trans);

  /** Get the global pose matrix of this VME for the given time "t".*/
  void GetAbsMatrix(mafMatrix &matrix,mafTimeStamp t=-1) const;
  /** Get the global pose matrix of this VME for the given time "t".*/
  mafMatrix *GetAbsMatrix() const;
  /** return the transform representing the Abs pose (typically the VME's AbsMatrix pipe) */
  virtual mafTransformBase * GetAbsTransform() const;
  
  /** Get the global pose of this VME for the given time "t".*/
  void GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t=-1) const;
  /** Get the global pose of this VME for the given time "t".*/
  void GetAbsPose(double xyz[3],double rxyz[3],mafTimeStamp t=-1) const;
  
#ifdef MAF_USE_VTK
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
  void GetTimeBounds(mafTimeStamp tbounds[2]) const;

  /**
    Get TimeBounds for this VME. TimeBounds interval is defined by the minimum
    and maximum time stamps against the MatrixVector and VMEItems time stamps.
    If only the pose time stamps are required use the mafMatrixVector::GetTimeBounds()
    function. For the time bounds of the VME items only use the mafVMEGenericAbstract::GetItemsTimesList()*/
  void GetLocalTimeBounds(mafTimeStamp tbounds[2]) const;
  
  /**
    Extract the 4D bounds for this VME only, i.e. the space bounds along all the time range
    for this VME. */
  void GetVME4DBounds(double bounds[6]) const;
  void GetVME4DBounds(mafOBB &bounds) const;

  /**
    Extract the 4D bounds for all the subtree starting a this VME, i.e. the space bounds 
    along all the time for the VME in the subtree.*/
  void Get4DBounds(double bounds[6]) const;
  void Get4DBounds(mafOBB &bounds) const;
  
  /**
    Return the space bound of the subtree for the current time, taking in consideration
    the current data bounds and the current pose matrix*/
  void GetBounds(double bounds[6]) const;
  virtual void GetBounds(mafOBB &bounds,mafTimeStamp t=-1, mafVMEIterator *iter=NULL) const;
  
  /**
    Return the space bound of the VME (not the subtree) for the current time, taking in consideration 
    the current data bounds and the current pose matrix.*/
  void GetVMEBounds(double bounds[6]) const;
  virtual void GetVMEBounds(mafOBB &bounds,mafTimeStamp t=-1, mafVMEIterator *iter=NULL) const;
  
  /**
    Return the local space bound (in local coordinates) of the VME (not the whole subtree) for the 
    current time, taking in consideration the current data bounds and the current pose matrix.*/
  void GetVMELocalBounds(double bounds[6]) const;
  virtual void GetVMELocalBounds(mafOBB &bounds,mafTimeStamp t=-1, mafVMEIterator *iter=NULL) const;

  /** return the time for which this output was computed */
  mafTimeStamp GetTimeStamp() const;
  
  /** 
    Used by source VME to set internal bounds structure: never use this method directly. */
  void SetBounds(const mafOBB &bounds);

  /** IDs for the GUI */
  enum VMEOUTPUT_WIDGET_ID
  {
    ID_FIRST = MINID,
    ID_LAST
  };

  /** create and return the GUI for changing the node parameters */
  mafGUI *GetGui();

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
  virtual mafGUI  *CreateGui();

  /** retrieve bounds of the output data not considering the VME pose matrix and the visibility. */
  virtual void GetDataBounds(mafOBB &bounds,mafTimeStamp t) const;

  mafVME *                  m_VME;      ///< pointer to source VME
  mafString                 m_DataType; ///< the type of data stored in object expressed as a string
  mafOBB                    m_Bounds;   ///< bounds of the output data (i.e. for current time)
  mafGUI                   *m_Gui;      ///< user interface

  mafAutoPointer<mafTransformBase> m_Transform; ///< the transform generating the output pose matrix

private:
  mafVMEOutput(const mafVMEOutput&); // Not implemented
  void operator=(const mafVMEOutput&); // Not implemented
};

#endif
