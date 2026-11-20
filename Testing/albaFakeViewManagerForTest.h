/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaFakeViewManagerForTest.h
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _albaFakeViewManagerForTest_H
#define _albaFakeViewManagerForTest_H

#include "albaAbsViewManager.h"

/** View manager registered by the test executables: there are no views, so every service does nothing */
class albaFakeViewManagerForTest : public albaAbsViewManager
{
public:
	virtual void CameraUpdate(bool onlySelected = false) override {}
	virtual void CameraReset(bool fitSelectedVme = false) override {}
	virtual void EnableCameraUpdateSkip() override {}
	virtual void DisableCameraUpdateSkip() override {}
	virtual void PropertyUpdate(bool fromTag = false) override {}
	virtual albaView *GetFromOpenedViews(const char *label) override { return NULL; }
	virtual albaView *ViewCreate(int id) override { return NULL; }
};
#endif
