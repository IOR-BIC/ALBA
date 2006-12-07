/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurfaceParametric.h,v $
  Language:  C++
  Date:      $Date: 2006-12-07 15:01:03 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMESurfaceParametric_h
#define __mafVMESurfaceParametric_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class vtkPolyData;
class mmgRollOut;

/** mafVMESurfaceParametric - this class represents a surface that is a specific geometry 
like cylinder, sphere, cube.
*/
class MAF_EXPORT mafVMESurfaceParametric : public mafVME
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events */
  /** @{ */
  //MAF_ID_DEC(CHANGE_PARAMETER);      ///< Event raised by change parameter
  /** @} */

  mafTypeMacro(mafVMESurfaceParametric, mafVME);
  
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Copy the contents of another mafVMESurfaceParametric into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another mafVMESurfaceParametric. */
  virtual bool Equals(mafVME *vme);

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
	obtained merging timestamps for matrixes and VME items*/
	virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

	/** return always false since (currently) the slicer is not an animated VME (position 
	is the same for all timestamps). */
	virtual bool IsAnimated();

	/** Return the suggested pipe-typename for the visualization of this vme */
	virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};

	/** Return pointer to material attribute. */
	mmaMaterial *GetMaterial();

	/** return an xpm-icon that can be used to represent this node */
	static char ** GetIcon();

	void SourceGui();

	enum PARAMETRIC_SURFACE_WIDGET_ID
	{
	  CHANGE_PARAMETER = Superclass::ID_LAST,
		CHANGE_VALUE_SPHERE,
		CHANGE_VALUE_CONE,
		CHANGE_VALUE_CYLINDER,
		CHANGE_VALUE_CUBE,
    CHANGE_VALUE_PLANE,

		ID_ROLLOUT_SPHERE,
		ID_ROLLOUT_CONE,
		ID_ROLLOUT_CYLINDER,
		ID_ROLLOUT_CUBE,
    ID_ROLLOUT_PLANE,
    ID_LAST
	};

  enum PARAMETRIC_SURFACE_TYPE_ID
  {
		PARAMETRIC_SPHERE = 0,
		PARAMETRIC_CONE,
		PARAMETRIC_CYLINDER,
		PARAMETRIC_CUBE,
    PARAMETRIC_PLANE,
  };

protected:
  mafVMESurfaceParametric();
  virtual ~mafVMESurfaceParametric();

	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
	virtual void InternalUpdate();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();
  
	mmgRollOut *m_RollOutSphere;
	mmgRollOut *m_RollOutCone;
	mmgRollOut *m_RollOutCylinder;
	mmgRollOut *m_RollOutCube;
  mmgRollOut *m_RollOutPlane;

	mafTransform *m_Transform; 
	vtkPolyData  *m_PolyData;
  
  int m_GeometryType;
	double m_SphereRadius;
  double m_SpherePhiRes;
  double m_SphereTheRes;
  double m_ConeHeight;
  double m_ConeRadius;
  int m_ConeCapping;
  double m_ConeRes;
  double m_CylinderHeight;
  double m_CylinderRadius;
  double m_CylinderRes;
  double m_CubeXLength;
  double m_CubeYLength;
  double m_CubeZLength;
  double m_PlaneXRes;
  double m_PlaneYRes;
  double m_PlaneOrigin[3];
  double m_PlanePoint1[3];
  double m_PlanePoint2[3];
  
private:
  mafVMESurfaceParametric(const mafVMESurfaceParametric&); // Not implemented
  void operator=(const mafVMESurfaceParametric&); // Not implemented
};
#endif
