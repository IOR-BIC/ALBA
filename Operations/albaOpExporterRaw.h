/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterRaw
 Authors: Stefano Perticoni - Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterRAW_H__
#define __albaOpExporterRAW_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaOp;
class albaEvent;
class albaVME;
class albaString;

//----------------------------------------------------------------------------
// albaOpExporterRAW :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpExporterRAW: public albaOp
{
public:
  albaOpExporterRAW(const wxString &label);
 ~albaOpExporterRAW(); 
  albaOp* Copy();

  void OnEvent(albaEventBase *alba_event);

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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  void OpStop(int result);

  wxString		m_ProposedDirectory;///<Default directory where to save file .raw
  albaString		m_FileName;///<Name of the file/files where the exporter will save raw data
	albaString		m_FileNameZ;///<Name of the file txt where save z coordinates
  int		m_SingleFile;///<Flag: false if the exporter will create a single file; true if the exporter will create raw slices
  int		m_Offset;///<The name of the slices will start with this offset
};
#endif
