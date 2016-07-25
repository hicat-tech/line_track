CV_LIBPATH=./opencv

#/opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc -I. -o line_track main.c libserial.c libhisiv.a -lpthread -lm

/opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-g++  \
                -I. -o line_track main.cpp libserial.c libhisiv.a -lpthread -lm \
 -I$CV_LIBPATH/include -I$CV_LIBPATH/include/opencv -L$CV_LIBPATH/lib -lopencv_highgui -lopencv_imgproc -lopencv_core
