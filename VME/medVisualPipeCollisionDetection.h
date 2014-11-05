/*=========================================================================

 Program: MAF2Medical
 Module: medVisualPipeCollisionDetection
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

  /** set name of aarray of scalars that rappresent cells to exclude to collision */
  void SetScalarNameToExclude(wxString name){m_ScalarNameToExclude = name;};

  /** set opacity value */
  void SetOpacity(double opacity);

  /** enable or disable pipe update after inputs matrix changed */
  void EnableUpdate(bool enable){m_EnablePipeUpdate = enable;};

  /** set color for not collision cells */
  void SetColorForNotCollisionSurface(double rgb[3]);

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

  double m_ColorNotCollisionCells[3];
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
  bool m_EnablePipeUpdate;
  wxString m_ScalarNameToExclude;
};  
#endif // __mafPipeSurface_H__
