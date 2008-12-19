/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolylineSlice.h,v $
  Language:  C++
  Date:      $Date: 2008-12-19 16:05:47 $
  Version:   $Revision: 1.11.2.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipePolylineSlice_H__
#define __mafPipePolylineSlice_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"


//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class mafAxes;
class vtkMAFFixedCutter;
class vtkPlane;
class vtkTubeFilter;
class vtkPolyData;
class vtkMAFPolyDataToSinglePolyLine;
class vtkMAFToLinearTransform;
class vtkClipPolyData;
class vtkAppendPolyData;

//----------------------------------------------------------------------------
// mafPipePolylineSlice :
//----------------------------------------------------------------------------
class mafPipePolylineSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipePolylineSlice,mafPipe);

               mafPipePolylineSlice();
  virtual     ~mafPipePolylineSlice ();

  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /**Return the radius of the tube*/	
  double GetRadius();

  /**Set the radius value of the tube*/
  void SetRadius(double radius); 

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  void SetSlice(double *Origin);

	void SetNormal(double *Normal);

  /** Set spline mode of the polyline */
  void SetSplineMode(int flag){m_SplineMode = flag;};
  int GetSplineMode(){return m_SplineMode;};

  void SplineModeOn(){SetSplineMode(1);UpdateProperty();};
  void SplineModeOff(){SetSplineMode(0);UpdateProperty();};

  /** Set spline mode of the polyline */
  void SetFill(int flag){m_Fill = flag;};
  int GetFill(){return m_Fill;};

  void FillOn(){SetFill(1);UpdateProperty();};
  void FillOff(){SetFill(0);UpdateProperty();};

  /** Function that retrieve a spline polyline when  input is a polyline */ 
  vtkPolyData *SplineProcess(vtkPolyData *polyData);

  /** Show actor of sliced polyline*/
  void ShowActorOn();

  /** Hide actor of sliced polyline*/
  void ShowActorOff();

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_CHOOSE_TEXTURE,
    ID_TEXTURE_MAPPING_MODE,
    ID_BORDER_CHANGE,
    ID_RADIUS_CHANGE,
    ID_SPLINE,
    ID_FILL,
    ID_LAST,
  };

  virtual mafGUI  *CreateGui();
  void UpdateProperty();

  void ROIEnable(bool enable){m_RoiEnable = enable;}
  void SetROI(double bounds[6]);
  void SetMaximumROI();

protected:

  vtkPolyData *RegionsCapping(vtkPolyData* inputCutters);
  vtkPolyData *CappingFilter(vtkPolyData* inputBorder);
  vtkPolyData *ExecuteROI(vtkPolyData *polydata);

  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;
	vtkTubeFilter           *m_Tube;
	vtkTubeFilter           *m_TubeRadial;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  mafAxes                 *m_Axes;
  vtkPlane				        *m_Plane;
  vtkMAFFixedCutter		      *m_Cutter;
  vtkPolyData             *m_PolySpline;
  vtkMAFPolyDataToSinglePolyLine *m_PolydataToPolylineFilter;
  vtkMAFToLinearTransform *m_VTKTransform;
  double				           m_Border;
  double                   m_Radius;
  int                      m_SplineMode;
  int                      m_Fill;

  vtkClipPolyData          *m_ClipPolyData;
  vtkClipPolyData          *m_ClipPolyDataUp;
  vtkClipPolyData          *m_ClipPolyDataDown;

  double	m_Origin[3];
  double	m_Normal[3];
  double  m_SplineCoefficient;
  
  vtkPolyData *m_PolyData;
  vtkAppendPolyData * m_AppendPolyData;

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;

  int m_RoiEnable;
  double  m_ROI[6];
  
};  
#endif // __mafPipePolylineSlice_H__
