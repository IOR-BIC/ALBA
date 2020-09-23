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

class albaAbsLogicManager;

class ALBA_EXPORT albaServiceLocator
{
public:

	static albaAbsLogicManager* GetLogicManager();
	static void SetLogicManager(albaAbsLogicManager* logic);
};
#endif