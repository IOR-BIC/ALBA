/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAdvancedSlicer.h,v $
  Language:  C++
  Date:      $Date: 2007-02-01 16:16:30 $
  Version:   $Revision: 1.5 $
  Authors:   Daniele Giunchi , Matteo Giacomoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEAdvancedSlicer_h
#define __mafVMEAdvancedSlicer_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mmgVMEChooserAccept.h"
#include "mafVMEVolume.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkVolumeSlicer;
class vtkTransformPolyDataFilter;
class vtkPlaneSource;
class vktImageData;
class vtkLookupTable;
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;

/** mafVMEAdvancedSlicer - a procedural VME computing the slice of its parent VME.
  mafVMEAdvancedSlicer is a node implementing a slicer of a VME (currently only VME-Volume).
  The sliced volume is the partent VME.
  @sa mafVMEVolume
  @todo
  - 
  */
class MAF_EXPORT mafVMEAdvancedSlicer : public mafVME
{
public:

  mafTypeMacro(mafVMEAdvancedSlicer,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Slicer into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Slicer. */
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

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeSurfaceTextured");};
  
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();
  
  enum ADVANCED_PROBER_WIDGET_ID
  {
  	ID_VOLUME_LINK = Superclass::ID_LAST,
  	ID_POSITION,
  	ID_NORMAL,
    ID_MODALITY,
		ID_WIDTH,
		ID_HEIGHT,
    ID_LAST
  };

  enum ADVANCED_PROBER_MODALITY
  {
     ID_SIMPLE_SLICER_MODE = 0,
     ID_PROJECTED_SLICER_MODE, 	
  };

  int GetStatus(){return m_Status;}
  void SetStatus(int status){m_Status = status;}
  
  int GetNumberOfProfiles(){return m_NumberOfProfiles;}
  void SetNumberOfProfiles(int numberOfProfiles){m_NumberOfProfiles = numberOfProfiles;}
  
  double GetStepProfiles(){return m_StepProfiles;}
  void SetStepProfiles(double stepProfiles){m_StepProfiles = stepProfiles;}

	/** Get the link to the volume.*/
	mafNode *GetVolumeLink();

	/** Set the link to the volume.*/
	void SetVolumeLink(mafNode *volume);

	/** Precess events coming from other objects */ 
	virtual void OnEvent(mafEventBase *maf_event);

	class mafVMEAccept : public mmgVMEChooserAccept
	{
	public:

		mafVMEAccept() {};
		~mafVMEAccept() {};

		bool Validate(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEVolume));};
	};

	mafVMEAccept *m_VMEAccept;

	/** Set the plane normal */
	void SetNormal(float normal[3]);
  
protected:
  mafVMEAdvancedSlicer();
  virtual ~mafVMEAdvancedSlicer();

	/** Internally used to create a new instance of the GUI.*/
	virtual mmgGui *CreateGui();

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  mafTransform*     m_Transform; ///< pose matrix for the slicer plane
  vtkVolumeSlicer*  m_PSlicer;  ///< slicer object used to extract the cut contour
  vtkVolumeSlicer*  m_ISlicer;  ///< slicer object used to compute the slice image

  vtkTransformPolyDataFilter *m_BackTransform;

  int               m_TextureRes;
  double            m_Xspc;
  double            m_Yspc;
  
  int               m_Status;
  int               m_NumberOfProfiles;
  double            m_StepProfiles;
	
	//Gui Variable
	mafString					m_VolumeName;
	int								m_Width;
	int								m_Height;
	float						m_Normal[3];
	double						m_Pos[3];

	vtkImageData			*m_Texture;
	vtkPlaneSource		*m_Plane;
	vtkLookupTable		*m_Lut;

private:
  mafVMEAdvancedSlicer(const mafVMEAdvancedSlicer&); // Not implemented
  void operator=(const mafVMEAdvancedSlicer&); // Not implemented
};

#endif
