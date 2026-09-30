/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERefSys
 Authors: Marco Petrone, Paolo Quadrani, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVMERefSys_h
#define __albaVMERefSys_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVME.h"
#include "albaVMELandmark.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class albaTransform;
class albaVMEOutputSurface;
class vtkArrowSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;
class mmaMaterial;

/* albaVMERefSys
 A procedural reference system */
class ALBA_EXPORT albaVMERefSys : public albaVME
{
public:
  albaTypeMacro(albaVMERefSys,albaVME);

	enum RefSysModalities
	{
		REFSYS_FREE,
		REFSYS_ORIGIN,
		REFSYS_PLANE,
	};

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  enum REFSYS_WIDGET_ID
  {
    ID_SCALE_FACTOR = Superclass::ID_LAST,
		ID_REF_SYS_ORIGIN,
		ID_POINT1,
		ID_POINT2,
		ID_RADIO,
    ID_LAST
  };

	static bool LandmarkAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVMELandmark));};

  /** Precess events coming from other objects */ 
  virtual void OnEvent(albaEventBase *alba_event);

  /** Copy the contents of another VME-RefSys into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another VME-RefSys. */
  virtual bool Equals(albaVME *vme);

  /** Used to change the axes size */
  void SetScaleFactor(double scale);

  /** Return the axes size */
  double GetScaleFactor();

  /** return the right type of output */  
  albaVMEOutputSurface *GetSurfaceOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const albaMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();
  
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeRefSys");};

	
	albaVME *GetOriginVME();
  albaVME *GetPoint1VME();
  albaVME *GetPoint2VME();

	/** Return true if you can use a move operation on this vme */
	bool IsMovable();

	/** generate an abstract matrix used to put a refsys in the origin who is complanar with p1, andp2*/
	static albaMatrix GetAbsMatrixFromPoints(double * origin, double * point1, double * point2);
	
protected:
  albaVMERefSys();
  virtual ~albaVMERefSys();
  
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

	/** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

	albaString m_Point1VmeName;
	albaString m_Point2VmeName;
	albaString m_OriginVmeName;

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

	int m_Modality;

  albaTransform *m_Transform; ///< pose matrix for the slicer plane

private:
  albaVMERefSys(const albaVMERefSys&); // Not implemented
  void operator=(const albaVMERefSys&); // Not implemented

  // TODO: REFACTOR THIS 
  // Move these methods in a suitable logging helper class
  void LogVector3( double *vector , const char *logMessage /*= NULL*/ );
  void LogPoint3( double *point, const char *logMessage );
  void LogVTKMatrix4x4( vtkMatrix4x4 *mat, const char *logMessage );
  void LogALBAMatrix4x4( albaMatrix *mat, const char *logMessage );
};
#endif
