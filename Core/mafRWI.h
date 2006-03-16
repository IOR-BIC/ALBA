/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRWI.h,v $
  Language:  C++
  Date:      $Date: 2006-03-16 09:17:22 $
  Version:   $Revision: 1.10 $
  Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafRWI_H__
#define __mafRWI_H__
//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafRWIBase.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------
class mafNode;
class vtkGridActor;
class vtkLight;
class vtkCamera;
class vtkRenderer;
class vtkRenderWindow;
class mafRWIBase;
class mafSceneGraph;
class mafAxes;
class mmgGui;
class mmgPicButton;
class vtkSimpleRulerActor2D;

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------
enum RWI_LAYERS
{
	ONE_LAYER =0,
  TWO_LAYER
};
//----------------------------------------------------------------------------
// mafRWI :
//----------------------------------------------------------------------------
class mafRWI : public mafObserver
{
public:
										mafRWI(wxWindow *parent, RWI_LAYERS layers = ONE_LAYER, bool use_grid = false, bool show_axes = false, bool show_ruler = false, int stereo = 0);
	virtual					 ~mafRWI();

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  void OnEvent(mafEventBase *maf_event);

	/** Reset the camera position. If vme is passed as parameter, the camera is resetted to fill the vme into the view. */
	void CameraReset(mafNode *vme = NULL);

	/** Reset the camera position according to the bounds. */
	void CameraReset(double bounds[6]);
  
	/** Update the renderwindow. */
	void CameraUpdate();

	/** Set the camera position. */
	void CameraSet(int cam_position);

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
	void SetGridColor(wxColor col);

	/** Set the background color for the render window. */
	void SetBackgroundColor(wxColor col);

	/** Set the stereo type. */
	void SetStereo(int stereo_type);

	/** Set the visibility for the axes actor. */
	void SetAxesVisibility(bool show);

  /** Set the visibility for the rule actor. */
  void SetRuleVisibility(bool show = true);

  /** Set the scale factor to convert the data scale to the visualized scale of the rule. 
      By default the ruler shows the same scale of the data. */
  void SetRulerScaleFactor(double scale_factor);

  /** Set the label of the ruler. Example the unit measure of the data. */
  void SetRulerLegend(mafString &ruler_legend);

  mmgGui *GetGui();

  void LinkCamera(bool linc_camera = true);

  mafSceneGraph    *m_Sg; 
  mafRWIBase			 *m_RwiBase;
  vtkRenderer      *m_RenFront; ///< Renderer used to show actors on the first layer.
  vtkRenderer      *m_RenBack; ///< Renderer used to show actors on the second layer.
  vtkRenderWindow  *m_RenderWindow;
  vtkLight         *m_Light;
  vtkCamera				 *m_Camera;
  int               m_CameraPosition; ///< Used to store camera position ID.
  
protected:
	/** Compute the bounds for the visible actors; if vme is passed, the bounds of vme are calculated. */
	double *ComputeVisibleBounds(mafNode *node = NULL);
  mmgGui *CreateGui();

  mmgGui       *m_Gui;
  wxColour	    m_BGColour;
  wxColour	    m_GridColour;
  double        m_GridPosition;
  mmgPicButton *m_CameraButtons[6];
  wxBoxSizer	 *m_Sizer;
	
  mafAxes          *m_Axes; ///< Actor representing a global reference system.
  vtkGridActor     *m_Grid; ///< Actor representing a grid showed into the render window.
  int               m_ShowGrid; ///< Flag used to show/hide the grid.
  int               m_GridNormal;
  int               m_ShowAxes;  ///< Flag used to show/hide axes in low left corner of the view
  int               m_LinkCamera;///< Flag that enable to synchronize camera interaction to other camera

  double        m_RulerScaleFactor;
  mafString     m_RulerLegend;
  vtkSimpleRulerActor2D *m_Ruler;
  int           m_ShowRuler; ///< Flag used to show/hide ruler actor into a parallel view
  int           m_StereoType;
  mafObserver  *m_Listener;

  mafString m_StereoMovieDir;
  int       m_StereoMovieEnable;
};
#endif
