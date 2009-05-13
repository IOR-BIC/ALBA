/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolylineSlice_BES.h,v $
  Language:  C++
  Date:      $Date: 2009-05-13 15:48:46 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipePolylineSlice_H__B
#define __mafPipePolylineSlice_H__B

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipeSlice.h"

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
// mafPipePolylineSlice_BES :
//----------------------------------------------------------------------------
class mafPipePolylineSlice_BES : public mafPipeSlice
{
public:
  mafTypeMacro(mafPipePolylineSlice_BES,mafPipeSlice);

               mafPipePolylineSlice_BES();
  virtual     ~mafPipePolylineSlice_BES ();

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
  
  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  

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
  double  m_SplineCoefficient;

  vtkPolyData *m_PolyData;
  vtkAppendPolyData * m_AppendPolyData;

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;

  int m_RoiEnable;
  double  m_ROI[6];
  
};  
#endif // __mafPipePolylineSlice_H__B
