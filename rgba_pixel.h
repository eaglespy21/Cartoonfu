
class rgba_pixel{
public:
  ~rgba_pixel(){}
  rgba_pixel(){}
  const float& operator[](const unsigned int index);
  const float& red() const; //Use these from now on and then convert .g's from before and make rgba private. Problem Solved!
  const float& green() const;
  const float& blue() const;
  const float& alpha() const;
  float r;
  float g;  
  float b;
  float a;  
};
