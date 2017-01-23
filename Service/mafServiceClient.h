/*=========================================================================

 Program: MAF2
 Module:  mafServiceClient.h
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _mafServiceClient_H
#define _mafServiceClient_H

class mafAbsLogicManager;

class mafServiceClient
{
	public:
		mafServiceClient();
	
		mafAbsLogicManager* GetLogicManager();

  protected:
		mafAbsLogicManager* m_logic;
};
#endif