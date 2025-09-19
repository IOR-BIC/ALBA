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
class albaVMEGroup;
class albaVMEVolumeGray;
class vtkImageData;

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
  void AddFileName(const char *file_name);
	  
  void SetBuildVolumeFlag(bool buildVolumeFlag) {m_BuildVolumeFlag = buildVolumeFlag;};
  void SetSpacing(double *spacing);;

  void Import();

  void SetNaturalTag(albaVME* vme);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

	void AddImageToList(std::vector<vtkImageData*>& images, vtkImageData* image, const char* name);
    
  albaVMEGroup     *m_ImportedGroup;
  albaVMEVolumeGray *m_ImportedVolume;

  std::vector<std::string>	m_Files;
  int           m_BuildVolumeFlag;
  double        m_Spacing[3];
	int          m_SkipWrongSize;
	int          m_SkipWrongType;
	int          m_XFlip;
	int          m_YFlip;
	int          m_ZFlip;
};
#endif
