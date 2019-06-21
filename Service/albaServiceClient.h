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

class albaAbsLogicManager;

class albaServiceClient
{
	public:
		albaServiceClient();
	
		albaAbsLogicManager* GetLogicManager();

  protected:
		albaAbsLogicManager* m_Logic;
};
#endif