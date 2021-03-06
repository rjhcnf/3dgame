#ifndef TITLE_H_
#define TITLE_H_

#include "Base.h"
class FontDraw;

class Title : public Base
{
 public:
  static Title *instance();
  static void create();
  static void destroy();
  //update function for Base class
  Base *update();

  //keybord handler
  void keybord_handler(unsigned char key);

  //draw function
  void draw();

  //timer function
  void timer();

  enum TitleDisp { PLAY = 0,
		       SETTING
  };
  
  TitleDisp title_disp_;
  FontDraw *font_draw_;
 private:
  Title();
  ~Title();
  
  static Title *title_;

};

#endif
