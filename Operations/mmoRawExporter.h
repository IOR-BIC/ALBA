/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoRawExporter.h,v $
  Language:  C++
  Date:      $Date: 2006-11-23 17:17:14 $
  Version:   $Revision: 1.2 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoRAWExporter_H__
#define __mmoRAWExporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafOp;
class mafEvent;
class mafNode;
class mafString;

//----------------------------------------------------------------------------
// mmoRAWExporter :
//----------------------------------------------------------------------------
/** */
class mmoRAWExporter: public mafOp
{
public:
  mmoRAWExporter(wxString label);
 ~mmoRAWExporter(); 
  mafOp* Copy();

  void   OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();
	
	/** Export the volume as a stack of images. */
  void SaveVolume();

	//Set the filename for the .raw to export
	void SetFileName(const char *file_name){m_FileName = file_name;};

	//Set the filename for the .txt Z coordinates to export
	void SetFileZ(const char *file_name){m_FileNameZ = file_name;};

	//Enable or disable the gui for multi/single file mode
	void SetSingleFile(int enable);

protected:
  void OpStop(int result);

  wxString		m_ProposedDirectory;///<Default directory where to save file .raw
  mafString		m_FileName;///<Name of the file/files where the exporter will save raw data
	mafString		m_FileNameZ;///<Name of the file txt where save z coordinates
  int		m_SingleFile;///<Flag: false if the exporter will create a single file; true if the exporter will create raw slices
  int		m_Offset;///<The name of the slices will start with this offset
};
#endif
