#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringSerial.h>
//#include <wiringPi.h>
#include <time.h>

#include "uart.h"
#include "send_img_ssh.h"
#include "map_video.h"

using namespace cv;

void display_img(Mat frame, Point2d shift);
void save_img(Mat frame, Point2d shift, int imgCounter, double time, int log);
Point2d calc_shift(Mat* curr, Mat* prev, Mat* frame,
                   Mat* prev64f, Mat* curr64f, Mat* hann);


//argv[1] - serial port
//argv[2] - param scp
//argv[3] - log
int main(int argc, char* argv[])
{
    VideoCapture video(0);
    Mat frame, curr, prev, curr64f, prev64f, hann;
    int uartfd = 1;//serialOpen(argv[1], 115200);//open (argv[1], O_RDWR);
    if (uartfd < 0)
    {
            fprintf (stderr, "error %d opening %s: %s", errno, argv[1], strerror (errno));
            return 0;
    }
    //init_uart_fd(uartfd);
    int isFirstImg = 1;
    int resultShiftData[4] = {};
    int imgCounter = 0;
    struct stat imgFileStat;
    time_t startTime;
    time_t currTime;
    time(&startTime);
    do {
        video >> frame;
        Point2d shift = calc_shift(&curr, &prev, &frame,
                                   &prev64f, &curr64f, &hann);

        time(&currTime);
        save_img(frame, shift, imgCounter, difftime(currTime, startTime), atoi(argv[3]));
        if (isFirstImg) {
            fork_and_exec_scp("img.jpg", argv[2]);
            isFirstImg = 0;
        }
        //display_img(frame, shift);
        stat("img.jpg", &imgFileStat);
        printf("x shift: %d, y shift %d, size %d\n", (int)shift.x, (int)shift.y, (int)imgFileStat.st_size);
        resultShiftData[0] = (int)shift.x;
        resultShiftData[1] = (int)shift.y;
        //serialPuts(uartfd, resultShiftData);
        //write (uartfd, resultShiftData, sizeof(*resultShiftData)*3);
        //write(uartfd, "hi!", 4);
        imgCounter ++;
    } while(1);

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

void save_img(Mat frame, Point2d shift, int imgCounter, double time, int log) {
    double radius = cv::sqrt(shift.x*shift.x + shift.y*shift.y);
    if(radius > 5)
    {
        Point center(frame.cols >> 1, frame.rows >> 1);
        cv::circle(frame, center, (int)radius, cv::Scalar(0, 255, 0), 3, CV_AA);
        cv::line(frame, center, Point(center.x + (int)shift.x, center.y + (int)shift.y), cv::Scalar(0, 255, 0), 3, CV_AA);
    }

    imwrite("img.jpg", frame);
    if (log) {
        char newname[50] = {};
        sprintf(newname, "log/%.f_%d.jpg", time, imgCounter);
        printf("%s\n", newname);
        imwrite(newname, frame);
    }
}

Point2d calc_shift(Mat* curr, Mat* prev, Mat* frame,
                   Mat* prev64f, Mat* curr64f, Mat* hann) {
    cvtColor(*frame, *curr, CV_RGB2GRAY);

    if(prev->empty())
    {
        *prev = curr->clone();
        createHanningWindow(*hann, curr->size(), CV_32F);
    }

    prev->convertTo(*prev64f, CV_32F);
    curr->convertTo(*curr64f, CV_32F);

    return phaseCorrelate(*prev64f, *curr64f, *hann);
}
