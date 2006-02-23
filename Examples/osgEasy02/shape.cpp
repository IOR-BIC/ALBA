#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Drawable>
#include <osg/StateSet>
#include <osg/MatrixTransform>


/*
        albero generato
        ===============
               Gruppo 
                 | 
           MatrixTransform
              /      \
           Geode    Matrice
           /   \   
      Drawable StateSet 
         /   
       Shape 

*/


osg::Group *createShape()
{
  osg::Group *group = new osg::Group;

  //---------------------------------
  // Sfera
  //---------------------------------

  // shape: geometria di una primitiva
  double radius = 0.7;
  osg::ref_ptr<osg::Sphere> sphereShape = new osg::Sphere(osg::Vec3d(0,0,0),radius);
  
  // Drawable: oggetto disegnabile -- riceve la geometria 
  osg::ref_ptr<osg::ShapeDrawable> sphereDrawable = new osg::ShapeDrawable(sphereShape.get());

  // StateSet: insieme dei Flag che verranno passati alle OpenGL 
  // al momento di disegnare i Drawable Associati
  osg::ref_ptr<osg::StateSet> sphereStateSet = new osg::StateSet();
  sphereStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

  // Geode -- Drawable+StateSet
  osg::ref_ptr<osg::Geode> sphereGeode    = new osg::Geode();
  sphereGeode->setStateSet(sphereStateSet.get());
  sphereGeode->addDrawable(sphereDrawable.get());

  // MatrixTransform -- modo veloce di spostare un oggetto
  osg::ref_ptr<osg::MatrixTransform> sphere = new osg::MatrixTransform();
  sphere->addChild(sphereGeode.get());
  sphere->setMatrix( osg::Matrix::translate(0,0,0));
  
  group->addChild(sphere.get());

  //---------------------------------
  // Box
  //---------------------------------

  double size = 1;
  osg::ref_ptr<osg::Box> boxShape = new osg::Box(osg::Vec3d(0,0,0),size);

  osg::ref_ptr<osg::ShapeDrawable> boxDrawable = new osg::ShapeDrawable(boxShape.get());

  osg::ref_ptr<osg::StateSet> boxStateSet = new osg::StateSet();
  boxStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

  osg::ref_ptr<osg::Geode> boxGeode    = new osg::Geode();
  boxGeode->setStateSet(boxStateSet.get());
  boxGeode->addDrawable(boxDrawable.get());

  osg::ref_ptr<osg::MatrixTransform> box = new osg::MatrixTransform();
  box->addChild(boxGeode.get());
  box->setMatrix( osg::Matrix::translate(2,0,0));

  group->addChild(box.get());

  //---------------------------------
  // Cone
  //---------------------------------
  
  osg::ref_ptr<osg::Cone> coneShape = new osg::Cone(osg::Vec3d(0,0,0),size,size);

  osg::ref_ptr<osg::ShapeDrawable> coneDrawable = new osg::ShapeDrawable(coneShape.get());

  osg::ref_ptr<osg::StateSet> coneStateSet = new osg::StateSet();
  coneStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

  osg::ref_ptr<osg::Geode> coneGeode    = new osg::Geode();
  coneGeode->setStateSet(coneStateSet.get());
  coneGeode->addDrawable(coneDrawable.get());

  osg::ref_ptr<osg::MatrixTransform> cone = new osg::MatrixTransform();
  cone->addChild(coneGeode.get());
  cone->setMatrix( osg::Matrix::translate(4,0,0));

  group->addChild(cone.get());

  //---------------------------------
  // Capsula
  //---------------------------------

  osg::ref_ptr<osg::Capsule> capsuleShape = new osg::Capsule(osg::Vec3d(0,0,0),size/2.0,size);

  osg::ref_ptr<osg::ShapeDrawable> capsuleDrawable = new osg::ShapeDrawable(capsuleShape.get());

  osg::ref_ptr<osg::StateSet> capsuleStateSet  = new osg::StateSet();
  capsuleStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

  osg::ref_ptr<osg::Geode>         capsuleGeode    = new osg::Geode();
  capsuleGeode->setStateSet(capsuleStateSet.get());
  capsuleGeode->addDrawable(capsuleDrawable.get());

  osg::ref_ptr<osg::MatrixTransform> capsule = new osg::MatrixTransform();
  capsule->addChild(capsuleGeode.get());
  capsule->setMatrix( osg::Matrix::translate(6,0,0));

  group->addChild(capsule.get());
  
  return group;
}
