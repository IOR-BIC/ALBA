/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFIdType64.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:05:44 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#pragma  once

#ifdef VTK_USE_64BIT_IDS
typedef vtkIdType vtkIdType64;
#else


# ifdef _WIN32
typedef __int64 vtkIdType64;
# else // _WIN32
typedef long long vtkIdType64;
# endif // _WIN32

#if defined(_MSC_VER) && (_MSC_VER < 1300)
# if !defined(VTK_NO_INT64_OSTREAM_OPERATOR)
VTK_COMMON_EXPORT ostream& vtkIdTypeOutput(ostream& os, __int64 id);
inline ostream& operator << (ostream& os, __int64 id)
{
	return vtkIdTypeOutput(os, id);
}
# endif
# if !defined(VTK_NO_INT64_ISTREAM_OPERATOR)
VTK_COMMON_EXPORT istream& vtkIdTypeInput(istream& is, __int64& id);
inline istream& operator >> (istream& is, __int64& id)
{
	return vtkIdTypeInput(is, id);
}
# endif
#endif

#endif
