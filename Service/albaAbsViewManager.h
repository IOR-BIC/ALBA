/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaAbsViewManager.h
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _albaAbsViewManager_H
#define _albaAbsViewManager_H

#include "albaConfigure.h"

class albaView;
class albaVME;

/** Services of the view manager, reached through albaServiceClient::GetViewManager() */
class ALBA_EXPORT albaAbsViewManager
{
public:
	/** Update the camera of all the views, or only of the selected one; postponed while the camera update skip is enabled */
	virtual void CameraUpdate(bool onlySelected = false) = 0;

	/** Reset the camera of the selected view to fit the selected VME (fitSelectedVme = true) or all */
	virtual void CameraReset(bool fitSelectedVme = false) = 0;

	/** Postpone the camera updates until the matching DisableCameraUpdateSkip; calls can be nested. Useful to avoid flickering or incomplete visualizations */
	virtual void EnableCameraUpdateSkip() = 0;

	/** End a camera update skip: the outermost one updates the cameras if an update was requested meanwhile */
	virtual void DisableCameraUpdateSkip() = 0;

	/** Update the visual properties of vme (the selected VME if NULL) in all the views, from its tags if fromTag is true */
	virtual void PropertyUpdate(albaVME *vme = NULL, bool fromTag = false) = 0;

	/** Return the open view with the given label, NULL if none */
	virtual albaView *GetFromOpenedViews(const char *label) = 0;

	/** Create the view at index id of the plugged view-list */
	virtual albaView *ViewCreate(int id) = 0;

	/** Remove the view from the view-list and delete it (called by its frame when it is closed) */
	virtual void ViewDelete(albaView *view) = 0;

	/** Move and resize the frame of the open view with the given label */
	virtual void ViewResize(const char *label, int x, int y, int width, int height) = 0;

	/** Activate the view with the given label, creating it if it is not open */
	virtual void ViewOpen(const char *label) = 0;

	/** Delete the view with the given label (if open) and destroy its frame */
	virtual void ViewClose(const char *label) = 0;

	/** Return the selected view, NULL if none */
	virtual albaView *GetSelectedView() = 0;
};
#endif
