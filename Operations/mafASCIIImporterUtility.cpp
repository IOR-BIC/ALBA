/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafASCIIImporterUtility.cpp,v $
Language:  C++
Date:      $Date: 2006-05-18 10:29:00 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafASCIIImporterUtility.h"

#include <vcl_fstream.h>

//----------------------------------------------------------------------------
int mafASCIIImporterUtility::ReadMatrix(vnl_matrix<double> &M, const char *fname)
//----------------------------------------------------------------------------
{
  //Read raw motion data
  vcl_ifstream v_raw_matrix(fname, std::ios::in);

	if(v_raw_matrix.is_open() != 0)
	{	
		M.read_ascii(v_raw_matrix);
    return MAF_OK;
	}
 	return MAF_ERROR;
}
