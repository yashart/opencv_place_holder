#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "uart.h"

using namespace cv;

void display_img(Mat frame, Point2d shift);

int main(int argc, char* argv[])
{
    VideoCapture video(0);
    Mat frame, curr, prev, curr64f, prev64f, hann;
    int key = 0;

    do
    {
        for(int i = 0; i < 7; i++) {
            video.grab();
        }
        video >> frame;
        cvtColor(frame, curr, CV_RGB2GRAY);

        if(prev.empty())
        {
            prev = curr.clone();
            createHanningWindow(hann, curr.size(), CV_64F);
        }

        prev.convertTo(prev64f, CV_64F);
        curr.convertTo(curr64f, CV_64F);

        Point2d shift = phaseCorrelate(prev64f, curr64f, hann);

        //display_img(frame, shift);
        
        //printf("x shift: %d, y shift %d\n", (int)shift.x, (int)shift.y);
        //key = waitKey(2);

        int fd = open (argv[1], O_RDWR | O_NOCTTY | O_SYNC);
        if (fd < 0)
        {
                fprintf (stderr, "error %d opening %s: %s", errno, argv[1], strerror (errno));
                return 0;
        }

        set_interface_attribs (fd, 115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
        set_blocking (fd, 0);                // set no blocking


        write (fd, "hello!\n", 7);           // send 7 character greeting
        usleep ((7 + 25) * 100);
        char buf [100];
        int n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
        printf("Hello, %s", buf);

        //prev = curr.clone();
    } while((char)key != 27);

    return 0;
}

void display_img(Mat frame, Point2d shift) {
        double radius = cv::sqrt(shift.x*shift.x + shift.y*shift.y);	
	if(radius > 5)
        {
            Point center(frame.cols >> 1, frame.rows >> 1);
            cv::circle(frame, center, (int)radius, cv::Scalar(0, 255, 0), 3, CV_AA);
            cv::line(frame, center, Point(center.x + (int)shift.x, center.y + (int)shift.y), cv::Scalar(0, 255, 0), 3, CV_AA);
        }
        
        imshow("phase shift", frame);
}


