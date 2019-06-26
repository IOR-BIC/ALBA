/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipeCollisionDetection
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVisualPipeCollisionDetection_H__
#define __albaVisualPipeCollisionDetection_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

#include <vector>

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class albaVME;
class albaMatrix;
class vtkALBACollisionDetectionFilter;
class vtkActor;
class vtkPolyDataMapper;
class vtkIdList;

//----------------------------------------------------------------------------
// albaPipeSurface :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaVisualPipeCollisionDetection : public albaPipe
{
public:
  albaTypeMacro(albaVisualPipeCollisionDetection,albaPipe);

  /** constructor. */
  albaVisualPipeCollisionDetection();
  /** destructor. */
  ~albaVisualPipeCollisionDetection ();

  /** process events coming from gui */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

  /** function that create the pipeline instancing vtk graphic pipe. */
  /*virtual*/ void Create(albaSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  
  /** During selection of vme, it can visualize graphic element (actually empty)*/
  /*virtual*/ void Select(bool select); 

  /** Update the visual pipeline of the surface*/
  void UpdatePipeline(bool force = false);

  /** static function to select a surface in the tree */
  static bool SurfaceAccept(albaVME*node);

  /** set the surface to compute collision - remove all old surfaces */
  void SetSurfaceToCollide(albaVME *surface);

  /** add a surface to compute collision */
  void AddSurfaceToCollide(albaVME *surface);

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
  /*virtual*/ albaGUI  *CreateGui();

  double m_ColorNotCollisionCells[3];
  bool *m_CellToExlude;
  std::vector<vtkActor *> m_SurfacebToCollideActor;
  std::vector<vtkPolyDataMapper *> m_SurfaceToCollideMapper;
  std::vector<albaVME *> m_SurfacesToCollide;
  vtkPolyDataMapper *m_Mapper;
  vtkActor *m_Actor;
  vtkALBACollisionDetectionFilter *m_CollisionFilter;
  albaMatrix *m_Matrix0;
  std::vector<albaMatrix *> m_Matrix1;
  bool m_ShowSurfaceToCollide;
  bool m_EnablePipeUpdate;
  wxString m_ScalarNameToExclude;
};  
#endif // __albaPipeSurface_H__
