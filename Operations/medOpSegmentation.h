/*=========================================================================
Program:   LHP
Module:    $RCSfile: medOpSegmentation.h,v $
Language:  C++
Date:      $Date: 2011-08-29 09:22:30 $
Version:   $Revision: 1.1.2.5 $
Authors:   Eleonora Mambrini, Gianluigi Crimi
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __medOpSegmentation_H__
#define __medOpSegmentation_H__

#include "mafOp.h"
#include "mafMatrix.h"
#include "mafNode.h"
#include "medVMESegmentationVolume.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafAction;
class medDeviceButtonsPadMouseDialog;
class mafDeviceManager;
class mafEvent;
class mafGUIDialog;
class mafGUIButton;
class mafGUI;
class mafGUILutSlider;
class medGUILutHistogramSwatch;
class mafGUIRollOut;
class mafGUIFloatSlider;
class mafInteractorPicker;
class mafInteractorCameraMove;
class mafInteractor;
class mafInteractorGenericMouse;
class mafInteractorSER;
class mafInteractorPER;
class mafMatrix;
class mafVMEVolumeGray;
class mafVME;
class mafVMEImage;

class mmiSelectPoint;

class medViewSliceGlobal;
class medVMESegmentationVolume;
class mafVMESurface;

class medInteractorPERScalarInformation;

class medInteractorSegmentationPicker;
class medInteractorPERBrushFeedback;

class vtkLookupTable;
class vtkActor2D;
class vtkTextMapper;
class vtkSphereSource;
class vtkPolyDataMapper;
class vtkActor;
class vtkStructuredPoints;
class vtkUnsignedCharArray;

//----------------------------------------------------------------------------
// medOpSegmentation :
//----------------------------------------------------------------------------
/**
This operation accept a volume as input and produces a surface as output of segmentation procedure.
User can select:
- manual segmentation
- automatic segmentation
- segmentation refinement
or he can load a previous segmentation.
*/
class medOpSegmentation: public mafOp
{

public:

  mafTypeMacro(medOpSegmentation, mafOp);

  //----------------------------------------------------------------------------
  // Constants:
  //----------------------------------------------------------------------------
  enum GUI_IDS
  {
    ID_OPERATION_TYPE = MINID,
    ID_PRE_SEGMENTATION,
    ID_AUTO_SEGMENTATION,
    ID_MANUAL_SEGMENTATION,
    ID_REFINEMENT,
    ID_LOAD_SEGMENTATION,
    ID_OK,
    ID_CANCEL,
    ID_VIEW,
    ID_LUT_CHOOSER,
    ID_ALL_SLICES,
    ID_SLICE_SLIDER,
    ID_SLICE_TEXT,
    ID_SLICE_NEXT, 
    ID_SLICE_PREV,
    ID_SLICE_PLANE,
	  ID_PRE_VOLUME_SPACING,
	  ID_PRE_VOLUME_ZERO_VALUE,
    ID_MANUAL_PICKING_ACTION,
    ID_MANUAL_PICKING_MODALITY,
    ID_MANUAL_CONTINUOUS_PICKING,
    ID_MANUAL_BRUSH_SHAPE,
    ID_MANUAL_BRUSH_SIZE,
    ID_MANUAL_REFINEMENT_REGIONS_SIZE,
    ID_MANUAL_CANCEL,
    ID_MANUAL_UNDO, 
    ID_MANUAL_REDO,
    ID_BUTTON_PREV,
    ID_BUTTON_NEXT,
    ID_AUTOMATIC_THRESHOLD,//IDs for automatic segmentation GUI
    ID_AUTOMATIC_INCREASE_MIN_THRESHOLD,
    ID_AUTOMATIC_INCREASE_MAX_THRESHOLD,
    ID_AUTOMATIC_DECREASE_MIN_THRESHOLD,
    ID_AUTOMATIC_DECREASE_MAX_THRESHOLD,
    ID_AUTOMATIC_DECREASE_MIDDLE_THRESHOLD,
    ID_AUTOMATIC_INCREASE_MIDDLE_THRESHOLD,
    ID_AUTOMATIC_ADD_RANGE,
    ID_AUTOMATIC_REMOVE_RANGE,
    ID_AUTOMATIC_LIST_OF_RANGE,
    ID_AUTOMATIC_UPDATE_RANGE,
    ID_AUTOMATIC_GLOBAL_THRESHOLD,
    ID_AUTOMATIC_GLOBAL_PREVIEW,
    ID_AUTOMATIC_INCREASE_MIN_RANGE_VALUE,
    ID_AUTOMATIC_INCREASE_MAX_RANGE_VALUE,
    ID_AUTOMATIC_DECREASE_MIN_RANGE_VALUE,
    ID_AUTOMATIC_DECREASE_MAX_RANGE_VALUE,
    ID_AUTOMATIC_DECREASE_MIDDLE_RANGE_VALUE,
    ID_AUTOMATIC_INCREASE_MIDDLE_RANGE_VALUE,
    ID_REFINEMENT_ACTION,
    ID_REFINEMENT_REGIONS_SIZE,
    ID_REFINEMENT_EVERY_SLICE,
    ID_REFINEMENT_ITERATIVE,
    ID_REFINEMENT_APPLY,
    ID_REFINEMENT_UNDO,
    ID_REFINEMENT_REDO,
    MINID,
  };

  enum OPERATIONS_IDS
  {
    PRE_SEGMENTATION = 0,
    AUTOMATIC_SEGMENTATION,
    MANUAL_SEGMENTATION,
    REFINEMENT_SEGMENTATION,
    LOAD_SEGMENTATION,
    NUMBER_OF_OPERATIONS,
  };

  enum MANUAL_SEGMENTATION_IDS
  {
    MANUAL_SEGMENTATION_SELECT = 0,
    MANUAL_SEGMENTATION_ERASE,
  };

  enum BRUSH_SHAPES
  {
    CIRCLE_BRUSH_SHAPE = 0,
    SQUARE_BRUSH_SHAPE,
  };

  enum REFINEMENT_SEGMENTATION_IDS
  {
    ID_REFINEMENT_ISLANDS_REMOVE = 0,
    ID_REFINEMENT_HOLES_FILL,
  };

  /** constructor. */
  medOpSegmentation(const wxString &label = "Segmentate Volume");
  /** destructor. */
  ~medOpSegmentation(); 

   /** Function that handles events sent from other objects. */
  void OnEvent(mafEventBase *maf_event);

  /** return the copy of the operation object */
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Return true if node is of type mafVMEVolume. */
  static bool VolumeAccept(mafNode* node) {return(node != NULL  && node->IsMAFType(medVMESegmentationVolume));};

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);
  /** Creates GUI including renderer window */
  virtual void CreateOpDialog();
  /** Destroys GUI */
  virtual void DeleteOpDialog(); 
  /** Creates GUI with commands for automatic segmentation. */
  virtual void CreateAutoSegmentationGui();
  /**Creates GUI with commands for manual segmentation. */
  virtual void CreateManualSegmentationGui();
  /**Creates GUI with commands for segmentation refinement. */
  virtual void CreateRefinementGui();
  /**Creates GUI with commands for loading a previous segmentation. */
  virtual void CreateLoadSegmentationGui();
  /** Creates GUI widgets to select the right slice (number of slice and plane)*/
  virtual void CreateSliceNavigationGui();
  
  //////////////////////////////////////////////////////////////////////////
  //Method to initialize the stuff
  //////////////////////////////////////////////////////////////////////////
  /** Initialize the volume dimensions attribute */
  void InitVolumeDimensions();
  /** Initialize the slice */
  void InitializeViewSlice();
  /** Initialize the volume spacing attribute */
  void InitVolumeSpacing();
  /** Initialize the Interactors  */
  void InitializeInteractors();
  /** Init GUI slice slider with volume parameters. */
  void InitGui();
  /** Init Manual Segmentation GUI according to input volume*/
  void InitManualSegmentationGui();
  /** Initialize the slices stuff for manual step */
  void InitManualVolumeSlice();
  /** Initialize the Threshold Volume*/
  void InitThresholdVolume();
  /** Initialize the Segmented Volume*/
  void InitSegmentedVolume();
  /** Initialize color table values for mask visualization. */
  void InitMaskColorLut(vtkLookupTable *lut);
  //////////////////////////////////////////////////////////////////////////

  /** Remove the VMEs nedded by the operation*/
  void RemoveVMEs();

  /**Update windowing */
  void UpdateWindowing();

  /** Get current slices origin coords origin[3] */
  void GetSliceOrigin(double *origin);

  /** Update slice widgets and labels. Set current slice position. */
  void UpdateSlice();

  /** Used to remove islands or fill holes in a binary volume*/
  bool Refinement();

  /***/
  void OnAutomaticSegmentationEvent(mafEvent *e);
  /***/
  void OnManualSegmentationEvent(mafEvent *e);
  /***/
  void OnRefinementSegmentationEvent(mafEvent *e);
  

  double GetPosFromSliceIndexZ();

  void onAutomaticStep();
  void onManualStep();
  void onManualStepExit();
  void onRefinementStep();

  /** Perform the initializations when the user press next button */
  void OnNextStep();
  /** Perform the initializations when the user press previous button */
  void OnPreviousStep();

  mafVMEVolumeGray* m_Volume; //<Input volume

  double m_SliceOrigin[3];    //<Origin of the slice plane
  int m_VolumeDimensions[3];    //<Dimensions of the volumes (number of slices)
  double m_VolumeSpacing[3];
  double m_VolumeBounds[6];
  bool m_VolumeParametersInitialized;
  int m_CurrentSliceIndex;    //<Index of the current slice position
  int m_OldSliceIndex;
  int m_CurrentSlicePlane;
  int m_OldSlicePlane;
  int m_NumSliceSliderEvents;

  int m_CurrentOperation;
  
  mafGUIDialog* m_Dialog;             //<Dialog - GUI
  medViewSliceGlobal* m_View;	              //<Rendering Slice view
  mafGUIButton* m_OkButton;           //<Button -GUI
  mafGUIButton* m_CancelButton;       //<Button -GUI
  mafGUIButton* m_LoadSegmentationButton;       //<Button -GUI
  mafGUI* m_GuiDialog;                //Dialog - GUI
  medGUILutHistogramSwatch	*m_LutWidget;       //<LUT widget
  mafGUILutSlider *m_LutSlider;       //<LUT Slider
  vtkLookupTable *m_ColorLUT;         //<Lookup table for LUT widget

  vtkLookupTable *m_SegmentationColorLUT;         //<Lookup table for segmented volume
  vtkLookupTable *m_ManualColorLUT;               //Lookup table for segmented volume in manual step.


  wxSlider *m_SliceSlider;
  wxTextCtrl *m_SliceText;

  mafGUIRollOut *m_SegmentationOperationsRollOut[5];
  mafGUI *m_SegmentationOperationsGui[5];

  wxComboBox *m_OperationsList;

  mafInteractor *m_OldBehavior;             //<Old volume behavior
  mafNode *m_OldVolumeParent;               //<Old volume parent

  medDeviceButtonsPadMouseDialog* m_DialogMouse;
  mafDeviceManager *m_DeviceManager;        //<Device manager
  mafInteractorSER *m_SER;      //<Static event router                        
 
  mafMatrix m_Matrix;

  mafVMEVolumeGray *m_ThresholdVolume;
  mafVMEVolumeGray *m_OutputVolume;

  wxStaticText *m_SnippetsLabel;

  //////////////////////////////////////////////////////////////////////////
  //Manual segmentation stuff
  //////////////////////////////////////////////////////////////////////////
  typedef struct urstate
  {
    int plane;
    int slice;
    vtkUnsignedCharArray *dataArray;
  } UndoRedoState;

  /** Save the volume mask for the procedural segmentation volume */
  void InitManualVolumeMask();
  /***/
  void UpdateVolumeSlice();
  /***/

  void ApplyVolumeSliceChanges();
  /***/
  void SelectBrushImage(double x, double y, double z, bool selection);
  
  /***/
  void OnBrushEvent(mafEvent *e);

  /***/
  void ReloadUndoRedoState(vtkDataSet *dataSet, UndoRedoState state);
  
  /***/
  void ResetManualUndoList();
  
  /***/
  void ResetManualRedoList();
  
  /***/
  void EnableManualSegmentationGui();

  mafVMEVolumeGray *m_ManualVolumeMask;
  mafVMEVolumeGray *m_ManualVolumeSlice;

  wxComboBox *m_ManualEditingActionComboBox;
  mafGUIFloatSlider *m_ManualBrushSizeSlider;
  wxTextCtrl     *m_ManualBrushSizeText;
  wxRadioBox *m_ManualBrushShapeRadioBox;
  mafGUIButton *m_ManualApplyChanges;
  //int m_ManualSegmentationMode;
  int m_ManualSegmentationAction;
  int m_ManualBrushShape;
  double m_ManualBrushSize;
  int m_ManualRefinementRegionsSize; 

  
  wxComboBox *m_ManualRefinementComboBox;
  wxTextCtrl  *m_ManualRefinementRegionSizeText;
  
  std::vector<UndoRedoState> m_ManualUndoList;
  std::vector<UndoRedoState> m_ManualRedoList;
  bool m_PickingStarted;

  medInteractorSegmentationPicker *m_ManualPicker;

  //lhpInteractorPERScalarInformation *m_ManualStandardPER;   //<Dinamic event router  
  medInteractorPERBrushFeedback *m_ManualPER;
  //////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////
  //Automatic segmentation stuff
  //////////////////////////////////////////////////////////////////////////
  
  /***/
  void SetSelectionAutomaticListOfRange(int index);

  /** Check that the range has right values */
  bool AutomaticCheckRange(int indexToExclude = -1);

  /** Function called when the user use the fine button to change the threshold*/
  void OnAutomaticChangeThresholdManually(int eventID);

  /** Check if all thresholds exist */
  bool CheckNumberOfThresholds();

  /** Update the value of threshold label for the current slice */
  void UpdateThresholdLabel();

  /** Update the value of slice label for the current slice */
  void UpdateSliceLabel();

  /**When the user select manually the scalar value */
  void OnAutomaticPicker(mafEvent *e);

  /** Gui update when the user change threshold type */
  void OnChangeThresholdType();

  /** Add a new range using gui values */
  void OnAutomaticAddRange();

  /** Remove the select range */
  void OnAutomaticRemoveRange();

  /** Update the select range using gui values */
  void OnAutomaticUpdateRange();

  /** Perform the preview of automatic segmentation */
  void OnAutomaticPreview();

  /** Function called when the user use the fine button to change the range of the automatic segmentation */
  void OnAutomaticChangeRangeManually(int eventID);

  medVMESegmentationVolume *m_SegmentatedVolume;

  int m_AutomaticGlobalThreshold;
  double m_AutomaticThreshold;
  double m_AutomaticUpperThreshold;
  double m_AutomaticMouseThreshold;
  wxListBox *m_AutomaticListOfRange;
  mafGUILutSlider *m_AutomaticRangeSlider;

  struct AutomaticInfoRange 
  {
    int m_StartSlice;
    int m_EndSlice;
    double m_ThresholdValue;
    double m_UpperThresholdValue;
  };
  std::vector<AutomaticInfoRange> m_AutomaticRanges;
  
  mafGUILutSlider *m_AutomaticThresholdSlider;


  vtkTextMapper *m_AutomaticThresholdTextMapper;
  vtkActor2D *m_AutomaticThresholdTextActor;

  vtkTextMapper *m_AutomaticScalarTextMapper;
  vtkActor2D *m_AutomaticScalarTextActor;



  vtkTextMapper *m_AutomaticSliceTextMapper;
  vtkActor2D *m_AutomaticSliceTextActor;

  medInteractorSegmentationPicker *m_AutomaticPicker;
  medInteractorPERScalarInformation *m_AutomaticPER;
  //////////////////////////////////////////////////////////////////////////



  //////////////////////////////////////////////////////////////////////////
  //Segmentation Refinement stuff
  //////////////////////////////////////////////////////////////////////////
  
  /** Save the volume mask for the procedural segmentation volume */
  void InitRefinementVolumeMask();

  /** Save the volume mask for the procedural segmentation volume */
  void SaveRefinementVolumeMask();
  /***/
  void ResetRefinementUndoList();
  /***/
  void ResetRefinementRedoList();

  bool ApplyRefinementFilter(vtkStructuredPoints *inputImage, vtkStructuredPoints *outputImage);

  mafVMEVolumeGray *m_RefinementVolumeMask;

  int m_RefinementSegmentationAction;
  int m_RefinementRegionsSize;
  int m_RefinementMajorityThreshold;
  int m_RefinementEverySlice;
  int m_RefinementIterative;

  std::vector<vtkUnsignedCharArray *> m_RefinementUndoList;
  std::vector<vtkUnsignedCharArray *> m_RefinementRedoList;
 
};
#endif
