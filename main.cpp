#ifdef __APPLE__
#include<GLUT/glut.h>
#else
#include<GL/glut.h>
#endif
#include<iostream>
#include "image.h"
#include "openglFunctions.h"
static int original_flag = 0;  
static bool flag_step_3 = false;
static bool final_step_flag = false;
static bool flag_autoprocess = true;
std::string out_filename, in_filename, step1_filename, step3_filename, step4_filename;
char* kernel_filename;
Image firstImage, firstImageCpy, copyImage, step1Image, step3Image, step4Image; 
std::string image1File("results/image1.png"), copyOriginalImageFile("results/copyOriginal.png"), image2File("results/image2.png"), image3File("results/image3.png");
std::string image4File("results/image4.png"), image5File("results/image5.png"), image6File("results/image6.png");
char* kernelFile("filters/sobol-vert.filt");
char* kernelFile1("filters/sobol-horiz.filt");
char* kernelFile2("filters/diagonal1.filt");
char* kernelFile3("filters/diagonal2.filt");
char* kernelFile4("filters/gaussian.filt");
static int stepCount=-1;
void step4Function();
void detectEdges(std::string in_file, char* kernel_file, std::string out_file); 
void initStep13and4Images(std::string in_file, std::string file);
static int width, height;
static float g, c, b;
void drawImage(){
  glClear(GL_COLOR_BUFFER_BIT);
  if(!final_step_flag) glViewport(0,0,width, height);  
  else glViewport(0,0,step1Image.getWidth(), step1Image.getHeight());
  glRasterPos2i(0,0);
  if(!final_step_flag){
  if(!original_flag){
    glDrawPixels(width, height, GL_RGBA, GL_FLOAT, firstImage.getInvertedImage()[0]);
  }
  else if(original_flag) {
    std::cout<<"Drawing original image\n";
    glDrawPixels(width,height,GL_RGBA,GL_FLOAT, firstImageCpy.getInvertedImage()[0]);
  }
  }
  else{
    std::cout<<"Drawing composited image\n";
    glDrawPixels(step1Image.getWidth(),step1Image.getHeight(),GL_RGBA,GL_FLOAT, step1Image.getInvertedImage()[0]);
  }
  glFlush();
}

void handleKey(unsigned char key, int x, int y){
  switch(key){
    case 't':
    case 'T':
      break;
    case 's':
    case 'S':
      stepCount++; 
      switch(stepCount){
        std::cout<<stepCount<<std::endl;
        case 0: //detect horizontal edges 
          detectEdges(in_filename, kernelFile, image1File);
        break;
        case 1: //detect vertical edges 
          detectEdges(in_filename, kernelFile1, image2File);
        break;
        case 2: //detect right to left diagonal edges
          detectEdges(in_filename, kernelFile2, image3File);
        break;
        case 3: //detect left to right diagonal edges 
          detectEdges(in_filename, kernelFile3, image4File);
        break;
        case 4:
        //Compositing horizontal edges on original image
          initStep13and4Images(in_filename, image1File);
          step4Function();

          step1Image.writeCurrentDisplayImage("results/image3_1.png");
          final_step_flag = true;
        break;
        case 5:
        //Compositing vertical edges
        initStep13and4Images("results/image3_1.png", image2File);
        step4Function();

        step1Image.writeCurrentDisplayImage("results/image3_2.png");
        final_step_flag = true;
        break;
        case 6:

        //Compositing right to left edges
        initStep13and4Images("results/image3_2.png", image3File);
        step4Function();

        step1Image.writeCurrentDisplayImage("results/image3_3.png");
        final_step_flag = true;
        break;
        case 7:

        //Compositing left to right edges
        initStep13and4Images("results/image3_3.png", image4File);
        step4Function();

        step1Image.writeCurrentDisplayImage("results/image3_4.png");
        final_step_flag = true;
        break;
        case 8:
        //Applying gaussian blur
        firstImage.readImage("results/image3_4.png");
        width = firstImage.getWidth();
        height = firstImage.getHeight();
        firstImage.allocateMemory();
        firstImage.insertPixels();
        firstImage.updateDisplayImage();
        firstImage.writeCurrentDisplayImage(copyOriginalImageFile);

        firstImage.initKernel(kernelFile4);
        firstImage.normalizeKernel();
        firstImage.convoluteImage();
        firstImage.updateDisplayImageStep4();
        firstImage.writeCurrentDisplayImage(image6File);
        final_step_flag = false;
        break;
        default:
        std::cout<<"default\n";
        break;
     }             
     glutPostRedisplay();
      
     break;
    case 'e':
    case 'E':
      break;
    case 'c':
    case 'C':
      break; 
    case 'd':
    case 'D':
      break;
    case 'r':
    case 'R':
      //Showing original image
      original_flag = !original_flag;
      glutPostRedisplay();
      break;  
    case 'w':
    case 'W':
      break;
    case 'q':
    case 'Q':
    case 27:
      exit(7);
      break;
    default:
      return;
  }
}

void initStep13and4Images(std::string in_file, std::string file){ //initialization of the two images to be composited 
  step1Image.readImage(in_file); 
  width = step1Image.getWidth(); //width and height of all images will be the same 
  height = step1Image.getHeight();
  step1Image.allocateMemory();
  step1Image.insertPixels();
  step1Image.updateDisplayImage();
  step3Image.readImage(file);
  step3Image.allocateMemory();
  step3Image.insertPixels();
  step3Image.updateDisplayImage();
}

float temp[4];
void step4Function(){ //Composition function 
  for(int i=0;i<height;i++){
    for(int j=0;j<width;j++){
      if(step3Image.getAlphaPixel(i,j) > 0 ){
        temp[0]=temp[1]=temp[2] = 0; //setting edges to black
        temp[3] = 1;
        step1Image.setPixel(i,j,temp);
      }
    }
  }
  step1Image.updateDisplayImageStep4();
}  

void detectEdges(std::string in_file, char* kernel_file, std::string out_file){ //Convolution of original image to get different edges 
  firstImage.readImage(in_file);
  width = firstImage.getWidth();
  height = firstImage.getHeight();
  firstImage.allocateMemory();
  firstImage.insertPixels();
  firstImage.updateDisplayImage();
  firstImage.writeCurrentDisplayImage(copyOriginalImageFile);
  firstImage.initKernel(kernel_file);
  firstImage.normalizeKernel();
  firstImage.convoluteImage();
  firstImage.updateDisplayImage();
  firstImage.writeCurrentDisplayImage(out_file);
}  

void handleMouse(int button, int state, int x, int y){}

int main(int argc, char**argv){
  int choice=0;
  glutInit(&argc, argv);

  //Handling arguments 
  in_filename = argv[1];

  firstImage.readImage(in_filename); //just to get width and height
  width = firstImage.getWidth(); //reading original image to display in the beginning 
  height = firstImage.getHeight();
  firstImage.allocateMemory();
  firstImage.insertPixels();
  firstImage.updateDisplayImage();
  firstImage.writeCurrentDisplayImage(copyOriginalImageFile);

  firstImageCpy.readImage(in_filename); //Copy of original Image
  width = firstImageCpy.getWidth(); //To show with reset button 
  height = firstImageCpy.getHeight();
  firstImageCpy.allocateMemory();
  firstImageCpy.insertPixels();
  firstImageCpy.updateDisplayImage();
  firstImageCpy.writeCurrentDisplayImage(copyOriginalImageFile);

  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA);
  glutInitWindowSize(firstImage.getWidth(), firstImage.getHeight());
  glutCreateWindow("Cartoonfu\n");
  glutDisplayFunc(drawImage);
  glutKeyboardFunc(handleKey);
  glutMouseFunc(handleMouse);
  //define drawing coordinate system which starts from lower left(0,0) and top left(width, height)
  //but of course displayfunc rasters from top left.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, firstImage.getWidth(), 0, firstImage.getHeight());
  glClearColor(1,1,1,1);
  glutMainLoop();
  return 0;
}  
