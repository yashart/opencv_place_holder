#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

void display_img(Mat frame, Point2d shift);

int main(int, char* [])
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

        display_img(frame, shift);
        
        printf("x shift: %d, y shift %d\n", (int)shift.x, (int)shift.y);
        key = waitKey(2);

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
