/*=========================================================================

 Program: MAF2
 Module: mafPipePolylineSlice
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipePolylineSlice_H__B
#define __mafPipePolylineSlice_H__B

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
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
class vtkMAFTubeFilter;
class vtkPolyData;
class vtkMAFPolyDataToSinglePolyLine;
class vtkMAFToLinearTransform;
class vtkClipPolyData;
class vtkAppendPolyData;

/**
  class name: mafPipePolylineSlice.
  Pipe which manage the visualisation of the section of a polyline.
*/
class MAF_EXPORT mafPipePolylineSlice : public mafPipeSlice
{
public:
  /** RTTI Macro*/
  mafTypeMacro(mafPipePolylineSlice,mafPipeSlice);

  /** Constructor. */
               mafPipePolylineSlice();
  /** Destructor. */
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

  /** Pipe creation */
  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  /** called if vme is selected  */
  virtual void Select(bool select); 
  
  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  

  /** Set spline mode of the polyline */
  void SetSplineMode(int flag){m_SplineMode = flag;};
  /** Get spline mode of the polyline */
  int GetSplineMode(){return m_SplineMode;};

  /** Set spline mode of the polyline  to ON */
  void SplineModeOn(){SetSplineMode(1);UpdateProperty();};
  /** Set spline mode of the polyline  to OFF*/
  void SplineModeOff(){SetSplineMode(0);UpdateProperty();};

  /** Set fill variable of the polyline */
  void SetFill(int flag){m_Fill = flag;};
  /** Get fill variable of the polyline */
  int GetFill(){return m_Fill;};
  
  /** Set fill variable of the polyline to ON */
  void FillOn(){SetFill(1);UpdateProperty();};
  /** Set fill variable of the polyline to OFF*/
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

  /** gui creation */
  virtual mafGUI  *CreateGui();
  /** update all properties of the pipe */
  void UpdateProperty();

  /** enable the check on a region of interest */
  void ROIEnable(bool enable){m_RoiEnable = enable;}
  /** define a region of interest  in which the section is visible */
  void SetROI(double bounds[6]);
  /** set region of interests as maximum */
  void SetMaximumROI();

  /**Set if actor is pickable */
  void SetActorPicking(int enable);

protected:
  /** cap the regions*/
  vtkPolyData *RegionsCapping(vtkPolyData* inputCutters);
  /** apply a capping filter  */
  vtkPolyData *CappingFilter(vtkPolyData* inputBorder);
  /** execute filter which apply region of interest*/
  vtkPolyData *ExecuteROI(vtkPolyData *polydata);

  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;
	vtkMAFTubeFilter           *m_Tube;
	vtkMAFTubeFilter           *m_TubeRadial;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  mafAxes                 *m_Axes;
  vtkPlane				        *m_Plane;
  vtkMAFFixedCutter		      *m_Cutter;
  vtkPolyData             *m_PolyFilteredLine;
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
