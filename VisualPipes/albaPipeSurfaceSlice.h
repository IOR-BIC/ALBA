/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceSlice
 Authors: Silvano Imboden,Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeSurfaceSlice_H__B
#define __albaPipeSurfaceSlice_H__B

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeSlice.h"
#include "albaPipe.h"
#include "albaVMEImage.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class albaAxes;
class albaGUIMaterialButton;
class vtkALBAFixedCutter;
class vtkPlane;
class vtkSphereSource;
class vtkALBAToLinearTransform;
class vtkLookupTable;

/** 
  class name: albaPipeSurfaceSlice 
  Visual pipe that allow to visualize a surface polydata sliced according to 
  a slicing plain. The result effect is the contour of the surface cut by the plain.
*/
class ALBA_EXPORT albaPipeSurfaceSlice : public albaPipe, public albaPipeSlice
{
public:
  /** RTTI Macro */
  albaTypeMacro(albaPipeSurfaceSlice,albaPipe);

  /** Constructor. */
  albaPipeSurfaceSlice();

  /** Destructor. */
  virtual     ~albaPipeSurfaceSlice ();

  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /** process events coming from Gui */
  virtual void OnEvent(albaEventBase *alba_event);

	void UpdateScalars();


	void UpdateLUTAndMapperFromNewActiveScalars();


	/** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  

  /** Show bounding box when selection is on. */
	void ShowBoxSelectionOn(){m_ShowSelection=true;};

  /** Hide bounding box when selection is off. */
	void ShowBoxSelectionOff(){m_ShowSelection=false;};

	/** Set the actor picking*/
	void SetActorPicking(int enable);

	/** Set the lookup table */
	void SetLookupTable(vtkLookupTable *table);

	/** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_CHOOSE_TEXTURE,
    ID_TEXTURE_MAPPING_MODE,
		ID_BORDER_CHANGE,
		ID_LUT,
		ID_SCALAR_MAP_ACTIVE,
		ID_LAST,
  };

  //bool ImageAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVMEImage));};
  /** creation of gui */
  virtual albaGUI  *CreateGui();

protected:
  vtkTexture              *m_Texture;
  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;

  vtkActor                *m_OutlineActor;
  albaAxes                 *m_Axes;
  vtkPlane				        *m_Plane;
  vtkALBAFixedCutter		      *m_Cutter;

  vtkSphereSource         *m_SphereSource;
	vtkLookupTable *m_Table;

  double				           m_Border;

	bool	m_ShowSelection;
	bool	m_Pickable;


  int m_ScalarVisibility;
  albaGUIMaterialButton *m_MaterialButton;

	vtkALBAToLinearTransform* m_VTKTransform;

//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 

  /**  Generate texture coordinate for polydata according to the mapping mode*/
  void GenerateTextureMapCoordinate();

};  
#endif // __albaPipeSurfaceSlice_H__B
