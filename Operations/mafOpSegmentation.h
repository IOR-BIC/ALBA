/*=========================================================================

 Program: MAF2
 Module: mafOpSegmentation
 Authors: Eleonora Mambrini, Gianluigi Crimi, Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpSegmentation_H__
#define __mafOpSegmentation_H__

#include "mafOp.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafVMESegmentationVolume.h"
#include "mafDefines.h"
#include "vtkSystemIncludes.h"
#include "wx/gauge.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafAction;
class mafDeviceButtonsPadMouseDialog;
class mafDeviceManager;
class mafEvent;
class mafGUIDialog;
class mafGUIButton;
class mafGUI;
class mafGUILutSlider;
class mafGUILutHistogramSwatch;
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
class mafVME;
class mmiSelectPoint;
class mafVMESurface;
class mafInteractorPERScalarInformation;
class mafInteractorSegmentationPicker;
class mafInteractorPERBrushFeedback;
class vtkLookupTable;
class vtkActor2D;
class vtkTextMapper;
class vtkSphereSource;
class vtkPolyDataMapper;
class vtkActor;
class vtkStructuredPoints;
class vtkUnsignedCharArray;
class mafViewSliceSegmentation;
class wxStaticBoxSizer;

//----------------------------------------------------------------------------
// mafOpSegmentation :
//----------------------------------------------------------------------------
/**
This operation accept a volume as input and produces a surface as output of segmentation procedure.
User can select:
- manual segmentation
- automatic segmentation
- segmentation refinement
or he can load a previous segmentation.
*/
class MAF_EXPORT mafOpSegmentation: public mafOp
{

public:

  /** Instantiate mafOpSegmentation as mafObject inherited from mafOp */
  mafTypeMacro(mafOpSegmentation, mafOp);

  //----------------------------------------------------------------------------
  // Constants:
  //----------------------------------------------------------------------------
  enum GUI_IDS
  {
    ID_OPERATION_TYPE = MINID,
    ID_PRE_SEGMENTATION,
    ID_INIT_SEGMENTATION,
    ID_EDIT_SEGMENTATION,
    ID_LOAD,
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
    ID_MANUAL_PICKING_MODALITY,
    ID_MANUAL_TOOLS,
    ID_MANUAL_BRUSH_SHAPE,
    ID_MANUAL_BRUSH_SIZE,
    ID_MANUAL_BUCKET_ACTION,
    ID_MANUAL_BUCKET_GLOBAL,
    ID_MANUAL_REFINEMENT_REGIONS_SIZE,
    ID_MANUAL_CANCEL,
    ID_MANUAL_UNDO, 
    ID_MANUAL_REDO,
    ID_BUTTON_PREV,
    ID_BUTTON_NEXT,
    ID_AUTO_THRESHOLD,
    ID_AUTO_INC_MIN_THRESHOLD,
    ID_AUTO_INC_MAX_THRESHOLD,
    ID_AUTO_DEC_MIN_THRESHOLD,
    ID_AUTO_DEC_MAX_THRESHOLD,
    ID_AUTO_DEC_MIDDLE_THRESHOLD,
    ID_AUTO_INC_MIDDLE_THRESHOLD,
    ID_AUTO_ADD_RANGE,
    ID_AUTO_REMOVE_RANGE,
    ID_AUTO_LIST_OF_RANGE,
    ID_AUTO_UPDATE_RANGE,
    ID_AUTO_GLOBAL_THRESHOLD,
    ID_AUTO_INC_MIN_RANGE,
    ID_AUTO_INC_MAX_RANGE,
    ID_AUTO_DEC_MIN_RANGE,
    ID_AUTO_DEC_MAX_RANGE,
    ID_AUTO_DEC_MIDDLE_RANGE,
    ID_AUTO_INC_MIDDLE_RANGE,
    ID_REFINEMENT_ACTION,
    ID_REFINEMENT_EVERY_SLICE,
    ID_REFINEMENT_ITERATIVE,
    ID_REFINEMENT_APPLY,
    ID_REFINEMENT_UNDO,
    ID_REFINEMENT_REDO,
    ID_ENABLE_TRILINEAR_INTERPOLATION,
    ID_REFINEMENT_REMOVE_PENINSULA_REGIONS,
    MINID,
  };

  enum OPERATIONS_IDS
  {
    INIT_SEGMENTATION = 0,
    EDIT_SEGMENTATION,
    NUMBER_OF_PHASES,
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
  mafOpSegmentation(const wxString &label = "Segmentation");

  /** destructor. */
  ~mafOpSegmentation(); 

   /** Function that handles events sent from other objects. */
  void OnEvent(mafEventBase *maf_event);

  /** return the copy of the operation object */
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Return true if node is of type mafVMESegmentationVolume. */
  static bool SegmentationVolumeAccept(mafVME* node) {return(node != NULL  && node->IsMAFType(mafVMESegmentationVolume));};

  /** Return true if node is of type mafVMEVolume. */
  static bool SegmentedVolumeAccept(mafVME* node);
	 
protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Creates GUI including renderer window */
  virtual void CreateOpDialog();
  
  /** Destroys GUI */
  virtual void DeleteOpDialog(); 
  
  /** Creates GUI with commands for automatic segmentation. */
  virtual void CreateInitSegmentationGui();
  
  /**Creates GUI with commands for manual segmentation. */
  virtual void CreateEditSegmentationGui();
  
  /** Creates GUI widgets to select the right slice (number of slice and plane)*/
  virtual void CreateSliceNavigationGui();
  
  //////////////////////////////////////////////////////////////////////////
  //Method to initialize the stuff
  //////////////////////////////////////////////////////////////////////////
  
  /** Initialize the volume dimensions attribute */
  void InitVolumeDimensions();
  
  /** Initialize the slice */
  void InitializeView();
  
  /** Initialize the volume spacing attribute */
  void InitVolumeSpacing();
  
  /** Initialize the Interactors  */
  void InitializeInteractors();
  
  /** Init GUI slice slider with volume parameters. */
  void InitGui();
  
  /** Initialize the Segmented Volume*/
  void InitSegmentationVolume();
	  
  /**Update windowing */
  void UpdateWindowing();

  /** Get current slices origin coords origin[3] */
  void GetSliceOrigin(double *origin);

  /** Update slice widgets and labels. Set current slice position. */
  void UpdateSlice();

  /** Used to remove islands or fill holes in a binary volume*/
  bool Refinement();

  /** Receive events from Automatic segmentation gui */
  void OnAutomaticSegmentationEvent(mafEvent *e);
  
  /** Receive events from Manual segmentation gui */
  void OnManualSegmentationEvent(mafEvent *e);
  
  /** Receive events from Refinement segmentation gui */
  void OnRefinementSegmentationEvent(mafEvent *e);
  
  /** Receive events from Load segmentation gui */
  void OnLoadSegmentationEvent();

  /** Get the z position of the specified slice index */
  double GetPosFromSliceIndexZ();

  /** Initialize Automatic step */
  void OnAutomaticStep();

  /** De initialize Initialize Automatic step */
  void OnAutomaticStepExit();

  /** Initialize Manual step */
  void OnManualStep();

  /** De initialize Initialize Manual step */
  void OnManualStepExit();

  /** Initialize Refinement step */
  void OnRefinementStep();

  /** Initialize Load step */
  void OnLoadStep();

  /** Perform the initializations when the user press next button */
  void OnNextStep();

	void Init();

  /** Perform the initializations when the user press previous button */
  void OnPreviousStep();

  mafVMEVolumeGray* m_Volume;         //<Input volume
	mafString m_VolumeName;        //<Loaded volume name
	mafVMEVolumeGray *m_SegmentationVolume;     //<Segmentation volume
	vtkImageData     *m_SegmetationSlice;
  double m_SliceOrigin[3];            //<Origin of the slice plane
  int m_VolumeDimensions[3];          //<Dimensions of the volumes (number of slices)
  double m_VolumeSpacing[3];          //<Volume spacing
  double m_VolumeBounds[6];           //<Volume bounds
  bool m_VolumeParametersInitialized; //<Specify if volume parameters are initialized
  int m_CurrentSliceIndex;            //<Index of the current slice position
  int m_OldSliceIndex;                //<Old slice index
  int m_CurrentSlicePlane;            //<Current slicing plane (xy,xz,yx)
  int m_OldSlicePlane;                //<Old slice plane
  int m_NumSliceSliderEvents;         //<Number of events raised by the slider in a single interaction
  int m_CurrentOperation;             //<Current step
  mafGUIDialog* m_Dialog;             //<Dialog - GUI
	mafViewSliceSegmentation* m_View;                 //<Rendering Slice view
  mafGUIButton* m_OkButton;           //<Button -GUI
  mafGUIButton* m_CancelButton;       //<Button -GUI
  mafGUIButton* m_LoadSegmentationButton; //<Button -GUI
  mafGUI* m_GuiDialog;                  //Dialog - GUI
  mafGUILutHistogramSwatch	*m_LutWidget; //<LUT widget
  mafGUILutSlider *m_LutSlider;         //<LUT Slider
  vtkLookupTable *m_ColorLUT;           //<Lookup table for LUT widget
  vtkLookupTable *m_SegmentationColorLUT; //<Lookup table for segmented volume
  vtkLookupTable *m_ManualColorLUT;     //Lookup table for segmented volume in manual step.
  wxSlider *m_SliceSlider;              //<Slice slider - GUI
  wxTextCtrl *m_SliceText;              //<Slice text box - GUI
  mafGUIRollOut *m_SegmentationOperationsRollOut[5]; //< Steps roll-out - GUI
  mafGUI *m_SegmentationOperationsGui[2]; //<Steps gui
  wxComboBox *m_OperationsList;         //<List if operations (not used)
  mafInteractor *m_OldBehavior;         //<Old volume behavior
  mafVME *m_OldVolumeParent;           //<Old volume parent
  mafDeviceButtonsPadMouseDialog* m_DialogMouse; //<Dialog mouse
  mafDeviceManager *m_DeviceManager;    //<Device manager
  mafInteractorSER *m_SER;              //<Static event router                        
  mafMatrix m_Matrix;                   //<Volume initial matrix
	mafVMESurface *m_OutputSurface;          //<Output Surface
  wxStaticText *m_SnippetsLabel;        //<Suggestion labels - GUI
  mafInteractorSegmentationPicker *m_SegmentationPicker; //<Segmentation picker for interaction

  //////////////////////////////////////////////////////////////////////////
  //Manual segmentation stuff
  //////////////////////////////////////////////////////////////////////////
  typedef struct urstate
  {
    int plane;
    int slice;
    vtkUnsignedCharArray *dataArray;
  } UndoRedoState; //<Undo Redo data structure

  /** Apply volume slice changes*/
  void ApplyVolumeSliceChanges();
  
  /** Set flags that indicate that the user start drae interaction */
  void StartDraw(mafEvent *e, bool erase);

  /** Select brush image depending on shape and size */
  void SelectBrushImage(double x, double y, double z, bool selection);
  
  /** Trap events raised from the brush */
  void OnBrushEvent(mafEvent *e);

  /** Relead the previous/next undo/redo state*/
  void ReloadUndoRedoState(vtkDataSet *dataSet, UndoRedoState state);
  
  /** Reset undo list*/
  void ResetManualUndoList();
  
  /** Reset redo list */
  void ResetManualRedoList();
  
  /** Enable manual segmentation gui widget */
  void EnableManualSegmentationGui();

  /** Used in brush preview to restore the real drawn image */
  void UndoBrushPreview();

  /** Create the real drawn image */
  void CreateRealDrawnImage();

  mafGUIFloatSlider *m_ManualBrushSizeSlider;   //<Brush size slider - GUI
  wxTextCtrl *m_ManualBrushSizeText;            //<Brush size text box - GUI
  wxRadioBox *m_ManualBrushShapeRadioBox;       //<Brush shape radio - GUI
  wxStaticBoxSizer *m_BrushEditingSizer;
  mafGUIButton *m_ManualApplyChanges;           //<Apply changes button - GUI
  int m_ManualSegmentationAction;               //<Manual segmentation action (draw/erease)
  int m_ManualSegmentationTools;                //<Manual segmentation tools (bucket/brush)
  int m_ManualBucketActions;                    //<
  int m_ManualBrushShape;                       //<Brush shape
  double m_ManualBrushSize;                     //<Brush size
  int m_ManualRefinementRegionsSize;            //<Refinement region size
  wxComboBox *m_ManualRefinementComboBox;       //<Refinement action combo - GUI
  wxTextCtrl *m_ManualRefinementRegionSizeText; //<Refinement size text - GUI
  std::vector<UndoRedoState> m_ManualUndoList;  //< Undo stack
  std::vector<UndoRedoState> m_ManualRedoList;  // Redo stack
  bool m_PickingStarted;                        //<Determine if picking has started
  mafInteractorPERBrushFeedback *m_ManualPER;   //<Dynamic event router
  double m_CurrentBrushMoveEventCount;          //<Id for mouse move event raised by the brush
  vtkUnsignedCharArray *m_RealDrawnImage;       //<Real drawn image used in brush preview
  int m_LastMouseMovePointID;                   //<Last point id in mouse move event
  mafGUILutSlider* m_ManualRangeSlider;         //<
  wxStaticBoxSizer *m_BucketEditingSizer;       //<
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

  /** Gui update when the user change threshold type */
  void OnChangeThresholdType();

  /** Add a new range using gui values */
  void OnAutomaticAddRange();

  /** Remove the select range */
  void OnAutomaticRemoveRange();

  /** Update the select range using gui values */
  void OnAutomaticUpdateRange();

   /** Function called when the user use the fine button to change the range of the automatic segmentation */
  void OnAutomaticChangeRangeManually(int eventID);

  /** Get camera attributes */
  void GetCameraAttribute(double *focalPoint, double* scaleFactor);

  /** Get the visualized bounds */
  void GetVisualizedBounds(double focalPoint[3], double scaleFactor, double bounds[4]);
  
  /** Reset zoom view */
  bool ResetZoom(vtkDataSet* dataset,double visbleBounds[4]);

  /** Update threshold real-time preview*/
  void UpdateThresholdRealTimePreview();

  int m_AutomaticGlobalThreshold;   //<Global threshold range lower bound
  double m_AutomaticThreshold;      //<Global threshold range lower bound
  double m_AutomaticUpperThreshold; //<Global threshold range upper bound
  double m_AutomaticMouseThreshold; //<Mouse threshold
  wxListBox *m_AutomaticListOfRange;//<List of threshold ranges
  mafGUILutSlider *m_AutomaticRangeSlider;//<Range slider - GUI

  struct AutomaticInfoRange 
  {
    int m_StartSlice;
    int m_EndSlice;
    double m_ThresholdValue;
    double m_UpperThresholdValue;
  };
  std::vector<AutomaticInfoRange> m_AutomaticRanges; //<Automatic range structure
  
  mafGUILutSlider *m_AutomaticThresholdSlider;//<Threshold slider

  vtkTextMapper *m_AutomaticThresholdTextMapper;//<Text mapper for threshold visualization
  vtkActor2D *m_AutomaticThresholdTextActor;    //<Text actor for threshold visualization
  vtkTextMapper *m_AutomaticScalarTextMapper;   //<Text mapper for scalar visualization 
  vtkActor2D *m_AutomaticScalarTextActor;       //<Text actor for scalar visualization
  vtkTextMapper *m_AutomaticSliceTextMapper;    //<Text mapper for slice visualization
  vtkActor2D *m_AutomaticSliceTextActor;        //<Text mapper for slice visualization

  mafInteractorPERScalarInformation *m_AutomaticPER; //<Interactor for scalar value visualization on mouse move
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Segmentation Refinement stuff
  /** Reset refinement undo list */
  void ResetRefinementUndoList();
  
  /** Reset refinement redo list */
  void ResetRefinementRedoList();

  /** Apply refinement algorithm implemented with ITK (not used) */
  bool ApplyRefinementFilter(vtkStructuredPoints *inputImage, vtkStructuredPoints *outputImage);

  /** Apply refinement algorithm implemented with vtk only */
  bool ApplyRefinementFilter2(vtkStructuredPoints *inputImage, vtkStructuredPoints *outputImage);

  /** Update slice visualisation on threshold step */
  void OnEventUpdateThresholdSlice();

  /** Update slice visualisation on manual step */
  void OnUpdateSlice();

  /** Update threshold volume data  for preview or output */
  void UpdateThresholdVolumeData();

  int m_RefinementSegmentationAction;       //<Refinement action fill holes or remove islands
  int m_RefinementRegionsSize;              //<Size for region recognition
  int m_RefinementMajorityThreshold;        //<Used in itk algorithm (not yet exposed and used)
  int m_RefinementEverySlice;               //<Determine if refinement performed on every slice
  int m_RefinementIterative;                //<Iterative refinement (not yet used)

  double m_InitialFocalPoint[3];            //<Initial camera focal point
  double m_InitialScaleFactor;              //<Initial camera scale factor

  std::vector<vtkUnsignedCharArray *> m_RefinementUndoList; //<Refinement undo list
  std::vector<vtkUnsignedCharArray *> m_RefinementRedoList; //<Refinement redo list

  int m_MajorityThreshold;                   //<Used in itk algorithm (not yet exposed and used)

  int m_OldAutomaticThreshold;      //<Used to update real time threshold preview only if needed
  int m_OldAutomaticUpperThreshold; //<Used to update real time threshold preview only if needed

  int m_RemovePeninsulaRegions; //<Determine if refinement filter removes penisula regions or not
  int m_OLdWindowingLow;        //<Windowing original value to restore after exiting operation
  int m_OLdWindowingHi;         //<Windowing original value to restore after exiting operation

  int m_GlobalFloodFill;  //< global or local bucket tool
  int m_FloodErease;      //< switch fill/erase for bucket tool

  wxGauge *m_ProgressBar; //< display progress
	  
  /** flood fill algorithm */
  void FloodFill(vtkIdType seed);

  /** Apply flood fill filter */
  int ApplyFloodFill(vtkImageData *inputImage, vtkImageData *outputImage, vtkIdType seed);

  void EnableSizerContent(wxSizer* sizer, bool enable);

  void OnEventFloodFill(mafEvent *e);

};
#endif
