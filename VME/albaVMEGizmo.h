/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGizmo
 Authors: Marco Petrone, Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVMEGizmo_h
#define __albaVMEGizmo_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVME.h"
#include "albaObserver.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class vtkAlgorithmOutput;
class albaTransform;
class albaVMEOutputSurface;
class mmaMaterial;

/* albaVMEGizmo - this class represent a non persistent node of the tree.
 albaVMEGizmo is a non persistent node of the tree used by application modules
 to place objects in the scene which have not to be stored on disk. Examples
 are visual gizmos used to manipulate objects during interaction. This kind of
 can store internally VTK polydata representing the gizmo to be displayed.
 @sa
 albaVME

@todo
- create a test program */
class ALBA_EXPORT albaVMEGizmo : public albaVME
{
public:
  albaTypeMacro(albaVMEGizmo,albaVME);

  /**
  For VME Gizmo it is allowed to directly set the Data to be used
  to display the node.*/
  void SetData(vtkPolyData *data);

	/**
	For VME Gizmo it is allowed to directly set the Data to be used
	to display the node.*/
	void SetDataConnection(vtkAlgorithmOutput *intput);

  /** return the polydata used to display this gizmo */
  vtkPolyData *GetData();
  
  /** Copy the contents of another VME-Gizmo into this one. */
  virtual int DeepCopy(albaVME *a);
  
  /** 
  Set the orchestrator object: albaVMEGizmo's are used togetheter in more complex objects 
  like albaGizmoRotate, albaGizmoTranslate ,... The master gizmo act as mediator between
  single albaVMEGizmos.
  The optional mediator ivar holds a reference to the orchestrator.
  */
  void SetMediator(albaObserver *mediator) {m_Mediator = mediator;};
  albaObserver *GetMediator() {return m_Mediator;};

  /** Compare with another VME-Gizmo. */
  virtual bool Equals(albaVME *vme);

  /** return the right type of output */  
  albaVMEOutputSurface *GetSurfaceOutput();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const albaMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();
  
  /** return an xpm-icon that can be used to represent this node */
  //static char ** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeGizmo");};

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
  albaVMEGizmo();
  virtual ~albaVMEGizmo();
  
  /** called to prepare the update of the output */
  //virtual void InternalPreUpdate();

  /** update the output data structure */
  //virtual void InternalUpdate();

  albaTransform *m_Transform; ///< pose matrix for the slicer plane
  vtkPolyData  *m_GizmoData;
	vtkAlgorithmOutput *m_InputConnection;

  albaString m_TextValue;
  double m_TextPosition[3];
  double m_TextColour[3];
  int    m_TextVisibility;
	
  albaObserver *m_Mediator;

private:
  albaVMEGizmo(const albaVMEGizmo&); // Not implemented
  void operator=(const albaVMEGizmo&); // Not implemented

  /** Hidden VME functions */
};
#endif
