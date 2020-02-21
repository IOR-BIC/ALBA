/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRWI
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaRWI_H__
#define __albaRWI_H__
//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaRWIBase.h"
#include "albaObserver.h"
#include "albaAxes.h"

//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------
class albaVME;
class vtkALBAGridActor;
class vtkLight;
class vtkCamera;
class vtkRenderer;
class vtkRenderWindow;
class albaRWIBase;
class albaSceneGraph;
class albaAxes;
class albaGUI;
class albaGUIPicButton;
class vtkALBATextOrientator;
class vtkALBAProfilingActor;

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------
enum RWI_LAYERS
{
	ONE_LAYER = 0,
	TWO_LAYER
};
//----------------------------------------------------------------------------
// albaRWI :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaRWI : public albaObserver
{
public:
	albaRWI();
	albaRWI(wxWindow *parent, RWI_LAYERS layers = ONE_LAYER, bool use_grid = false, bool show_axes = false, int stereo = 0, bool show_orientator = false, int axesType = albaAxes::TRIAD) ;
	virtual	~albaRWI();

	virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};

	virtual void OnEvent(albaEventBase *alba_event);

	/** Create all the elements necessary to build the rendering scene.*/
	void CreateRenderingScene(wxWindow *parent, RWI_LAYERS layers = ONE_LAYER, bool use_grid = false, bool show_axes = false,  int stereo = 0, bool show_orientator = false, int axesType = albaAxes::TRIAD);

	/** Reset the camera position. If vme is passed as parameter, the camera is resetted to fill the vme into the view. */
	void CameraReset(albaVME *vme = NULL, double zoom = 1);

	/** Reset the camera position according to the bounds. */
	void CameraReset(double bounds[6], double zoom = 1);

	/** Update the renderwindow. */
	void CameraUpdate();

	/** Set the camera position. */
	void CameraSet(int cam_position, double zoom = 1);


	/** Set the camera position, set custom pos , custom viewUp , zoom and if projection is parallel */
	void CameraSet(double pos[3],double viewUp[3], double zoom = 1., bool parallelProjection = false);

	/** Set the render window's size. */
	void SetSize(int x, int y, int w,int h);

	/** Show the render window. */
	void Show(bool show);

	/** Set the normal for the grid actor. */
	void SetGridNormal(int normal_id);

	/** Set the position for the grid actor. */
	void SetGridPosition(double position);

	/** Set the visibility for the grid actor. */
	void SetGridVisibility(bool show);

	/** Set the color for the grid actor. */
	void SetGridColor(const wxColor &col);

	/** Set the background color for the render window. */
	void SetBackgroundColor(const wxColor &col);

	/** Get the background color for the render window. */
	wxColor GetBackgroundColor() { return m_BGColour; };

	/** Set the stereo type. */
	void SetStereo(int stereo_type);

	/** Set the visibility for the axes actor. */
	void SetAxesVisibility(bool show);

	/** Set the visibility for the orientation actor. */
	void SetOrientatorVisibility(bool show = true);

	/** Set the visibility for the orientation single actor. */
	void SetOrientatorSingleActorVisibility(bool showLeft = true, bool showDown = true, bool showRight = true, bool showUp = true);

	/** Set the text for the orientator actor. */
	void SetOrientatorSingleActorText(const char* textLeft, const char* textDown, const char* textRight, const char* textUp);

	/** Set Orientator Text Properties */
	void SetOrientatorProperties(double rgbText[3], double rgbBackground[3], double scale = 1);

	/** Set Profiling Information Text Visibility*/
	void SetProfilingActorVisibility(bool show);

	virtual albaGUI *GetGui();

	/** Update member variables and GUI elements according to vtkCamera position, focal point, view up, ...*/
	void UpdateCameraParameters();

	/* Reset camera clipping range using visualized actors */
  void ResetCameraClippingRange();

	albaSceneGraph    *m_Sg; 
	albaRWIBase			 *m_RwiBase;
	vtkRenderer      *m_RenFront; ///< Renderer used to show actors on the first layer
	vtkRenderer      *m_RenBack; ///< Renderer used to show actors on the second layer
	vtkRenderer		 *m_AlwaysVisibleRenderer; /// < Renderer used to superimpose utility stuff to main render window
	vtkRenderWindow  *m_RenderWindow;
	vtkLight         *m_Light;
	vtkCamera				 *m_Camera;
	int               m_CameraPositionId; ///< Integer representing a preset for camera position, focal point and view up
	double            m_CameraPosition[3]; ///< Vector representing the camera position
	double            m_FocalPoint[3]; ///< Vector representing the camera focal point

	double            m_CameraViewUp[3]; ///< Vector representing the camera view-up
	double            m_CameraOrientation[3]; ///< Vector representing the camera orientation

	double            m_StepCameraOrientation; ///< Step with which rotate the camera around its focal point.
	double            m_TopBottomAccumulation;
	double            m_LeftRigthAccumulation; 
	double            m_TopBottomAccumulationLast;
	double            m_LeftRigthAccumulationLast; 

protected:
	/** Compute the bounds for the visible actors; if vme is passed, the bounds of vme are calculated. */
	double *ComputeVisibleBounds(albaVME *vme = NULL);
	albaGUI *CreateGui();

	albaGUI       *m_Gui;
	wxColour	    m_BGColour;
	wxColour	    m_GridColour;
	double        m_GridPosition;
	albaGUIPicButton *m_CameraButtons[6];
	wxBoxSizer	 *m_Sizer;

	albaAxes          *m_Axes; ///< Actor representing a global reference system.
	vtkALBAGridActor     *m_Grid; ///< Actor representing a grid showed into the render window.
	int               m_ShowGrid; ///< Flag used to show/hide the grid.
	int               m_GridNormal;
	int               m_ShowAxes;  ///< Flag used to show/hide axes in low left corner of the view

	int								m_ParallelView; ///< Flag used to set Camera Mode Orthographic or Perspective

	int           m_StereoType;
	vtkALBATextOrientator     *m_Orientator;
	int          m_ShowProfilingInformation;
	int          m_ShowOrientator;
	albaObserver  *m_Listener;


	vtkALBAProfilingActor *m_ProfilingActor;

	albaString m_StereoMovieDir;
	int       m_StereoMovieEnable;
	int       m_AxesType;
};
#endif
