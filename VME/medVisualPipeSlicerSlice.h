/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVisualPipeSlicerSlice.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:19:42 $
  Version:   $Revision: 1.3 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medVisualPipeSlicerSlice_H__
#define __medVisualPipeSlicerSlice_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class mafAxes;
class mafGUIMaterialButton;
class vtkMAFFixedCutter;
class vtkPlane;
class vtkAppendPolyData;
class vtkSphereSource;


//----------------------------------------------------------------------------
// medVisualPipeSlicerSlice :
//----------------------------------------------------------------------------
class medVisualPipeSlicerSlice : public mafPipe
{
public:
  mafTypeMacro(medVisualPipeSlicerSlice,mafPipe);

               medVisualPipeSlicerSlice();
  virtual     ~medVisualPipeSlicerSlice ();

  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
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

  //bool ImageAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEImage));};

  virtual mafGUI  *CreateGui();
protected:
  
  vtkPolyDataMapper	      *m_Mapper1;
  vtkActor                *m_Actor1;
  vtkPolyDataMapper	      *m_Mapper2;
  vtkActor                *m_Actor2;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  mafAxes                 *m_Axes;
  vtkPlane				        *m_Plane1;
  vtkPlane				        *m_Plane2;
  vtkMAFFixedCutter		      *m_Cutter1;
  vtkMAFFixedCutter		      *m_Cutter2;
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
  mafGUIMaterialButton *m_MaterialButton;

};  
#endif // __medVisualPipeSlicerSlice_H__
