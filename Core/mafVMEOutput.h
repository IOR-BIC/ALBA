/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutput.h,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:37:02 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEOutput_h
#define __mafVMEOutput_h

#include "mafObject.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafMatrix;
class mafTransform;
class mafOBB;
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
    update for checking about data type and thus without actually loading the data
    from file. */
  virtual const char *GetDataType()=0;

  
  /**
    Update all the output data structures (data, matrix and abs matrix.*/
  virtual void Update();
    
  /**
    Return the TimeBounds of the whole subtree, i.e recurse the GetLocalTimeBounds()
    function over all the VME in the subtree.*/
  void GetTimeBounds(mafTimeStamp tbounds[2]);
  
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
  void GetSpaceBounds(double bounds[6]);
  virtual void GetSpaceBounds(mafOBB &bounds,mafTimeStamp t=-1, mafNodeIterator *iter=NULL);
  
  /**
    Return the space bound of the VME (not the subtree) for the current time, taking in consideration 
    the current data bounds and the current pose matrix*/
  void GetVMESpaceBounds(double bounds[6]);
  virtual void GetVMESpaceBounds(mafOBB &bounds,mafTimeStamp t=-1, mafNodeIterator *iter=NULL);
  
  /**
    Return the local space bound (in local coordinates) of the VME (not the subtree) for the current time,
    taking in consideration the current data bounds and the current pose matrix*/
  void GetVMELocalSpaceBounds(double bounds[6]);
  virtual void GetVMELocalSpaceBounds(mafOBB &bounds,mafTimeStamp t=-1, mafNodeIterator *iter=NULL);
    
protected:
  mafVMEOutput(); // to be allocated with New()
  virtual ~mafVMEOutput(); // to be deleted with Delete()

  /**
   To be use to override the current data pointer. By default CurrentData
   stores a pointer to the DataPipe output. */
  //void SetCurrentData(vtkDataSet *data);

  mafVME                    *m_VME; ///< pointer to source VME
private:
  mafVMEOutput(const mafVMEOutput&); // Not implemented
  void operator=(const mafVMEOutput&); // Not implemented
};

#endif
