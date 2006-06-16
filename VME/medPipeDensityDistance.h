/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeDensityDistance.h,v $
  Language:  C++
  Date:      $Date: 2006-06-16 13:09:09 $
  Version:   $Revision: 1.1 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeDensityDistance_H__
#define __medPipeDensityDistance_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"
#include "mmgVMEChooserAccept.h"
#include "mafVMEImage.h"
#include "mafVMEVolume.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class vtkPolyDataNormals;
class vtkDistanceFilter;
class vtkColorTransferFunction;
class vtkScalarBarActor;

class mafAxes;
class mmgMaterialButton;
class mafNode;
class mafVMEVolume;

//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
class medPipeDensityDistance : public mafPipe
{
public:
  mafTypeMacro(medPipeDensityDistance,mafPipe);

               medPipeDensityDistance();
  virtual     ~medPipeDensityDistance ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  void SetVolume(mafNode* volume);

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_DENSITY_DISTANCE = Superclass::ID_LAST,
    ID_FIRST_THRESHOLD,
    ID_SECOND_THRESHOLD,
    ID_MAX_DISTANCE,
    ID_SELECT_VOLUME,
		ID_NUM_SECTIONS,
		ID_BAR_TIPOLOGY,
		ID_AREA,
		ID_AREA_DISTANCE,
    ID_LAST
  };

  class mafVolumeAccept : public mmgVMEChooserAccept
	{
		public:
			
			mafVolumeAccept() {};
		 ~mafVolumeAccept() {};

		bool Validate(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEVolume));};
	};
  mafVolumeAccept *m_VolumeAccept;

protected:
  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;
  mafAxes                 *m_Axes;

  vtkPolyDataNormals      *m_Normals;
  vtkDistanceFilter       *m_DistanceFilter;
  vtkColorTransferFunction *m_Table;
  vtkScalarBarActor       *m_ScalarBar;
  mafNode                 *m_Volume;

  int m_DensityDistance;
  int m_NumSections;
  int m_RenderingDisplayListFlag;
  int m_FirstThreshold;
  int m_SecondThreshold;
  int m_MaxDistance;
	int m_BarTipology;

	double m_Area[3];
	double m_AreaDistance[3];

  wxColour	m_LowColour;
	wxColour	m_MidColour;
	wxColour	m_WhiteColour;
	wxColour	m_MidColour1;
	wxColour	m_MidColour2;
	wxColour	m_HiColour;

  mmgMaterialButton *m_MaterialButton;

  /** 
  Generate texture coordinate for polydata according to the mapping mode*/
  void GenerateTextureMapCoordinate();

  virtual mmgGui  *CreateGui();

  /**
  Update the visual pipeline of the surface*/
  void UpdatePipeline();
};  
#endif // __mafPipeSurface_H__
