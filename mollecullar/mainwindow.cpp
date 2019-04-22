#include "mainwindow.h"

mainwindow::mainwindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::mainwindowClass)
{
	ui->setupUi(this);

	string classesFile = "obj.names";
	ifstream ifs(classesFile.c_str());
	string line;
	while (getline(ifs, line)) classes.push_back(line);

	string modelConfiguration = "yolov3-obj.cfg";
	string modelWeights = "yolov3-obj_final.weights";
	net = readNetFromDarknet(modelConfiguration, modelWeights);
	net.setPreferableBackend(DNN_BACKEND_OPENCV);
	net.setPreferableTarget(DNN_TARGET_CPU);

	ui->statusBar->showMessage("Finish loading YOLO_v3 network !", 10000);
}

mainwindow::~mainwindow()
{
	delete ui;
}

void mainwindow::on_inputPushButton_pressed()
{
	ui->label_2->clear();
	ui->label_3->clear();

	QString fileName = QFileDialog::getOpenFileName(this, "Open Input Image", QDir::currentPath(), "Images (*.jpg *.png *.bmp)");
	if (QFile::exists(fileName))
	{
		ui->textEdit->setText(fileName);
		input_img = imread(fileName.toStdString());
		QImage qImage = QImage((const unsigned char*)(input_img.data), input_img.cols, input_img.rows, input_img.step, QImage::Format_RGB888);
		ui->label_2->setPixmap(QPixmap::fromImage(qImage.scaled(ui->label_2->rect().width(), ui->label_2->rect().height())));
	}
}

void mainwindow::on_detectPushButton_pressed()
{
	Mat blob;
	ouput_img = input_img.clone();
	blobFromImage(ouput_img, blob, 1 / 255.0, Size(inpWidth, inpHeight), Scalar(0, 0, 0), true, false);
	net.setInput(blob);

	vector<Mat> outs;
	net.forward(outs, getOutputsNames(net));

	postprocess(ouput_img, outs);

	QImage qImage1 = QImage((const unsigned char*)(ouput_img.data), ouput_img.cols, ouput_img.rows, ouput_img.step, QImage::Format_RGB888);
	ui->label_3->setPixmap(QPixmap::fromImage(qImage1.scaled(ui->label_3->rect().width(), ui->label_3->rect().height())));

	string num_result = "Num of Asymmetric: " + to_string(num_mollecullar.NUM_A) + "  ||  Num of Symmetric: " + to_string(num_mollecullar.NUM_S);
	ui->statusBar->showMessage(QString::fromStdString(num_result), 10000);

}

void mainwindow::on_saveresultPushButton_pressed()
{
	QString output_fileName = QFileDialog::getSaveFileName(this, "Select Output Image", QDir::currentPath(), "*.jpg;;*.png;;*.bmp");
	imwrite(output_fileName.toStdString(), ouput_img);
}

void mainwindow::closeEvent(QCloseEvent *event)
{
	int result = QMessageBox::warning(this, "Exit", "Are you sure you want to close this program?", QMessageBox::Yes, QMessageBox::No);
	if (result == QMessageBox::Yes)
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}


// Remove the bounding boxes with low confidence using non-maxima suppression
void mainwindow::postprocess(Mat& frame, const vector<Mat>& outs)
{
	vector<int> classIds;
	vector<float> confidences;
	vector<Rect> boxes;

	for (size_t i = 0; i < outs.size(); ++i)
	{
		// Scan through all the bounding boxes output from the network and keep only the
		// ones with high confidence scores. Assign the box's class label as the class
		// with the highest score for the box.
		float* data = (float*)outs[i].data;
		for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
		{
			Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
			Point classIdPoint;
			double confidence;
			// Get the value and location of the maximum score
			minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
			if (confidence > confThreshold)
			{
				int centerX = (int)(data[0] * frame.cols);
				int centerY = (int)(data[1] * frame.rows);
				int width = (int)(data[2] * frame.cols);
				int height = (int)(data[3] * frame.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				classIds.push_back(classIdPoint.x);
				confidences.push_back((float)confidence);
				boxes.push_back(Rect(left, top, width, height));
			}
		}
	}

	// Perform non maximum suppression to eliminate redundant overlapping boxes with
	// lower confidences
	vector<int> indices;
	NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

	num_mollecullar.NUM_A = 0;
	num_mollecullar.NUM_S = 0;
	for (size_t i = 0; i < indices.size(); ++i)
	{
		int idx = indices[i];
		Rect box = boxes[idx];
		if (classIds[idx] == 0) {
			rectangle(frame, Point(box.x, box.y), Point(box.x + box.width, box.y + box.height), Scalar(255, 0, 0), 10);
			num_mollecullar.NUM_A += 1;
		}
		else {
			rectangle(frame, Point(box.x, box.y), Point(box.x + box.width, box.y + box.height), Scalar(0, 0, 255), 10);
			num_mollecullar.NUM_S += 1;
		}
	}
}

// Get the names of the output layers
vector<String> mainwindow::getOutputsNames(const Net& net)
{
	static vector<String> names;
	if (names.empty())
	{
		//Get the indices of the output layers, i.e. the layers with unconnected outputs
		vector<int> outLayers = net.getUnconnectedOutLayers();

		//get the names of all the layers in the network
		vector<String> layersNames = net.getLayerNames();

		// Get the names of the output layers in names
		names.resize(outLayers.size());
		for (size_t i = 0; i < outLayers.size(); ++i)
			names[i] = layersNames[outLayers[i] - 1];
	}
	return names;
}