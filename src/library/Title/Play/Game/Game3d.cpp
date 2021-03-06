#include <fstream>
#include <iostream>
#include <cstdlib>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glui.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <assert.h>
#include <time.h>
#include <math.h>
#include "DrawData.h"
//#include "bmp.h"
#include "Game3d.h"
#include "Play.h"
#include "Pause.h"
#include "Clear.h"
#include "Failure.h"

using namespace std;

pthread_mutex_t mutex_for_states_ = PTHREAD_MUTEX_INITIALIZER;

//update function for Play class
Base *Game3d::update() {

  // Base *temp_base = this;
  // if (seq_trig_ == STARTKEY)
  //   temp_base = new Pause;4
  // else if (seq_trig_ == CLEAR) {
  //   temp_base = new Clear;
  //   Game3d::destroy();
  // }
  // else if(seq_trig_ == FAILURE) {
  //   temp_base = new Failure;
  //   int is_player1;
  //   int is_player2;
  //   get_player_position(PLAYER1, is_player1, is_player1);
  //   get_player_position(PLAYER2, is_player2, is_player2);    
  //   if (is_player1 == 0) 
  //     temp_base->is_winner_ = 2;  
  //   else
  //     temp_base->is_winner_ = 1;
  //   Game3d::destroy();
  // }
  // else if (seq_trig_ == EXIT)
  //   exit(0);
  // else temp_base = this;

  // seq_trig_ = NONE;
  // return temp_base;
}

//keybord handler
void Game3d::keybord_handler(unsigned char key) {

  switch (key) {
  case 'w': { 
    // fprintf(stderr, "player1_.x_:%f, player1_.y_:%f, player1_.z_:%f\n",
    // 	    player1_.x_, player1_.y_, player1_.z_);
    //    player1_.z_--;

    if (collid_test(player1_.x_, player1_.y_, player1_.z_, 
		    player1_.x_ + cos(player1_.theta_),
		    player1_.y_,
		    player1_.z_ + sin(player1_.theta_), PLAYER) == NONE) {
    player1_.z_ += sin(player1_.theta_);
    player1_.x_ += cos(player1_.theta_);
    }
    //    else fprintf(stderr, "collid :: player1_.x_:%f, player1_.y_:%f, player1_.z_:%f\n", player1_.x_, player1_.y_, player1_.z_);
    drawtree_.root_->set_state(Node::WALK);
    player1_.time_for_animation_ = time(NULL);
  }
      break;
  case 'd':
    //  player1_.x_++;
    player1_.theta_  += 0.05;
    // fprintf(stderr, "theta:%f\n", player1_.theta_);
    break;
  case 'x':
    // player1_.z_++;

    if (collid_test(player1_.x_, player1_.y_, player1_.z_, 
		    player1_.x_ - cos(player1_.theta_),
		    player1_.y_,
		    player1_.z_ - sin(player1_.theta_), PLAYER) == NONE) {

    player1_.z_ -= sin(player1_.theta_);
    player1_.x_ -= cos(player1_.theta_);
    }
    break;
  case 'a':
    //player1_.x_--;
    player1_.theta_   -= 0.05;
    //    fprintf(stderr, "theta:%f\n", player1_.theta_);
    break;
  case 's':
    player1_.v0_ = 1;

    break;
  case 'z':
    if (player1_.phi_ < 90 )
      player1_.phi_   += 0.1;
    break;
  case 'c':
    if (player1_.phi_ > 0 )
      player1_.phi_   -= 0.1;
    break;
  case 'f': {
    //    fprintf(stderr, "Fire!!\n");
    Target *target;
    target = get_target();
    
    Missile *missile = new Missile;
    missile->target_ = target;    
    missile->whose_ = &player1_;
    Vertex player_v(player1_.x_, player1_.y_, player1_.z_);
    missile->v_ = player_v;
    
    missile->time_ = 0;
    missile->speed_ = 3;
    missile->theta_ = player1_.theta_;
    missile->phi_ = player1_.phi_;
    player1_.missiles_.push_back(missile);
  }
    break;
  default:
    break;
  }
      
    // fprintf(stderr, "player1_.theta_:%f\n",
    // 	 player1_.theta_);
  player1_.posture_theta_ = -57.295 * player1_.theta_;
  // if (player1_.posture_theta_ - player1_.theta_ > 72.8)
  //   player1_.posture_theta_ = player1_.theta_ + 72.8;
  // if (player1_.posture_theta_ - player1_.theta_ < -72.8)
  //   player1_.posture_theta_ = player1_.theta_ - 72.8;


}



void Game3d::set_lookat(LookAtCordi *lookat, const Player &player) {
  static double fi = 0;
  // fi += 1;
   // fi /= 57.295;

  lookat->posi_x_ = player.x_ - 1.5 * cos(player.theta_ + fi/57.295);
  lookat->posi_y_ = player.y_ + 0.2;
  lookat->posi_z_ = player.z_ - 1.5 * sin(player.theta_ + fi/57.295);
  lookat->lookat_x_ =  cos(player.theta_ + fi/57.295) + player.x_;
  lookat->lookat_y_ = tan(player1_.phi_) + player.y_;
  lookat->lookat_z_ =  sin(player.theta_ + fi/57.295) + player.z_;
  // fprintf(stderr, "  lookat->lookat_z_:%f\n",   lookat->lookat_z_);
}


void Game3d::gravity() {
  double g = 0.25; //gravitational acceleration 
 
    std::vector<Obstacle>::iterator it;
    for (it = obstacles_.begin(); it != obstacles_.end(); ++it) {
      it->time_ += 1;
      if (collid_test(it->x_, it->y_, it->z_, it->x_, it->y_ - 0.5 * g * it->time_ * it->time_, it->z_, OBSTACLE, distance(obstacles_.begin(), it)) == NONE) {
    	 it->y_ -= 0.5 * g * it->time_ * it->time_; 
	 // fprintf(stderr, "it->y_:%d\n", it->y_);
       }
      else if (collid_test(it->x_, it->y_, it->z_, it->x_, it->y_ - 0.5 * g * it->time_ * it->time_, it->z_, OBSTACLE, distance(obstacles_.begin(), it)) == FLOOR) {
	//	 fprintf(stderr, "floor collid!\n");
      }
       else {
	 //	 fprintf(stderr, "obstacle collid!\n");
    	 it->time_ = 0;
       }
    }

    player1_.time_ += 1;
    // fprintf(stderr, "player1_.y_:%f, d_player1_y:%f, player1_.time_:%f\n", player1_.y_, player1_.y_ -  player1_.v0_ * player1_.time_ + 0.5 * g * player1_.time_ * player1_.time_, player1_.time_);

    if (collid_test(player1_.x_, player1_.y_, player1_.z_, 
		     player1_.x_, player1_.y_before_jump_ + player1_.v0_ * player1_.time_- 
		     0.5 * g * player1_.time_ * player1_.time_, player1_.z_, PLAYER) == NONE) {

      player1_.y_ = player1_.y_before_jump_  + player1_.v0_ * player1_.time_ - 0.5 * g * player1_.time_ * player1_.time_;
    }
    else {
      // sleep(1);
      //   fprintf(stderr, "gra collid\n");
      player1_.time_ = 0;
      player1_.v0_ = 0;
      player1_.y_before_jump_ = player1_.y_;
    }
    player1_.y_ = static_cast<int>(player1_.y_);



    std::vector<Player*>::iterator eit;
    for (eit = enemies_.begin(); eit != enemies_.end(); ++eit) {
      (*eit)->time_ += 1;
      if (collid_test((*eit)->x_, (*eit)->y_, (*eit)->z_, 
		      (*eit)->x_, (*eit)->y_before_jump_ + (*eit)->v0_ * (*eit)->time_- 
		      0.5 * g * (*eit)->time_ * (*eit)->time_, (*eit)->z_, PLAYER) == NONE) {
	
	(*eit)->y_ = (*eit)->y_before_jump_  + (*eit)->v0_ * (*eit)->time_ - 0.5 * g * (*eit)->time_ * (*eit)->time_;
      }
      else {
	// sleep(1);
	//   fprintf(stderr, "gra collid\n");
	(*eit)->time_ = 0;
	(*eit)->v0_ = 0;
	(*eit)->y_before_jump_ = (*eit)->y_;
      }
      (*eit)->y_ = static_cast<int>((*eit)->y_);
    }

}


int Game3d::collid_test(double now_x, double now_y,  double now_z,
			 double next_x, double next_y, double next_z, 
			ObType objest, int idx) {
  double r = 0;
  unsigned num_vertexs_of_face = 4; // now both of obstacle and player are QUADS shape

  switch (objest) {
  case (PLAYER):
    r = 0.174;
    break;
  case (OBSTACLE):
    r = 0.87;
    break;
  };
  double offset_y = 0.1;
  Vertex s(now_x, now_y + offset_y, now_z );
  Vertex d(next_x - now_x, next_y - now_y, next_z - now_z);

  Batch *batch = graphic_database_->get_batch("obstacle");
  const VertexBuffer *vertex_buffer = batch->get_vertex_buffer();
  const IndexBuffer *index_buffer = batch->get_index_buffer();
  unsigned num_of_faces = 
    batch->get_index_buffer()->get_idxs_size() / num_vertexs_of_face;

  std::vector<Obstacle>::iterator it;
  for (it = obstacles_.begin(); it != obstacles_.end(); ++it) {
    if (idx == distance(obstacles_.begin(), it))
      continue;
    for (int i = 0; i < num_of_faces; i++) {
      unsigned idx_o = 
	batch->get_index_buffer()->get_idx(i * num_vertexs_of_face + 0);
      unsigned idx_a = 
	batch->get_index_buffer()->get_idx(i * num_vertexs_of_face + 1);
      unsigned idx_b = 
	batch->get_index_buffer()->get_idx(i * num_vertexs_of_face + 3);
      // fprintf(stderr, "idx_o:%d, idx_a:%d, idx_b:%d\n", 
      //idx_o, idx_a, idx_b);
      Vertex vn(batch->get_normal_buffer()->get_vertex(i).x_,
		batch->get_normal_buffer()->get_vertex(i).y_,
		batch->get_normal_buffer()->get_vertex(i).z_);
      Vertex o(batch->get_vertex_buffer()->get_vertex(idx_o).x_ + it->x_,
	       batch->get_vertex_buffer()->get_vertex(idx_o).y_ + it->y_,
	       batch->get_vertex_buffer()->get_vertex(idx_o).z_ + it->z_);
      //   fprintf(stderr, "o.x_:%f, o.y_:%f, o.z_:%f\n", o.x_, o.y_, o.z_);
      Vertex a(batch->get_vertex_buffer()->get_vertex(idx_a).x_ + it->x_,
	       batch->get_vertex_buffer()->get_vertex(idx_a).y_ + it->y_,
	       batch->get_vertex_buffer()->get_vertex(idx_a).z_ + it->z_);
      Vertex b(batch->get_vertex_buffer()->get_vertex(idx_b).x_ + it->x_,
	       batch->get_vertex_buffer()->get_vertex(idx_b).y_ + it->y_,
	       batch->get_vertex_buffer()->get_vertex(idx_b).z_ + it->z_);
      Vertex vf = o - s;
      
      double const_t = (vn * vf) / (vn * d);
      // t check
      if (!(const_t >= 0 && const_t <= 1)) continue;

      Vertex vg = (s + d * const_t) - o;
      Vertex ve = b - o;
      Vertex vd = a - o;
      double vgvd = vg * vd;
      double vdve = vd * ve;
      double vgve = vg * ve;
      double vdvd = vd * vd;
      double veve = ve * ve;
      double vdvd2veve2 = vdve * vdve;
      double vevevdvd = veve * vdvd;

      double const_v = (vgvd * vdve - vgve * vdvd) / (vdvd2veve2 - vevevdvd);
      // v check
      if (!(const_v >= 0 && const_v <= 1)) continue;
     
      double const_u = (vgve * vdve - vgvd * veve) / (vdvd2veve2 - vevevdvd);
      //u check
      if (!(const_u >= 0 && const_u <= 1)) continue;
      return distance(obstacles_.begin(), it);;
    }
  }
  for (int j = -100; j < 5; ++j) {
    for (int i = -50; i < 50; ++i) {
      Vertex vn(0, 1, 0);
      Vertex o((double)i, -0.5, (double)j);
      Vertex a((double)i, -0.5, (double)j + 1);
      Vertex b((double)i + 1, -0.5, (double)j);
      Vertex vf = o - s;
      double const_t = (vn * vf) / (vn * d);
      // t check
      if (!(const_t >= 0 && const_t <= 1)) continue;

      Vertex vg = (s + d * const_t) - o;
      Vertex ve = b - o;
      Vertex vd = a - o;
      double vgvd = vg * vd;
      double vdve = vd * ve;
      double vgve = vg * ve;
      double vdvd = vd * vd;
      double veve = ve * ve;
      double vdvd2veve2 = vdve * vdve;
      double vevevdvd = veve * vdvd;

      double const_v = (vgvd * vdve - vgve * vdvd) / (vdvd2veve2 - vevevdvd);
      // v check
      if (!(const_v >= 0 && const_v <= 1)) continue;
     
      double const_u = (vgve * vdve - vgvd * veve) / (vdvd2veve2 - vevevdvd);
      //u check
      if (!(const_u >= 0 && const_u <= 1)) continue;
      return FLOOR;
    }
  }    
  
  return NONE;
}

//draw function
void Game3d::draw() {
  //clear buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //set lookat
  LookAtCordi look;
  set_lookat(&look, player1_);

  // fprintf(stderr, "look.posi_x_:%f\n, look.posi_y_:%f\n, look.posi_z_:%f\n, look.lookat_x_:%f\n, look.lookat_y_:%f\n, look.lookat_z_:%f\n",look.posi_x_, look.posi_y_, look.posi_z_, look.lookat_x_, look.lookat_y_, look.lookat_z_);
  
  glPushMatrix();
  gluLookAt(look.posi_x_, look.posi_y_, look.posi_z_, 
	    look.lookat_x_, look.lookat_y_, look.lookat_z_, 0.0, 1.0, 0.0);

  //draw player

  drawtree_.set_root();

  
  drawtree_.root_->draw(drawtree_.time_[Node::WALK]);
  drawtree_.time_[Node::WALK] += 0.3;
  // glPushMatrix();
  // glTranslated(player1_.x_, player1_.y_, player1_.z_);
  // glRotatef(player1_.posture_theta_ , 0, 1, 0);
  // glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, draw_stuff::red);
  // glutSolidCube(0.1);

  // glPushMatrix();
  // glTranslated(-0.008, 0.06, 0.0022);
  // glutSolidCube(0.06);
  // glPopMatrix();


  // glPushMatrix();
  // glTranslated(-0.008, -0.1, -0.05);
  // glRotatef(30, 1, 0, 0);
  // glScalef(0.3, 1.0, 0.3); 
  // glutSolidCube(0.1);
  // glPopMatrix();

  // glPushMatrix();
  // glTranslated(-0.008, -0.1, 0.05);
  // glRotatef(-30, 1, 0, 0);
  // glScalef(0.3, 1.0, 0.3); 
  // glutSolidCube(0.1);
  // glPopMatrix();

  // glPushMatrix();
  // glTranslated(-0.008, 0, -0.08);
  // glRotatef(30, 1, 0, 0);
  // glScalef(0.3, 1.0, 0.3); 
  // glutSolidCube(0.1);
  // glPopMatrix();

  // glPushMatrix();
  // glTranslated(-0.008, 0, 0.08);
  // glRotatef(-30, 1, 0, 0);
  // glScalef(0.3, 1.0, 0.3); 
  // glutSolidCube(0.1);
  // glPopMatrix();


  // glPopMatrix();

  //draw enemies
  std::vector<Player*>::iterator itee;
  for(itee = enemies_.begin(); itee != enemies_.end(); ++itee) {
  glPushMatrix();
  glTranslated((*itee)->x_, (*itee)->y_, (*itee)->z_);
  glRotatef((*itee)->theta_ * 57.295, 0, 1, 0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, draw_stuff::green);
  glutSolidCube(0.1);

  glPushMatrix();
  glTranslated(-0.008, 0.06, 0.0022);
  glutSolidCube(0.06);
  glPopMatrix();


  glPushMatrix();
  glTranslated(-0.008, -0.1, -0.05);
  glRotatef(30, 1, 0, 0);
  glScalef(0.3, 1.0, 0.3); 
  glutSolidCube(0.1);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-0.008, -0.1, 0.05);
  glRotatef(-30, 1, 0, 0);
  glScalef(0.3, 1.0, 0.3); 
  glutSolidCube(0.1);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-0.008, 0, -0.08);
  glRotatef(30, 1, 0, 0);
  glScalef(0.3, 1.0, 0.3); 
  glutSolidCube(0.1);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-0.008, 0, 0.08);
  glRotatef(-30, 1, 0, 0);
  glScalef(0.3, 1.0, 0.3); 
  glutSolidCube(0.1);
  glPopMatrix();

  glPopMatrix();

  }


  // draw ground and obstacles
  glBegin(GL_QUADS);
  glNormal3d(0.0, 1.0, 0.0);
  for (int j = -100; j < 5; ++j) {
    for (int i = -50; i < 50; ++i) {
      glMaterialfv(GL_FRONT, GL_DIFFUSE, draw_stuff::ground[(i + j) & 1]);
      glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
      glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
      glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
      glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
    }
  }
  glEnd();

  vector<Obstacle>::iterator it;
  Batch *batch = graphic_database_->get_batch("obstacle");
  Model *model = new Model(batch);
  for (it = obstacles_.begin(); it != obstacles_.end(); ++it) {
    model->x_ = it->x_;
    model->y_ = it->y_;
    model->z_ = it->z_;
    model->draw();
  }

  //draw player missiles 
  std::list<Missile*>::iterator ite;
  for(ite = player1_.missiles_.begin(); ite != player1_.missiles_.end(); ++ite) {
  glPushMatrix();
  glTranslated((*ite)->v_.x_, (*ite)->v_.y_, (*ite)->v_.z_);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, draw_stuff::yellow);
  glutSolidSphere(0.3, 12, 12);
  glPopMatrix();

  }

  //draw enemies missiles
  std::vector<Player*>::iterator eit;
  for(eit = enemies_.begin(); eit != enemies_.end(); ++eit) {
    std::list<Missile*>::iterator emit;
    for(emit = (*eit)->missiles_.begin(); emit != (*eit)->missiles_.end(); ++emit) {      
      glPushMatrix();
      glTranslated((*emit)->v_.x_, (*emit)->v_.y_, (*emit)->v_.z_);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, draw_stuff::red);
      glutSolidSphere(0.3, 12, 12);
      glPopMatrix();
    }
  }

  //drwa 2D

  // // glMatrixMode(GL_MODELVIEW);
  // // glLoadIdentity();
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // gluOrtho2D(0, 100, 0, 100);
  // glDisable(GL_DEPTH_TEST);
  // glDisable(GL_CULL_FACE);
    
  // glColor3f(1, 1, 1);
  // glPushMatrix();
  // glBegin(GL_QUADS); 
  // glVertex3f(-50, 50, 0.0f); 
  // glVertex3f(-50, -50, 0.0f); 
  // glVertex3f(50, -50, 0.0f); 
  // glVertex3f(50, 50, 0.0f); 
  // glEnd();   
  // glPopMatrix();
  // glEnable(GL_DEPTH_TEST);
  
  glPopMatrix();
  glFlush();
}

//timer function
void Game3d::timer() {
  // jump process 
  enemies_action();
  gravity();
  if (time(NULL) - player1_.time_for_animation_ > 0.5)
    drawtree_.root_->set_state(Node::STAND);
  std::list<Missile*>::iterator it;
  for(it = player1_.missiles_.begin(); it != player1_.missiles_.end(); ++it) {
     (*it)->move();
    //    fprintf(stderr,"move\n");
  }
  

  std::vector<Player*>::iterator eit;
  for(eit = enemies_.begin(); eit != enemies_.end(); ++eit) {
    std::list<Missile*>::iterator emit;
    for (emit = (*eit)->missiles_.begin(); emit != (*eit)->missiles_.end(); ++emit) {
      (*emit)->move();
    }
  }

}

Game3d *Game3d::instance() {
  return Game3d::game3d_;
}

void Game3d::create() {
  assert(!game3d_ && "object Game3d exists already!!\n");
  game3d_ = new Game3d();

}

void Game3d::destroy() {
  delete game3d_;
  game3d_ = NULL;
}

Game3d::Game3d(){
    init_stage();
    init_batch();
    init_enemies();
    drawtree_.init_root();
    fprintf(stderr, "constracta\n");

  

  // load_stage("../data/stage/stage1.txt");
  // set_size(5);
  // init_game3d(game3d_init::player_mode, game3d_init::game3d_level);
  // seq_trig_ = NONE;
  // player1_state_ = {1, 1, 0};
  // player1__state_ = {1, 1, 0};
  // bomb_blasted_ = false;
  // flush_cnt_ = 0;
}
Game3d::~Game3d(){}

void Game3d::init_stage() {  
  
  srand(time(NULL));
  //decide places of block
  std::vector<int> block_idxs; 
  for (int i = 0; i < game3d_init::num_blocks; i++) {
    int idx = rand() % 100;
    block_idxs.push_back(idx);
  }

  //decide places of obstales in eace block (x, y)
  std::vector<int>::iterator it;
  for (it = block_idxs.begin(); it != block_idxs.end(); ++it) {
    for (int j = 0; j < game3d_init::num_obstacles_in_block; j++) {
      Obstacle obstacle;
      int idx = rand() % 100;
      int x = idx % 10;
      int z = idx / 10;
      int obstacle_id = rand() % (NUMOBSTACLEID - 1);
      obstacle.x_ = (*it % 10) * 10 + x - 50; 
      obstacle.y_ = 0;
      obstacle.z_ = ((*it / 10) * 10 + z) * -1;
      obstacle.id_ = static_cast<ObstacleID>(obstacle_id);
      obstacles_.push_back(obstacle);
    }
  }

  //decide places of obstales in eace block (z) 
  //in case there are several obstacle at given (x, y)
  std::vector<Obstacle>::iterator ite;
  for (ite = obstacles_.begin(); ite != obstacles_.end(); ++ite) {
    int index = distance(obstacles_.begin(), ite);
    for (int k = 0; k < obstacles_.size(); k++) {
      if (k != index && ite->x_ == obstacles_[k].x_ && 
	  ite->z_ == obstacles_[k].z_ &&
	ite->y_ == obstacles_[k].y_) 
	obstacles_[k].y_++;
    }

  }

}

void Game3d::init_batch() {

  GLdouble vertex[][3] = {
      { 0.0, 0.0, 0.0 },
      { 1.0, 0.0, 0.0 },
      { 1.0, 1.0, 0.0 },
      { 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 1.0 },
      { 1.0, 0.0, 1.0 },
      { 1.0, 1.0, 1.0 },
      { 0.0, 1.0, 1.0 }
    };

    VertexBuffer *vertex_buffer = new VertexBuffer;
    for (int i = 0; i < 8; i++) {
      Vertex tmp_vertex(vertex[i]);
      vertex_buffer->insert_vertex(i, tmp_vertex);
    }
    vertex_buffer->set_vertex_name("obstacle");
    
    IndexBuffer *index_buffer = new IndexBuffer;
    unsigned face[][4] = {
      { 3, 2, 1, 0 },
      { 2, 6, 5, 1 },
      { 6, 7, 4, 5 },
      { 7, 3, 0, 4 },
      { 0, 1, 5, 4 },
      { 7, 6, 2, 3 }
    };
    
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 4; j++) {

	index_buffer->insert_idx(i * 4 + j, face[i][j]);
      }
    }
    index_buffer->set_index_name("obstacle");

    GLdouble normal[][3] = {
      { 0.0, 0.0,-1.0 },
      { 1.0, 0.0, 0.0 },
      { 0.0, 0.0, 1.0 },
      {-1.0, 0.0, 0.0 },
      { 0.0,-1.0, 0.0 },
      { 0.0, 1.0, 0.0 }
    };

    VertexBuffer *normal_buffer = new VertexBuffer;
    for (int i = 0; i < 6; i++) {
      Vertex tmp_vertex(normal[i]);
      normal_buffer->insert_vertex(i, tmp_vertex);
    }
    normal_buffer->set_vertex_name("obstacle");
    
    const GLenum glenum = GL_QUADS;
    Batch *batch = new Batch(vertex_buffer, index_buffer,
			     normal_buffer, glenum, "obstacle");
    graphic_database_ = new GraphicDatabase();
    graphic_database_->insert_batch(graphic_database_->get_num_batches(), 
				    batch);




};

void Game3d::init_enemies() {
  for (int i = 0; i < game3d_init::num_enemies; i++) {
    Player* enemy = new Player;
    srand(time(NULL) + i);
    int idx = rand() % 100 - 50;
    int idz = -1 * rand() % 100;
    enemy->seed_for_action_ = i;
    enemy->x_ = idx;
    enemy->z_ = idz;
    enemies_.push_back(enemy);
  }
};

void Game3d::load_graphic_database() {

};

Target* Game3d::get_target() {
 
  return NULL;
};

void Missile::move() {
  time_ += 0.1;
  Game3d *game = Game3d::instance();
  if (target_ == NULL) {
    
    Vertex next(v_.x_ + time_ * speed_ * cos(theta_), v_.y_, v_.z_ + time_ * speed_ * sin(theta_));

    Vertex next_posi =  next;
    int idx = game->collid_test(v_.x_, v_.y_,  v_.z_,
		    next_posi.x_, next_posi.y_, next_posi.z_, 
				Game3d::OBSTACLE);
    if(idx != Game3d::NONE) {
      game->erase_obstacle(idx);
      game->get_player1().missiles_.remove(this);
    }
    else v_ = next;
    
  }

};

void Game3d::enemies_action() {
  std::vector<Player*>::iterator it;
  for (it = enemies_.begin(); it != enemies_.end(); ++it) {
    Player::Input next_input = (*it)->next_action((*it)->pre_action_);
    (*it)->action(next_input);
  }
};

Player::Input Player::next_action(Input pre_action) {

  std::map<double, Input> input_weights;
  switch(pre_action) {
  case (JUMP):
    input_weights[0.1] = FORWARD;
    input_weights[0.2] = RIGHT;
    input_weights[0.3] = LEFT;
    input_weights[0.4] = BACK;
    input_weights[0.5] = JUMP;
    input_weights[1.0] = FIRE;
    break;
  default:
    input_weights[0.2] = FORWARD;
    input_weights[0.4] = RIGHT;
    input_weights[0.55] = LEFT;
    input_weights[0.6] = BACK;
    input_weights[0.8] = JUMP;
    input_weights[1.0] = FIRE;
    break;
  }
  if(seed_for_action_ < 65000)seed_for_action_ ++;
  else seed_for_action_ = 0;

  srand(time(NULL) + seed_for_action_);
  double linear = rand()*1.0/RAND_MAX;  
  std::map<double, Input>::iterator mit;
  mit = input_weights.upper_bound(linear);
  pre_action_ = mit->second;
  return  mit->second;

};

void Player::action(Input input) {
  Game3d *game = Game3d::instance();
  switch(input) {
  case (FORWARD) :
   if (game->collid_test(x_, y_, z_, 
  		    x_ + sin(theta_),
  		    y_,
  			 z_ - cos(theta_), Game3d::PLAYER) == Game3d::NONE) {
    z_ -= cos(theta_);
    x_ += sin(theta_);
    }
   break;
  case (RIGHT) :
    theta_  += 0.01;
    break;
  case (LEFT) :
    theta_   -= 0.01;
    break;
  case (BACK) :
    if (game->collid_test(x_, y_, z_, 
  		    x_ - sin(theta_),
  		    y_,
  		    z_ + cos(theta_),Game3d::PLAYER) == Game3d::NONE) {

    z_ += cos(theta_);
    x_ -= sin(theta_);
    }
    break;
  case (JUMP) :
    face_to_player();
    v0_ = 1;
    break;
  case (FIRE) :
    {

      Target *target;
      target = game->get_target();
      
      Missile *missile = new Missile;
      missile->target_ = target;    
      missile->whose_ = this;
      Vertex player_v(x_, y_, z_);
      missile->v_ = player_v;
      
      missile->time_ = 0;
      missile->speed_ = 3;
      missile->theta_ = theta_;
      missile->phi_ = phi_;
      missiles_.push_back(missile);
    }
    break;
  default :
    break;
  }

};

void Player::face_to_player() {
  Game3d *game = Game3d::instance();
  Vertex o(x_, 0, z_);
  Vertex a(x_ + cos(theta_), 0, z_ + sin(theta_));
  Vertex b(game->get_player1().x_, 0, game->get_player1().z_);
  
  Vertex oa(cos(theta_), 0, sin(theta_));
  Vertex ob = b - o;
 
  double oaob = oa * ob;
  double loa = sqrt(oa * oa);
  double lob = sqrt(ob * ob);
  
  double costheta = oaob / (loa * lob);
  double theta = acos(costheta);
  if (theta > 0) theta_ += theta;
};

enum PART { LEFTARM = 0,
	    RIGHTARM,
	    LEFTLEG,
	    RIGHTLEG,
	    HEAD,
	    NUMOFPARTS
};

Animation *get_stand_animation(PART part) {

  Vertex translate[4];
  Vertex rotation[4];
  Vertex scale[4];
  double theta[4];

  Animation *animation = new Animation();
  Vertex trab, rotb, scab;
  double thetab;
  switch (part) {
  case LEFTLEG: {
    Vertex tmptra(-0.008, -0.1, -0.05);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.15, 1.2, 0.3);
    scab = tmpsca;
    thetab = 30;
  }
    break;
  case RIGHTLEG: {
    Vertex tmptra(-0.008, -0.1, 0.05);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.15, 1.2, 0.3);
    scab = tmpsca;
    thetab = -30;
  }
    break;
  case LEFTARM: {
    Vertex tmptra(-0.008, 0, 0.08);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.15, 1.2, 0.3);
    scab = tmpsca;
    thetab = -30;
  }
    break;
  case RIGHTARM: {
    Vertex tmptra(-0.008, 0, -0.08);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.15, 1.2, 0.3);
    scab = tmpsca;
    thetab = 30;
  }
    break;
  case HEAD: {
    Vertex tmptra(-0.008, 0.06, 0.0022);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.6, 0.6, 0.6);
    scab = tmpsca;
    thetab = 0;
  }
    break;
  default:
    break;
  }
 // glPushMatrix();
  // glTranslated(-0.008, 0.06, 0.0022);
  // glutSolidCube(0.06);
  // glPopMatrix();

  animation->tra_base_ = trab;
  animation->rot_base_ = rotb;
  animation->sca_base_ = scab;
  animation->theta_base_ = thetab;

  //step 0
  Vertex tra0(0, 0, 0);
  Vertex rot0(0, 0, 1);
  Vertex sca0(1, 1, 1);
  translate[0] = tra0;
  rotation[0] = rot0;
  scale[0] = sca0;
  theta[0] = 0;

  //step 1
  Vertex tra1(0, 0, 0);
  Vertex rot1(0, 0, 1);
  Vertex sca1(1, 1, 1);
  translate[1]  = tra1; 
  rotation[1] = rot1;
  scale[1] = sca1;
  theta[1] = 0;

  //step 2
  Vertex tra2(0, 0, 0);
  Vertex rot2(0, 0, 1);
  Vertex sca2(1, 1, 1);
  translate[2] = tra2; 
  rotation[2] = rot2;
  scale[2] = sca2;
  theta[2] = 0;

  //step 3
  Vertex tra3(0, 0, 0);
  Vertex rot3(0, 0, 1);
  Vertex sca3(1, 1, 1);
  translate[3] = tra3;
  rotation[3] = rot3;
  scale[3] = sca3;
  theta[3] = 0;

  animation->set_animation(translate, rotation, 
			     scale, theta);

  return animation;
};


Animation *get_walk_animation(PART part) {

  Vertex translate[4];
  Vertex rotation[4];
  Vertex scale[4];
  double theta[4];

  Animation *animation = new Animation();
  Vertex trab, rotb, scab;
  double thetab;
  switch (part) {
  case LEFTLEG: {
    Vertex tmptra(-0.008, -0.1, -0.05);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.15, 1.2, 0.3);
    scab = tmpsca;
    thetab = 30;
  }
    break;
  case RIGHTLEG: {
    Vertex tmptra(-0.008, -0.1, 0.05);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.15, 1.2, 0.3);
    scab = tmpsca;
    thetab = -30;
  }
    break;
  case LEFTARM: {
    Vertex tmptra(-0.008, 0, 0.08);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.15, 1.2, 0.3);
    scab = tmpsca;
    thetab = -30;
  }
    break;
  case RIGHTARM: {
    Vertex tmptra(-0.008, 0, -0.08);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.15, 1.2, 0.3);
    scab = tmpsca;
    thetab = 30;
  }
    break;
  case HEAD: {
    Vertex tmptra(-0.008, 0.06, 0.0022);
    trab = tmptra;
    Vertex tmprot(1, 0, 0);
    rotb = tmprot;
    Vertex tmpsca(0.6, 0.6, 0.6);
    scab = tmpsca;
    thetab = 0;
  }
    break;
  default:
    break;
  }
 // glPushMatrix();
  // glTranslated(-0.008, 0.06, 0.0022);
  // glutSolidCube(0.06);
  // glPopMatrix();

  animation->tra_base_ = trab;
  animation->rot_base_ = rotb;
  animation->sca_base_ = scab;
  animation->theta_base_ = thetab;

  //step 0
  Vertex tra0(0, 0, 0);
  Vertex rot0(0, 0, 1);
  Vertex sca0(1, 1, 1);
  translate[0] = tra0;
  rotation[0] = rot0;
  scale[0] = sca0;
  theta[0] = 100;

  //step 1
  Vertex tra1(0, 0, 0);
  Vertex rot1(0, 0, 1);
  Vertex sca1(1, 1, 1);
  translate[1]  = tra1; 
  rotation[1] = rot1;
  scale[1] = sca1;
  theta[1] = 700;

  //step 2
  Vertex tra2(0, 0, 0);
  Vertex rot2(0, 0, 1);
  Vertex sca2(1, 1, 1);
  translate[2] = tra2; 
  rotation[2] = rot2;
  scale[2] = sca2;
  theta[2] = 260;

  //step 3
  Vertex tra3(0, 0, 0);
  Vertex rot3(0, 0, 1);
  Vertex sca3(1, 1, 1);
  translate[3] = tra3;
  rotation[3] = rot3;
  scale[3] = sca3;
  theta[3] = 310;

  animation->set_animation(translate, rotation, 
			     scale, theta);

  return animation;
};

 // glPushMatrix();
 //  glTranslated(-0.008, 0.06, 0.0022);
 //  glutSolidCube(0.06);
 //  glPopMatrix();


 //  glPushMatrix();
 //  glTranslated(-0.008, -0.1, -0.05);
 //  glRotatef(30, 1, 0, 0);
 //  glScalef(0.3, 1.0, 0.3); 
 //  glutSolidCube(0.1);
 //  glPopMatrix();

 // 
 //  glPushMatrix();
 //  glTranslated(-0.008, 0, -0.08);
 //  glRotatef(30, 1, 0, 0);
 //  glScalef(0.3, 1.0, 0.3); 
 //  glutSolidCube(0.1);
 //  glPopMatrix();

 //  glPushMatrix();
 //  glTranslated(-0.008, 0, 0.08);
 //  glRotatef(-30, 1, 0, 0);
 //  glScalef(0.3, 1.0, 0.3); 
 //  glutSolidCube(0.1);
 //  glPopMatrix();

 //  glPopMatrix();

 



void DrawTree::set_root() {

    Game3d *game = Game3d::instance();
    Animation *animation = new Animation();
    for (int i = 0; i < 4; i++) {
      Vertex vt(game->get_player1().x_, 
		game->get_player1().y_, game->get_player1().z_);

      animation->tra_base_ = vt;
      Vertex vtn(0, 0, 0);
      animation->translates_[i] = vtn;
      Vertex vr(0, 1, 0);
      animation->rot_base_ = vr;
      Vertex vrn(0, 1, 0);
      animation->rotation_[i] = vrn; 
      Vertex vs(1, 1, 1);
      animation->sca_base_ = vs;
      Vertex vsn(1, 1, 1);
      animation->scale_[i] = vsn;
      animation->theta_base_ = game->get_player1().posture_theta_;   	
      animation->theta_[i] = 0;
      
    };
   

   
    for (int j = 0; j < Node::NUMOFSTATES; j++) {
      root_->animation_[j] = animation;
    };
};

void DrawTree::init_root() {
    /////set animation node of left leg part
  Node *root = new Node();
  root_ = root;
  root_->num_child_nodes_ = NUMOFPARTS;

    Node *leftleg = new Node();
    Animation *stand_leftleg_ani = get_stand_animation(LEFTLEG);
    leftleg->animation_[Node::STAND] = stand_leftleg_ani;
    Animation *walk_leftleg = get_walk_animation(LEFTLEG);
    leftleg->animation_[Node::WALK] = walk_leftleg;
    leftleg->num_child_nodes_ = 0;
    leftleg->num_child_nodes_ = 0;
    root_->child_node_.push_back(leftleg);
    /////
    Node *rightleg = new Node();
    Animation *stand_rightleg_ani = get_stand_animation(RIGHTLEG);
    rightleg->animation_[Node::STAND] = stand_rightleg_ani;
    Animation *walk_rightleg = get_walk_animation(RIGHTLEG);
    rightleg->animation_[Node::WALK] = walk_rightleg;
    rightleg->num_child_nodes_ = 0;
    root_->child_node_.push_back(rightleg);
    ////
    Node *leftarm = new Node();
    Animation *stand_leftarm_ani = get_stand_animation(LEFTARM);
    leftarm->animation_[Node::STAND] = stand_leftarm_ani;
    Animation *walk_leftarm = get_walk_animation(LEFTARM);
    leftarm->animation_[Node::WALK] = walk_leftarm;
    leftarm->num_child_nodes_ = 0;
    root_->child_node_.push_back(leftarm);
    ////
    Node *rightarm = new Node();
    Animation *stand_rightarm_ani = get_stand_animation(RIGHTARM);
    rightarm->animation_[Node::STAND] = stand_rightarm_ani;
    Animation *walk_rightarm = get_walk_animation(RIGHTARM);
    rightarm->animation_[Node::WALK] = walk_rightarm;
    rightarm->num_child_nodes_ = 0;
    root_->child_node_.push_back(rightarm);
    ////
    Node *head = new Node();
    Animation *stand_head_ani = get_stand_animation(HEAD);
    head->animation_[Node::STAND] = stand_head_ani;
    Animation *walk_head = get_walk_animation(HEAD);
    head->animation_[Node::WALK] = walk_head;
    head->num_child_nodes_ = 0;
    root_->child_node_.push_back(head);

    //////////////////////////////////////////////////////
    
};

