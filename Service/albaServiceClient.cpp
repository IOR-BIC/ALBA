/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaServiceClient.cpp
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"
#include "albaServiceLocator.h"

//-------------------------------------------------------------------------
albaServiceClient::albaServiceClient()
{
	m_Logic = NULL;
}

//-------------------------------------------------------------------------
albaAbsLogicManager* albaServiceClient::GetLogicManager()
{
	if (m_Logic == NULL)
		m_Logic = albaServiceLocator::GetLogicManager();

	return m_Logic;
}

