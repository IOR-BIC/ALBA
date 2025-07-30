/*=========================================================================

Program: ALBA
Module: albaOpImporterAnsysCommon.h
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterAnsysCommon_H__
#define __albaOpImporterAnsysCommon_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaTextFileReaderHelper.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEMesh;
class albaEvent;
class albaProgressBarHelper;

struct AnsysComponent
{
  std::string Name;
  std::string ElementsFileName;
  int *Ranges;
  int RangeNum;
};

struct AnsysElement 
{
  int Id; 
  int NodesNumber;
  int Type; 
  int MatId;
  int *Nodes;
};

struct AnsysMaterial
{
	int Id;
	double Ex;
	double Nuxy;
	double Dens;
};

//----------------------------------------------------------------------------
// lhpOpImporterAnsysFile :
//----------------------------------------------------------------------------
/** 
Importer for Ansys Input files
*/
class ALBA_EXPORT albaOpImporterAnsysCommon : public albaTextFileReaderHelper, public albaOp
{
public:

	albaOpImporterAnsysCommon(const wxString &label = "MeshImporter");
	~albaOpImporterAnsysCommon(); 

	virtual void OnEvent(albaEventBase *alba_event);

	/** Set the input Ansys file */
	void SetFileName(const char *ansysInputFileNameFullPath) {m_AnsysInputFileNameFullPath = ansysInputFileNameFullPath;};
	
  /** Get the input Ansys file */
  wxString GetFileName() {return m_AnsysInputFileNameFullPath.ToAscii();};

  /** Builds operation's interface. */
  void OpRun();

	/** Import the mesh*/
	int Import();

protected:
 
	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  virtual albaString GetWildcard() = 0;

	virtual int ParseAnsysFile(albaString fileName) = 0;

  int ReadNBLOCK(FILE *outFile);
  int ReadEBLOCK();
  int ReadMPDATA();
  int ReadCMBLOCK();

  int WriteElements(int part);
	int WriteMaterials();

  void AddElement(int Id, int nNodes, int type, int matId, int *nodes);
  bool IsInRange(int elemId, int partId);

	wxString m_AnsysInputFileNameFullPath;

	int m_ImporterType;

	albaString m_CacheDir;
	albaString m_DataDir;

  long m_OperationProgress;
	/** Nodes file name*/
	wxString m_NodesFileName;

	/** Materials file name*/
	wxString m_MaterialsFileName;


  
	int m_CurrentMatId;  

  std::vector<AnsysElement> m_Elements;
  std::vector<AnsysComponent> m_Components;
	std::vector<AnsysMaterial> m_Materials;
};
#endif
