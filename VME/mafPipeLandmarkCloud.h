/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeLandmarkCloud.h,v $
  Language:  C++
  Date:      $Date: 2008-02-01 14:08:40 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeLandmarkCloud_H__
#define __mafPipeLandmarkCloud_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkPolyDataNormals;
class vtkExtendedGlyph3D;
class vtkPolyDataMapper;
class vtkActor;
class mafVMELandmarkCloud;
class mafVMELandmark;
class vtkDataSet;
class mmgMaterialButton;

//----------------------------------------------------------------------------
// mafPipeLandmarkCloud :
//----------------------------------------------------------------------------
/** This visual pipe represent mafVMELandmarkClound and mafVMELandmark as a set of
spheres each one centered to the landmark position. The visual pipe is observer of
the landmark cloud and according to the Open/Close cloud status the visual pipe
switch the mode to manage the data contained into the cloud (cloud close) or into the
landmarks (cloud open).*/
class mafPipeLandmarkCloud : public mafPipe
{
public:
  mafTypeMacro(mafPipeLandmarkCloud,mafPipe);

               mafPipeLandmarkCloud();
  virtual     ~mafPipeLandmarkCloud ();

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

  /** Answer events coming from the Gui and from cloud */
  void OnEvent(mafEventBase *maf_event);

	/** Change the visibility of the bounding box actor representing the selection for the vme. */
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_LM_CLOUD_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_LAST
  };

protected:
  vtkSphereSource			   *m_SphereSource;
  vtkPolyDataNormals     *m_Normals;
  vtkExtendedGlyph3D     *m_Glyph;
  vtkPolyDataMapper			 *m_CloudMapper;
  vtkActor               *m_CloudActor;

  vtkActor               *m_CloudSelectionActor;

  double m_Radius;
  mafVMELandmark         *m_Landmark;
	mafVMELandmarkCloud		 *m_Cloud;

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;
  mmgMaterialButton *m_MaterialButton;

  /** Create visual-pipe for closed cloud or single landmark */
  void CreateClosedCloudPipe(vtkDataSet *data, double radius, double resolution);

  /** Remove visual-pipe for closed cloud */
  void RemoveClosedCloudPipe();

  virtual mmgGui  *CreateGui();

  /** Update the properties according to the vme's tags. */
	void										UpdateProperty(bool fromTag = false);
};
#endif // __mafPipeLandmarkCloud_H__
