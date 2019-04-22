# Synapse-counting  
> Use YOLO_v3 (darknet) to detect Asymmetric Synapse and Symmetric Synapse (Windows MSVS2015 and OpenCV4.1) on CPU

## Synapse dataset  
We use the Annotation tool **[Yolo_mark](https://github.com/lcylmhlcy/Yolo_mark)** to label the synapse images.  
One txt for one image.  
More details, please refer to **[Yolo_mark](https://github.com/lcylmhlcy/Yolo_mark)**

Please download the dataset by (**[BaiduYun](https://pan.baidu.com/s/1I0lbDRqd73iX1EL5W2yCRA) passwork: 0i02**)

## Requirements
- Windows
- MSVS2015
- Qt 5.9 for msvc2015_64
- OpenCV 4.1

## How to use
1. Use MSVS2015 to run `mollecullar.sln`.
2. Set `Release && x64`. Then, configure the project with OpenCV4.1 and Qt5.9.
3. Download (**[the darknet weight](https://pan.baidu.com/s/1Gy9fsNqFrZ95ZGAyOIMQoA) passwork: n7il**), and put it near cfg file.
3. Click `run`.

## Interface

1. Initial interface
![img](https://github.com/lcylmhlcy/Synapse-counting/raw/master/img/1.png)

2. Detection interface
![img](https://github.com/lcylmhlcy/Synapse-counting/raw/master/img/2.png)

