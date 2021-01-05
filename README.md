# kinect v2制作surfelwarp数据集

本人使用ubuntu18.04，kinect v2，libfreenect2。

[TOC]



### 1.环境配置

#### libfreenect2

新建kinect v2文件夹，并在该文件夹下运行：

- 下载 libfreenect2 源码
  `git clone https://github.com/OpenKinect/libfreenect2.git`
  `cd libfreenect2`

- 下载upgrade deb 文件
  `cd depends; ./download_debs_trusty.sh`

- 安装编译工具
  `sudo apt-get install build-essential cmake pkg-config`

- 安装 libusb. 版本需求 >= 1.0.20.
  `sudo dpkg -i debs/libusb*deb`

- 安装 TurboJPEG
  `sudo apt-get install libturbojpeg libjpeg-turbo8-dev`

- 安装 OpenGL
  `sudo dpkg -i debs/libglfw3*deb`
  `sudo apt-get install -f`
  `sudo apt-get install libgl1-mesa-dri-lts-vivid`

- 还有一些可选项，如：OPCL、CUDA等，可参见上述参考链接。

- Build
  `cd ..`
  `mkdir build && mkdir install`

  `cd build`

  `cmake .. -DCMAKE_INSTALL_PREFIX=../install -DENABLE_CXX11=ON`
  `make`
  `sudo make install`(此处我安装在libfreenect2文件夹下自己新建的install文件夹)

- 设定udev rules:
  `sudo cp ../platform/linux/udev/90-kinect2.rules /etc/udev/rules.d/`
  然后重新插拔Kinect2.

- 尝试运行Demo程序:
  `./bin/Protonect`， 不出意外， 应该能够看到图像。



### 2.保存图像

在kinect文件夹下运行：

`git clone https://github.com/Wangyouai/kinect-v2-make-dataset-for-surfelwarp.git`

修改cmakelist中的：

```
set(freenect2_DIR "/home/wya/project/kinect/libfreenect2/install/lib/cmake/freenect2")
```

和main.cpp中

```c++
string save_path = "/home/wya/project/kinect/Dataset";  //根据自己需要修改
```

将其改为自己的地址

其中数据集所在文件夹需要新建depth和rgb两个文件夹

![](https://gitee.com/wang-youai/image/raw/master/image/2021-01-04%2022-44-22%20%E7%9A%84%E5%B1%8F%E5%B9%95%E6%88%AA%E5%9B%BE.png)

使用clion打开工程，cmake后运行即可。

### 3.注意事项：

1. 输出的RGB图为24位，depeh图为16位。
2. 则在此处需要修改输出图像的数据格式

> ```c++
> rgbmat.convertTo(rgbmat,CV_8UC3,1,0);
> depthmat.convertTo(depthmat,CV_16UC1,1,0);
> ```

### 4.出现问题及解决方法

- **初始化图像接受数组错误**

  通过查资料和看代码得知：kinect v2的图像数据流中，RGB、IR、Depth的像素格式都是32位：`RGB---CV_8UC4`、`IR、Depth---CV_32FC1`，之前我在初始化接收矩阵时一直是按RGB---CV_8UC3、`IR、Depth---CV_8UC1`

- **图像保存的大小格式错误**

  kinect v2输出的RGB图大小为1920x1080，而Depth为512x424;使用cv.resize时应统一为较小的大小，比如512x424。（此时RGB会产生畸变，故需要将RGB图像映射到Depth中）

### 5.TODO

- [ ] rgb2depth图像未完成，surfelwarp重构效果差
- [ ] 进行实时重构
