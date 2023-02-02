/*=========================================================================
Program:   Albedo
Module:    albaOpInteractorSliderSample.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpInteractorSliderSample_H__
#define __albaOpInteractorSliderSample_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
//#include "albaOperationsDefines.h"

#include "albaOp.h"
#include "albaVMEVolumeGray.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaInteractorSlider;
class albaView;
class vtkRectilinearGrid;
class vtkImageData;

// Class Name: albaOpInteractorSliderSample
class ALBA_EXPORT albaOpInteractorSliderSample: public albaOp
{
public:

	//Widgets ID's	
	enum OP_INTERACTION_SAMPLE_ID
	{
		ID_NONE = MINID,
		ID_SLIDER_SHOW,
		ID_SLIDER_LABELS_SHOW,
		ID_SLIDER_EDIT,
		ID_SLIDER_INIT,
	};

  albaOpInteractorSliderSample(const wxString &label = "Interactor Slider Sample");
 ~albaOpInteractorSliderSample(); 

	//virtual void OnEvent(albaEventBase *alba_event);
  
  albaTypeMacro(albaOpInteractorSliderSample, albaOp);

  albaOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(albaVME *node);

	/** Builds operation's interface. */
  void OpRun();

  /** Precess events coming from other objects */
  void OnEvent(albaEventBase *alba_event);

protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Create the GUI */
  virtual void CreateGui();

	albaInteractorSlider *m_InteractorSlider;
	albaView *m_View;

	double m_SliderValue;
	double m_SliderMin;
	double m_SliderMax;
	int m_SliderSteps;

	int m_SliderPosX;
	int m_SliderPosY;
	int m_SliderLenght;
	int m_SliderOrientation;

	int m_SliderShow;
	int m_SliderLabelsShow;

	void GetSliceOrigin(double *origin);
	void UpdateVolumeSlice();

	albaVMEVolumeGray* m_Volume;	//<Input volume
	int m_SlicePlane;						//<Current slicing plane (xy,xz,yx)

	enum PLANE_TYPE
	{
		YZ = 0,
		XZ,
		XY,
	};
};
#endif
