#include "linedetector.h"
#include <iostream>
#include <string>

static int maxDistance=180;
static int blurSize=1;
static int binaryThresh=170;
static int threshLines = 10;
static int minLengthOfLine = 5;

static const int max_blurSize=20;
static const int max_binaryThresh=255;
static const int max_threshLines=255;
static const int max_minLengthOfLine=255;
static int visionHeight=320;
static int visionWidth=640;

const std::string trackbarWindowName = "Trackbars";

void LineDetector::createTrackbars()
{
    cv::namedWindow(trackbarWindowName);
    cv::createTrackbar("blurSize",trackbarWindowName,&blurSize,max_blurSize,on_trackbar);
    cv::createTrackbar("binaryThresh",trackbarWindowName,&binaryThresh,max_binaryThresh,on_trackbar);
    cv::createTrackbar("threshLines",trackbarWindowName,&threshLines,max_threshLines,on_trackbar);
    cv::createTrackbar("minLengthOfLine",trackbarWindowName,&minLengthOfLine,max_minLengthOfLine,on_trackbar);
}

void LineDetector::on_trackbar(int,void*)
{
    ;
}

void LineDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    int odd=2*blurSize+1;
    cv::GaussianBlur(input, output, cv::Size( odd, odd ), 0, 0 );
}

void LineDetector::makeBinary(cv::Mat &input, cv::Mat &output)
{
    cv::threshold(input,output,binaryThresh,255,cv::THRESH_BINARY_INV);
}

void LineDetector::grayTransform(cv::Mat &input, cv::Mat &output)
{
    cv::cvtColor(input,output,cv::COLOR_BGR2GRAY);
}

void LineDetector::detectEdge(cv::Mat &input, cv::Mat &output)
{
    cv::Mat kernel = (cv::Mat_<char>(3,3) <<  0, 0,  0,
                                   -1,  0, 1,
                                    0, 0,  0);
    cv::filter2D(input,output,input.depth(),kernel);
}

void LineDetector::detectPoints(cv::Mat &input)
{
    this->detectedPoints.clear();
    std::vector<cv::Vec4i> detectedLines;
    HoughLinesP(input, detectedLines, 1, CV_PI/180, threshLines, minLengthOfLine, 5 );
    for( size_t i = 0; i < detectedLines.size(); ++i)
    {
        cv::Vec4i l = detectedLines[i];
        this->detectedPoints.push_back(cv::Point(l[0],l[1]));
        this->detectedPoints.push_back(cv::Point(l[2],l[3]));
    }
}

void LineDetector::drawPoints(cv::Mat &output)
{
    for( size_t i = 0; i < this->detectedPoints.size(); ++i)
    {
        cv::line(output,this->detectedPoints[i],this->detectedPoints[i],cv::Scalar(0,0,255), 4, cv::LINE_AA);
    }
}

cv::Mat LineDetector::cutFrame(cv::Mat &input)
{
    cv::Mat output(input,cv::Rect(input.cols/2-visionWidth/2,input.rows-visionHeight,visionWidth,visionHeight));
    return output;
}

void LineDetector::recognizeLines(cv::Mat &input)
{
    bool line=false;
    bool ifLineExsist[2]={false,false};
    int indexOfLine[2];
    int whichLine=0;
    unsigned int pixelCount=0;
    int heightToSearch=170;
    const int amountOfCols=input.cols;
    int sumInCols[amountOfCols];
    for(int i=0;i<amountOfCols;i++)
    {
        sumInCols[i]=cv::sum(input(cv::Rect(i,input.rows-heightToSearch,1,heightToSearch)))[0]/255;
        if(whichLine<2)
        {
            if(sumInCols[i]>0)
            {
                if(line==false)
                {
                    ifLineExsist[whichLine]=true;
                    indexOfLine[whichLine]=i;
                    line=true;
                }
                pixelCount++;
            }
            else if(line==true && pixelCount>2)
            {
                indexOfLine[whichLine]-=int(pixelCount/2);
                whichLine++;
                pixelCount=0;
            }
            else
            {
                ifLineExsist[whichLine]=false;
                line=false;
                pixelCount=0;
            }
        }
    }

    switch (whichLine) {
    case 0:
        //no line detected
        break;
    case 1:
        // one line detected
        if(indexOfLine[0]<input.cols/2 && this->action!=2)
            this->action=0;
        else if(this->action!=0)
            this->action=2;
        break;
    case 2:
        this->action=1;
        // two lines detected
    default:
        break;
    }
}

void LineDetector::distanceFromCenter()
{
    int middleX=int(visionWidth/2);
    int minL=0;
    int minR=INT_MAX;
    switch(this->action)
    {
    case 1:
        for(size_t i=0;i<this->detectedPoints.size();++i)
        {
            if(this->detectedPoints[i].x<middleX)
            {
                if(this->detectedPoints[i].x>minL)
                    minL=this->detectedPoints[i].x;
            }
            else
            {
                if(this->detectedPoints[i].x<minR)
                    minR=this->detectedPoints[i].x;
            }
        }
        this->distanceToCenter=(minR-middleX)-(middleX-minL);
        break;
    case 2:
        this->distanceToCenter=-1*maxDistance;
        break;
    case 0:
        this->distanceToCenter=maxDistance;
        break;
    }
}

void LineDetector::drawDistanceInfo(cv::Mat &output)
{
    switch (this->action) {
    case 0:
        cv::putText(output,"turn right",cv::Point(output.cols/2,output.rows/2),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0));
        break;
    case 2:
        cv::putText(output,"turn left",cv::Point(output.cols/2,output.rows/2),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0));
        break;
    case 1:
        std::string stringToDraw=intToStr(this->distanceToCenter);
        cv::putText(output,stringToDraw,cv::Point(output.cols/2,output.rows/2),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0));
        cv::line(output,cv::Point(output.cols/2,output.rows/2+25),cv::Point(output.cols/2,output.rows/2-45),cv::Scalar(255,0,0),2);
        break;
    }
}

std::string LineDetector::intToStr(int n)
{
     std::string tmp;
     if(n < 0) {
      tmp = "-";
      n = -n;
     }
     if(n > 9)
      tmp += intToStr(n / 10);
     tmp += n % 10 + 48;
     return tmp;
}

void LineDetector::mapingDistance()
{
    if(this->distanceToCenter>0)
        this->distanceToCenter = int((this->distanceToCenter - 0) * (255 - 127) / (maxDistance - 0) + 127);
    else
    {
        this->distanceToCenter = int((this->distanceToCenter + maxDistance) * (127 - 0) / (0 + maxDistance) + 0);
        //this->distanceToCenter*=-1;
        //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}

