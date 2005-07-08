/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGizmo.h,v $
  Language:  C++
  Date:      $Date: 2005-07-08 13:29:34 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone, Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEGizmo_h
#define __mafVMEGizmo_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafTransform;
class mafVMEOutputSurface;
class mmaMaterial;

/* mafVMEGizmo - this class represent a non persistent node of the tree.
 mafVMEGizmo is a non persistent node of the tree used by application modules
 to place objects in the scene which have not to be stored on disk. Examples
 are visual gizmos used to manipulate objects during interaction. This kind of
 can store internally VTK polydata representing the gizmo to be displayed.
 @sa
 mafVME

@todo
- create a test program */
class MAF_EXPORT mafVMEGizmo : public mafVME
{
public:
  mafTypeMacro(mafVMEGizmo,mafVME);

  /**
  For VME Gizmo it is allowed to directly set the Data to be used
  to display the node.*/
  void SetData(vtkPolyData *data);
  
  /** return the ploydata used to display this gizmo */
  vtkPolyData *GetData() {return m_GizmoData;}
  
  /** Copy the contents of another VME-Slicer into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Slicer. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputSurface *GetSurfaceOutput();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();
  
  /** return an xpm-icon that can be used to represent this node */
  //static char ** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeGizmo");};

protected:
  mafVMEGizmo();
  virtual ~mafVMEGizmo();
  
  /** called to prepare the update of the output */
  //virtual void InternalPreUpdate();

  /** update the output data structure */
  //virtual void InternalUpdate();

  mafTransform *m_Transform; ///< pose matrix for the slicer plane
  vtkPolyData  *m_GizmoData;

private:
  mafVMEGizmo(const mafVMEGizmo&); // Not implemented
  void operator=(const mafVMEGizmo&); // Not implemented

  /** Hidden VME functions */
};
#endif
