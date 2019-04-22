#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QCloseEvent>
#include <QMessageBox>
#include "opencv2/opencv.hpp"
#include "ui_mainwindow.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

struct NUM_MOLLECULLAR {
	int NUM_A = 0;
	int NUM_S = 0;
};


class mainwindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit mainwindow(QWidget *parent = 0); //Q_NULLPTR
	~mainwindow();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void on_inputPushButton_pressed();
	void on_detectPushButton_pressed();
	void on_saveresultPushButton_pressed();

private:
	Ui::mainwindowClass *ui;

	Mat input_img;
	Mat ouput_img;
	Net net;
	NUM_MOLLECULLAR num_mollecullar;

	const float confThreshold = 0.5; // Confidence threshold
	const float nmsThreshold = 0.4;  // Non-maximum suppression threshold
	const int inpWidth = 416;  // Width of network's input image
	const int inpHeight = 416; // Height of network's input image
	vector<string> classes;

	void postprocess(Mat& frame, const vector<Mat>& out);
	vector<String> getOutputsNames(const Net& net);
};
