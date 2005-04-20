/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRWI.h,v $
  Language:  C++
  Date:      $Date: 2005-04-20 10:02:58 $
  Version:   $Revision: 1.2 $
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
class mafEventListener;
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
class mafRWI
{
public:
										mafRWI(wxWindow *parent, RWI_LAYERS layers = ONE_LAYER, bool use_grid = false, int stereo = 0);
	virtual					 ~mafRWI();

  virtual void SetListener(mafEventListener *Listener) {m_Listener = Listener;};

	/** Reset the camera position. If vme is passed as parameter, the camera is resetted to fill the vme into the view. */
	void CameraReset(mafNode *vme = NULL);

	/** Reset the camera position according to the bounds. */
	void CameraReset(float bounds[6]);
  
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
	void SetGridPosition(float position);

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

  mafSceneGraph    *m_sg; 
  mafRWIBase			 *m_rwi;
  vtkRenderer      *m_r1; ///< Renderer used to show actors on the first layer.
  vtkRenderer      *m_r2; ///< Renderer used to show actors on the second layer.
  vtkRenderWindow  *m_rw;
  vtkLight         *m_l;
  vtkCamera				 *m_c;
  int               m_cam_position; ///< Used to store camera position ID.
  
	mafAxes          *m_axes; ///< Actor representing a global reference system.
	vtkGridActor     *m_grid; ///< Actor representing a grid showed into the render window.
	bool              m_show_grid; ///< Flag used to show/hide the grid.

protected:
	/** Compute the bounds for the visible actors; if vme is passed, the bounds of vme are calculated. */
	float* ComputeVisibleBounds(mafNode *vme = NULL);
	
  int               m_stereo_type;

  mafEventListener *m_Listener;	
};
#endif
