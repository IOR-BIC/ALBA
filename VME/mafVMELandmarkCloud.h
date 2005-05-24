/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMELandmarkCloud.h,v $
  Language:  C++
  Date:      $Date: 2005-05-24 14:37:32 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone, Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMELandmarkCloud_h
#define __mafVMELandmarkCloud_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEPointSet.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMELandmark;
class mmaMaterial;

/** mafVMELandmarkCloud - this class represents a cloud of landmarks
mafVMELandmarkRigidCloud is a concrete VME object storing a cloud of landmarks whose
relative position can change along time. This VME is a specialization of mafVMEPointSet
with all time frames having the same number of points (thus each point exist in all 
time frames) and a table of names assigned to each point. This table are
stored into the VME TagArray with tag names "LM_NAME". This class also supports time-varying
visibility for landmarks: for any timestamp can be defined a visibility. Notice that for each
time stamp for which is defined a position is also defined a default visibility (see SetDefaultVisibility)
and vice versa for each time stamp for which a visibility is defined also a position is automatically set
eventually copying the position returned by interpolator if not. This is due to internal data 
representation, and defines that time base for positions and visibility attributes are the same.
Last but not least, this class has an explosion/collapsing mecchanism allowing to extract all landmarks
from the cloud and represent them as mafVMELandmarks children (see Open() and Close()). The cloud has an internal state
which determines whether all functions relative to edit and query landmarks acts on vtkPolydata points or
on external mafVMELandmarks children.
@sa
mflVME mafPointSet.*/
class MAF_EXPORT mafVMELandmarkCloud : public mafVMEPointSet
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events */
  /** @{ */
  MAF_ID_DEC(CLOUDE_OPEN_CLOSE);      ///< Event rised by Open and Close functions
  MAF_ID_DEC(CLOUDE_RADIUS_MODIFIED); ///< Event rised when the radius is changed with a SetRadius()
  MAF_ID_DEC(CLOUDE_SPHERE_RES);      ///< Event rised when the sphere resolution is changed with a SetSphereResolution()
  /** @} */

  mafTypeMacro(mafVMELandmarkCloud, mafVMEPointSet);
  
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *event);

  /**
  Add a new landmark an returns its index. BEWARE: landmark is added to all the
  VME-items, and its position should for each time frame by means SetPoint()
  The second syntax is used to add points in t=0, and is used in place of corresponding
  function of ex. RigidLandmarkClouds*/
  int AppendLandmark(const char *name);
  int AppendLandmark(double x,double y,double z,const char *name) {int idx=this->AppendLandmark(name); return this->SetLandmark(idx, x, y, z);}

  /**
  Set/Get a landmark. In case the specified idx is invalid return MAF_ERROR*/
  int SetLandmark(int idx,double x,double y,double z,mafTimeStamp t=0);
  int SetLandmark(const char *name,double x,double y,double z,mafTimeStamp t=0) {return this->SetLandmark(this->FindLandmarkIndex(name),x,y,z,t);}
  //int SetLandmarkForTimeFrame(int idx,double x,double y,double z,unsigned long tid,mafTimeStamp t);
  int GetLandmark(int idx, double &x,double &y,double &z,mafTimeStamp t=0);
  int GetLandmark(const char *name, double &x,double &y,double &z,mafTimeStamp t=0) {return this->GetLandmark(this->FindLandmarkIndex(name),x,y,z,t);} 
  int GetLandmark(int idx, double xyz[3],mafTimeStamp t=0);
  int GetLandmark(const char *name, double xyz[3],mafTimeStamp t=0) {return this->GetLandmark(this->FindLandmarkIndex(name), xyz,t);}

  /**
  Find the index of a landmark given its name. This only works when cloud is OPEN*/
  mafVMELandmark *GetLandmark(const char *name);

  /**
  Get the landmark by index. This only works when cloud is OPEN*/
  mafVMELandmark *GetLandmark(int idx);

  /**
  Find the index of a landmark given its name*/
  int FindLandmarkIndex(const char *name);

  /**
  Remove a landmark. In case the id is invalid return MAF_ERROR
  BEWARE: landmark is removed from all the VME-items*/
  int RemoveLandmark(int idx);

  /**
  Set/Get the number of landmarks stored in this VME. BEWARE the number of
  landmarks is the same for all the time frames (items)*/
  int GetNumberOfLandmarks();
  virtual int SetNumberOfLandmarks(int num);

  /**
  Set/get the name for idx-th landmark. Return null in case no landmark name 
  was defined for that index.*/
  virtual const char *GetLandmarkName(int idx);
  virtual void SetLandmarkName(int idx,const char *name);
  
  /**
  Set/Get the m_Radius of this landmark. The radius is stored as an TAG item: "LM_RADIUS".
  If no radius has been ever set, the default value is "1".*/
  virtual void SetRadius(double rad);
  virtual double GetRadius();

  /**
  Set/Get the resolution of the landmarks' spheres. The resolution is stored as an TAG item: "LM_SPHERE_RESOLUTION".
  If no resolution has been ever set, default value is "15".*/
  virtual void SetSphereResolution(int res);
  virtual int GetSphereResolution();

  /**
  Cloud states: Open/Close*/
  enum {
    UNSET_CLOUD=0,
    CLOSED_CLOUD,
    OPEN_CLOUD
  };

  enum {
    ID_LM_RADIUS = Superclass::ID_LAST,
    ID_LAST
  };

  /**
  Return true if the cloud is Open/Exploded, false otherwise.*/
  bool IsOpen() {return this->GetState() == OPEN_CLOUD;}

  /**
  This function opens the cloud by creating a number of of mafVMELandmark children
  corresponding to the landmarks in the cloud. The cloud changes its state from close to open,
  and some extra features are enabled. In particular the cloud keeps synchronized the children 
  landmarks and the points in the cloud. It is also possible to retrieve the children by
  landmark index or name. Notice that visibility is preserved while opening/closing.
  This function rises a "OpenCloseEvent" event.*/
  virtual void Open();

  /**
  This function closes the cloud, i.e. the children landmarks are collapsed in the cloud.
  This function rises a "OpenCloseEvent" event.*/
  virtual void Close();

  /**
  Set/Get the visibility property for a given landmark for a given 
  time stamp. The visibility is stored as a scalar attribute of the
  associated polydata's point. Notice that visibility is implicitly defined
  as 'm_DefaultVisibility' when a position is specified for a timestamp nd nothing is specified
  for the visibility at that time. On the other hand, if visibility is defined
  for some time a position is implicitelly defined for that time stamp.*/
  virtual int SetLandmarkVisibility(int idx,bool a,mafTimeStamp t=0);
  int SetLandmarkVisibility(const char *name,bool a,mafTimeStamp t=0) {return this->SetLandmarkVisibility(this->FindLandmarkIndex(name),a,t);}
  virtual bool GetLandmarkVisibility(int idx,mafTimeStamp t=0);
  bool GetLandmarkVisibility(const char *name,mafTimeStamp t=0) {return this->GetLandmarkVisibility(this->FindLandmarkIndex(name),t);}

  /**
  Return true if the landamrk is visible for the given time (CurrentTime is the default)*/
  bool IsVisible(int idx,mafTimeStamp t=-1) {return this->GetLandmarkVisibility(idx,t);}
  bool IsVisible(const char *name,mafTimeStamp t=-1) {return this->GetLandmarkVisibility(this->FindLandmarkIndex(name),t);}

  /**
  Set/Get the default visibility attribute. When default visibility is set to true, the creation 
  of a new time frame (by setting a landmark position for a given timestamp), makes all the other
  landmarks to be also visible at the same timestamp. If false, the visibility of the others landmark is
  automatically set to false for the other landmarks. Notice currently this is not a persistant attribute.*/
  void SetDefaultVisibility(int v) {this->m_DefaultVisibility = v; this->Modified();}
  int GetDefaultVisibility() {return m_DefaultVisibility;}
  void DefaultVisibilityOn() {SetDefaultVisibility(1);};
  void DefaultVisibilityOff() {SetDefaultVisibility(0);};

  /**
  Return number of visible landmarks*/
  int GetNumberOfVisibleLandmarks(mafTimeStamp t=0);
  
  /**
  Return true if it's a rigid landmark cloud (i.e. it has a single time frame)*/
  bool IsRigid();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  void Print(std::ostream &os, const int tabs);

protected:
  mafVMELandmarkCloud();
  virtual ~mafVMELandmarkCloud();

  /**
  Remove a landmark name from the TagArray*/
  void RemoveLandmarkName(int idx);

  /**
  Internal function to instatiate a VMEItem with for timestamp t containing a new polydata*/
  virtual vtkPolyData *NewPolyData(mafTimeStamp t);

  /**
  Internal functions redefined to support cells*/
  virtual int AppendPoint(vtkPolyData *polydata,double x,double y,double z,int num=1);

  /**
  Internal functions used to set/get visibility for point idx of the given polydata*/
  virtual int SetLandmarkVisibility(vtkPolyData *polydata,int idx,bool a);
  virtual bool GetLandmarkVisibility(vtkPolyData *polydata,int idx);

  /**
  Get set cloud set retrieving it from TAG if necessary*/
  int GetState();
  void SetState(int state);

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  int m_NumberOfLandmarks;
  int m_State;
  int m_DefaultVisibility;
  int m_SphereResolution;
  double m_Radius;

  mafString m_NumberOfLandmarksString;

private:
  mafVMELandmarkCloud(const mafVMELandmarkCloud&); // Not implemented
  void operator=(const mafVMELandmarkCloud&); // Not implemented

  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /**
  Hidden parents' functions: the root node does not have items,
  its a simple grouping node, and does not have a parent...*/
  virtual int AppendPoint(double x,double y,double z,mafTimeStamp t) {mafErrorMacro("Unsupported function, use AppendLandmark instead!"); return -1;}
  int SetPoint(int idx,double x,double y,double z,mafTimeStamp t) {mafErrorMacro("Unsupported function, use SetLandmark instead!"); return MAF_ERROR;}
  virtual int RemovePoint(int idx,mafTimeStamp t=-1) {mafErrorMacro("Unsupported function, use RemoveLandmark instead!"); return MAF_ERROR;};
  virtual void SetNumberOfPoints(int num,mafTimeStamp t=-1) {mafErrorMacro("Unsupported function, use SetNumberOfLandmarks instead!");};
  virtual int RemovePoint(vtkPolyData *polydata,int idx) {return this->Superclass::RemovePoint(polydata,idx);}
};
#endif