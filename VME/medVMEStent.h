/*=========================================================================
  Program:   MAF2Medical
  Module:    $RCSfile: medVMEStent.h,v $
  Language:  C++
  Date:      $Date: 2012-10-23 10:15:31 $
  Version:   $Revision: 1.1.2.7 $
  Authors:   Hui Wei
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medVMEStent_h 
#define __medVMEStent_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medVMEDefines.h"  //very important for MED_VME_EXPORT
#include "mafVME.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class mafVMEOutputPolyline;
class vtkPolyData;
class vtkTransform;

/**
*/
class MED_VME_EXPORT medVMEStent : public mafVME
{
public:

  mafTypeMacro(medVMEStent, mafVME);
  /* constructor */
  medVMEStent();
  /* destructor */
  virtual ~medVMEStent();
  
  //---------------------------Setter-/-Getter------------------------------------  
  /** Copy the contents of another medVMEStent into this one. */
  virtual int DeepCopy(mafNode *a);
  /** Compare with another mafVMESurfaceParametric. */
  virtual bool Equals(mafVME *vme);
  
  /** Return the suggested pipe-typename for the visualization of this vme */
  //virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};
  //mafVMEOutputSurface *GetSurfaceOutput();//return the right type of output 

  virtual mafString GetVisualPipe() {return mafString("mafPipePolyline");};
  mafVMEOutputPolyline *GetPolylineOutput(); //return the right type of output 

  vtkPolyData *GetPolyData(){InternalUpdate();return m_PolyData;};
  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();



  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();
	
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Get the vtkPolyData generated from the model */
  vtkPolyData  *GetVtkPolyData(){InternalUpdate();return m_PolyData;};

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);
  
  //------set properties-----------
  void SetCenterLine(vtkPolyData *line);
  void SetStentDiameter(double diameter){m_Stent_Diameter = diameter;}
  void SetStentCrownLength(double crownL){m_Crown_Length = crownL;}
  void SetStentConfiguration(int stentCfig){m_Id_Stent_Configuration = stentCfig;}
  void SetLinkLength(double linkLength){m_Link_Length = linkLength;}
  void SetLinkAlignment(int linkAlgn){m_Link_Alignment = linkAlgn;}
  void SetLinkOrientation(int linkOrit){m_Link_orientation = linkOrit;}

protected:
	  enum STENT_WIDGET_ID
	  {
		
		CHANGE_VALUE,
		STENT_DIAMETER,
		CROWN_LENGTH,
		STRUT_ANGLE,
		STRUT_THICKNESS,
		ID_STENT_CONFIGURATION,
		ID_LINK_CONNECTION,
		LINK_LENGTH,
		NUMBER_OF_LINKS,//(along circumference)
		LINK_ALIGNMENT,
		LINK_ORIENTATION,
		ID_LAST
	  };
	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();
	/** update the output data structure */
	virtual void InternalUpdate();
	
	void DoDeformation(int type);
	
	/** Internally used to create a new instance of the GUI.*/
	virtual mafGUI *CreateGui();

	mafTransform *m_Transform; 
	vtkPolyData  *m_PolyData;
	vtkPolyData  *m_Centerline; 

	double m_Stent_Diameter;
	double m_Crown_Length;
	double m_Strut_Angle;
	double m_Strut_Thickness;
	int m_Id_Stent_Configuration;

	int m_Id_Link_Connection;
	double m_Link_Length;
	int m_Link_Alignment;
	int m_Link_orientation;  

private:
  int m_numberOfCycle;
  vtkPolyData *stentPolyLine;  

};
#endif
