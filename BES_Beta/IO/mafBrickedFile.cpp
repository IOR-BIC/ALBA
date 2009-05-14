/**=======================================================================
  
  File:    	 mafBrickedFile.cpp
  Language:  C++
  Date:      11:2:2008   12:18
  Version:   $Revision: 1.1.2.1 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
This abstract class contains the stuff common to both, the reader and writer
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafBrickedFile.h"

mafCxxAbstractTypeMacro(mafBrickedFile);
#include "mafMemDbg.h"

//ctor
mafBrickedFile::mafBrickedFile()
{
	m_pMainIdxTable = NULL;
	m_pExIdxTable = NULL;
	m_pLowResLevel = NULL;
  m_BrickFile = NULL;
	m_Listener = NULL;  
  m_pXYZCoords[0] = NULL;
  m_pXYZCoords[1] = NULL;
  m_pXYZCoords[2] = NULL;
}

mafBrickedFile::~mafBrickedFile()
{
	DeallocateBuffers();

  if (m_BrickFile != NULL)
  {
	  m_BrickFile->Close();
    m_BrickFile->Delete();
  }
}