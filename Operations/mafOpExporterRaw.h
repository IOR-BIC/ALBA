/*=========================================================================

 Program: MAF2
 Module: mafOpExporterRaw
 Authors: Stefano Perticoni - Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterRAW_H__
#define __mafOpExporterRAW_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafOp;
class mafEvent;
class mafVME;
class mafString;

//----------------------------------------------------------------------------
// mafOpExporterRAW :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpExporterRAW: public mafOp
{
public:
  mafOpExporterRAW(const wxString &label);
 ~mafOpExporterRAW(); 
  mafOp* Copy();

  void   OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

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
