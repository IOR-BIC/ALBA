/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaServiceClient.h
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _albaServiceClient_H
#define _albaServiceClient_H

#include "albaServiceLocator.h"

class albaAbsLogicManager;

/** Base class of the objects that use the services registered in albaServiceLocator */
class ALBA_EXPORT albaServiceClient
{
public:
	/** Return the registered logic manager, NULL if none */
	albaAbsLogicManager* GetLogicManager() { return albaServiceLocator::GetLogicManager(); }

	/** Return the registered view manager, NULL if none */
	albaAbsViewManager* GetViewManager() { return albaServiceLocator::GetViewManager(); }

	/** Return the registered help manager, NULL if the application has no help */
	albaAbsHelpManager* GetHelpManager() { return albaServiceLocator::GetHelpManager(); }
};
#endif