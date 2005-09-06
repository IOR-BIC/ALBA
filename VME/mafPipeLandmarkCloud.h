/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeLandmarkCloud.h,v $
  Language:  C++
  Date:      $Date: 2005-09-06 11:12:02 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeLandmarkCloud_H_
#define _mafPipeLandmarkCloud_H_

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkPolyDataNormals;
class vtkExtendedGlyph3D;
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class mafVMELandmarkCloud;
class mafVMELandmark;
class vtkDataSet;

//----------------------------------------------------------------------------
// mafPipeLandmarkCloud :
//----------------------------------------------------------------------------
class mafPipeLandmarkCloud : public mafPipe
{
public:
  mafTypeMacro(mafPipeLandmarkCloud,mafPipe);

               mafPipeLandmarkCloud();
  virtual     ~mafPipeLandmarkCloud ();

  virtual void Create(mafSceneNode *n);

  /** Answer events coming from the gui and from cloud */
  void OnEvent(mafEventBase *maf_event);

	/** Change the visibility of the bounding box actor representing the selection for the vme. */
  virtual void Select     (bool select); 

  vtkSphereSource			   *m_SphereSource;
	vtkPolyDataNormals     *m_Normals;
  vtkExtendedGlyph3D     *m_Glyph;
  vtkPolyDataMapper			 *m_CloudMapper;
  vtkActor               *m_CloudActor;

  vtkOutlineCornerFilter *m_OutlineFilter;
  vtkPolyDataMapper      *m_CloudSelectionMapper;
  vtkProperty            *m_CloudSelectionProperty;
  vtkActor               *m_CloudSelectionActor;

protected:
  double m_Radius;
  mafVMELandmark         *m_Landmark;
	mafVMELandmarkCloud		 *m_Cloud;

  /** Create visual-pipe for closed cloud or single landmark */
  void CreateClosedCloudPipe(vtkDataSet *data, double radius, double resolution);

  /** Remove visual-pipe for closed cloud */
  void RemoveClosedCloudPipe();

  /** Update the properties according to the vme's tags. */
	void										UpdateProperty(bool fromTag = false);
};

#endif // _mafPipeLandmarkCloud_H_

