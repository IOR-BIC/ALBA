/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutput.h,v $
  Language:  C++
  Date:      $Date: 2005-03-11 10:07:45 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEOutput_h
#define __mafVMEOutput_h

#include "mafObject.h"
#include "mafString.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafMatrix;
class mafTransform;
class mafOBB;
class mafVME;
class mafNodeIterator;
class vtkDataSet;

/** mafVMEOutput - the output data structure of a VME node.
  mafVMEOutput is the output produced by a VME node.

  @todo
  -
  */
class MAF_EXPORT mafVMEOutput : public mafObject
{
public:
  mafAbstractTypeMacro(mafVMEOutput,mafObject);

  mafVME *GetVME();

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs);

  /** Return the VME pose, this function queries the MatrixPipe for producing a matrix */
  void GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t=-1);
  /** Return the VME pose */
  void GetPose(double xyz[3],double rxyz[3],mafTimeStamp t=-1);
  /** Return the VME pose */
  virtual void GetMatrix(mafMatrix *matrix,mafTimeStamp t=-1);
  /** Return the VME pose matrix for the current time */
  virtual mafMatrix *GetMatrix();
  
  /** Get the global pose matrix of this VME for the given time "t".*/
  void GetAbsMatrix(mafMatrix *matrix,mafTimeStamp t=-1);
  /** Get the global pose matrix of this VME for the given time "t".*/
  mafMatrix *GetAbsMatrix();
  
  /** Get the global pose of this VME for the given time "t".*/
  void GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t=-1);
  /** Get the global pose of this VME for the given time "t".*/
  void GetAbsPose(double xyz[3],double rxyz[3],mafTimeStamp t=-1);
  
  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkDataSet *GetVTKData()=0;

  /**
    Return the DataType to be produced as output. This can be used to avoid calling
    update for checking about data type directly in the output dataset and thus
    without actually loading the data from file. */
  virtual const char *GetDataType() {return m_DataType;}

  /**
    Update all the output data structures (data, matrix and abs matrix).*/
  virtual void Update();
    
  /**
    Return the TimeBounds of the whole subtree, i.e recurse the GetLocalTimeBounds()
    function over all the VME in the subtree.*/
  void GetTimeBounds(mafTimeStamp tbounds[2]);

  /**
    Get TimeBounds for this VME. TimeBounds interval is defined by the minimum
    and maximum time stamps against the MatrixVector and VMEItems time stamps.
    If only the pose time stamps are required use the mafMatrixVector::GetTimeBounds()
    function. For the time bounds of the VME items only use the mafVMEGeneric::GetItemsTimesList()*/
  void GetLocalTimeBounds(mafTimeStamp tbounds[2]);
  
  /**
    Extract the 4D bounds for this VME only, i.e. the space bounds along all the time range
    for this VME. The first of these two functions allows to specify the time
    interval for which the time bound is required*/
  void GetLocal4DBounds(mafTimeStamp start, mafTimeStamp end, double bounds[6]);
  void GetVME4DBounds(double bounds[6]);
  void GetVME4DBounds(mafOBB &bounds);

  /**
    Extract the 4D bounds for all the subtree starting a this VME, i.e. the space bounds 
    along all the time for the VME in the subtree. The first of these two functions allows
    to specify the time interval for which the time bound is required*/
  void Get4DBounds(mafTimeStamp start, mafTimeStamp end, double bounds[6]);
  void Get4DBounds(double bounds[6]);
  void Get4DBounds(mafOBB &bounds);
  
  /**
    Return the space bound of the subtree for the current time, taking in consideration
    the current data bounds and the current pose matrix*/
  void GetBounds(double bounds[6]);
  virtual void GetBounds(mafOBB &bounds,mafTimeStamp t=-1, mafNodeIterator *iter=NULL);
  
  /**
    Return the space bound of the VME (not the subtree) for the current time, taking in consideration 
    the current data bounds and the current pose matrix*/
  void GetVMEBounds(double bounds[6]);
  virtual void GetVMEBounds(mafOBB &bounds,mafTimeStamp t=-1, mafNodeIterator *iter=NULL);
  
  /**
    Return the local space bound (in local coordinates) of the VME (not the subtree) for the current time,
    taking in consideration the current data bounds and the current pose matrix*/
  void GetVMELocalBounds(double bounds[6]);
  virtual void GetVMELocalBounds(mafOBB &bounds,mafTimeStamp t=-1, mafNodeIterator *iter=NULL);

  /** return the time for which this output was computed */
  mafTimeStamp GetCurrentTime();
    
protected:
  mafVMEOutput(); // to be allocated with New()
  virtual ~mafVMEOutput(); // to be deleted with Delete()

  /** internally used to compute bounds for non procedural data */
  virtual void GetDataBounds(mafOBB &bounds,mafTimeStamp t)=0; 

  mafVME                    *m_VME;     ///< pointer to source VME
  mafString                 m_DataType; ///< the type of data stored in object expressed as a string
  mafAutoPointer<mafMatrix> m_Matrix;   ///< the output pose matrix

private:
  mafVMEOutput(const mafVMEOutput&); // Not implemented
  void operator=(const mafVMEOutput&); // Not implemented
};

#endif
