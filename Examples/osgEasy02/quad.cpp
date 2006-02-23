#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Drawable>
#include <osg/StateSet>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/Image>
#include <osg/PrimitiveSet>
#include <osgDB/ReadFile>



/*
      albero generato
      ===============
          Gruppo 
             | 
           Geode    
           /   \   
     Drawable  StateSet 
      |             |   
      |- vertex     |- texture
      |- normals
      |- texcoord
      |- cells   
       
*/


osg::Group *createQuad()
{

  osg::ref_ptr<osg::Vec3Array>        vert     = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec3Array>        norm     = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec2Array>        texc     = new osg::Vec2Array();
  osg::ref_ptr<osg::DrawElementsUInt> cells    = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS); 
  osg::ref_ptr<osg::Vec4Array>        colors   = new osg::Vec4Array();
  osg::ref_ptr<osg::Geometry>         geom     = new osg::Geometry();
  osg::ref_ptr<osg::Texture2D>        tex      = new osg::Texture2D();
  osg::ref_ptr<osg::StateSet>         stateset = new osg::StateSet();

  vert->push_back( osg::Vec3d( 0,0,0) );
  vert->push_back( osg::Vec3d( 1,0,0) );
  vert->push_back( osg::Vec3d( 1,0,1) );
  vert->push_back( osg::Vec3d( 0,0,1) );
  
  norm->push_back( osg::Vec3d( 0,1,0) );
  
  cells->push_back(0);
  cells->push_back(1);
  cells->push_back(2);
  cells->push_back(3);
  
  colors->push_back( osg::Vec4d(1,1,1,1) );

  texc->push_back( osg::Vec2d(0,0) );
  texc->push_back( osg::Vec2d(0,1) );
  texc->push_back( osg::Vec2d(1,1) );
  texc->push_back( osg::Vec2d(1,0) );
    
  geom->addPrimitiveSet( cells.get() ); 
  geom->setVertexArray( vert.get() ); 
  geom->setNormalArray( norm.get()); 
  geom->setNormalBinding( osg::Geometry::AttributeBinding::BIND_OVERALL );
  geom->setTexCoordArray(0,texc.get());
  geom->setColorArray(colors.get());
  geom->setColorBinding(osg::Geometry::BIND_OVERALL);

  // todo: non usare path hard-coded
  osg::Image *img = osgDB::readImageFile("D:\\__OSG_DATA\\Images\\lena256.jpg");
  tex->setImage(img);

  stateset->setTextureAttributeAndModes(0,tex.get(),osg::StateAttribute::ON);
  stateset->setMode(GL_LIGHTING,   osg::StateAttribute::ON);
  stateset->setMode(GL_CULL_FACE,  osg::StateAttribute::OFF);
  stateset->setMode(GL_BLEND,      osg::StateAttribute::ON);
  stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

  osg::ref_ptr<osg::Geode> geode = new osg::Geode();
  geode->addDrawable(geom.get()); 
  geode->setStateSet(stateset.get());

  osg::Group *group = new osg::Group;
  group->addChild(geode.get());
  return group;
}
