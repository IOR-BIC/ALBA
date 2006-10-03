/*=========================================================================

 Program:   Visualization Toolkit
 Module:    $RCSfile: itkRawMotionImporterUtility.cxx,v $
 Language:  C++
 Date:      $Date: 2006-10-03 13:20:48 $
 Version:   $Revision: 1.1 $

=========================================================================*/
#include "itkRawMotionImporterUtility.h"

#include <vcl_fstream.h>

//----------------------------------------------------------------------------
int itkRawMotionImporterUtility::ReadMatrix(vnl_matrix<double> &M, const char *fname)
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
  

