#ifndef __mafOpImporterBBF_H__
#define __mafOpImporterBBF_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEVolumeLarge;

/** 
class name : mafOpImporterBBF
Import operation that try to read bbf data file and set it into the corresponding VME 
that accept the bbf data format. If no VME can accept the format a message box will be shown 
to the user and no data will be imported.
bbf files come from a raw volume file .By using importe ->images->RAW VOLUME can generate bbf files .
*/
class MED_EXPORT mafOpImporterBBF: public mafOp 
{
public:
  /** constructor. */
  mafOpImporterBBF(const wxString &label = "BBFImporter");
  /** destructor. */
 ~mafOpImporterBBF(); 
  
  /** RTTI macro */
  mafTypeMacro(mafOpImporterBBF, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
  /*virtual*/ mafOp* Copy();

	/** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
  /*virtual*/ void OpRun();

	/** Import bbf data, return MAF_OK on success. */
  virtual int ImportBBF();

  /** Set the vtk filename to be imported. 
      This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

protected:
  wxString m_File;
  wxString m_FileDir;
  
  mafVMEVolumeLarge *m_VmeLarge;
};
#endif