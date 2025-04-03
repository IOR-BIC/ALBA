/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeLandmarkCloud
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeLandmarkCloud_H__
#define __albaPipeLandmarkCloud_H__

#include "albaPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkPolyDataNormals;
class vtkALBAExtendedGlyph3D;
class vtkPolyDataMapper;
class vtkActor;
class albaVMELandmarkCloud;
class albaVMELandmark;
class vtkDataSet;
class albaGUIMaterialButton;
class vtkALBALandmarkCloudOutlineCornerFilter;

//----------------------------------------------------------------------------
// albaPipeLandmarkCloud :
//----------------------------------------------------------------------------
/** This visual pipe represent albaVMELandmarkClound and albaVMELandmark as a set of
spheres each one centered to the landmark position. The visual pipe is observer of
the landmark cloud and according to the Open/Close cloud status the visual pipe
switch the mode to manage the data contained into the cloud (cloud close) or into the
landmarks (cloud open).*/
class ALBA_EXPORT albaPipeLandmarkCloud : public albaPipe
{
public:
  albaTypeMacro(albaPipeLandmarkCloud,albaPipe);

               albaPipeLandmarkCloud();
  virtual     ~albaPipeLandmarkCloud ();

  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

  /** Answer events coming from the Gui and from cloud */
  void OnEvent(albaEventBase *alba_event);

	/** Change the visibility of the bounding box actor representing the selection for the vme. */
  virtual void Select(bool select); 

  /** Set the scalar visibility. */
  void SetScalarVisibility(int visibility){m_ScalarVisibility = visibility;};

  /** IDs for the GUI */
  enum PIPE_LM_CLOUD_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_LAST
  };

protected:
  vtkSphereSource			   *m_SphereSource;
  vtkPolyDataNormals     *m_Normals;
  vtkALBAExtendedGlyph3D     *m_Glyph;
  vtkPolyDataMapper			 *m_CloudMapper;
  vtkActor               *m_CloudActor;

	vtkALBALandmarkCloudOutlineCornerFilter *m_CloundCornerFilter;

  vtkActor               *m_CloudSelectionActor;

  double m_Radius;
  albaVMELandmark         *m_Landmark;
	albaVMELandmarkCloud		 *m_Cloud;

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;
  albaGUIMaterialButton *m_MaterialButton;

  /** Create visual-pipe for closed cloud or single landmark */
  virtual void CreateCloudPipe(vtkDataSet *data, double radius, double resolution);

  /** Remove visual-pipe for closed cloud */
  virtual void RemoveClosedCloudPipe();

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual albaGUI *CreateGui();

  /** Update the properties according to the vme's tags. */
	void UpdateProperty(bool fromTag = false);
};
#endif // __albaPipeLandmarkCloud_H__
