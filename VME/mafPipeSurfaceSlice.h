/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceSlice
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeSurfaceSlice_H__
#define __mafPipeSurfaceSlice_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"
#include "mafVMEImage.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class mafAxes;
class mafGUIMaterialButton;
class vtkMAFFixedCutter;
class vtkPlane;
class vtkSphereSource;
class vtkMAFToLinearTransform;

//----------------------------------------------------------------------------
// mafPipeSurfaceSlice :
//----------------------------------------------------------------------------
/** Visual pipe that allow to visualize a surface polydata sliced according to 
a slicing plain. The result effect is the contour of the surface cut by the plain.*/
class MAF_EXPORT mafPipeSurfaceSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurfaceSlice,mafPipe);

               mafPipeSurfaceSlice();
  virtual     ~mafPipeSurfaceSlice ();

  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /** process events coming from Gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Set the origin of the slice*/
  void SetSlice(double *Origin);

  /** Set the normal of the slice*/
	void SetNormal(double *Normal);

  /** Create visual-pipe for closed cloud or single landmark */
  void CreateClosedCloudPipe();

  /** Remove visual-pipe for closed cloud */
  void RemoveClosedCloudPipe();

	void ShowBoxSelectionOn(){m_ShowSelection=true;};
	void ShowBoxSelectionOff(){m_ShowSelection=false;};

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_CHOOSE_TEXTURE,
    ID_TEXTURE_MAPPING_MODE,
    ID_LAST,
	ID_BORDER_CHANGE
  };

  //bool ImageAccept(mafVME*node) {return(node != NULL && node->IsMAFType(mafVMEImage));};

  virtual mafGUI  *CreateGui();
protected:
  vtkTexture              *m_Texture;
  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;

  vtkActor                *m_OutlineActor;
  mafAxes                 *m_Axes;
  vtkPlane				        *m_Plane;
  vtkMAFFixedCutter		      *m_Cutter;

  vtkSphereSource         *m_SphereSource;

  double				           m_Border;

	bool	m_ShowSelection;

  double	m_Origin[3];
  double	m_Normal[3];

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;
  mafGUIMaterialButton *m_MaterialButton;

	vtkMAFToLinearTransform* m_VTKTransform;

//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 

  /** 
  Generate texture coordinate for polydata according to the mapping mode*/
  void GenerateTextureMapCoordinate();

};  
#endif // __mafPipeSurfaceSlice_H__
