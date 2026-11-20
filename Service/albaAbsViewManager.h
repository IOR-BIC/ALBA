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

	/** Update the visual properties of all the views (from the VMEs tags if fromTag is true) */
	virtual void PropertyUpdate(bool fromTag = false) = 0;

	/** Return the open view with the given label, NULL if none */
	virtual albaView *GetFromOpenedViews(const char *label) = 0;

	/** Create the view at index id of the plugged view-list */
	virtual albaView *ViewCreate(int id) = 0;
};
#endif
