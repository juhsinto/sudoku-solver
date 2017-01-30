/*
 * File:   SudokuVision.h
 * Author: Jm
 *
 * Created on September 10, 2014, 3:57 AM
 */

#ifndef SUDOKUVISION_H
#define	SUDOKUVISION_H
#include <vector>
#include <iostream>
using namespace std;

//openCV headers
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

// enable debug mode, set to 1
#define DEBUG 0

#if DEBUG
    #define SSTR( x ) dynamic_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x ) ).str()
#endif


class SudokuVision
{
public:
    SudokuVision();

    // [STUB], what the class is actually supposed to return
    vector< vector<int> > getData();

private:
    // Storage for images
    cv::Mat cam_frame, mask, res;

    // blur, grey, close_morph_ellipse, normalize
    Mat preprocessImage(Mat input);

    // Find and sort contours
    Mat getSudokuContour(Mat& input);

    Mat getVerticalLines(Mat& input);

    Mat getHorizontalLines(Mat& input);

    Mat getWarpedPuzzle(Mat& input);

};
#endif	/* SUDOKUVISION_H */


SudokuVision::SudokuVision() {

        while (true) {
            cam_frame = cv::imread("sudoku.jpg", 1);
            imshow("original image", cam_frame);
            res = preprocessImage(cam_frame);

            Mat onlyPuzzle = getSudokuContour(res);
            imshow("only puzzle", onlyPuzzle);

            // Mat vertilines = getVerticalLines(onlyPuzzle);
            // imshow("vertical lines", vertilines);
            //
            // Mat horilines = getHorizontalLines(onlyPuzzle);
            // imshow("horizontal lines", horilines);
            //
            // Mat intersection;
            // bitwise_and(vertilines, horilines, intersection);
            // imshow("line intersections", intersection);
            //
            // Mat warpedPuzzle = getWarpedPuzzle(intersection);
            // imshow("warped puzzle", warpedPuzzle);

            if (waitKey(30) >= 0)
                break;
        }
//-----------------------------------

//   VideoCapture cap(0);
//
//    // check if we succeeded
//    if (!cap.isOpened()) {
//        cout << "oh shit, no webcam" << endl;
//    }
//
//    while (true) {
//        cap >> cam_frame;
//        imshow("Camera view", cam_frame);
//
//        // make copy of cam_frame
//        cam_frame.copyTo(image);
//
//        // greyscale + adaptive thresh +  medianBlur
//        preprocessImage(image);
//
//        // find contours + draw contours
//        getContours(image);
//
//        processContours();
//
//        iteratePuzzle();
//
//        if (waitKey(30) >= 0)
//            break;
//    }
}

Mat SudokuVision::preprocessImage(Mat input) {

    Mat close, div, res;

    GaussianBlur(input, input, Size(5,5), 0);
    cvtColor(input, input, COLOR_BGR2GRAY);

    Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(11,11));
    morphologyEx(input, close, MORPH_CLOSE, kernel1);
    divide(input, close, div, 1 , CV_32F);
    normalize(div,res,0,255,NORM_MINMAX, CV_8U );

    #if DEBUG
        imshow("Ater preprocess", res);
    #endif
    return res;
}

Mat SudokuVision::getSudokuContour(Mat& input) {

    Mat res, thresh;

    // adaptive threshold
    adaptiveThreshold(input, thresh, 255, 0, 1, 19, 2);

    vector<vector<cv::Point> > contours;
    findContours(thresh.clone(), contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // iterate through each contour and find largest index
    int max_area = 0;
    int largest_contour_index = 0;
    for (int i = 0; i < contours.size(); i++) {

        double area = contourArea(contours[i]);
        if (area > 1000) {
            if (area > max_area) {
                max_area = area;
                largest_contour_index = i;
            }
        }
    }

    mask = Mat::zeros(input.size(), CV_8UC1);
    drawContours(mask, contours, largest_contour_index, Scalar(255), -1);
    drawContours(mask, contours, largest_contour_index, Scalar(0), 2);
    bitwise_and(input, mask, res);

    return res;
}

Mat SudokuVision::getVerticalLines(Mat& input) {
    Mat close;

    Mat kernelx = getStructuringElement(MORPH_RECT, Size(2, 10));
    Mat dx;
    Sobel(input, dx, CV_16S, 1, 0);
    convertScaleAbs(dx, dx);
    normalize(dx, dx, 0, 255, NORM_MINMAX);
    threshold(dx, close, 0, 255, THRESH_OTSU | THRESH_BINARY);
    morphologyEx(close, close, MORPH_DILATE, kernelx);

    vector<vector<cv::Point> > contours;
    findContours(close.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) {
        Rect boundingRectContour = boundingRect(contours[i]);

        int h = boundingRectContour.height;
        int w = boundingRectContour.width;
        int x = boundingRectContour.x;
        int y = boundingRectContour.y;

        if (h / w > 5) {
            drawContours(close, contours, i, Scalar(255), -1);
        } else {
            drawContours(close, contours, i, Scalar(0), -1);
        }
    }
    Mat tempKernel; // = getStructuringElement(MORP, Size(1,1));
    morphologyEx(close, close, MORPH_CLOSE, tempKernel, Point(-1, -1), 2);
    return close;
}

Mat SudokuVision::getHorizontalLines(Mat& input) {

    Mat close;
    Mat kernely = getStructuringElement(MORPH_RECT, Size(10,2));
    Mat dy;
    Sobel(input, dy, CV_16S, 0, 2);
    convertScaleAbs(dy, dy);
    normalize(dy, dy, 0, 255, NORM_MINMAX);
    threshold(dy, close, 0, 255, THRESH_OTSU | THRESH_BINARY);
    morphologyEx(close, close, MORPH_DILATE, kernely);

    vector<vector<cv::Point> > contours;
    findContours(close.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) {
        Rect boundingRectContour = boundingRect(contours[i]);

        int h = boundingRectContour.height;
        int w = boundingRectContour.width;
        int x = boundingRectContour.x;
        int y = boundingRectContour.y;

        if (w/h > 5) {
            drawContours(close, contours, i, Scalar(255), -1);
        } else {
            drawContours(close, contours, i, Scalar(0), -1);
        }
    }
    Mat tempKernel; // = getStructuringElement(MORP, Size(1,1));
    morphologyEx(close, close, MORPH_DILATE, tempKernel, Point(-1, -1), 2);
    return close;
}

Mat SudokuVision::getWarpedPuzzle(Mat& input) {

    vector<cv::Point> centroids;
    vector<vector<cv::Point> > contours;
    findContours(input.clone(), contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    Mat beforeSort = cam_frame.clone();

    for (int i = 0; i < contours.size(); i++) {
        vector<cv::Point> contour = contours.at(i);
        Moments mom = moments(contour);
        cv::Point center = cv::Point(int(mom.m10 / mom.m00), int(mom.m01 / mom.m00));
        #if DEBUG
            circle(beforeSort, center, 4, Scalar(0, 255, 0), -1);
            putText(beforeSort, SSTR( i ) , center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0));
        #endif
        centroids.push_back(center);
    }

    #if DEBUG
        imshow("before sorting centroid points", beforeSort);
        Mat afterSort = cam_frame.clone();
    #endif

    std::reverse(centroids.begin(), centroids.end());

    struct {
        bool operator()(const cv::Point p1, const cv::Point p2) {
            return p1.x < p2.x;
        }
    } pointXGreater;

    for (int i = 0; i < 10; i++) {
        std::sort(centroids.begin() + i * 10, centroids.begin() + (i + 1) * 10, pointXGreater);
    }

    for (int i = 0; i < centroids.size(); i++) {
        Point pt = centroids.at(i);
        #if DEBUG
               circle(afterSort, pt, 4, Scalar(0, 255, 0), -1);
               putText(afterSort, SSTR( i ) , pt, FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0));
        #endif
    }

    #if DEBUG
        imshow("after sorting centroid points", afterSort);
    #endif

    Mat bm = Mat(centroids).reshape(2, 10);
    Mat output(450, 450, CV_8UC1);

    for (int i = 0; i < centroids.size(); i++) {
        int ri = i / 10;
        int ci = i % 10;

        if (ci != 9 && ri != 9) {
            cv::Point2f src[4];
            src[0] = cv::Point2f(bm.at<cv::Point>(ri, ci).x, bm.at<cv::Point>(ri, ci).y);
            src[1] = cv::Point2f(bm.at<cv::Point>(ri, ci + 1).x, bm.at<cv::Point>(ri, ci + 1).y);
            src[2] = cv::Point2f(bm.at<cv::Point>(ri + 1, ci).x, bm.at<cv::Point>(ri + 1, ci).y);
            src[3] = cv::Point2f(bm.at<cv::Point>(ri + 1, ci + 1).x, bm.at<cv::Point>(ri + 1, ci + 1).y);

            cv::Point2f dst[4];
            dst[0] = cv::Point2f(0, 0);
            dst[1] = cv::Point2f(50, 0);
            dst[2] = cv::Point2f(0,50);
            dst[3] = cv::Point2f(50,50);

            Mat retval = getPerspectiveTransform(src, dst);
            Mat warp;

            warpPerspective(res, warp, retval, cv::Size(50, 50));
            warp.copyTo(output(Rect(ci * 50, ri * 50, warp.cols, warp.rows)));
        }
    }
    return output;
}

//vector< vector<int> > SudokuVision::getData() {
//
//    int inputPuzzle[9][9] = {
//        {3, 0, 5, 4, 0, 0, 8, 0, 0},
//        {0, 0, 0, 7, 0, 3, 0, 6, 0},
//        {0, 0, 2, 0, 1, 0, 0, 0, 9},
//
//        {0, 0, 0, 3, 9, 0, 0, 0, 0},
//        {8, 4, 0, 0, 0, 0, 0, 7, 5},
//        {0, 0, 0, 0, 7, 5, 0, 0, 0},
//
//        {5, 0, 0, 0, 3, 0, 4, 0, 0},
//        {0, 3, 0, 5, 0, 1, 0, 0, 0},
//        {0, 0, 1, 0, 0, 7, 3, 0, 2},
//    };
//
//    vector< vector<int> > myVec;
//    for (int i = 0; i < 9; i++)
//    {
//        vector<int> temp;
//        for (int j = 0; j < 9; j++)
//        {
//            temp.push_back(inputPuzzle[i][j]);
//        }
//        myVec.push_back(temp);
//    }
//    return myVec;
//}
