// Test mafVMEGeneric class

#include "mafVME.h"
#include "mafDataPipeCustom.h"
#include "mafVMEOutputSurface.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkConeSource.h"
#include <iostream>

//-------------------------------------------------------------------------
class mafTestVME: public mafVME
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafTestVME,mafVME);
  mafTestVME();
  ~mafTestVME();
  void SetMatrix(const mafMatrix &mat) {m_Transform->SetMatrix(mat);}
  void SetTypeToCone(){m_Type=0;Modified();}
  void SetTypeToSphere(){m_Type=1;Modified();}
  void SetRadius(double r) {m_Radius=r;Modified();}
  void GetLocalTimeStamps(std::vector<double> &vect) {vect.clear();}
  virtual void InternalPreUpdate();
  virtual void InternalUpdate();
  mafTransform *m_Transform;
  vtkSphereSource *m_Sphere;
  vtkConeSource *m_Cone;
  double m_Radius;
  int m_Type;
  int m_PreUpdateConunter;
  int m_UpdateConunter;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafTestVME)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafTestVME::mafTestVME():m_PreUpdateConunter(0),m_UpdateConunter(0),m_Type(0),m_Radius(1)
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  vtkNEW(m_Sphere);
  vtkNEW(m_Cone);
  SetDataPipe(mafDataPipeCustom::New());
  SetOutput(mafVMEOutputSurface::New());
  GetOutput()->SetTransform(m_Transform);
}
//-------------------------------------------------------------------------
mafTestVME::~mafTestVME()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  vtkDEL(m_Sphere);
  vtkDEL(m_Cone);
  // pipe and output deleted by VME
}

//-------------------------------------------------------------------------
void mafTestVME::InternalPreUpdate()
//-------------------------------------------------------------------------
{
  m_PreUpdateConunter++;
  mafDataPipeCustom *dpipe=(mafDataPipeCustom *)GetDataPipe();
  switch (m_Type)
  {
    case 0: 
    {
      dpipe->GetVTKDataPipe()->SetInput(m_Cone->GetOutput());
    }
    break;
    case 1: 
    {
      dpipe->GetVTKDataPipe()->SetInput(m_Sphere->GetOutput());
    }
    break;
  }
  
}

//-------------------------------------------------------------------------
void mafTestVME::InternalUpdate()
//-------------------------------------------------------------------------
{
  m_UpdateConunter++; // increment the counter
  // set vtk filters radius before of their execution
  m_Sphere->SetRadius(m_Radius);
  m_Cone->SetRadius(m_Radius);
}
//-------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------
{
  // create a small tree with a root, a volume and a slicer 
  mafSmartPointer<mafTestVME> vme;
  vme->SetName("test vme");

  vtkDataSet *data=vme->GetOutput()->GetVTKData();

  MAF_TEST(data->IsA("vtkPolyData"));

  vtkPolyData *surface=(vtkPolyData *)data;

  MAF_TEST(vme->m_PreUpdateConunter==1);
  MAF_TEST(vme->m_UpdateConunter==0);


  surface->Update();
  MAF_TEST(vme->m_PreUpdateConunter==1);
  MAF_TEST(vme->m_UpdateConunter==1);
  MAF_TEST(vme->m_Sphere->GetRadius()==1);
  MAF_TEST(vme->m_Cone->GetRadius()==1);

  vme->SetRadius(2);
  surface->Update();
  MAF_TEST(vme->m_PreUpdateConunter==2);
  MAF_TEST(vme->m_UpdateConunter==2);

  vme->SetTypeToSphere();
  MAF_TEST(vme->GetOutput()->GetVTKData()==surface);
  MAF_TEST(vme->m_PreUpdateConunter==3);
  MAF_TEST(vme->m_UpdateConunter==2);
  
  surface->Update();
  MAF_TEST(vme->m_PreUpdateConunter==3);
  MAF_TEST(vme->m_UpdateConunter==3);
  
  surface->Update();
  MAF_TEST(vme->m_PreUpdateConunter==3);
  MAF_TEST(vme->m_UpdateConunter==3);
  
  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
