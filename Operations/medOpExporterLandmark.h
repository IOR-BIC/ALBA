/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterLandmark.h,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:37:17 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani, Daniele Giunchi (porting MAf 2.0)
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpExporterLandmark_H__
#define __medOpExporterLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// medOpExporterLandmark :
//----------------------------------------------------------------------------
/** Exporter for the landmark coordinates: the data are exported in ASCII format. 
Each raw represents a landmark and contains the (x,y,z) coordinate.*/
class medOpExporterLandmark: public mafOp
{
public:
	medOpExporterLandmark(const wxString &label = "Landmark Exporter");
	~medOpExporterLandmark(); 
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Build the interface of the operation, i.e the dialog that let choose the name of the output file. */
	void OpRun();

  /** Export landmarks contained into a mafVMELandmarkCloud.*/
  void ExportLandmark();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};

protected:
	wxString	m_File;
	wxString	m_FileDir;
};
#endif
