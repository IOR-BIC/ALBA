/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: mafVMEVolumeLargeUtils.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 09:50:16 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef mafVMEVolumeLargeUtils_h__
#define mafVMEVolumeLargeUtils_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "vtkMafFile.h"
#include "vtkMAFIdType64.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class mafObject;
class mafObserver;
class mafString;
/**
  class name: mafVMEVolumeLargeUtils
  the class that gather several static methods usefull for import a large volume dataset.
*/
class MAF_EXPORT mafVMEVolumeLargeUtils
{
public:
  /** Returns the size of the given file in bytes */
  inline static vtkIdType64 GetFileLength(const char * filename) {
    return (vtkIdType64)vtkMAFFile::GetFileSize(filename);
  }

  //returns true, if the file is large and must be imported as large volume
  //NB. standard VTK reading processes does not support large files
  inline static bool IsFileLarge(const char * filename) {
    return GetFileLength(filename) > INT_MAX;
  }

  //returns true, if the volume to be imported is larger than
  //the given limit (in MB) and should be processed as VMEVolumeLarge
  static bool IsVolumeLarge(int VOI[6], int nDataType, 
    int nNumOfComp, int nMemLimit);

  /** 
  Performs various checks in order to determine whether the given
  volume can be imported. It may display one or more dialogs.
  Returns 0, if the Volume cannot be imported at all,
  1, if it is small volume and 2, if it is large volume.
  For large volumes, it returns also path, where the brick files
  should be placed (see szOutDir) */
  static int VolumeLargeCheck(mafObject* caller, mafObserver* listener,
    int VOI[6], int nDataType, int nNumOfComp, 
    int nMemLimit, mafString& szOutDir);

  /** Get directory into which the volume files should be stored.
  Typical call: GetBrickedLayoutDir(this, m_Listener)
  If an error occurs, an empty string is returned.*/
  static mafString GetBrickedLayoutDir(void* sender, 
    mafObserver* listener);

  //BES: 17.7.2008 - moved to mafDecl.h as mafFormatDataSize
  ///** Formats the specified size to B, KB, MB or GB */
  //static void FormatDataSize(vtkIdType64 size, mafString& szOut);  

  /** Displays a warning that the volume to be imported is large
  Returns true, if the user confirms the import (i.e., it can proceed) */
  static bool ConfirmVolumeLargeImport();

  /** Displays a warning that the volume to be imported is large
  Returns true, if the user confirms the import (i.e., it can proceed) */
  static bool ConfirmVolumeLargeSpaceConsumtion(vtkIdType64 nEstimatedSize);

  /** Displays a message box with the information about consumed space */
  static void DisplayVolumeLargeSpaceConsumtion(vtkIdType64 nRealSize);
};

#endif // mafVMEVolumeLargeUtils_h__