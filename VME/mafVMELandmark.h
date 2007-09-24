/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMELandmark.h,v $
  Language:  C++
  Date:      $Date: 2007-09-24 13:52:27 $
  Version:   $Revision: 1.12 $
  Authors:   Marco Petrone, Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMELandmark_h
#define __mafVMELandmark_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGenericAbstract.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mmaMaterial;

/** mafVMELandmark - this class represents a single Landmark
 mafVMELandmark is a concrete VME object storing a single landmark, whose
 position can change along time (trajectory). This VME can exist only as 
 a child of a VME-LandmarkGroup, and can per re-parented only under such a type
 of VME. Moreover this is a leaf node, i.e. cannot have children VME.
 Trajectory of the landmark is stored in the Matrix Vector, thus this VME
 does not have any attaced item too: the GetCurrentData() creates runtime a 
 vtkPolyData with a single point (no Vertex Cell) which is not saved to MSF.
 @sa
 mafVME mflVMEGroup
 @todo
- create a new VMEType for landmarks not defined as VMECloud children, which
 are completely different type and could be simply a father of this landmark class. */
class MAF_EXPORT mafVMELandmark : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMELandmark, mafVMEGenericAbstract);

  void Print(std::ostream &os, const int tabs=0);

  /** Copy the contents of another landmark into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another landmark. */
  virtual bool Equals(mafVME *vme);

  /**
  Set/Get the Radius of this landmark. Notice that Radius is
  currently defines only for a whole cloud of landmarks, thus 
  this functions simply set/get the parent LandmarkGroup radius
  If this landmark does not owns to a LandmarkGroup Set() does nothing
  and Get() returns -1;*/
  void SetRadius(double rad);
  double GetRadius();

  /**
  Set/Get the resolution of the landmarks' spheres. The resolution is stored as an TAG item: "LM_SPHERE_RESOLUTION".
  If no resolution has been ever set, default value is "15".*/
  void SetSphereResolution(int res);
  int GetSphereResolution();

  /**
  Redefines parent functions. Since this object has by default 1 point this
  functions set/get the VME position.*/
  int SetPoint(double x,double y,double z,mafTimeStamp t=-1);
  int SetPoint(double xyz[3],mafTimeStamp t=-1); 
  int GetPoint(double &x,double &y,double &z,mafTimeStamp t=-1);
  int GetPoint(double xyz[3],mafTimeStamp t=-1);

  /**
  Set/Get the visibility property at a given time frame. The visibility is stored as a scalar attribute of the
  associated polydata's point. Notice that visibility is implicant defined
  as 'true' when a position is specified for a time stamp and nothing is specified
  for the visibility at that time. On the other hand, if visibility is defined
  for some time a position is implicitly defined for that time stamp.*/
  int SetLandmarkVisibility(bool a,mafTimeStamp t=-1);
  bool GetLandmarkVisibility(mafTimeStamp t=-1);

  /**
  This is overridden to allow share seamless the Tag array of the landmark with the TAG array of the cloud.*/
  mafTagArray *GetTagArray() {if (mafVME *parent=this->GetParent()) return parent->GetTagArray(); return NULL;}

  /**
  Overriden to always return [-r,+r,-r,+r,-r,+r] or invalid,
  depending on visibility for given time*/
  //void GetVMELocalSpaceBounds(double bounds[6]) {this->Superclass::GetVMELocalSpaceBounds(bounds);}
  //void GetVMELocalSpaceBounds(mflBounds &bounds,mafTimeStamp t=-1, mafVMEIterator *iter=NULL);

  /**
  Redefined to avoid chaning landmark scale (i.e. the scale is forced to the visibility value)*/
  virtual void SetMatrix(mafMatrix &mat);

  /** return icon */
  static char** GetIcon();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeLandmarkCloud");};

  /** Set the current TimeStamp */
  virtual void SetTimeStamp(mafTimeStamp t);

  /** called to check if the vme can be re-parented under the new parent */
  virtual bool CanReparentTo(mafNode *parent);

protected:
  mafVMELandmark();
  virtual ~mafVMELandmark();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  vtkPolyData *m_Polydata; ///< Polydata storing the landmark point
  mafString m_Position[3];

private:
  mafVMELandmark(const mafVMELandmark&); // Not implemented
  void operator=(const mafVMELandmark&); // Not implemented
};
#endif
