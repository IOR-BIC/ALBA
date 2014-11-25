/*=========================================================================

 Program: MAF2Medical
 Module: medOpSegmentation
 Authors: Eleonora Mambrini, Gianluigi Crimi, Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpSegmentation_H__
#define __medOpSegmentation_H__

#include "mafOp.h"
#include "mafMatrix.h"
#include "mafNode.h"
#include "medVMESegmentationVolume.h"
#include "mafDefines.h"
#include "vtkSystemIncludes.h"
#include "wx/gauge.h"


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
class mafNode;

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
class medViewSliceNotInterpolated;
class wxStaticBoxSizer;

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
class MAF_EXPORT medOpSegmentation: public mafOp
{

public:

  /** Instantiate medOpSegmentation as mafObject inherited from mafOp */
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
    ID_RESET_LOADED,
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
    ID_AUTOMATIC_THRESHOLD,
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
    ID_ENABLE_TRILINEAR_INTERPOLATION,
    ID_REFINEMENT_REMOVE_PENINSULA_REGIONS,
    MINID,
  };

  enum OPERATIONS_IDS
  {
    PRE_SEGMENTATION = 0,
    LOAD_SEGMENTATION,
    AUTOMATIC_SEGMENTATION,
    MANUAL_SEGMENTATION,
    REFINEMENT_SEGMENTATION,
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
  medOpSegmentation(const wxString &label = "Segmentation");

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

  /** Return true if node is of type medVMESegmentationVolume. */
  static bool SegmentationVolumeAccept(mafNode* node) {return(node != NULL  && node->IsMAFType(medVMESegmentationVolume));};

  /** Return true if node is of type mafVMEVolume. */
  static bool SegmentedVolumeAccept(mafNode* node);

  /** Returns the output Volume*/
  mafVMEVolumeGray *GetOutputVolume(){return m_OutputVolume;};

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

  /** Initialize the Threshold Volume single slice */
  void InitThresholdVolumeSlice();

  /** Initialize the Segmented Volume*/
  void InitSegmentedVolume();

  /** Initialize color table values for mask visualization. */
  void InitMaskColorLut(vtkLookupTable *lut);

  /** Init empty volume slice according to input volume*/
  void InitEmptyVolumeSlice();

  /** Init a slice volume with the specified scalars type according to input volume*/
  template <class ImageType>
  void InitDataVolumeSlice(mafVMEVolumeGray *slice);
  //////////////////////////////////////////////////////////////////////////

  /** Remove the VMEs neded by the operation*/
  void RemoveVMEs();

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
  void OnLoadSegmentationEvent(mafEvent *e);

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

  /** Perform the initializations when the user press previous button */
  void OnPreviousStep();

  mafVMEVolumeGray* m_Volume;         //<Input volume
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
  medViewSliceNotInterpolated* m_View;                 //<Rendering Slice view
  mafGUIButton* m_OkButton;           //<Button -GUI
  mafGUIButton* m_CancelButton;       //<Button -GUI
  mafGUIButton* m_LoadSegmentationButton; //<Button -GUI
  mafGUI* m_GuiDialog;                  //Dialog - GUI
  medGUILutHistogramSwatch	*m_LutWidget; //<LUT widget
  mafGUILutSlider *m_LutSlider;         //<LUT Slider
  vtkLookupTable *m_ColorLUT;           //<Lookup table for LUT widget
  vtkLookupTable *m_SegmentationColorLUT; //<Lookup table for segmented volume
  vtkLookupTable *m_ManualColorLUT;     //Lookup table for segmented volume in manual step.
  wxSlider *m_SliceSlider;              //<Slice slider - GUI
  wxTextCtrl *m_SliceText;              //<Slice text box - GUI
  mafGUIRollOut *m_SegmentationOperationsRollOut[5]; //< Steps roll-out - GUI
  mafGUI *m_SegmentationOperationsGui[5]; //<Steps gui
  wxComboBox *m_OperationsList;         //<List if operations (not used)
  mafInteractor *m_OldBehavior;         //<Old volume behavior
  mafNode *m_OldVolumeParent;           //<Old volume parent
  medDeviceButtonsPadMouseDialog* m_DialogMouse; //<Dialog mouse
  mafDeviceManager *m_DeviceManager;    //<Device manager
  mafInteractorSER *m_SER;              //<Static event router                        
  mafMatrix m_Matrix;                   //<Volume initial matrix
  mafVMEVolumeGray *m_LoadedVolume;     //<Loaded volume
  mafString m_LoadedVolumeName;        //<Loaded volume name
  mafVMEVolumeGray *m_ThresholdVolume;  //<Volume mask for thresholding
  mafVMEVolumeGray *m_ThresholdVolumeSlice; //<Single slice volume mask for real-time thresholding preview
  mafVMEVolumeGray *m_EmptyVolumeSlice; //<Single slice volume that represent current slice (for real-time thresholding preview)
  mafVMEVolumeGray *m_OutputVolume;     //<Output volume
  mafVMESurface *m_OutputSurface;          //<Output Surface
  wxStaticText *m_SnippetsLabel;        //<Suggestion labels - GUI
  medInteractorSegmentationPicker *m_SegmentationPicker; //<Segmentation picker for interaction

  //////////////////////////////////////////////////////////////////////////
  //Manual segmentation stuff
  //////////////////////////////////////////////////////////////////////////
  typedef struct urstate
  {
    int plane;
    int slice;
    vtkUnsignedCharArray *dataArray;
  } UndoRedoState; //<Undo Redo data structure

  /** Save the volume mask for the procedural segmentation volume */
  void InitManualVolumeMask();
  
  /** Update manual volume slice volume*/
  void UpdateVolumeSlice();

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

  mafVMEVolumeGray *m_ManualVolumeMask;         //< Manual volume mask
  mafVMEVolumeGray *m_ManualVolumeSlice;        //< Single slice manual volume mask
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
  medInteractorPERBrushFeedback *m_ManualPER;   //<Dynamic event router
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

  /** Perform the preview of automatic segmentation */
  void OnAutomaticPreview();

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

  medVMESegmentationVolume *m_SegmentatedVolume; //<Segmentation volume
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

  medInteractorPERScalarInformation *m_AutomaticPER; //<Interactor for scalar value visualization on mouse move
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Segmentation Refinement stuff
  //////////////////////////////////////////////////////////////////////////
  
  /** Save the volume mask for the procedural segmentation volume */
  void InitRefinementVolumeMask();

  /** Save the volume mask for the procedural segmentation volume */
  void SaveRefinementVolumeMask();
  
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
  void OnEventUpdateManualSlice();

  /** Update threshold volume data  for preview or output */
  void UpdateThresholdVolumeData();

  mafVMEVolumeGray *m_RefinementVolumeMask; //<Refinement volume mask
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
  int m_FloodErease;      //< switch fill/erease for bucket tool

  wxGauge *m_ProgressBar; //< display progress

  /** Delete children of input vme if they are output of the input volume  */
  void DeleteOutputs(mafNode* vme);

  /** Determine if the specified vme is an output of the input volume */
  bool IsOutput(mafNode* vme);

  /** flood fill algorithm */
  void FloodFill(vtkIdType seed);

  /** Apply flood fill filter */
  int ApplyFloodFill(vtkImageData *inputImage, vtkImageData *outputImage, vtkIdType seed);

  void EnableSizerContent(wxSizer* sizer, bool enable);

  void OnEventFloodFill(mafEvent *e);

};
#endif
