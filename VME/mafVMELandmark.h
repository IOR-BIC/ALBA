/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMELandmark.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:47:58 $
  Version:   $Revision: 1.1 $
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
class mafNode;
class vtkPolyData;

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

  void Print(std::ostream &os, const int tabs);

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
  int SetPoint(int idx,double x,double y,double z,mafTimeStamp t=-1);
  int SetPoint(int idx,double xyz[3],mafTimeStamp t=-1); 
  int GetPoint(int idx, double &x,double &y,double &z,mafTimeStamp t=-1);
  int GetPoint(int idx, double xyz[3],mafTimeStamp t=-1);

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

protected:
  mafVMELandmark();
  virtual ~mafVMELandmark();

  virtual bool CanReparentTo(mafNode *parent);

private:
  mafVMELandmark(const mafVMELandmark&); // Not implemented
  void operator=(const mafVMELandmark&); // Not implemented
};
#endif