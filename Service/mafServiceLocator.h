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

#ifndef _mafServiceLocator_H
#define _mafServiceLocator_H

class mafAbsLogicManager;

class mafServiceLocator
{
public:
	 static mafAbsLogicManager* GetLogicManager();
	 static void SetLogicManager(mafAbsLogicManager* logic);

  protected:
};
#endif