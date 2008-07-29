/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERefSys.h,v $
  Language:  C++
  Date:      $Date: 2008-07-29 11:51:58 $
  Version:   $Revision: 1.8 $
  Authors:   Marco Petrone, Paolo Quadrani, Stefano Perticoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMERefSys_h
#define __mafVMERefSys_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMELandmark.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafTransform;
class mafVMEOutputSurface;
class vtkArrowSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;
class mmaMaterial;

/* mafVMERefSys
 A procedural reference system */
class MAF_EXPORT mafVMERefSys : public mafVME
{
public:
  mafTypeMacro(mafVMERefSys,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  enum REFSYS_WIDGET_ID
  {
    ID_SCALE_FACTOR = Superclass::ID_LAST,
		ID_REF_SYS_ORIGIN,
		ID_POINT1,
		ID_POINT2,
		ID_RADIO,
		ID_FIXED,
    ID_LAST
  };

	static bool LandmarkAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMELandmark));};

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Copy the contents of another VME-RefSys into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-RefSys. */
  virtual bool Equals(mafVME *vme);

  /** Used to change the axes size */
  void SetScaleFactor(double scale);

  /** Return the axes size */
  double GetScaleFactor();

  /** return the right type of output */  
  mafVMEOutputSurface *GetSurfaceOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();
  
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};

	/** 
  Set links for the ref-sys*/
  void SetRefSysLink(const char *link_name, mafNode *n);

	mafVME *GetOriginVME();
  mafVME *GetPoint1VME();
  mafVME *GetPoint2VME();

protected:
  mafVMERefSys();
  virtual ~mafVMERefSys();
  
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

	mafString m_Point1VmeName;
	mafString m_Point2VmeName;
	mafString m_OriginVmeName;

  vtkArrowSource						*m_XArrow;
  vtkArrowSource						*m_YArrow;
  vtkArrowSource						*m_ZArrow;

  vtkTransformPolyDataFilter*m_XAxis;
  vtkTransform							*m_XAxisTransform;

  vtkTransformPolyDataFilter*m_YAxis;
  vtkTransform							*m_YAxisTransform;

  vtkTransformPolyDataFilter*m_ZAxis;
  vtkTransform							*m_ZAxisTransform;

  vtkTransformPolyDataFilter*m_ScaleAxis;
  vtkTransform							*m_ScaleAxisTransform;

  vtkAppendPolyData					*m_Axes;

  double m_ScaleFactor;

  mafTransform *m_Transform; ///< pose matrix for the slicer plane

	int m_Radio;

private:
  mafVMERefSys(const mafVMERefSys&); // Not implemented
  void operator=(const mafVMERefSys&); // Not implemented

  // TODO: REFACTOR THIS 
  // Move these methods in a suitable logging helper class
  void LogVector3( double *vector , const char *logMessage /*= NULL*/ );
  void LogPoint3( double *point, const char *logMessage );
  void LogVTKMatrix4x4( vtkMatrix4x4 *mat, const char *logMessage );
  void LogMAFMatrix4x4( mafMatrix *mat, const char *logMessage );
};
#endif
