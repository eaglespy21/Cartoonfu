#include<iostream>
#include<OpenImageIO/imageio.h>
#include<vector>
#include "image.h"
#include "specialFunctions.h"
#include<fstream> 
OIIO_NAMESPACE_USING

void Image::readImage(std::string filename){ 
  std::cout<<"readImage(): Loading specs of the Image\n";
  ImageInput *in = ImageInput::open(filename);
  if(!in){
    std::cerr<<"could not open file"<<std::endl;
    exit(-1); 
  }
  spec = in->spec();
  width = spec.width;
  height = spec.height;
  channels = spec.nchannels;
  in_linear_image.reserve(width*height*channels);
  in_linear_image.resize(width*height*channels); //sets the length of the vector
  in->read_image(TypeDesc::FLOAT, &in_linear_image[0]);
  //cleanup
  in->close();
  delete in;
}
void Image::insertPixels(){
  std::cout<<"inserting pixels into rgba_pixel objects (note: the image is inverted)(check:have you called allocate memory first?) \n";
  for(int row=0; row<height; row++){
    for(int col=0; col<width; col++){
      in_image[row][col].r = in_linear_image[(row*width+col)*channels + 0];
      in_image[row][col].g = in_linear_image[(row*width+col)*channels + 1];
      in_image[row][col].b = in_linear_image[(row*width+col)*channels + 2];
      if(channels == 4){
        in_image[row][col].a = in_linear_image[(row*width+col)*channels + 3];
      }
      else{
        in_image[row][col].a = 1.0;
      }
    }
  }
} 
void Image::writeImage(std::string filename){
  std::cout<<"writeImage() : writing image to file: "<<filename<<" (note: always a four channel image)\n";
  std::cout<<"Initialize temporary output linear array, openimageio takes linear array to write to a file\n";
  std:: vector<float> temp_out_linear_image;
  temp_out_linear_image.reserve(width*height*4); //channels = 4 always
  temp_out_linear_image.resize(width*height*4);
  ImageOutput *out = ImageOutput::create(filename);
  if(!out){
    std::cerr<<"error in opening: "<<filename<<std::endl;
    exit(0); 
  }  
  ImageSpec spec(width,height,4,TypeDesc::FLOAT);
  out->open(filename, spec);
  //linearize function starts here
  std::cout<<"Linearizing rgba pixels into a 1-D array for output\n";
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      temp_out_linear_image[(row*width+col)*4 + 0 ] = in_image[row][col].r;  
      temp_out_linear_image[(row*width+col)*4 + 1 ] = in_image[row][col].g;
      temp_out_linear_image[(row*width+col)*4 + 2 ] = in_image[row][col].b;
      temp_out_linear_image[(row*width+col)*4 + 3 ] = in_image[row][col].a;
    }
  }
  out->write_image(TypeDesc::FLOAT, &temp_out_linear_image[0]);
  if(!out->close()){
    std::cerr<<"Error in closing\n";
    delete out;
    exit(0);
  }
  delete out;  
} 
   
void Image::allocateMemory(){
  std::cout<<"allocateMemory(), allocating memory for display_image and in_image, can only be done after readImage() is called\n";
  in_image = new rgba_pixel*[height];
  in_image[0] = new rgba_pixel[width*height]; //this is the trick, right here to make sure that there is a contiguous block of memory for each image, pretty cool 
  display_image = new rgba_pixel*[height];
  display_image[0] = new rgba_pixel[width*height];
  convolute_image = new rgba_pixel*[height];
  convolute_image[0] = new rgba_pixel[width*height];
  for(int i = 1;i<height;i++){
    in_image[i] = in_image[i-1] + width;
    display_image[i] = display_image[i-1] + width;
    convolute_image[i] = convolute_image[i-1] + width;
  }
}  
void Image::updateDisplayImage(){ //for step3 only edges will appear ( FOR EDGE DETECTION)
  //memory for display_image should be allocated as soon as insertPixels gets called or even before that. If display gives seg fault its because insert pixels isnt called
  //which implies allocateMemory was not called
  std::cout<<"updateDisplayImage()\n";
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      display_image[height-row-1][col].r = in_image[row][col].r; //this is where inversion takes place. and this is the only occurance of inversion
      display_image[height-row-1][col].g = in_image[row][col].g;
      display_image[height-row-1][col].b = in_image[row][col].b;
      if(display_image[height-row-1][col].r  >0.1 || display_image[height-row-1][col].g >0.1 || display_image[height-row-1][col].b >0.1){
        display_image[height-row-1][col].a = 1;
      } //for step3 (only edges will appear in written image)
      else{
        display_image[height-row-1][col].a = 0;
      }
    }
  }
}

void Image::updateDisplayImageStep4(){ //and even for blur (FOR CONVOLUTION AND FOR BLUR)
  std::cout<<"updateDisplayImage()\n";
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      display_image[height-row-1][col].r = in_image[row][col].r;
      display_image[height-row-1][col].g = in_image[row][col].g;
      display_image[height-row-1][col].b = in_image[row][col].b;
      display_image[height-row-1][col].a = 1; //for step4 (normal convolution, smoothing and compositiong edges with blurred image)
    }
  }
}

void Image::updateDisplayImageGreyscale(){
  std::cout<<"updateDisplayImage()\n";
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      display_image[height-row-1][col].r = in_image[row][col].r;
      display_image[height-row-1][col].g = in_image[row][col].r;
      display_image[height-row-1][col].b = in_image[row][col].r;
      display_image[height-row-1][col].a = 1; //for (just greyscale)
    }
  }
}

void Image::writeCurrentDisplayImage(std::string filename){
  std::cout<<"writeCurrentDisplayImage() : writing current buffer image to file: "<<filename<<" (note: always a four channel image)\n";
  std::cout<<"Initialize temporary output linear array, openimageio takes linear array to write to a file\n";
  std:: vector<float> temp_out_linear_image;
  temp_out_linear_image.reserve(width*height*4); //channels = 4 always
  temp_out_linear_image.resize(width*height*4);
  ImageOutput *out = ImageOutput::create(filename);
  if(!out){
    std::cerr<<"error in opening: "<<filename<<std::endl;
    exit(0); // now we are returning 0 till now -2 -1 
  }
  ImageSpec spec(width,height,4,TypeDesc::FLOAT);
  out->open(filename, spec);
  //linearize function starts here
  std::cout<<"Linearizing rgba pixels into a 1-D array for output\n";
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      temp_out_linear_image[(row*width+col)*4 + 0 ] = display_image[height-row-1][col].r; //inverting the image back when pushing back into 1-D array  
      temp_out_linear_image[(row*width+col)*4 + 1 ] = display_image[height-row-1][col].g; //inversion needs to happen because display is inverted.
      temp_out_linear_image[(row*width+col)*4 + 2 ] = display_image[height-row-1][col].b;  
      temp_out_linear_image[(row*width+col)*4 + 3 ] = display_image[height-row-1][col].a;
    }
  }
  out->write_image(TypeDesc::FLOAT, &temp_out_linear_image[0]);
  if(!out->close()){
    std::cerr<<"Error in closing\n";
    delete out;
    exit(0);
  }
  delete out; 
}


const int& Image::getWidth() const{
  return width;
}
const int& Image::getHeight() const{
  return height;
}
const int& Image::getChannels() const{
  return channels;
}
rgba_pixel** Image::getInvertedImage(){  
  return display_image;  
}
void Image::updateDisplayWithConvolutedImage(){
  std::cout<<"updateDisplayWithConvolutedImage\n";
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      display_image[height-row-1][col].r = convolute_image[row][col].r;
      display_image[height-row-1][col].g = convolute_image[row][col].g;
      display_image[height-row-1][col].b = convolute_image[row][col].b;
      display_image[height-row-1][col].a = convolute_image[row][col].a;
    }
  }  
}

void Image::doToneMapping(float gamma){
  std::cout<<"Doing tone mapping with gamma as: "<<gamma<<std::endl;
  float lw,log_lw,ld; 
  //compute Lw
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      lw = (20.0*in_image[row][col].r + 40.0*in_image[row][col].g + in_image[row][col].b)/61.0;
      log_lw = log(lw);
      ld = exp(gamma * log_lw);
      in_image[row][col].r = (ld/lw) * in_image[row][col].r;
      in_image[row][col].g = (ld/lw) * in_image[row][col].g;
      in_image[row][col].b = (ld/lw) * in_image[row][col].b; 
    }
  }
  //compute ld
}

float **log_lw_array; 
float **B;

void Image::doToneMappingWithConvolution(float c){
  std::cout<<"Doing tone mapping with convolution with c as: "<<c<<std::endl;
  float lw,log_lw,ld;
  log_lw_array = new float*[height]; 
  log_lw_array[0] = new float[width*height];
  B = new float*[height];
  B[0] = new float[height*width];
  for(int i=1;i<height;i++){
    log_lw_array[i] = log_lw_array[i-1] + width;
    B[i] = B[i-1] + width;
  } 
  std::cout<<"Its the height and width\n";
  float S;  
  float sum_log_lw = 0.0;
  int row_inner, col_inner;
  float max_B = 0, min_B = 9999;
  float gamma;
  //compute Lw
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      lw = (20.0*in_image[row][col].r + 40.0*in_image[row][col].g + in_image[row][col].b)/61.0;
      log_lw_array[row][col] = log(lw);
    }
  }
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      for(int i=0;i<kernel_size;i++){
        for(int j=0;j<kernel_size;j++){
          row_inner =row-i; //these are kernel reflections 
          col_inner = col-j;
          if(row_inner > height-1){ row_inner = height-1;}  
          if(col_inner > width-1){ col_inner = width-1;} 
          if(row_inner < 0.0){ row_inner = 0.0;}
          if(col_inner < 0.0){ col_inner = 0.0;}
          sum_log_lw += kernel[i][j] * log_lw_array[row_inner][col_inner];
        }
      }
      B[row][col] = sum_log_lw; 
      max_B = max(max_B, sum_log_lw);
      min_B = min(min_B, sum_log_lw);
      sum_log_lw = 0.0; 
    }
  }
  gamma = log(c)/(max_B - min_B);
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      lw =(20.0*in_image[row][col].r + 40.0*in_image[row][col].g + in_image[row][col].b)/61.0;
      log_lw = log(lw);
      S = log_lw - B[row][col];
      ld = exp(gamma * B[row][col] + S);
      in_image[row][col].r = (ld/lw) * in_image[row][col].r;
      in_image[row][col].g = (ld/lw) * in_image[row][col].g;
      in_image[row][col].b = (ld/lw) * in_image[row][col].b;
    }
  }
}

void Image::initKernel(char* filename){ 
  kernel_sum = 0.0;
  std::ifstream read;
  read.open(filename);
  read>>kernel_size;
  //allocate memory for kernel
  kernel = new float*[kernel_size];
  kernel[0] = new float[kernel_size*kernel_size];
  for(int i=1;i<kernel_size;i++){
    kernel[i] = kernel[i-1] + kernel_size;
  }
  //insert values into kernel
  for(int i=0;i<kernel_size;i++){
    for(int j=0;j<kernel_size;j++){
      read>>kernel[i][j];
      kernel_sum +=kernel[i][j];
    }
  }
  if( kernel_sum == 0){
    for(int i=0;i<kernel_size;i++){
      for(int j=0;j<kernel_size;j++){
        kernel_sum += max(0.0,kernel[i][j]); //it should take only positive numbers
      }
    }
  }
  std::cout<<"Kernel_sum = "<<kernel_sum<<std::endl;
  for(int i=0;i<kernel_size;i++){
    for(int j=0;j<kernel_size;j++){
      std::cout<<kernel[i][j]<<" ";
    }
    std::cout<<"\n";
  }
}

void Image::doToneMappingWithBiLateralFilter(float c){
  std::cout<<"Doing tone mapping with convolution with c as: "<<c<<std::endl;
  float lw,log_lw,ld;
  log_lw_array = new float*[height]; 
  log_lw_array[0] = new float[width*height];
  B = new float*[height];
  B[0] = new float[height*width];
  for(int i=1;i<height;i++){
    log_lw_array[i] = log_lw_array[i-1] + width;
    B[i] = B[i-1] + width;
  }
  std::cout<<"Its the height and width\n";
  float S;  
  float sum_log_lw = 0.0;
  int row_inner, col_inner;
  float max_B = 0, min_B = 9999;
  float gamma;
  float d,w = 0;
  float lw_center;
  //compute Lw
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      lw = (20.0*in_image[row][col].r + 40.0*in_image[row][col].g + in_image[row][col].b)/61.0;
      log_lw_array[row][col] = log(lw);
    }
  }
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      lw_center = (20.0*in_image[row][col].r + 40.0*in_image[row][col].g + in_image[row][col].b)/61.0;
      for(int i=0;i<kernel_size;i++){
        for(int j=0;j<kernel_size;j++){
          row_inner =row-i; //these are kernel reflections 
          col_inner = col-j;
          if(row_inner > height-1){ row_inner = height-1;}  
          if(col_inner > width-1){ col_inner = width-1;} 
          if(row_inner < 0.0){ row_inner = 0.0;}
          if(col_inner < 0.0){ col_inner = 0.0;}
          lw = (20.0*in_image[row_inner][col_inner].r + 40.0*in_image[row_inner][col_inner].g + in_image[row_inner][col_inner].b)/61.0; 
          d = log_lw_array[row][col] - log_lw_array[row_inner][col_inner]; 
          w = exp(- max(0.0, min(0.1, d*d))); 
          sum_log_lw += (kernel[i][j] * log_lw_array[row_inner][col_inner])/w;
        }
      }
      B[row][col] = sum_log_lw; 
      max_B = max(max_B, sum_log_lw);
      min_B = min(min_B, sum_log_lw);
      sum_log_lw = 0.0;
      w = 0.0;
    }
  }
  gamma = log(c)/(max_B - min_B);
  std::cout<<"out of surgery, convolution done\n";
  std::cout<<"Starting operations 2 to 5 now\n";
  std::cout<<"gamma: "<<gamma<<" max_B: "<<max_B<<" min_B: "<<min_B<<std::endl;
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      lw =(20.0*in_image[row][col].r + 40.0*in_image[row][col].g + in_image[row][col].b)/61.0;
      log_lw = log(lw);
      S = log_lw - B[row][col];
      ld = exp(gamma * B[row][col] + S);
      in_image[row][col].r = (ld/lw) * in_image[row][col].r;
      in_image[row][col].g = (ld/lw) * in_image[row][col].g;
      in_image[row][col].b = (ld/lw) * in_image[row][col].b;
    }
  }
  std::cout<<"out of second surgery, image ready to be displayed\n";
}
  

void Image::normalizeKernel(){
  std::cout<<"normalizeKernel() with factor:"<<kernel_sum<<"\n";
  for(int i=0;i<kernel_size;i++){
    for(int j=0;j<kernel_size;j++){
      kernel[i][j] /= kernel_sum;
    }
  }
  std::cout<<"Normalized kernel: \n";
  for(int i=0;i<kernel_size;i++){
    for(int j=0;j<kernel_size;j++){
      std::cout<<kernel[i][j]<<" ";
    }
    std::cout<<"\n";
  }
}

void Image::convoluteImage(){
  std::cout<<"convoluteImage()\n";
  int row_inner,col_inner;
  float sum_r=0.0, sum_g=0.0, sum_b=0.0, sum_a=0.0;
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){
      for(int i=0;i<kernel_size;i++){
        for(int j=0;j<kernel_size;j++){
          row_inner = row-i; //reflected about the center horizaontal axis of the kernel
          col_inner = col-j; //reflected about the center vertical axis of the kernel
          //boundary conditions
          if(row_inner > height-1){ row_inner = height-1;}  
          if(col_inner > width-1){ col_inner = width-1;} 
          if(row_inner < 0.0) { row_inner = 0.0;}
          if(col_inner < 0.0) { col_inner = 0.0;}
          //convolution operation
          sum_r += kernel[i][j]* in_image[row_inner][col_inner].r;
          sum_g += kernel[i][j]* in_image[row_inner][col_inner].g;
          sum_b += kernel[i][j]* in_image[row_inner][col_inner].b;
          sum_a += kernel[i][j]* in_image[row_inner][col_inner].a; 
        }
      }
      convolute_image[row][col].r = sum_r;
      convolute_image[row][col].g = sum_g;
      convolute_image[row][col].b = sum_b;
      convolute_image[row][col].a = sum_a;
      sum_r = 0.0; sum_g = 0.0; sum_b = 0.0; sum_a = 0.0;
    }
  }
  for(int row=0;row<height;row++){
    for(int col=0;col<width;col++){   
      in_image[row][col].r = convolute_image[row][col].r;
      in_image[row][col].g = convolute_image[row][col].g;
      in_image[row][col].b = convolute_image[row][col].b;
      in_image[row][col].a = convolute_image[row][col].a;
    }
  }
}

Image::~Image(){
}

float color[4]; //local memory issues !!!
float* Image::getPixel(int x, int y){
  color;
  color[0] = in_image[x][y].r;
  color[1] = in_image[x][y].g;
  color[2] = in_image[x][y].b;
        if(channels == 4){
          color[3] = in_image[x][y].a;
      }
      else{
        color[3] = 1.0;
      }

  return color;
}
 
float Image::getRedPixel(int x, int y){
  return in_image[x][y].r;
}
float Image::getGreenPixel(int x, int y){
  return in_image[x][y].g;
}
float Image::getBluePixel(int x, int y){
  return in_image[x][y].b;
}
float Image::getAlphaPixel(int x, int y){
  return in_image[x][y].a;
}
int count = 0;
void Image::setPixel(int x, int y,float *color){
  in_image[x][y].r = color[0];
  in_image[x][y].g = color[1];
  in_image[x][y].b = color[2];
          if(channels == 4){
          in_image[x][y].a = color[3];
      }
      else{
        in_image[x][y].a = 1.0;
      }
}

