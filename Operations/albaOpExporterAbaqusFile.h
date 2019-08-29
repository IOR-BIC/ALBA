/*=========================================================================
  Program:   Bonemat
  Module:    albaOpExporterAbaqusFile.h
  Language:  C++
  Date:      $Date: 2010-11-26 16:46:11 $
  Version:   $Revision: 1.1.1.1.2.1 $
  Authors:   Nicola Vanella 
==========================================================================
  Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

	This software is distributed WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpExporterAbaqusFile_H__
#define __albaOpExporterAbaqusFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOpExporterFEMCommon.h"
#include "albaVMEMesh.h"
#include "vtkUnstructuredGrid.h"
#include "vtkIntArray.h"
#include <map>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEMesh;
class albaEvent;
class albaProgressBarHelper;

#define MAX_ELEMENT_NODES 10

//----------------------------------------------------------------------------
// appOpExporterAbaqusInputFile :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpExporterAbaqusFile : public albaOpExporterFEMCommon
{
public:
	albaOpExporterAbaqusFile(const wxString &label = "albaOpExporterAbaqusFile");
	~albaOpExporterAbaqusFile(); 
	
  albaTypeMacro(albaOpExporterAbaqusFile, albaOpExporterFEMCommon);
    
  albaOp* Copy();

    /** Apply vme abs matrix to data geometry */
  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};

  /** Set/Get output file name*/
  void SetOutputFileName(const char *outputFileName) {m_AbaqusOutputFileNameFullPath = outputFileName;};
  const char *GetOutputFileName() {return m_AbaqusOutputFileNameFullPath.c_str();};

  /** Export the input mesh by writing it in Abaqus .inp format */
  int Write();

  virtual void OnEvent(albaEventBase *alba_event);

	void UpdateGui();

  /** Return true for the acceptable vme type. */
  bool Accept(albaVME *node);

  /** Builds operation's interface. */
	void OpRun();

  /** Return the "pid" of the wxExecute() ansysWriter.py process; use only for debugging
  purposes */
  long GetPid();

protected:

  albaString GetWildcard();
  
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  

  void OpStop(int result);

  void OnOK();

  static int compareElem(const void *p1, const void *p2);

  int WriteHeaderFile(FILE *file);
  int WriteNodesFile(FILE *file);
  int WriteElementsFile(FILE *file);
  int WriteMaterialsFile(FILE *file);  

  albaProgressBarHelper *m_ProgressHelper;
  float m_TotalElements;
  long m_CurrentProgress;

  int m_IntCharSize;
  
  wxString m_AbaqusOutputFileNameFullPath;

  int m_ImporterType;
  albaVMEMesh *m_ImportedVmeMesh;

  /** Abaqus input file name */
  albaString m_AbaqusInputFileName;

  int m_ABSMatrixFlag;

  long m_Pid;  

  enum ABAQUS_EXPORTER_ID
  {
    ID_ABS_MATRIX_TO_STL = MINID,
		ID_ENABLE_BACKCALCULATION,
		ID_MIN_ELASTICITY,
		ID_RHO_DENSITY_INTERVALS_NUMBER,
		ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_0,
		ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_1,
		ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_2,
		ID_DENSITY_ONE_INTERVAL_ROLLOUT,
		ID_DENSITY_INTERVAL_0,
		ID_DENSITY_INTERVAL_1,
		ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
		ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
		ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,
		ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
		ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
		ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,
		ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
		ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
		ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,
		ID_DENSITY_THREE_INTERVALS_ROLLOUT,
  };

	int m_EnableBackCalculation;
	bool m_HasConfiguration;
	
	albaGUIRollOut *m_GuiRollOutDensityOneInterval;
	albaGUIRollOut *m_GuiRollOutDensityThreeIntervals;

	albaGUI *m_GuiASDensityOneInterval;
	albaGUI *m_GuiASDensityThreeIntervals;

	BonematConfiguration m_Configuration;

	void LoadConfigurationTags();
	double GetDoubleTag(wxString tagName);

  struct ExportElement
  {
    int elementID;
    int matID;
    int elementType;
    int elementReal;
    int cellID;
  };

  struct ExportElset
  {
    int matID;
    std::vector<int> elementsIDVect;
  };

  std::vector<ExportElset> m_Elsets;
  std::map<int,int> m_MatIDMap;
};
#endif
