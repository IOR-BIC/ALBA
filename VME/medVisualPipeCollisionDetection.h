/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVisualPipeCollisionDetection.h,v $
  Language:  C++
  Date:      $Date: 2012-04-06 10:08:46 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medVisualPipeCollisionDetection_H__
#define __medVisualPipeCollisionDetection_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafPipe.h"

#include <vector>

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafVME;
class mafMatrix;
class vtkMEDCollisionDetectionFilter;
class vtkActor;
class vtkPolyDataMapper;
class vtkIdList;

//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
class MED_VME_EXPORT medVisualPipeCollisionDetection : public mafPipe
{
public:
  mafTypeMacro(medVisualPipeCollisionDetection,mafPipe);

  /** constructor. */
  medVisualPipeCollisionDetection();
  /** destructor. */
  ~medVisualPipeCollisionDetection ();

  /** process events coming from gui */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

  /** function that create the pipeline instancing vtk graphic pipe. */
  /*virtual*/ void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  
  /** During selection of vme, it can visualize graphic element (actually empty)*/
  /*virtual*/ void Select(bool select); 

  /** Update the visual pipeline of the surface*/
  void UpdatePipeline(bool force = false);

  /** static function to select a surface in the tree */
  static bool SurfaceAccept(mafNode *node);

  /** set the surface to compute collision - remove all old surfaces */
  void SetSurfaceToCollide(mafVME *surface);

  /** add a surface to compute collision */
  void AddSurfaceToCollide(mafVME *surface);

  /** set on show of surfaces to collide */
  void ShowSurfaceToCollideOn();

  /** set off (default) show of surfaces to collide */
  void ShowSurfaceToCollideOff();

  /** set a list of cells to exclude to the collision */
  void SetListOfCellToExclude(bool *list);

  void SetScalarNameToExclude(wxString name){m_ScalarNameToExclude = name;};

protected:

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SELECT_SURFACE = Superclass::ID_LAST,
    ID_ADD_SURFACE,
    ID_LAST
  };

  /** Creation of the gui that will be attached to visual prop panel.*/
  /*virtual*/ mafGUI  *CreateGui();

  bool *m_CellToExlude;
  std::vector<vtkActor *> m_SurfacebToCollideActor;
  std::vector<vtkPolyDataMapper *> m_SurfaceToCollideMapper;
  std::vector<mafVME *> m_SurfacesToCollide;
  vtkPolyDataMapper *m_Mapper;
  vtkActor *m_Actor;
  vtkMEDCollisionDetectionFilter *m_CollisionFilter;
  mafMatrix *m_Matrix0;
  std::vector<mafMatrix *> m_Matrix1;
  bool m_ShowSurfaceToCollide;
  wxString m_ScalarNameToExclude;
};  
#endif // __mafPipeSurface_H__
