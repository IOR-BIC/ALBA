/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterImage
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterImage_H__
#define __albaOpImporterImage_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include <string>
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaVMEImage;
class albaVMEVolumeRGB;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,std::string);
#endif

//----------------------------------------------------------------------------
// albaOpImporterImage :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpImporterImage: public albaOp
{
public:
  albaOpImporterImage(const wxString &label = "ImageImporter");
 ~albaOpImporterImage(); 
	virtual void OnEvent(albaEventBase *alba_event);
  
  albaTypeMacro(albaOpImporterImage, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .stl to import */
  void SetFileName(const char *file_name);
	  
  void SetBuildVolumeFlag(bool buildVolumeFlag) {m_BuildVolumeFlag = buildVolumeFlag;};
  void SetFilePrefix(albaString filePrefix){m_FilePrefix = filePrefix;};  
  void SetFilePattern(albaString filePattern){m_FilePattern = filePattern;};  
  void SetFileExtension(albaString fileExtension){m_FileExtension = fileExtension;};  
  void SetFileOffset(int fileOffset){m_FileOffset = fileOffset;};  
  void SetFileSpacing(int fileSpacing){m_FileSpacing = fileSpacing;};
  void SetImageZSpacing(double imageZSpacing){m_ImageZSpacing = imageZSpacing;};

  void ImportImage();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);
	  
  void BuildImageSequence();
  void BuildVolume();

  albaVMEImage     *m_ImportedImage;
  albaVMEVolumeRGB *m_ImportedImageAsVolume;

  std::vector<std::string>	m_Files;
	wxString      m_FileDirectory;
  wxString      m_FilePrefix;
  wxString      m_FilePattern;
  wxString      m_FileExtension;
  int           m_FileOffset;
  int           m_FileSpacing;
  int           m_NumFiles;
  int           m_BuildVolumeFlag;
  double        m_ImageZSpacing;
};
#endif
