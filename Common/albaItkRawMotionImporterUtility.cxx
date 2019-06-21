/*=========================================================================

 Program:   Visualization Toolkit
 Module:    $RCSfile: albaItkRawMotionImporterUtility.cxx,v $
 Language:  C++
 Date:      $Date: 2012-04-06 09:04:52 $
 Version:   $Revision: 1.1.2.2 $

=========================================================================*/
#include "albaItkRawMotionImporterUtility.h"

#include <vcl_fstream.h>

//----------------------------------------------------------------------------
int albaItkRawMotionImporterUtility::ReadMatrix(vnl_matrix<double> &M, const char *fname)
{
  //Read raw motion data
  vcl_ifstream v_raw_matrix(fname, std::ios::in);

	
	if(v_raw_matrix.is_open() != 0)
	{	
		M.read_ascii(v_raw_matrix);
    return 0;
	}

 	return 1;
}
  

