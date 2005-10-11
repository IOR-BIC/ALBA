/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoImageImporter.h,v $
  Language:  C++
  Date:      $Date: 2005-10-11 17:49:01 $
  Version:   $Revision: 1.1 $
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
  mmoImageImporter(wxString label);
 ~mmoImageImporter(); 
	virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  void ImportImage();
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
