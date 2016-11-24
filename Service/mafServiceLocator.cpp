/*=========================================================================

 Program: MAF2
 Module:  mafServiceLocator
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafServiceLocator.h"

static mafAbsLogicManager* glo_logic;
//-------------------------------------------------------------------------
mafAbsLogicManager* mafServiceLocator::GetLogicManager()
{
	return glo_logic;
}

//-------------------------------------------------------------------------
void mafServiceLocator::SetLogicManager(mafAbsLogicManager* logic)
{
	glo_logic = logic;
}
