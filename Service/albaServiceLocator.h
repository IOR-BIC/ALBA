/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaServiceLocator
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _albaServiceLocator_H
#define _albaServiceLocator_H

#include "albaAbsViewManager.h"

class albaAbsLogicManager;

class ALBA_EXPORT albaServiceLocator
{
public:

	/** Return the registered logic manager, NULL if none */
	static albaAbsLogicManager* GetLogicManager() { return m_LogicManager; }

	/** Register the logic manager used by all the service clients */
	static void SetLogicManager(albaAbsLogicManager* logic) { m_LogicManager = logic; }

	/** Return the registered view manager, NULL if none */
	static albaAbsViewManager* GetViewManager() { return m_ViewManager; }

	/** Register the view manager used by all the service clients */
	static void SetViewManager(albaAbsViewManager* viewManager) { m_ViewManager = viewManager; }

protected:
	static albaAbsLogicManager* m_LogicManager; ///< the registered logic manager
	static albaAbsViewManager* m_ViewManager;   ///< the registered view manager
};
#endif