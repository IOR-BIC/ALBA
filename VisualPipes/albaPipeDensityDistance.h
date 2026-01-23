/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeDensityDistance
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeDensityDistance_H__
#define __albaPipeDensityDistance_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"
#include "albaVMEImage.h"
#include "albaVMEVolume.h"
#include "vtkPolyDataNormals.h"
#include "vtkALBADistanceFilter.h"
#include "vtkPolyDataMapper.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkTexture;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class vtkColorTransferFunction;
class vtkScalarBarActor;
class vtkTriangleFilter;
class albaAxes;
class albaGUIMaterialButton;
class albaVME;
class albaVMEVolume;

//----------------------------------------------------------------------------
// albaPipeSurface :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeDensityDistance : public albaPipe
{
public:
  albaTypeMacro(albaPipeDensityDistance,albaPipe);

               albaPipeDensityDistance();
  virtual     ~albaPipeDensityDistance ();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  virtual void Create(albaSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  void SetVolume(albaVME* volume);

  void SetDistanceMode(){m_DensityDistance=0;UpdatePipeline();};
  void SetDiscreteDensityMode(){m_BarTipology=0;m_DensityDistance=1;UpdatePipeline();};
  void SetContinuosDensityMode(){m_BarTipology=1;m_DensityDistance=1;UpdatePipeline();};
  void SetFirstThreshold(int firstThreshold){m_FirstThreshold=firstThreshold;UpdatePipeline();};
  void SetSecondThreshold(int secondThreshold){m_SecondThreshold=secondThreshold;UpdatePipeline();};
  void SetMaxDistance(int maxDistance){m_MaxDistance=maxDistance;UpdatePipeline();};

  void EnableMAPSFilterOff();
  void EnableMAPSFilterOn();
  void EnableMAPSFilter(bool enable);
  bool GetEnableMAPSFilter(){return m_EnableMAPSFilter;};

  double* GetDensityArea(){return m_Area;};
  double* GetDistanceArea(){return m_AreaDistance;};

  double GetTotalArea();

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

  static bool VolumeAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVMEVolume));};

  /**
  Update the visual pipeline of the surface*/
  void UpdatePipeline();

protected:
  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;
  albaAxes                 *m_Axes;
  vtkTriangleFilter       *m_TriangleFilter;
  vtkPolyDataNormals      *m_Normals;
  vtkALBADistanceFilter       *m_DistanceFilter;
  vtkColorTransferFunction *m_Table;
  vtkScalarBarActor       *m_ScalarBar;
  albaVME                 *m_Volume;

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

  albaGUIMaterialButton *m_MaterialButton;

  bool m_EnableMAPSFilter;

  /** 
  Generate texture coordinate for polydata according to the mapping mode*/
  void GenerateTextureMapCoordinate();

  virtual albaGUI  *CreateGui();
};  
#endif // __albaPipeSurface_H__
