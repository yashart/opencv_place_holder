#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

int main(int, char* [])
{
    // Создание видеопотока с камеры
    VideoCapture video(0);
    Mat frame, curr, prev, curr64f, prev64f, hann;
    int key = 0;

    do
    {
        video >> frame; // Очередной фрейм
        cvtColor(frame, curr, CV_RGB2GRAY); // Перевод в градации серого

        if(prev.empty())
        {
            prev = curr.clone(); // клонирование изображения
            createHanningWindow(hann, curr.size(), CV_64F); // Создание окна Ханна
        }

        prev.convertTo(prev64f, CV_64F);
        curr.convertTo(curr64f, CV_64F);

        Point2d shift = phaseCorrelate(prev64f, curr64f, hann); // Фазовая корреляция
        //double radius = cv::sqrt(shift.x*shift.x + shift.y*shift.y); // Вычисление радиуса отклонения

        /*
        if(radius > 5)
        {
            // вывод на экран окружности и направления смещения
            Point center(curr.cols >> 1, curr.rows >> 1);
            cv::circle(frame, center, (int)radius, cv::Scalar(0, 255, 0), 3, CV_AA);
            cv::line(frame, center, Point(center.x + (int)shift.x, center.y + (int)shift.y), cv::Scalar(0, 255, 0), 3, CV_AA);
        }
        */

        //imshow("phase shift", frame);
        printf("x shift: %d, y shift %d\n", (int)shift.x, (int)shift.y);
        key = waitKey(2);

        prev = curr.clone();
    } while((char)key != 27); // Esc to exit...

    return 0;
}
