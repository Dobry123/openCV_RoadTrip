#include "linedetector.cpp"
#include <iostream>
#include "comunication.cpp"

const int camIndex=0;

bool showResults=true;
bool sendToSTM=true;

cv::Mat inputFrame;
//cv::Mat inputVisionFrame;
cv::Mat outputVisionFrame;
cv::Mat transformedFrame;

int main(int argc, char *argv[])
{
    cv::VideoCapture cap(camIndex);
    LineDetector lineDetector;
    Comunication comunication;
    if(showResults)
        lineDetector.createTrackbars();

    while((char)cv::waitKey(1)!='q')
    {
        cap>>inputFrame;
        if(inputFrame.empty())
            break;
        outputVisionFrame=inputFrame.clone();
        outputVisionFrame=lineDetector.cutFrame(outputVisionFrame);
        lineDetector.applyBlur(outputVisionFrame,transformedFrame);
        lineDetector.grayTransform(transformedFrame,transformedFrame);
        lineDetector.makeBinary(transformedFrame,transformedFrame);
        lineDetector.detectEdge(transformedFrame,transformedFrame);
        if(showResults)
            imshow("contour",transformedFrame);
        lineDetector.detectPoints(transformedFrame);
        lineDetector.recognizeLines(transformedFrame);
        lineDetector.distanceFromCenter();

        if(showResults)
        {
            lineDetector.drawPoints(outputVisionFrame);
            lineDetector.drawDistanceInfo(outputVisionFrame);
            imshow("input",inputFrame);
            imshow("output",outputVisionFrame);
        }
        lineDetector.mapingDistance();
        std::cout<<lineDetector.distanceToCenter<<std::endl;
        if(sendToSTM)
            comunication.send(lineDetector.distanceToCenter);
    }

    return 0;
}
