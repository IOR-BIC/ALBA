/*=========================================================================
Program:   Bonemat
Module:    albaOpImporterAbaqusFile.h
Language:  C++
Date:      $Date: 2010-11-23 16:50:26 $
Version:   $Revision: 1.1.1.1.2.3 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpImporterAbaqusFile_H__
#define __albaOpImporterAbaqusFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOpImporterFile.h"
#include "albaTextFileReaderHelper.h"
#include <map>
#include <set>
#include "albaMatrix.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEMesh;
class albaEvent;
class wxBusyInfo;
class albaProgressBarHelper;

struct AbaqusElset
{
  std::string name;
  std::string matName;
};

struct AbaqusElement 
{
  int Id; 
  int nodesNumber;
  int type; 
  int *nodes;
};

struct AbaqusTransform
{
  std::string partName;

  albaMatrix matrix;
};

struct AbaqusPart
{
  std::string name;
  std::vector<AbaqusElement> elementsVector;
  std::vector<AbaqusElset> elsetsVector;
	std::map<int,int> elementsToElset;
};

//----------------------------------------------------------------------------
// albaOpImporterAbaqusFile :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpImporterAbaqusFile : public albaTextFileReaderHelper, public albaOpImporterFile
{
public:
  albaOpImporterAbaqusFile(const wxString &label = "AbaqusImporter");
  ~albaOpImporterAbaqusFile(); 

  albaTypeMacro(albaOpImporterAbaqusFile, albaOpImporterFile);

  virtual void OnEvent(albaEventBase *alba_event);
  
  albaOp* Copy();

  /** Builds operation's interface. */
  void OpRun();

  /** Import the mesh, return ALBA_OK on success.*/
  int ImportFile();
  
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  int ParseAbaqusFile(albaString fileName);

  int SetPart(std::string name="", int part=-1);

  int ReadHeader(FILE *outFile);
  int ReadNodes(FILE *outFile);
  int ReadElements();
  int ReadElset();
  int ReadSolid();
  int ReadMaterials(FILE *outFile);
  int ReadInstance();
  int WriteElements();

  
  int m_ImporterType;
  albaVMEMesh *m_ImportedVmeMesh;

  albaString m_CacheDir;
  albaString m_DataDir;

  long m_OperationProgress;

  std::map<std::string,int> m_MatIDMap;
  int m_LastMatId;

  std::vector<AbaqusPart> m_Parts;
  std::vector<AbaqusElement>* m_Elements;
  std::vector<AbaqusElset>* m_Elsets;
  std::vector<AbaqusTransform> m_Transforms;

	std::map<int,int>* m_ElementsToElset;

  //
  albaMatrix RotationMatrixUnnormalizedDirVector(double a, double b, double c, double d, double e, double f, double theta);
};
#endif

