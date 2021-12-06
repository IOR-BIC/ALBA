/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpImporterC3DBTK.h,v $
Language:  C++
Date:      $Date: 2010-02-23 13:35:49 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni, Crimi Gianluigi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "albaDefines.h" 

#ifdef ALBA_USE_BTK


#ifndef __lhpOpImporterC3DBTK_H__
#define __lhpOpImporterC3DBTK_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "btkAcquisitionFileReader.h"
#include "btkForcePlatformsExtractor.h"
#include "btkGroundReactionWrenchFilter.h"
#include "btkForcePlatformWrenchFilter.h"
#include <map>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEMesh;
class albaEvent;
class albaVMELandmarkCloud;
class albaVMESurface;
class albaVMEVector;
class albaVMEAnalog;
class albaVMEGroup;

//----------------------------------------------------------------------------
// lhpOpImporterC3DBTK :
//----------------------------------------------------------------------------
/**
This operation is a wrapper of C3D Reader SDK of Aurion. Need C3D_Reader.lib, C3D_Reader.dll,
LicenseAurion.dll. The license can be obtained by Aurion following messagebox created by dll.
*/
class ALBA_EXPORT albaOpImporterC3D : public albaOp
{
public:
	albaOpImporterC3D(const wxString &label = "C3D Importer");
	~albaOpImporterC3D();

	albaTypeMacro(albaOpImporterC3D, albaOp);

	virtual void OnEvent(albaEventBase *alba_event);

	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	/** Import the c3d*/
	bool Import();

	//methods useful for test

	/* Set/Get Full pathname of c3d File*/
	void SetC3DFileName(const char *filec3d) { Clear(); m_C3DInputFileNameFullPaths.resize(1); m_C3DInputFileNameFullPaths[0] = filec3d; }
	const char * GetC3DFileName() { if (m_C3DInputFileNameFullPaths.empty()) return NULL; return m_C3DInputFileNameFullPaths[0]; }

	/* Set/Get Full pathname of dictionary File*/
	void SetDictionaryFileName(const char *fileDict) { m_DictionaryFileName = fileDict; DictionaryUpdate(); }
	const char * GetDictionaryFileName() { return m_DictionaryFileName; }

	/* Get Group representing result of import */
	albaVMEGroup *GetGroup() { if (m_intData.empty()) return NULL; return m_intData[0].m_VmeGroup; }

	/* Get Landmark Cloud */
	albaVMELandmarkCloud *GetLandmarkCloudVME() { if (m_intData.empty()) return NULL; if (m_intData[0].m_Clouds.empty()) return NULL; return m_intData[0].m_Clouds.begin()->second; }

	/* Get Analog VME */
	albaVMEAnalog *GetAnalogVME() { return m_intData.empty() ? NULL : m_intData[0].m_VmeAnalog; }

	/* Get Platform vmes from a std list*/
	albaVMESurface *GetPlatformVME(int index = 0) { if (m_intData.empty()) return NULL; return(index >= 0 && index<m_intData[0].m_PlatformList.size()) ? m_intData[0].m_PlatformList[index] : NULL; }

	/* Get force vmes from a std list*/
	albaVMEVector *GetForceVME(int index = 0) { if (m_intData.empty()) return NULL; return(index >= 0 && index<m_intData[0].m_ForceList.size()) ? m_intData[0].m_ForceList[index] : NULL; }

	/* Get moment vmes from a std list*/
	albaVMEVector *GetMomentVME(int index = 0) { if (m_intData.empty()) return NULL; return(index >= 0 && index<m_intData[0].m_MomentList.size()) ? m_intData[0].m_MomentList[index] : NULL; }

	/* Set/Get Trajectory import flag*/
	void SetImportTrajectories(int flag) { m_ImportTrajectoriesFlag = flag; }
	int  GetImportTrajectories() { return m_ImportTrajectoriesFlag; }

	/* Set/Get Analog import flag*/
	void SetImportAnalog(int flag) { m_ImportAnalogFlag = flag; }
	int  GetImportAnalog() { return m_ImportAnalogFlag; }

	/* Set/Get Platform import flag*/
	void SetImportPlatform(int flag) { m_ImportPlatformFlag = flag; }
	int  GetImportPlatform() { return m_ImportPlatformFlag; }

	/* Set/Get Event import flag*/
	void SetImportEvent(int flag) { m_ImportEventFlag = flag; }
	int  GetImportEvent() { return m_ImportEventFlag; }

protected:

	struct _InternalC3DData;

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** Create the dialog interface for the importer. */
	virtual void CreateGui();

	/** Initialize all structures needed for import data*/
	void Initialize(const albaString &fullFileName, _InternalC3DData &intData);

	/** Open C3D File*/
	int OpenC3D(const albaString &fullFileName);

	/** Import the c3d Landmark coordinates*/
	void ImportTrajectories(_InternalC3DData &intData);

	/** Import the c3d analog data like EMG*/
	void ImportAnalog(_InternalC3DData &intData);

	/** Import the c3d platform data*/
	void ImportPlatform(_InternalC3DData &intData);

	/** Import the c3d events*/
	void ImportEvent(_InternalC3DData &intData);

	/** Cleans allocated memory*/
	void Clear();

	/** Import the c3d events*/
	albaVMEGroup* ImportSingleFile(const albaString &fullFileName, _InternalC3DData &intData);

	/** Do actions necessary on dictionary name changing*/
	void albaOpImporterC3D::DictionaryUpdate();

	/** Load dictionary from specified file*/
	bool albaOpImporterC3D::LoadDictionary();

	/** Destroy dictionary*/
	void albaOpImporterC3D::DestroyDictionary();

	btk::Acquisition::Pointer m_Acq;
	btk::ForcePlatformCollection::Pointer m_Pfc;
	btk::GroundReactionWrenchFilter::Pointer m_Grws;
	btk::ForcePlatformsExtractor::Pointer m_Pfe;
	std::vector<albaString>         m_C3DInputFileNameFullPaths;
	albaString                      m_FileDir;
	albaString                      m_DictionaryFileName;
	std::map<albaString, albaString> m_dictionaryStruct;
	int m_ImportTrajectoriesFlag;
	int m_ImportAnalogFlag;
	int m_ImportPlatformFlag;
	int m_ImportEventFlag;

	//vmes
	struct _InternalC3DData
	{
		_InternalC3DData();

		albaString m_FileName;

		albaVMEGroup                               *m_VmeGroup;
		std::map<albaString, albaVMELandmarkCloud*> m_Clouds;

		albaVMEAnalog *m_VmeAnalog;

		std::vector<albaVMESurface *> m_PlatformList;

		std::vector<albaVMEVector *> m_ForceList;
		std::vector<albaVMEVector *> m_MomentList;

		//data filled by Aurion importer
		//int m_Errcode;
		float m_AnalogRate, m_VideoRate;
		long m_LengthMs;

		//derived member
		double m_TrajectorySamplePeriod;
		double m_AnalogSamplePeriod;
		double m_VectogramSamplePeriod;

		int m_NumTotTrajectories;
		int m_NumTrajectories;
		int m_NumAngles;
		int m_NumMoments;
		int m_NumPowers;
		int m_NumFrames;
		int m_NumChannels;
		int m_NumSamples;
		int m_NumEvents;
		int m_NumPlatforms;

		char m_TrajectoryName[100];
		char m_ChannelName[100];
		char m_AngleName[100];
		char m_MomentName[100];
		char m_PowerName[100];
		char m_EventContext[100];

		char m_TrajectoryUnit[100];
		char m_ChannelUnit[100];
		char m_AngleUnit[100];
		char m_MomentUnit[100];
		char m_PowerUnit[100];

		double m_X;
		double m_Y;
		double m_Z;

		double m_AnalogValue;
		double m_EventValue;

		double m_CopX;
		double m_CopY;

		double m_ForceX;
		double m_ForceY;
		double m_ForceZ;

		double m_MomentX;
		double m_MomentY;
		double m_MomentZ;

		double m_CenterX;
		double m_CenterY;

		double m_OriginX;
		double m_OriginY;
		double m_OriginZ;
	};

	std::vector<_InternalC3DData> m_intData;

};
#endif

#endif // ALBA_USE_BTK