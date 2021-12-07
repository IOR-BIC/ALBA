/*=========================================================================

Program: ALBA
Module: albaOpExporterAnsysCommon.h
Authors: Nicola Vanella, Giangluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterAnsysCommon_H__
#define __albaOpExporterAnsysCommon_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOpExporterFEMCommon.h"

#include "vtkUnstructuredGrid.h"
#include "vtkIntArray.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

typedef struct ExportElementStruct
{
  int elementID;
  int matID;
  int elementType;
  int elementReal;
  int cellID;
} ExportElement;

#define MAX_ELEMENT_NODES 10

//----------------------------------------------------------------------------
// lhpOpExporterAnsysInputFile :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpExporterAnsysCommon : public albaOpExporterFEMCommon
{
public:
	albaOpExporterAnsysCommon(const wxString &label = "albaOpExporterAnsysCommon");
	~albaOpExporterAnsysCommon(); 

	albaAbstractTypeMacro(albaOpExporterAnsysCommon, albaOpExporterFEMCommon);
		
  /** Set/Get output file name*/
  void SetOutputFileName(const char *outputFileName) {m_AnsysOutputFileNameFullPath = outputFileName;};
  const char *GetOutputFileName() {return m_AnsysOutputFileNameFullPath.c_str();};

  /** Export the input mesh by writing it in Ansys .inp format */
  virtual int Write() = 0;
	  
  /** Builds operation's interface. */
	void OpRun();
	
protected:	
  virtual albaString GetWildcard() = 0;

	void Init();
	
	void OpStop(int result);

  void OnOK();

  ExportElement *CreateExportElements(albaVMEMesh * input, int rowsNumber, vtkUnstructuredGrid * inputUGrid, FILE * file);

	static int compareElem(const void *p1, const void *p2);
	
  int m_IntCharSize;

	/** Ansys input file name */
	wxString m_AnsysOutputFileNameFullPath;

	albaString m_FrequencyFileName;
	FILE *m_Freq_fp;
};
#endif
