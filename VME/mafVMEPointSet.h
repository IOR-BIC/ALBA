/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEPointSet.h,v $
  Language:  C++
  Date:      $Date: 2005-07-15 15:21:43 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone, Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEPointSet_h
#define __mafVMEPointSet_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGeneric.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafVMEOutputPointSet;

/** mafVMEPointSet - this class stores a set of points as a VME
mafVMEPointSet is a concrete VME object storing a cloud of Points whose
relative position can change along time and whose global position is represented.
by VME's trajectory. This VME is implemented as a number VME-item storing a 
vtkPolyData composed of only Vertex cells. Notice that in this VME the number of
points can change for each time frame. For constant number of points along frames
see mafVMELandmarkCloud
@sa
mafVME mafVMELandmarkCloud

@todo
- Remove cells from internal points representation */ 
class MAF_EXPORT mafVMEPointSet : public mafVMEGeneric
{
public:
  //mafAbstractTypeMacro(mafVMEPointSet,mafVMEGeneric);
  mafTypeMacro(mafVMEPointSet,mafVMEGeneric);

  /** 
  Set data for the give timestamp. This function automatically creates a
  a VMEItem for the data to be stored. The optional parameter specify if
  data must be copied, referenced, or detached from original source. Default
  is to copy data which creates a new data set copying the original one.
  Referencing means the data is kept connected to its original source, and
  detaching means disconnect is like referencing but also disconnect the data
  from the source.
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. */
  virtual int SetData(vtkPolyData *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** return the right type of output */  
  virtual mafVMEOutput *GetOutput();

  /** return the right type of output */  
  mafVMEOutputPointSet *GetPointSetOutput() {return (mafVMEOutputPointSet *)GetOutput();}

  /**
  Add a new Point to the given time frame and returns its index. If not item was
  defined for that time frame create a new one.*/
  virtual int AppendPoint(double x,double y,double z, mafTimeStamp t=-1);

  /**
  Remove a Point from the cloud at the given time frame. If the frame or the point 
  does not exist return VTK_ERROR*/
  virtual int RemovePoint(int idx, mafTimeStamp t=-1);
  
  /**
  Set/Get a point in the given time frame. Both functions return VTK_ERROR in case the specified idx does 
  not correspond to an existing Point*/
  virtual int SetPoint(int idx,double x,double y,double z,mafTimeStamp t=-1);
  virtual int SetPoint(int idx,double xyz[3],mafTimeStamp t=-1) {return this->SetPoint(idx,xyz[0],xyz[1],xyz[2],t);}
  virtual int GetPoint(int idx, double &x,double &y,double &z,mafTimeStamp t=-1);
  virtual int GetPoint(int idx, double xyz[3],mafTimeStamp t=-1);
  
  /**
  Return the number of points stored in the given time frame of this VME*/
  virtual int GetNumberOfPoints(mafTimeStamp t=-1);

  /**
  Set the number of Points stored in the given time frame this object. Automatically
  create points in (0,0,0) if necessary, or remove points... */
  virtual void SetNumberOfPoints(int num,mafTimeStamp t=-1);

  /** return icon */
  static char** GetIcon();

protected:
  mafVMEPointSet();
  virtual ~mafVMEPointSet();

  /**
  return the polydata of the VME-item at the given time frame. Return NULL if 
  it does not exist*/
  vtkPolyData *GetPolyData(mafTimeStamp t);

  /**
  return idx-th point of given polydata*/
  int GetPoint(vtkPolyData *polydata,int idx,double xyz[3]);

  /**
  return the polydata of the VME-item at the given time frame. Create a new one at time t.*/
  virtual vtkPolyData *NewPolyData(mafTimeStamp t);

  /**
  Add a new Point to the given polydata and returns its index*/
  virtual int AppendPoint(vtkPolyData *polydata,double x,double y,double z,int num=1);

  /**
  Remove a Point from the give polydata.*/
  virtual int RemovePoint(vtkPolyData *polydata,int idx);

  /**
  Set a Point position in the give polydata.*/
  int SetPoint(vtkPolyData *polydata,int idx, double x,double y,double z);

private:
  mafVMEPointSet(const mafVMEPointSet&); // Not implemented
  void operator=(const mafVMEPointSet&); // Not implemented

  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};
#endif