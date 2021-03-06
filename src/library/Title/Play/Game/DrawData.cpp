#include <vector>
#include <assert.h>
#include <fstream>
#include <string.h>
#include <GL/gl.h>
#include <GL/glut.h>
//#include <GL/glut.h>
#include "Game3d.h"
#include "DrawData.h"

const Vertex VertexBuffer::get_vertex(int idx) const{
  assert(idx >= 0 && idx <= vertexs_.size() && "vertexs_[idx] is not allowed!!\n");
  return vertexs_[idx];
};

void VertexBuffer::insert_vertex(int idx, Vertex vertex) {
 assert(idx >= 0 && idx <=  vertexs_.size() && "vertexs_[idx] is not allowed!!\n");
 std::vector<Vertex>::iterator it;
 it = vertexs_.begin() + idx;
 vertexs_.insert(it, vertex);
};

const char* VertexBuffer::get_vertex_name() {
  return vertex_name_;
};

void VertexBuffer::set_vertex_name(char *vertex_name) {
  vertex_name_ = vertex_name;
};

const unsigned IndexBuffer::get_idx(int idx) const {
  assert(idx >= 0 && idx <= idxs_.size() && "idxs_[idx] is not allowed!!\n");
  return idxs_[idx];
};

void IndexBuffer::insert_idx(int idx, unsigned index) {
 assert(idx >= 0 && idx <= idxs_.size() && "idxs_[idx] is not allowed!!\n");
 std::vector<unsigned>::iterator it;
 it = idxs_.begin() + idx;
 idxs_.insert(it, index);
 
};

const char* IndexBuffer::get_index_name() {
  return index_name_;
};

void IndexBuffer::set_index_name(char *index_name) {
  index_name_ = index_name;
};

Batch::Batch( const VertexBuffer* vertexbuffer,
	      const IndexBuffer* indexbuffer,
	      const VertexBuffer* normalbuffer,
	      const GLenum mode,
	      const char *batch_name): mode_(mode) {

  vertexbuffer_ = vertexbuffer;
  indexbuffer_ = indexbuffer;
  normalbuffer_ = normalbuffer;
  batch_name_ = batch_name;

};

unsigned GraphicDatabase::get_num_batches() {

  return batches_.size();
};

void GraphicDatabase::insert_batch(int idx, Batch *batch) {
 assert(idx >= 0 && idx <= batches_.size() && "batches_[idx] is not allowed!!\n");
 std::vector<Batch*>::iterator it;
 it = batches_.begin() + idx;
 batches_.insert(it, batch);
};

Batch * GraphicDatabase::get_batch(char *name) {
 std::vector<Batch*>::iterator it;
 for (it = batches_.begin(); it != batches_.end(); ++it) {
   
   if ( !strcmp((*it)->get_batch_name(), name) ) {
     return *it;
   }
   else {
     fprintf(stderr, "no such a batch!!\n");
     return 0;
   }
 }

};

Model::Model(Batch *batch) : batch_(batch), x_(0), y_(0), z_(0),
			     theta_(0), rx_(0), ry_(0), rz_(0) {

  // batch = batch_;
  
};

void Model::draw() {

  glPushMatrix();
  glTranslated(x_, y_ - 0.5, z_);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, draw_stuff::blue);
  unsigned num_vertexs_of_face;
  
  if (batch_->get_mode() == GL_QUADS)
    num_vertexs_of_face = 4;
  if (batch_->get_mode() == GL_TRIANGLES)
    num_vertexs_of_face = 3;

  unsigned num_of_faces = 
    batch_->get_index_buffer()->get_idxs_size() / num_vertexs_of_face;

  glBegin(batch_->get_mode());
  for (int j = 0; j < num_of_faces; ++j) {
    double nx = batch_->get_normal_buffer()->get_vertex(j).x_;
    double ny = batch_->get_normal_buffer()->get_vertex(j).y_;
    double nz = batch_->get_normal_buffer()->get_vertex(j).z_;
    glNormal3d(nx, ny, nz);
    for (int i = 0; i < num_vertexs_of_face; ++i) {
      unsigned idx = 
	batch_->get_index_buffer()->get_idx(j * num_vertexs_of_face + i);
      double vx = batch_->get_vertex_buffer()->get_vertex(idx).x_;
      double vy = batch_->get_vertex_buffer()->get_vertex(idx).y_;
      double vz = batch_->get_vertex_buffer()->get_vertex(idx).z_;      
      glVertex3d(vx, vy, vz);
    }
  }
  glEnd();
  glPopMatrix();

};


void Node::draw(int time) {
  
  int idx = time % 4;

  Vertex trans = 
    animation_[state_]->translates_[idx];
  Vertex rotat = 
    animation_[state_]->rotation_[idx];
  Vertex scal = 
    animation_[state_]->scale_[idx];
  double theta =
    animation_[state_]->theta_[idx];

  glPushMatrix();
 
  glTranslated(animation_[state_]->tra_base_.x_, 
	       animation_[state_]->tra_base_.y_, 
	       animation_[state_]->tra_base_.z_);
  glRotatef(animation_[state_]->theta_base_ , 
	    animation_[state_]->rot_base_.x_, 
	    animation_[state_]->rot_base_.y_, 
	    animation_[state_]->rot_base_.z_);
  glScalef(animation_[state_]->sca_base_.x_, 
	   animation_[state_]->sca_base_.y_, 
	   animation_[state_]->sca_base_.z_); 

  glTranslated(trans.x_, trans.y_, trans.z_);
  glRotatef(theta , rotat.x_, rotat.y_, rotat.z_);
  glScalef(scal.x_, scal.y_, scal.z_); 
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, draw_stuff::red);
  glutSolidCube(0.1);

 
  for (int i = 0; i < num_child_nodes_; i++) {
    child_node_[i]->draw(time);
  };
 
  glPopMatrix();

};

void Node::set_state(State state) {

  state_ = state;
  for (int i = 0; i <  num_child_nodes_; i++) {
    child_node_[i]->state_ = state; 
  }
  

};

void Animation::set_animation(Vertex *translate, 
			      Vertex *rotation, Vertex *scale,
			      double *theta) {

  for (int i = 0; i < 4; i++) {
    translates_[i] = translate[i];
    rotation_[i] = rotation[i];
    scale_[i] = scale[i];
    theta_[i] = theta[i];
  };

};
