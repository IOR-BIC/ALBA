/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterGRFWS.h,v $
  Language:  C++
  Date:      $Date: 2009-08-21 12:56:13 $
  Version:   $Revision: 1.3.2.1 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpImporterGRFWS_H__
#define __medOpImporterGRFWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVector;
class mafVMESurface;
class mafVMEGroup;

//----------------------------------------------------------------------------
// medOpImporterGRFWS :
//----------------------------------------------------------------------------
/** */
class medOpImporterGRFWS : public mafOp
{
public:
	medOpImporterGRFWS(const wxString &label = "GRF Importer");
	~medOpImporterGRFWS(); 

  void medOpImporterGRFWS::OpUndo();

  /** Copy the operation. */
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:

  mafVMESurface       *m_PlatformLeft;
  mafVMESurface       *m_PlatformRight;
  mafVMEVector        *m_VectorLeft;
  mafVMEVector        *m_VectorRight;
  mafVMEGroup         *m_Group;

  wxString             m_FileDir;
	wxString             m_File;

};
#endif
