/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoBmpExporter.h,v $
  Language:  C++
  Date:      $Date: 2007-06-06 08:26:04 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoBmpExporter_H__
#define __mmoBmpExporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mmoBmpExporter :
//----------------------------------------------------------------------------
/** */
class mmoBmpExporter: public mafOp
{
public:
  mmoBmpExporter(const wxString &label);
 ~mmoBmpExporter(); 
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();
	
	//Set the filename for the .bmp to export
	void SetFileName(const char *file_name){m_FileName = file_name;};

  /** Export the volume as a stack of bmp images. */
  void SaveBmp();

protected:
  void OpStop(int result);

  wxString		m_ProposedDirectory;///<Default directory where to save file .bmp
  mafString		m_FileName;///<Name of the file/files where the exporter will save bmp data
 };
#endif
