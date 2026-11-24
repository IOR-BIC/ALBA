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
#include "albaAbsHelpManager.h"
#include "albaAbsOpManager.h"

class albaAbsLogicManager;
class albaPositionalEventRouter;

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

	/** Return the registered help manager, NULL if the application has no help */
	static albaAbsHelpManager* GetHelpManager() { return m_HelpManager; }

	/** Register the help manager used by all the service clients */
	static void SetHelpManager(albaAbsHelpManager* helpManager) { m_HelpManager = helpManager; }

	/** Return the registered operation manager, NULL if the application has none */
	static albaAbsOpManager* GetOpManager() { return m_OpManager; }

	/** Register the operation manager used by all the service clients */
	static void SetOpManager(albaAbsOpManager* opManager) { m_OpManager = opManager; }

	/** Return the router of the mouse events of the render windows, NULL if the application has no interaction */
	static albaPositionalEventRouter* GetPositionalEventRouter() { return m_PositionalEventRouter; }

	/** Register the router of the mouse events (done by the interaction manager) */
	static void SetPositionalEventRouter(albaPositionalEventRouter* per) { m_PositionalEventRouter = per; }

protected:
	static albaAbsLogicManager* m_LogicManager; ///< the registered logic manager
	static albaAbsViewManager* m_ViewManager;   ///< the registered view manager
	static albaAbsHelpManager* m_HelpManager;   ///< the registered help manager
	static albaAbsOpManager* m_OpManager;       ///< the registered operation manager
	static albaPositionalEventRouter* m_PositionalEventRouter; ///< the registered router of the mouse events
};
#endif