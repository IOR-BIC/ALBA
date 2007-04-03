/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurfaceSlice.h,v $
  Language:  C++
  Date:      $Date: 2007-04-03 10:19:48 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeSurfaceSlice_H__
#define __mafPipeSurfaceSlice_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"
#include "mmgVMEChooserAccept.h"
#include "mafVMEImage.h"

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
class mmgMaterialButton;
class vtkFixedCutter;
class vtkPlane;
class vtkSphereSource;

//----------------------------------------------------------------------------
// mafPipeSurfaceSlice :
//----------------------------------------------------------------------------
class mafPipeSurfaceSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurfaceSlice,mafPipe);

               mafPipeSurfaceSlice();
  virtual     ~mafPipeSurfaceSlice ();

  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
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

  class mafTextureAccept : public mmgVMEChooserAccept
  {
  public:

    mafTextureAccept() {};
    ~mafTextureAccept() {};

    bool Validate(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEImage));};
  };

  mafTextureAccept *m_TextureAccept;

  virtual mmgGui  *CreateGui();
protected:
  vtkTexture              *m_Texture;
  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  mafAxes                 *m_Axes;
  vtkPlane				        *m_Plane;
  vtkFixedCutter		      *m_Cutter;

  vtkSphereSource         *m_SphereSource;

  double				           m_Border;

	bool	m_ShowSelection;

  double	m_Origin[3];
  double	m_Normal[3];

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;
  mmgMaterialButton *m_MaterialButton;

//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 

  /** 
  Generate texture coordinate for polydata according to the mapping mode*/
  void GenerateTextureMapCoordinate();

};  
#endif // __mafPipeSurfaceSlice_H__
