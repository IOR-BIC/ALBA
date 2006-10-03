/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoImageImporter.h,v $
  Language:  C++
  Date:      $Date: 2006-10-03 13:24:07 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani     
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoImageImporter_H__
#define __mmoImageImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <string>
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafVMEImage;
class mafVMEVolumeRGB;

//----------------------------------------------------------------------------
// mmoImageImporter :
//----------------------------------------------------------------------------
/** */
class mmoImageImporter: public mafOp
{
public:
  mmoImageImporter(wxString label = "ImageImporter");
 ~mmoImageImporter(); 
	virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mmoImageImporter, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .stl to import */
  void SetFileName(const char *file_name);

  
  void SetBuildVolumeFlag(bool buildVolumeFlag) {m_BuildVolumeFlag = buildVolumeFlag;};
  void SetFilePrefix(mafString filePrefix){m_FilePrefix = filePrefix;};  
  void SetFilePattern(mafString filePattern){m_FilePattern = filePattern;};  
  void SetFileExtension(mafString fileExtension){m_FileExtension = fileExtension;};  
  void SetFileOffset(int fileOffset){m_FileOffset = fileOffset;};  
  void SetFileSpacing(int fileSpacing){m_FileSpacing = fileSpacing;};
  void SetImageZSpacing(double imageZSpacing){m_ImageZSpacing = imageZSpacing;};

  void ImportImage();
protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  
  void BuildImageSequence();
  void BuildVolume();

  mafVMEImage     *m_ImportedImage;
  mafVMEVolumeRGB *m_ImportedImageAsVolume;

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
