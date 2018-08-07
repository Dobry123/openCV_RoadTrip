#ifndef LINEDETECTOR_H
#define LINEDETECTOR_H

#include <iostream>
#include <opencv2/opencv.hpp>

class LineDetector
{
public:
    int distanceToCenter;
    int action; //0-only left lane      1-two lanes     2-only right lane
    LineDetector()
    {
        action=1;
    }
    std::vector<cv::Point> detectedPoints;
    void createTrackbars();
    void applyBlur(cv::Mat &input, cv::Mat &output);
    void grayTransform(cv::Mat &input, cv::Mat &output);
    void detectEdge(cv::Mat &input, cv::Mat &output);
    void makeBinary(cv::Mat &input, cv::Mat &output);
    void detectPoints(cv::Mat &input);
    void drawPoints(cv::Mat &output);
    cv::Mat cutFrame(cv::Mat &intput);
    void recognizeLines(cv::Mat &intput);
    void distanceFromCenter();
    void drawDistanceInfo(cv::Mat &output);
    std::string intToStr(int n);
    void mapingDistance();

    static void on_trackbar(int, void*);

};





#endif // LINEDETECTOR_H
