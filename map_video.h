#ifndef MAP_VIDEO_H
#define MAP_VIDEO_H

#include <opencv2/core/core.hpp>

static int xioctl(int fd, int request, void *arg);
int init_mmap(int fd, uint8_t* buffer);
cv::Mat capture_image(int fd, uint8_t* buffer);

#endif // MAP_VIDEO_H
