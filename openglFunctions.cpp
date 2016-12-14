#include<iostream>
#include "image.h"
#ifdef __APPLE__
#include<GLUT/glut.h>
#else
#include<GL/glut.h>
#endif

void handleKey(unsigned char key, int x, int y){
  switch(key){
    case 'w':
    case 'W':
      firstImage.writeImage(out_filename);
      break;
    default:
      return;
  }
}

void handleMouse(int button, int state, int x, int y){}

void drawImage(){}    
