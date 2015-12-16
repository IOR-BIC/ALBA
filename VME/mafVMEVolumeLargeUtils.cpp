/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: mafVMEVolumeLargeUtils.cpp,v $ 
  Language: C++ 
  Date: $Date: 2011-06-16 09:15:38 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEVolumeLargeUtils.h"
#include "mafObserver.h"
#include "mafString.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFDataArrayDescriptor.h"
#include "vtkMAFLargeImageData.h"
#include "mafVolumeLargeWriter.h"


/*static*/ bool mafVMEVolumeLargeUtils::IsVolumeLarge(
  int VOI[6], int nDataType, int nNumOfComp, int nMemLimit)
{
  //returns true, if the volume to be imported is larger than
  //the given limit (in MB) and should be processed as VMEVolumeLarge
  vtkIdType64 size = (((vtkIdType64)(VOI[1] - VOI[0] + 1))*
    (VOI[3] - VOI[2] + 1)*(VOI[5] - VOI[4] + 1)*nNumOfComp*
    vtkMAFDataArrayDescriptor::GetDataTypeSize(nDataType)) 
    / (1024 * 1024);  //in MB

  return size >= nMemLimit;
}

/*static*/ mafString mafVMEVolumeLargeUtils::GetBrickedLayoutDir(
  void* sender, mafObserver* m_Listener)
{
  //Get directory into which the volume files should be stored.
  //Typical call: GetBrickedLayoutDir(this, m_Listener)
  //If an error occurs, an empty string is returned.

  //save the .MSF file
  mafEventMacro( mafEvent(sender, MENU_FILE_SAVE));	

  mafVMEVolumeGray* pTempVME = mafVMEVolumeGray::New();  
  pTempVME->Register(sender);   //cannot use mafNEW from a static routine
  mafEventMacro( mafEvent(sender, VME_ADD, pTempVME));

  mafEventIO e(sender, NODE_GET_STORAGE);  
  pTempVME->ForwardUpEvent(e);  

  mafEventMacro( mafEvent(sender, VME_REMOVE, pTempVME));  
  pTempVME->Delete();  

  mafString szStr;
  mafStorage* storage = e.GetStorage();
  if (storage == NULL)
    szStr = mafGetDirName(mafGetLastUserFolder().c_str(), 
    _("Select a folder for optimized volume files")).c_str();
  else
  {
    szStr = storage->GetURL();
    szStr.ExtractPathName();

    wxString szSep = wxFILE_SEP_PATH;
    if (!wxEndsWithPathSeparator(szStr))
      szStr += szSep;
    szStr += "LargeVolumes";	

#pragma warning(suppress: 6031) // warning C6031: Return value ignored: 'wxMkDir'
    ::wxMkDir(szStr);
  }

  return szStr;
}

//BES: 17.7.2008 - moved to mafDecl.h as mafFormatDataSize
////------------------------------------------------------------------------
////Formats the specified size to B, KB, MB or GB
///*static*/ void mafVMEVolumeLargeUtils::FormatDataSize(vtkIdType64 size, mafString& szOut)
////------------------------------------------------------------------------
//{
//  const char* SZUN[] = {"B", "KB", "MB", "GB", NULL};
//  const int LIMITS[] = { 16384, 4096, 1024, INT_MAX};
//
//  int idx = 0;
//  double nsize = (double)size;
//  while (SZUN[idx] != NULL)
//  {
//    if (nsize < LIMITS[idx])
//      break;
//
//    nsize /= 1024;
//    idx++;
//  }
//
//  szOut = wxString::Format("%g %s", RoundValue(nsize), SZUN[idx]);
//}


//------------------------------------------------------------------------
/*static*/ bool mafVMEVolumeLargeUtils::ConfirmVolumeLargeImport()
//------------------------------------------------------------------------
{
  //Displays a warning that the volume to be imported is large
  //Returns true, if the user confirms the import (i.e., it can proceed)
  wxString msg = _("The selected VOI is too large to fit the given memory limit and, therefore, "
    "if you continue, it will be imported as a VolumeLarge VME.\n"
    "NB: VolumeLarge VME does not support all operations that are available for Volume VME. \n"
    "In order to import the data as a Volume VME (small), reduce the number of slices to "
    "be imported or increase the memory limit.\n"
    "\nDo you want to proceed with the import?");

  return wxMessageBox(msg, _("Warning: VolumeLarge VME"), 
    wxYES_NO | wxCENTRE | wxICON_QUESTION) == wxYES;  
}

//------------------------------------------------------------------------
/*static*/ bool mafVMEVolumeLargeUtils
          ::ConfirmVolumeLargeSpaceConsumtion(vtkIdType64 nEstimatedSize)
//------------------------------------------------------------------------
{
  //Displays a warning that the volume to be imported is large
  //Returns true, if the user confirms the import (i.e., it can proceed)
  mafString szSize;
  mafFormatDataSize(nEstimatedSize, szSize);

  return (wxMessageBox(wxString::Format(
    _("The selected VOI will be imported as VolumeLarge VME, \n"
    "which includes the construction of a couple of optimized volume files \n"
    "with the total size up to %s (much less for medical data).\n\n"
    "The current project must be saved before proceeding.\n"
    "Do you want to continue?"), szSize.GetCStr()), 
    _("Confirmation"), wxYES_NO | wxICON_QUESTION) == wxYES);    
}

//------------------------------------------------------------------------
/*static*/ void mafVMEVolumeLargeUtils::DisplayVolumeLargeSpaceConsumtion(vtkIdType64 nRealSize)
//------------------------------------------------------------------------
{
  //Displays a message box with the information about consumed space
  mafString szSize;
  mafFormatDataSize(nRealSize, szSize);

  wxMessageBox(wxString::Format(
    _("An optimised volume file with the total size\n"
    "%s was successfuly constructed."					
    ), szSize.GetCStr()), _("Information"), wxOK | wxICON_INFORMATION);  
}

//------------------------------------------------------------------------
/*static*/ int mafVMEVolumeLargeUtils::VolumeLargeCheck(
      mafObject* caller, mafObserver* listener, int VOI[6], int nDataType, 
      int nNumOfComp, int nMemLimit, mafString& szOutDir)
//------------------------------------------------------------------------
{
  //Performs various checks in order to determine whether the given
  //volume can be imported. It may display one or more dialogs.
  //Returns 0, if the Volume cannot be imported at all,
  //1, if it is small volume and 2, if it is large volume.
  //For large volumes, it returns also path, where the brick files
  //should be placed (see szOutDir)
  if (!IsVolumeLarge(VOI, nDataType, nNumOfComp, nMemLimit))
    return 1; //small volume

  //display warning
  if (!ConfirmVolumeLargeImport())
    return 0;

  vtkMAFLargeImageData* img = vtkMAFLargeImageData::New();
  img->SetScalarType(nDataType);
  img->SetNumberOfScalarComponents(nNumOfComp);
  img->SetExtent(VOI);
  img->SetVOI(VOI);

  mafVolumeLargeWriter wr;
  wr.SetInputDataSet(img);
  img->Delete();

  if (!ConfirmVolumeLargeSpaceConsumtion(wr.GetEstimatedTotalSize()))
    return 0;

  szOutDir = GetBrickedLayoutDir(caller, listener);
  if (szOutDir.IsEmpty())
    return 0;	//cancel

  return 2;
}