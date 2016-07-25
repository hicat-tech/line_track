#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "libhisiv.h"
#include "libserial.h"
#include"cv.h"
#include"highgui.h"

#define RESOLUTION 128

static volatile int serial_fd = -1;
volatile static int force_exit = 0;

void sighandler(int sig)
{
    force_exit = 1;
}

int find_line(int *buf)
{
    int min = 50;
    int n = -1;
    int i;

    for (i = 0; i < RESOLUTION; i++) {
        if (buf[i] < min) {
            min = buf[i];
            n = i;
        }
    }

    return n;
}

int main(int argc,char* argv[])
{

    char data[1280*720];
    int result[RESOLUTION];
    int position = -1;
    int last_valid_position = -1;
    char command[64];
    int command_length = 0;
    int len;
    int i;
    int x,y;

    IplImage *img720 = cvCreateImageHeader(cvSize(1280,720), 8, 1);
	IplImage *img180 = cvCreateImage(cvSize(160, 90),8,1);

    cvSetImageData(img720, data, 1280);

    signal(SIGINT, sighandler);

	serial_fd = UART_Open("/dev/ttyAMA1");
	if (serial_fd < 0) {
		printf("Failed to open /dev/ttyAMA1\n");
	} else {
		printf("/dev/ttyAMA1 is opened");
	}
	UART_Init(serial_fd, 115200);

    while (!force_exit) {
        len = venc_getYUV(0, data);
        for(i=0; i<RESOLUTION; i++) {
            result[i] = 0;
        }
        for(i=0; i<RESOLUTION; i++) {
            for(y=635; y<(635+10); y++) {
                for(x=i*(1280/RESOLUTION); x<(i+1)*(1280/RESOLUTION); x++) {
                    result[i]+=data[y*1280+x];
                }
            }
            result[i] = result[i]/((1280/RESOLUTION)*10);
        }

        position = find_line(result);
        for(i=0; i<RESOLUTION; i++) {
            if (position == i) {
                printf("[ %d ]", result[i]);
            } else {
                printf(" %d ",result[i]);
            }
        }
        printf("\n");

        if (position >= 0 && position != last_valid_position) {
            cvRectangle(img720, cvPoint((position - 4) * 1280 / RESOLUTION, 600), cvPoint((position + 4) * 1280 / RESOLUTION, 700), cvScalar(255, 255, 255, 0),1);

            // cvRectangle(img720, cvPoint(100, 100), cvPoint(200, 200), cvScalar(255, 255, 255), 1);

            cvResize(img720, img180);

            cvSaveImage("/www/mmc/opencv/hi.jpg", img180);
        }

        if (position < 0) {
            if (last_valid_position < (RESOLUTION/2)) {
                command_length = snprintf(command, sizeof(command), "motor -64 64\n");
            } else {
                command_length = snprintf(command, sizeof(command), "motor 64 -64\n");
            }
        } else {
            int left, right;
            int delta = RESOLUTION / 2 - position;

            if (delta < 0) {
                delta = -delta;
            }
            delta = delta / 8 + 1;
            left = 32 + position / delta;
            right = 32 + (RESOLUTION - position) / delta;

            command_length = snprintf(command, sizeof(command), "motor %d %d\n", left, right);
            last_valid_position = position;
        }

        UART_Send(serial_fd, command, command_length);

        // usleep(10000);
    }

    command_length = snprintf(command, sizeof(command), "motor 0 0\n");
    UART_Send(serial_fd, command, command_length);


    cvReleaseImageHeader(&img720);
    cvReleaseImage(&img180);
    exit(0);
}
