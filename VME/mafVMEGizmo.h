/*=========================================================================

 Program: MAF2
 Module: mafVMEGizmo
 Authors: Marco Petrone, Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVMEGizmo_h
#define __mafVMEGizmo_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafObserver.h"
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
  
  /** return the polydata used to display this gizmo */
  vtkPolyData *GetData() {return m_GizmoData;}
  
  /** Copy the contents of another VME-Gizmo into this one. */
  virtual int DeepCopy(mafVME *a);
  
  /** 
  Set the orchestrator object: mafVMEGizmo's are used togetheter in more complex objects 
  like mafGizmoRotate, mafGizmoTranslate ,... The master gizmo act as mediator between
  single mafVMEGizmos.
  The optional mediator ivar holds a reference to the orchestrator.
  */
  void SetMediator(mafObserver *mediator) {m_Mediator = mediator;};
  mafObserver *GetMediator() {return m_Mediator;};

  /** Compare with another VME-Gizmo. */
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

  /**Set/Get Text Value , that is an information that can be renderized in a custom position*/
  void SetTextValue(const char* text);
  const char *GetTextValue();

  /**Set/Get Text Position , that is the position of Text Value in 3d position*/
  void SetTextPosition(double *position3D);
  double *GetTextPosition();

  /**Set/Get Visibility of Text Value in 3d position*/
  void SetTextVisibility(int visibility);
  int GetTextVisibility(){return m_TextVisibility;};

  /**Set/Get Colour of Text*/
  void SetTextColour(double colour[3]);
  double *GetTextColour();


protected:
  mafVMEGizmo();
  virtual ~mafVMEGizmo();
  
  /** called to prepare the update of the output */
  //virtual void InternalPreUpdate();

  /** update the output data structure */
  //virtual void InternalUpdate();

  mafTransform *m_Transform; ///< pose matrix for the slicer plane
  vtkPolyData  *m_GizmoData;

  mafString m_TextValue;
  double m_TextPosition[3];
  double m_TextColour[3];
  int    m_TextVisibility;
	
  mafObserver *m_Mediator;

private:
  mafVMEGizmo(const mafVMEGizmo&); // Not implemented
  void operator=(const mafVMEGizmo&); // Not implemented

  /** Hidden VME functions */
};
#endif
