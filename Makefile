#specify compiler 

CC = g++

#platform dependent variables 

ifeq ("$(shell uname)", "Darwin") #I guess Darwin means apple ? Maybe 
  #these paths are based on installing openImageio with brew, hence no need to include paths 
  OIIO_INC = 
  OIIO_LIB = -lOpenImageIO
  OPENGL_LIB = -framework Foundation -framework GLUT -framework OpenGL #framwork similar to xcode frameworks ? Must be
else
  ifeq ("$(shell uname)", "Linux")
    #on SOC machines we need to specifically include OIIO path, 
    # but then again I dont think we are and that we need to. 
    OIIO_INC =
    OIIO_LIB = -lOpenImageIO
    OPENGL_LIB = -lglut -lGL -lGLU
  endif
endif

#build LDFLAGS and CFLAGS based on varaiables above
LDFLAGS = $(OPENGL_LIB) $(OIIO_LIB)   
CFLAGS = -g $(OIIO_INC)

#run will be the name of my project 

run:  main.o rgba_pixel.o image.o specialFunctions.o
	${CC} ${CFLAGS} -o main main.o rgba_pixel.o image.o specialFunctions.o ${LDFLAGS}
main.o: main.cpp openglFunctions.h
	${CC} ${CFLAGS} -c main.cpp
rgba_pixel.o:  rgba_pixel.cpp rgba_pixel.h
	${CC} ${CFLAGS} -c rgba_pixel.cpp
#this was captial I before, and it was still working don't know how! 
image.o: image.cpp image.h specialFunctions.h 
	${CC} ${CFLAGS} -c image.cpp
specialFunctions.o: specialFunctions.cpp specialFunctions.h
	${CC} ${CFLAGS} -c specialFunctions.cpp
  
clean:
	rm -f core.* *.o *~ run
       

