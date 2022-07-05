/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSegmentation
 Authors: Eleonora Mambrini, Gianluigi Crimi, Alberto Losi, Nicola Vanella
 ==========================================================================
 Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE. See the above copyright notice for more information.
 =========================================================================*/

#ifndef __albaOpSegmentation_H__
#define __albaOpSegmentation_H__

#include "albaDefines.h"
#include "albaMatrix.h"
#include "albaOp.h"
#include "albaOpSegmentationHelper.h"
#include "albaVME.h"
#include "albaVMESegmentationVolume.h"

#include "vtkSystemIncludes.h"
#include "wx/gauge.h"
#include "albaLogicWithManagers.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaAction;
class albaDeviceButtonsPadMouseDialog;
class albaDeviceManager;
class albaEvent;
class albaGUI;
class albaGUIButton;
class albaGUIDialog;
class albaGUIFloatSlider;
class albaGUILutHistogramSwatch;
class albaGUILutSlider;
class albaGUIRollOut;
class albaInteractor;
class albaInteractorCameraMove;
class albaInteractorGenericMouse;
class albaInteractorPER;
class albaInteractorPERBrushFeedback;
class albaInteractorPERScalarInformation;
class albaInteractorPicker;
class albaInteractorSER;
class albaInteractorSegmentationPicker;
class albaMatrix;
class albaVME;
class albaVMEImage;
class albaVMESurface;
class albaVMEVolumeGray;
class albaViewSliceSegmentation;
class mmiSelectPoint;
class vtkActor;
class vtkActor2D;
class vtkLookupTable;
class vtkPolyDataMapper;
class vtkSphereSource;
class vtkImageData;
class vtkTextMapper;
class vtkUnsignedCharArray;
class wxStaticBoxSizer;

//----------------------------------------------------------------------------
// albaOpSegmentation :
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//Manual segmentation stuff
typedef struct urstate
{
	int m_Plane;
	int m_Slice;
	vtkUnsignedCharArray *m_Scalars;
} UndoRedoState; //<Undo Redo data structure

/**
This operation accept a volume as input and produces a surface as output of segmentation procedure.
User can select:
- manual segmentation
- automatic segmentation
- segmentation refinement
or he can load a previous segmentation.
*/
class ALBA_EXPORT albaOpSegmentation: public albaOp
{

public:

  /** Instantiate albaOpSegmentation as albaObject inherited from albaOp */
  albaTypeMacro(albaOpSegmentation, albaOp);

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
		ID_SHOW_LABELS,
		ID_PRE_VOLUME_SPACING,
		ID_PRE_VOLUME_ZERO_VALUE,
		ID_MANUAL_PICKING_MODALITY,
		ID_SWITCH_TO_YZ,
		ID_SWITCH_TO_XZ,
		ID_SWITCH_TO_XY,
		ID_MANUAL_TOOLS_BRUSH,
		ID_MANUAL_TOOLS_FILL,
		ID_MANUAL_TOOLS_3D_CONNECTIVITY,
		ID_MANUAL_BRUSH_SHAPE,
		ID_MANUAL_BRUSH_SIZE,
		ID_MANUAL_AUTOFILL_ON_RELASE,
    ID_MANUAL_BUCKET_ACTION,
    ID_MANUAL_BUCKET_GLOBAL,
    ID_MANUAL_REFINEMENT_REGIONS_SIZE,
    ID_MANUAL_CANCEL,
		ID_MANUAL_COPY_FROM_LAST_SLICE,
    ID_MANUAL_UNDO, 
    ID_MANUAL_REDO,
    ID_BUTTON_INIT,
    ID_BUTTON_EDIT,
    ID_AUTO_THRESHOLD,
    ID_AUTO_INC_MIN_THRESHOLD,
    ID_AUTO_INC_MAX_THRESHOLD,
    ID_AUTO_DEC_MIN_THRESHOLD,
    ID_AUTO_DEC_MAX_THRESHOLD,
    ID_AUTO_DEC_MIDDLE_THRESHOLD,
    ID_AUTO_INC_MIDDLE_THRESHOLD,
    ID_SPLIT_RANGE,
    ID_REMOVE_RANGE,
    ID_RANGE_SELECTION,
    ID_INIT_MODALITY,
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
		
  enum REFINEMENT_SEGMENTATION_IDS
  {
    ID_REFINEMENT_ISLANDS_REMOVE = 0,
    ID_REFINEMENT_HOLES_FILL,
  };

  /** constructor. */
  albaOpSegmentation(const wxString &label = "Segmentation", int disableInit=false);

  /** destructor. */
  ~albaOpSegmentation(); 

   /** Function that handles events sent from other objects. */
  void OnEvent(albaEventBase *alba_event);

	void SliceNext();

	void SlicePrev();

	/** return the copy of the operation object */
  albaOp* Copy();

  /** Builds operation's interface. */
	virtual void OpRun();

  /** Execute the operation. */
	virtual void OpDo();

  /** Makes the undo for the operation. */
  virtual void OpUndo();

  /** Return true if node is of type albaVMESegmentationVolume. */
  static bool SegmentationVolumeAccept(albaVME* node) {return(node != NULL  && node->IsALBAType(albaVMESegmentationVolume));};

  /** Return true if node is of type albaVMEVolume. */
  static bool SegmentedVolumeAccept(albaVME* node);
	 
	static int OpSegmentationEventFilter(wxEvent& event);


	/** Sets SliceIndex */
	void SetSlicingIndexes(int planeindex, int sliceIndex);

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

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
  
	virtual void Init();

  /** Initialize the volume dimensions attribute */
  void InitVolumeDimensions();
  
  /** Initialize the slice */
  virtual void InitializeView();
  
	/** Initialize the Interactors  */
  void InitializeInteractors();
  
  /** Init GUI slice slider with volume parameters. */
  void UpdateSliderValidator();
  
  /** Initialize the Segmented Volume*/
  void InitSegmentationVolume();
	  
  /** Update windowing */
  void UpdateWindowing();

  /** Get current slices origin coords origin[3] */
  void GetSliceOrigin(double *origin);

  /** Update slice widgets and labels. Set current slice position. */
  void UpdateSlice();

  /** Used to remove islands or fill holes in a binary volume*/
  bool Refinement();

  /** Receive events from Automatic segmentation gui */
  void OnInitEvent(albaEvent *e);

	void SetThresholdByRange();
	
  /** Receive events from Manual segmentation gui */
  void OnEditSegmentationEvent(albaEvent *e);

	void OnConnectivity3d();

	void CopyFromLastSlice();

	void OnUndoRedo(bool undo);
	
  /** Receive events from Refinement segmentation gui */
  void OnRefinementSegmentationEvent(albaEvent *e);
  
  /** Receive events from Load segmentation gui */
  void OnLoadSegmentation();

  /** De initialize Initialize Manual step */
  void OnEditStepExit();

  /** Initialize Refinement step */
  void OnRefinementStep();
	
  /** Perform the initializations when the user press next button */
  void OnEditStep();

	/** Perform the initializations when the user press previous button */
  void OnPreviousStep();

  albaVMEVolumeGray* m_Volume;         //<Input volume
	vtkImageData     *m_VolumeSlice;
	albaString m_VolumeName;        //<Loaded volume name
	albaVMEVolumeGray *m_SegmentationVolume;     //<Segmentation volume
	vtkImageData     *m_SegmentationSlice;
  
	double m_SliceOrigin[3];            //<Origin of the slice plane
  int m_VolumeDims[3];          //<Dimensions of the volumes (number of slices)
  double m_VolumeSpacing[3];          //<Volume spacing
  double m_VolumeBounds[6];           //<Volume bounds
	int m_SliceIndex;								//GuiVariable
	int m_GUISliceIndex;								//GuiVariable
	int m_OldSliceIndex;								//GuiVariable
	int m_SliceIndexByPlane[3];            //<Index of the current slice position
	int m_SlicePlane;//<Current slicing plane (xy,xz,yx)
	int m_GUISlicePlane;
	int m_OldSlicePlane;
  int m_NumSliceSliderEvents;         //<Number of events raised by the slider in a single interaction
  int m_CurrentPhase;             //<Current step
	int m_DisableInit;
	int m_ShowLabels;
  
	albaGUIDialog* m_Dialog;             //<Dialog - GUI
	albaViewSliceSegmentation* m_View;                 //<Rendering Slice view
  albaGUIButton* m_OkButton;           //<Button -GUI
  albaGUIButton* m_CancelButton;       //<Button -GUI
  albaGUIButton* m_LoadSegmentationButton; //<Button -GUI
  albaGUI* m_GuiDialog;                  //Dialog - GUI
  albaGUILutHistogramSwatch	*m_LutWidget; //<LUT widget
  albaGUILutSlider *m_LutSlider;         //<LUT Slider
  vtkLookupTable *m_ColorLUT;           //<Lookup table for LUT widget
  wxSlider *m_SliceSlider;              //<Slice slider - GUI
  wxTextCtrl *m_SliceText;              //<Slice text box - GUI
	wxNotebook *m_SegmentationOperationsNotebook;
  albaGUI *m_SegmentationOperationsGui[2]; //<Steps gui
  wxComboBox *m_OperationsList;         //<List if operations (not used)
  albaInteractor *m_OldBehavior;         //<Old volume behavior
  albaVME *m_OldVolumeParent;           //<Old volume parent
  albaDeviceButtonsPadMouseDialog* m_DialogMouse; //<Dialog mouse
  albaDeviceManager *m_DeviceManager;    //<Device manager
  albaInteractorSER *m_SER;              //<Static event router                        
  albaMatrix m_Matrix;                   //<Volume initial matrix
	albaVMESurface *m_OutputSurface;          //<Output Surface
  wxStaticText *m_SnippetsLabel;        //<Suggestion labels - GUI
  albaInteractorSegmentationPicker *m_SegmentationPicker; //<Segmentation picker for interaction

	albaGUI* m_AppendingOpGui;
	  	  
  /** Set flags that indicate that the user start draw interaction */
  void StartDraw(albaEvent *e);

	void AddUndoStep();

	UndoRedoState CreateUndoRedoState();
	
  /** Reset undo list*/
  void ClearManualUndoList();
  
  /** Reset redo list */
  void ClearManualRedoList();

  /** Used in brush preview to restore the real drawn image */
  void RestoreSliceBackup();

  /** Create the real drawn image */
  void CreateSliceBackup();

  albaGUIFloatSlider *m_BrushSizeSlider;   //<Brush size slider - GUI
  wxRadioBox *m_ManualBrushShapeRadioBox;       //<Brush shape radio - GUI
	wxRadioBox *m_InitModalityRadioBox;					//<Threshold Type radio - GUI
  wxStaticBoxSizer *m_BrushEditingSizer;
  albaGUIButton *m_ManualApplyChanges;           //<Apply changes button - GUI
  
	int m_ManualSegmentationTools;                //<Manual segmentation tools (bucket/brush)
	int m_OldManualSegmentationTools;
	int m_CursorId;
	int m_ManualBucketActions;                    //<
  int m_BrushShape;                       //<Brush shape
  int m_BrushSize;                     //<Brush size
	int m_BrushFillErase;               //< Brush Modality (draw/erase)
	int m_AutofillOnRelease;
	int m_FillThesholdPerc;                     //<Brush size
	
	
  int m_ManualRefinementRegionsSize;            //<Refinement region size
  wxComboBox *m_ManualRefinementComboBox;       //<Refinement action combo - GUI
  wxTextCtrl *m_ManualRefinementRegionSizeText; //<Refinement size text - GUI
  std::vector<UndoRedoState> m_UndoList;  //< Undo stack
  std::vector<UndoRedoState> m_RedoList;  // Redo stack
  albaInteractorPERBrushFeedback *m_EditPER;   //<Dynamic event router
  vtkUnsignedCharArray *m_SliceBackup;       //<Real drawn image used in brush preview
  int m_LastMouseMovePointID;                   //<Last point id in mouse move event
  wxStaticBoxSizer *m_FillEditingSizer;       //<
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Automatic segmentation stuff
 
  /***/
  void SetRangeListSelection(int index);

  /** Function called when the user use the fine button to change the threshold*/
  void OnThresholdUpate(int eventID=-1);
	 
  /** Update the value of threshold label for the current slice */
  void UpdateThresholdLabel();

  /** Update the value of slice label for the current slice */
  void UpdateSliceLabel();

  /** Gui update when the user change threshold type */
  void OnChangeThresholdType();

	void EnableDisableGuiRange();

	void OnSelectSlicePlane();

	void OnPickingEvent(albaEvent * e);

  /** Add a new range using gui values */
  void OnSplitRange();

  /** Remove the select range */
  void OnRemoveRange();
	
	void OnFillEdit();

  /** Update threshold real-time preview*/
  void UpdateThresholdRealTimePreview();

  int m_InitModality;   //Init Modality
  double m_Threshold[2]; //Threshold
  double m_AutomaticMouseThreshold; //<Mouse threshold

	int m_CurrentRange;
	bool m_IgnoreRangeSelEvent;
  wxListBox *m_RangesGuiList;//<List of threshold ranges
  std::vector<AutomaticInfoRange> m_RangesVector; //<Automatic range structure

  albaGUILutSlider *m_ThresholdSlider;//<Threshold slider

  vtkTextMapper *m_AutomaticThresholdTextMapper;//<Text mapper for threshold visualization
  vtkActor2D *m_AutomaticThresholdTextActor;    //<Text actor for threshold visualization
  vtkTextMapper *m_AutomaticScalarTextMapper;   //<Text mapper for scalar visualization 
  vtkActor2D *m_AutomaticScalarTextActor;       //<Text actor for scalar visualization
  vtkTextMapper *m_AutomaticSliceTextMapper;    //<Text mapper for slice visualization
  vtkActor2D *m_AutomaticSliceTextActor;        //<Text mapper for slice visualization

  albaInteractorPERScalarInformation *m_InitPER; //<Interactor for scalar value visualization on mouse move
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Segmentation Refinement stuff
  /** Reset refinement undo list */
  void ResetRefinementUndoList();

	/** Reset refinement redo list */
  void ResetRefinementRedoList();

  /** Apply refinement algorithm implemented with ITK (not used) */
  bool ApplyRefinementFilter(vtkImageData *inputImage, vtkImageData *outputImage);

  /** Apply refinement algorithm implemented with vtk only */
  bool ApplyRefinementFilter2(vtkImageData *inputImage, vtkImageData *outputImage);

  /** Update slice visualization on manual step */
  void OnUpdateSlice();

  int m_RefinementSegmentationAction;       //<Refinement action fill holes or remove islands
  int m_RefinementRegionsSize;              //<Size for region recognition
  int m_RefinementMajorityThreshold;        //<Used in itk algorithm (not yet exposed and used)
  int m_RefinementEverySlice;               //<Determine if refinement performed on every slice
  int m_RefinementIterative;                //<Iterative refinement (not yet used)

  std::vector<vtkUnsignedCharArray *> m_RefinementUndoList; //<Refinement undo list
  std::vector<vtkUnsignedCharArray *> m_RefinementRedoList; //<Refinement redo list

  int m_MajorityThreshold;                   //<Used in itk algorithm (not yet exposed and used)

  int m_OldAutomaticThreshold;      //<Used to update real time threshold preview only if needed
  int m_OldAutomaticUpperThreshold; //<Used to update real time threshold preview only if needed

  int m_RemovePeninsulaRegions; //<Determine if refinement filter removes penisula regions or not

	int m_SwitchTO;        //<Plane to switch to
 
  wxGauge *m_ProgressBar; //< display progress

	albaOpSegmentationHelper m_Helper;
	
  void EnableSizerContent(wxSizer* sizer, bool enable);

  void Fill(albaEvent *e);

	void SwitchPlane(albaEvent * e);

	void InitRanges();

	int PressKey(int keyCode, bool ctrl, bool alt, bool shift);

	void InitMouseCursors();

	int ReleaseKey(int keyCode, bool ctrl, bool alt, bool shift);

	std::vector<wxImage> m_CursorImageVect;

	enum MOUSE_CURSOR
	{
		CUR_DEFAULT = -1, // Arrow
		CUR_PENCIL,
		CUR_ERASE,
		CUR_FILL,
		CUR_FILL_ERASE,
		CUR_COLOR_PICK,
		CUR_COLOR_PICK_MAX,
		CUR_COLOR_PICK_MIN,
		CUR_PENCIL_SIZE,
		CUR_PENCIL_ERASE_SIZE,
		CUR_SLICE_UP,
		CUR_SLICE_DOWN,
	};

	void SetCursor(int cursorId);

private:

	eventfilterFunc m_OldEventFunc;
	void SelectRangeByCurrentSlice();
	void Conntectivity3D(albaEvent * e);

	void ForceSlicing();

	void AddFullVolumeUndoStep();

	UndoRedoState CreateVolumeUndoRedoState();

};
#endif

