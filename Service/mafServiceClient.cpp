/*=========================================================================

 Program: MAF2
 Module:  mafServiceClient.cpp
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafServiceClient.h"
#include "mafAbsLogicManager.h"
#include "mafServiceLocator.h"

mafServiceClient::mafServiceClient()
{
	m_logic = NULL;
}

//-------------------------------------------------------------------------
mafAbsLogicManager* mafServiceClient::GetLogicManager()
{
	if (m_logic == NULL)
		m_logic = mafServiceLocator::GetLogicManager();

	return m_logic;
}

