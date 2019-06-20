/*=========================================================================

Program: ALBA
Module: albaOpExporterAnsysCommon.h
Authors: Nicola Vanella

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
class albaVME;
class albaVMEMesh;
class albaEvent;
class albaProgressBarHelper;

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

	
  /** Apply vme abs matrix to data geometry */
  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};

  /** Set/Get output file name*/
  void SetOutputFileName(const char *outputFileName) {m_AnsysOutputFileNameFullPath = outputFileName;};
  const char *GetOutputFileName() {return m_AnsysOutputFileNameFullPath.c_str();};

  /** Export the input mesh by writing it in Ansys .inp format */
  virtual int Write() = 0;

  virtual void OnEvent(albaEventBase *alba_event);

  /** Return true for the acceptable vme type. */
  bool Accept(albaVME *node);

  /** Builds operation's interface. */
	void OpRun();

  /** Return the "pid" of the wxExecute() ansysWriter.py process; use only for debugging
  purposes */
  long GetPid();

protected:	
  virtual albaString GetWildcard() = 0;

  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  
  
	void Init();
	
	void OpStop(int result);

  void OnOK();

  ExportElement *CreateExportElements(albaVMEMesh * input, int rowsNumber, vtkUnstructuredGrid * inputUGrid, FILE * file);

	static int compareElem(const void *p1, const void *p2);

	albaProgressBarHelper *m_ProgressHelper;

  int m_IntCharSize;

  float m_TotalElements;
  long m_CurrentProgress;
  long m_OperationProgress;

  wxString m_AnsysOutputFileNameFullPath;

  int m_ImporterType;

  /** Ansys input file name */
  albaString m_AnsysInputFileName;

  int m_ABSMatrixFlag;

  long m_Pid;  

  enum ANSYS_EXPORTER_ID
  {
    ID_ABS_MATRIX_TO_STL = MINID,  
  };

};
#endif
