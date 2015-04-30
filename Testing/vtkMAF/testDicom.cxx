#include "mafConfigure.h"
#include "vtkDicomUnPacker.h"
#include "vtkRenderer.h"
#include "vtkTexture.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkImageData.h"

int main ( int argc, char* argv[] ) 
 {
    
	
  mafString filename=MAF_DATA_ROOT;  
	vtkDicomUnPacker *dicom_reader = vtkDicomUnPacker::New();
		dicom_reader->SetDictionaryFileName("dicom3.dic");

		dicom_reader->SetFileName();
		dicom_reader->Update();

	float *dicom_range = dicom_reader->GetOutput()->GetScalarRange();
	    		
	vtkWindowLevelLookupTable *dicom_lt = vtkWindowLevelLookupTable::New(); 
	    dicom_lt->SetLevel((dicom_range[1] + dicom_range[0]) / 2.0);
		dicom_lt->SetTableRange(dicom_range);
		dicom_lt->Build();

	vtkTexture *dicom_texture = vtkTexture::New();
	   dicom_texture->SetLookupTable(dicom_lt);
		dicom_texture->SetInput(dicom_reader->GetOutput());
		dicom_texture->InterpolateOn();
    
	vtkPlaneSource *dicom_plane = vtkPlaneSource::New();

	vtkPolyDataMapper *dicom_mapper=vtkPolyDataMapper::New();
	    
		dicom_mapper->SetInput(dicom_plane->GetOutput());
		

	vtkActor *dicom_actor = vtkActor::New();
		dicom_actor->SetMapper(dicom_mapper);
		dicom_actor->SetTexture(dicom_texture);

	vtkRenderer *ren_dicom = vtkRenderer::New();
		
	    	 
	vtkRenderWindow *renWin = vtkRenderWindow::New();
		renWin->AddRenderer(ren_dicom);
		renWin->SetSize(300, 300);
				
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
		iren->SetRenderWindow(renWin);
		iren->Initialize();
	
		ren_dicom->AddActor(dicom_actor);
		renWin->Render();
		iren->Start();

		return 0;

 }


