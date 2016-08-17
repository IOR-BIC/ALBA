/*=========================================================================

 Program: MAF2
 Module: mafVMELandmarkCloud
 Authors: Marco Petrone, Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
The cloud manage also children VMELandmarks, when a landmark is reparent to the cloud the landmark is added
to the out but is visibility is set to false. On the show operations the visibility of the landmark is changed.
The landmarks vme communicate with this cloud through parent variable, and some special management is defined 
on  mafLogicWithManagers
@sa
mafVME mafPointSet mafLandmark mafLogicWithManagers.*/
class MAF_EXPORT mafVMELandmarkCloud : public mafVMEPointSet
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events */
  /** @{ */
  MAF_ID_DEC(CLOUD_RADIUS_MODIFIED); ///< Event rised when the radius is changed with a SetRadius()
  MAF_ID_DEC(CLOUD_SPHERE_RES);      ///< Event rised when the sphere resolution is changed with a SetSphereResolution()
  /** @} */

  mafTypeMacro(mafVMELandmarkCloud, mafVMEPointSet);
  
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Copy the contents of another landmarkcloud into this one. */
  virtual int DeepCopy(mafVME *a);

  /** Compare with another landmarkcloud. */
  virtual bool Equals(mafVME *vme);

  /**
  Return the modification time.*/
  virtual unsigned long GetMTime();

  /**
  Add a new landmark an returns its index. BEWARE: landmark is added to all the
  VME-items, and its position should for each time frame by means SetPoint()
  The second syntax is used to add points in t=0, and is used in place of corresponding
  function of ex. RigidLandmarkClouds.
  The check for duplicates flag is used to inhibit append if the cloud contains a landmark 
  with the same name already. Set this flag to false for faster appending. 
*/
	int AppendLandmark(double x, double y, double z, const char *name);
	int AppendLandmark(const char *name) { return AppendLandmark(0,0,0,name); }

  
  /** Set/Get a landmark. In case the specified idx is invalid return MAF_ERROR*/
  int SetLandmark(int idx,double x,double y,double z,mafTimeStamp t=0);
	int SetLandmark(const char *name, double x, double y, double z, mafTimeStamp t = 0) { return this->SetLandmark(this->GetLandmarkIndex(name), x, y, z, t); }
	int SetLandmark(mafVMELandmark *lm, double x, double y, double z, mafTimeStamp t = 0) { return this->SetLandmark(this->GetLandmarkIndex(lm), x, y, z, t); }

	//int SetLandmarkForTimeFrame(int idx,double x,double y,double z,unsigned long tid,mafTimeStamp t);
  int GetLandmark(int idx, double &x,double &y,double &z,mafTimeStamp t=0);
  int GetLandmark(const char *name, double &x,double &y,double &z,mafTimeStamp t=0) {return this->GetLandmark(this->GetLandmarkIndex(name),x,y,z,t);} 
  int GetLandmark(int idx, double xyz[3],mafTimeStamp t=0);
  int GetLandmark(const char *name, double xyz[3],mafTimeStamp t=0) {return this->GetLandmark(this->GetLandmarkIndex(name), xyz,t);}

  /** Find the index of a landmark given its name.*/
  mafVMELandmark *GetLandmark(const char *name);

  /** Get the landmark by index.*/
  mafVMELandmark *GetLandmark(int idx);

	/** Get index for specified LM*/
	int GetLandmarkIndex(mafVMELandmark *lm);

	/** Find the index of a landmark given its name*/
	int GetLandmarkIndex(const char *name);


  /** Return the position of the landmark number 'idx' at the timestamp t.*/
  void GetLandmarkPosition(int idx, double pos[3], mafTimeStamp t=-1);

  
  /** Remove a landmark. In case the id is invalid return MAF_ERROR
  BEWARE: landmark is removed from all the VME-items*/
  int RemoveLandmark(int idx);

  /** Set/Get the number of landmarks stored in this VME. BEWARE the number of
  landmarks is the same for all the time frames (items)*/
  int GetNumberOfLandmarks();
  virtual int SetNumberOfLandmarks(int num);

  /** Set/get the name for idx-th landmark. Return null in case no landmark name 
  was defined for that index.*/
  virtual const char *GetLandmarkName(int idx);
  virtual void SetLandmarkName(int idx,const char *name);
  
  /**
  Set/Get the m_Radius of this landmark. The radius is stored as an TAG item: "LM_RADIUS".
  If no radius has been ever set, the default value is "1".*/
  virtual void SetRadius(double rad, bool force_update = false);
  virtual double GetRadius();

  /**
  Set/Get the resolution of the landmarks' spheres. The resolution is stored as an TAG item: "LM_SPHERE_RESOLUTION".
  If no resolution has been ever set, default value is "15".*/
  virtual void SetSphereResolution(int res, bool force_update = false);
  virtual int GetSphereResolution();

  
  enum LANDMAERK_CLOUD_WIDGET_ID
  {
    ID_LM_RADIUS = Superclass::ID_LAST,
    ID_LM_SPHERE_RESOLUTION,
    ID_LAST
  };

  /**
  Set/Get the visibility property for a given landmark for a given 
  time stamp. The visibility is stored as a scalar attribute of the
  associated polydata's point. Notice that visibility is implicitly defined
  as 'm_DefaultVisibility' when a position is specified for a timestamp nd nothing is specified
  for the visibility at that time. On the other hand, if visibility is defined
  for some time a position is implicitly defined for that time stamp.*/
  virtual int SetLandmarkVisibility(int idx,bool a,mafTimeStamp t=0);
  int SetLandmarkVisibility(const char *name,bool a,mafTimeStamp t=0) {return this->SetLandmarkVisibility(this->GetLandmarkIndex(name),a,t);}
  virtual bool GetLandmarkVisibility(int idx,mafTimeStamp t=0);
  bool GetLandmarkVisibility(const char *name,mafTimeStamp t=0) {return this->GetLandmarkVisibility(this->GetLandmarkIndex(name),t);}

  /**
  Set/Get the default visibility attribute. When default visibility is set to true, the creation 
  of a new time frame (by setting a landmark position for a given timestamp), makes all the other
  landmarks to be also visible at the same timestamp. If false, the visibility of the others landmark is
  automatically set to false for the other landmarks. Notice currently this is not a persistent attribute.*/
  void SetDefaultVisibility(int v) {this->m_DefaultVisibility = v; this->Modified();}
  int GetDefaultVisibility() {return m_DefaultVisibility;}
  void DefaultVisibilityOn() {SetDefaultVisibility(1);};
  void DefaultVisibilityOff() {SetDefaultVisibility(0);};

  /** Return number of visible landmarks*/
  int GetNumberOfVisibleLandmarks(mafTimeStamp t=0);
  
  /** Return true if it's a rigid landmark cloud (i.e. it has a single time frame)*/
  bool IsRigid();

  /** Return icon */
  static char** GetIcon();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeLandmarkCloud");};

  void Print(std::ostream &os, const int tabs=0);


  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

	/** Remove a child node*/
	virtual int AddChild(mafVME *node);

	/** Remove a child node*/
	virtual void RemoveChild(mafVME *node);

	void ShowLandmark(mafVMELandmark *lm, bool show);

	void ShowAllLandmarks(bool show=true);

	bool IsLandmarkShow(mafVMELandmark *lm);

	int GetLandmarkShowNumber();

	/** */
	void CreateLMStructureFromDataVector();

	mafVMELandmark *GetNearestLandmark(double pos[3]);

protected:
  mafVMELandmarkCloud();
  virtual ~mafVMELandmarkCloud();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  /** Internal function to instantiate a VMEItem with for timestamp t containing a new polydata*/
  virtual vtkPolyData *NewPolyData(mafTimeStamp t);

  /** Internal functions redefined to support cells*/
  virtual int AppendPoint(vtkPolyData *polydata,double x,double y,double z,int num=1);

  /** Internal functions used to set/get visibility for point idx of the given polydata*/
  virtual int SetLandmarkVisibility(vtkPolyData *polydata,int idx,bool a);
  virtual bool GetLandmarkVisibility(vtkPolyData *polydata,int idx);

	int SetLandmarkToPolydata(int idx, double x, double y, double z, bool visibility, mafTimeStamp &t);

	 
  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  int m_SingleFile;
  int m_NumberOfLandmarks;
  int m_DefaultVisibility;
  int m_SphereResolution;
  double m_Radius;

	int m_LanfmarkShowNumber;
	std::vector<mafVMELandmark *> m_LMChildren;
	std::vector<bool> m_LMChildrenShow;
	std::map<mafVMELandmark *, int> m_LMIndexesMap;


  
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
