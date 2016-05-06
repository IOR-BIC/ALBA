/*=========================================================================

 Program: MAF2
 Module: mafOpMergeDicomSeries
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "mafOp.h"
#include "vtkDirectory.h"

//----------------------------------------------------------------------------
// mafOpMergeDicomSeries : Merge dicom series located in the same folder
//----------------------------------------------------------------------------
/** 
Merge dicom series located in the same folder; this operation was implemented to correctly import and visualize data located in 
"\\HD01\Public\Dati\Dicom Regression\NIG004_MAGLU_SA" and "\\HD01\Public\Dati\Dicom Regression\NIG009-PAVVI_SA"
WARNING: This operation will be removed and integrated in the importer dicom operation as an optiona functionality
*/
class MAF_EXPORT mafOpMergeDicomSeries : public mafOp
{
public:
	/** constructor */
  mafOpMergeDicomSeries(wxString label = "Merge DICOM Series");

  /** destructor */
  ~mafOpMergeDicomSeries();

	/** RTTI macro */
	mafTypeMacro(mafOpMergeDicomSeries, mafOp);
	
	/** Copy. */
	mafOp* Copy();

	/** Builds operation's interface calling CreateGui() method. */
	virtual void OpRun();

  /** turn true for the acceptable vme type. */
  virtual bool Accept(mafVME*node){return true;};

protected:

  mafString m_DicomDirectoryABSFileName;
  vtkDirectory  *m_DICOMDirectoryReader;
  int m_DicomSeriesInstanceUID;
  int m_ChangeManufacturer;

  bool RanameSeriesAndManufacturer(const char *dicomDirABSPath, int dicomSeriesUID);
};