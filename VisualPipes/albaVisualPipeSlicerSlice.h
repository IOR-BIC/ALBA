/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipeSlicerSlice
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVisualPipeSlicerSlice_H__
#define __albaVisualPipeSlicerSlice_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class albaAxes;
class albaGUIMaterialButton;
class vtkALBAFixedCutter;
class vtkPlane;
class vtkAppendPolyData;
class vtkSphereSource;


//----------------------------------------------------------------------------
// albaVisualPipeSlicerSlice :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaVisualPipeSlicerSlice : public albaPipe
{
public:
  albaTypeMacro(albaVisualPipeSlicerSlice,albaPipe);

               albaVisualPipeSlicerSlice();
  virtual     ~albaVisualPipeSlicerSlice ();

  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  virtual void Create(albaSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  /** Set the origin of the slice*/
  void SetSlice1(double *Origin1);
  void SetSlice2(double *Origin2);

  /** Set the normal of the slice*/
	void SetNormal(double *Normal);

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

  //bool ImageAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVMEImage));};

  virtual albaGUI  *CreateGui();
protected:
  
  vtkPolyDataMapper	      *m_Mapper1;
  vtkActor                *m_Actor1;
  vtkPolyDataMapper	      *m_Mapper2;
  vtkActor                *m_Actor2;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  albaAxes                 *m_Axes;
  vtkPlane				        *m_Plane1;
  vtkPlane				        *m_Plane2;
  vtkALBAFixedCutter		      *m_Cutter1;
  vtkALBAFixedCutter		      *m_Cutter2;
  vtkSphereSource         *m_Sphere;
  vtkPolyDataMapper       *m_SphereMapper;
  vtkProperty             *m_SphereProperty;
  vtkActor                *m_SphereActor;
  //vtkAppendPolyData       *m_AppendPolydata;

  double				           m_Border;

	bool	m_ShowSelection;

  double	m_Origin1[3], m_Origin2[3];
  double	m_Normal[3];

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;
  albaGUIMaterialButton *m_MaterialButton;

};  
#endif // __albaVisualPipeSlicerSlice_H__
