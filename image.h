#include<vector>
#include "rgba_pixel.h"
#include<OpenImageIO/imageio.h>

OIIO_NAMESPACE_USING
class Image{
public:
  Image(){}  

  void readImage(std::string);
  void writeImage(std::string);
  void updateDisplayImage();
  void allocateMemory();
  void insertPixels();
  rgba_pixel** getInvertedImage(); 
  const int& getWidth() const;
  const int& getHeight() const;
  const int& getChannels() const;
  void doToneMapping(float);
  void writeCurrentDisplayImage(std::string);  
  void doToneMappingWithConvolution(float);
  void doToneMappingExtension(float);
  void doToneMappingWithBiLateralFilter(float);
  void initKernel(char*);  
  void normalizeKernel();
  void convoluteImage();
  void updateDisplayWithConvolutedImage();
  ~Image();
  float* getPixel(int x, int y);
  void setPixel(int x, int y, float [4]);
  float getRedPixel(int x, int y);
  float getGreenPixel(int x, int y);
  float getBluePixel(int x, int y);
  float getAlphaPixel(int x, int y);
  void updateDisplayImageStep4();
  void updateDisplayImageGreyscale();
private:
  rgba_pixel **in_image;
  rgba_pixel **display_image;
  rgba_pixel **convolute_image;
  ImageSpec spec;
  std::vector<float>in_linear_image;
  int width;
  int height;
  int channels;
  float **kernel;
  int kernel_size;
  float kernel_sum;
  Image* operator=(Image&); //So that shallow copies are not created 
  Image(Image&);
};    
