/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLogicStandardPlugs
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaLogicStandardPlugs_H__
#define __albaLogicStandardPlugs_H__

#include "albaDefines.h"

class albaLogicWithManagers;

/** albaLogicStandardPlugs - plugs the standard ALBA operations and views into a logic.
Kept apart from albaLogicWithManagers so that the logic does not need the includes of every operation and view.
albaLogicWithManagers::PlugStandardOperations and PlugStandardViews call these methods passing themselves. */
class ALBA_EXPORT albaLogicStandardPlugs
{
public:
	/** Plugs the standard operations, importers and exporters included, into logic */
	static void PlugStandardOperations(albaLogicWithManagers *logic);

	/** Plugs the standard views into logic */
	static void PlugStandardViews(albaLogicWithManagers *logic);
};

#endif
