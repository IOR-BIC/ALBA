/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterDicom
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterDicom_H__
#define __albaOpExporterDicom_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaGUIDicomSettings;
namespace gdcm {
	class DataSet;
}

#define DEFINE_TAG(name) if(!m_##name.IsEmpty()) { \
	gdcm::DataElement de##name(TAG_##name); \
	de##name.SetByteValue(m_##name.GetCStr(), m_##name.Length());\
	dcmDataSet.Replace(de##name);\
}

//----------------------------------------------------------------------------
// albaOpExporterDicom :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpExporterDicom: public albaOp
{
public:
  albaOpExporterDicom(const wxString &label = "VTKExporter");
 ~albaOpExporterDicom(); 
  
  albaTypeMacro(albaOpExporterDicom, albaOp);

  virtual albaOp* Copy();
	void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface. */
  virtual void OpRun();

  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};


  /** Set the filename for the .stl to export */
  void SetFolderName(const char *folder) {m_Folder = folder;};

  /** Export vtk data. */
  void ExportDicom();


 	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

	/**Get dicom settings*/
	albaGUIDicomSettings* GetSetting();
	
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/**Use this function to define application specific Dicom tags, see implementation for an example*/
	virtual void DefineAppSpecificTags(gdcm::DataSet & dcmDataSet);

	/**Returns the name of ith file*/
	albaString GetIthFilename(int i);

	void ScaleIntToUShortScalars(int * from, unsigned short *to, int scalarShift, int imgDim);

	void ScaleIntToShortScalars(int * from, short *to, int imgDim);

	void ScaleUIntToUShortScalars(unsigned int * from, unsigned short *to, int scalarShift, int imgDim);

	albaString  m_Folder;
  albaVME   *m_Vme; 
	int				m_ABSMatrixFlag;
	
	albaString m_PatientsName;
	albaString m_PatientsSex;
	albaString m_PatientsBirthDate;
	albaString m_PatientsWeight;
	albaString m_PatientsAge;
	albaString m_PatientID;
	albaString m_InstitutionName;
	albaString m_StudyDescription;
	albaString m_SeriesDescription;
	albaString m_AcquisitionDate;
	albaString m_PixelSpacing;
	albaString m_ProtocolName;
	albaString m_ManufacturersModelName;

	
private:

	template<typename InDataType, typename OutDataType>
	void ScalarScale(InDataType from, OutDataType to, int imgDim, int scalarShift=0);
};
#endif
