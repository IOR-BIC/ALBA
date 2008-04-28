/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterGRFWS.h,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:44:19 $
  Version:   $Revision: 1.3 $
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

//----------------------------------------------------------------------------
// medOpImporterGRFWS :
//----------------------------------------------------------------------------
/** */
class medOpImporterGRFWS : public mafOp
{
public:
	medOpImporterGRFWS(const wxString &label = "GRF Importer");
	~medOpImporterGRFWS(); 

  void medOpImporterGRFWS::OpDo();

  void medOpImporterGRFWS::OpUndo();

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

  wxString             m_FileDir;
	wxString             m_File;

};
#endif
