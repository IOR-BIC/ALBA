/*=========================================================================
Program:   ALBA
Module:    albaOpExporterAbaqusFile.h
Language:  C++
Date:      $Date: 2009-05-19 14:29:53 $
Version:   $Revision: 1.1 $
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
#include "vtkUnstructuredGrid.h"
#include "vtkIntArray.h"
#include <map>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------


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

  /** Set/Get output file name*/
  void SetOutputFileName(const char *outputFileName) {m_AbaqusOutputFileNameFullPath = outputFileName;};
  const char *GetOutputFileName() {return m_AbaqusOutputFileNameFullPath.char_str();};

  /** Export the input mesh by writing it in Abaqus .inp format */
  int Write();

protected:

  albaString GetWildcard();
   
  void OpStop(int result);

  void OnOK();

  static int compareElem(const void *p1, const void *p2);

  int WriteHeaderFile(FILE *file);
  int WriteNodesFile(FILE *file);
  int WriteElementsFile(FILE *file);
  int WriteMaterialsFile(FILE *file);  

  int m_IntCharSize;
  
  wxString m_AbaqusOutputFileNameFullPath;

  albaVMEMesh *m_ImportedVmeMesh;

  /** Abaqus input file name */
  albaString m_AbaqusInputFileName;
	 
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
