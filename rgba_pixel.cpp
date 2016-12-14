#include<iostream> //for cerr, cout
#include<cstdlib> // for exit()
#include "rgba_pixel.h"
const float& rgba_pixel::operator[]( const unsigned int index){
  if(index == 0){
    return r;
  } else if(index == 1){
    return g;
  } else if(index == 2){
    return b;
  } else if(index == 3){
    return a;
  } else{
    std::cerr<<"invalid index: "<<index<<std::endl; 
    exit(-1);
  } 
}
const float& rgba_pixel::red() const{
  return r;
}
const float& rgba_pixel::green() const{
  return g;
}
const float& rgba_pixel::blue() const{
  return b;
}
const float& rgba_pixel::alpha() const{
  return a;
}
