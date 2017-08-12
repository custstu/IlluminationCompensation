/*@author:Tom Liao*/
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;
typedef vector<unsigned char> vecDataBuffer;
/*function£º
compensate the illumination of an image
input£º
*[in]const vecDataBuffer & vecGrayImg - a gray image
*[in]int nGrayImgWidth - the width of image
*[in]int nGrayImgHeight - the height of image
*[in]int nKernelSize - filter kernel size
*[in]double nMdark - control parameter 1
*[in]double nMbright - control parameter 2
*[in]double nLobe - control parameter 3
output£º
*[out]vecDataBuffer &vecImprovedGrayImg - an illumination-compensated image
return£º
*0 ¨C valid, -1 - invalid
reference£º
¡°Fast centre¨Csurround contrast modification¡±£¬V.Vonikakis£¬I.Andreadis£¬A.Gasteratos.
*/
int illuminationCompensation(const vecDataBuffer &vecGrayImg, vecDataBuffer &vecImprovedGrayImg, int nGrayImgWidth, int nGrayImgHeight, int nKernelSize, double nMdark, double nMbright, double nLobe)
{
	if (nKernelSize % 2 == 0)
	{
		return -1;
	}
	unsigned char *pGrayImg = (unsigned char *)(&vecGrayImg[0]);
	vecImprovedGrayImg.clear();
	vecImprovedGrayImg.resize(nGrayImgHeight * nGrayImgWidth);
	unsigned char *pImprovedGrayImg = (unsigned char *)(&vecImprovedGrayImg[0]);
	Mat img(nGrayImgHeight, nGrayImgWidth, CV_8UC1, pGrayImg);
	Mat imgBlurred;
	double A, temp;

	blur(img, imgBlurred, Size(nKernelSize, nKernelSize));
	int length = nGrayImgWidth * nGrayImgHeight;
	for (int i = 0; i < length; i++)
	{
		if (imgBlurred.data[i] < 128)
		{
			A = (nMdark + imgBlurred.data[i] * imgBlurred.data[i] / nLobe) * (128 / (128 - imgBlurred.data[i] + 1e-10));
			temp = (255 + A) * img.data[i] / (A + img.data[i] + 1e-10);
			pImprovedGrayImg[i] = (unsigned char)((temp >= 255) ? 255 : temp);
		}
		else
		{
			A = (nMbright + (255 - imgBlurred.data[i]) * (255 - imgBlurred.data[i]) / nLobe) * (128 / (imgBlurred.data[i] - 127 + 1e-10));
			temp = A * img.data[i] / (A + 255 - img.data[i] + 1e-10);
			pImprovedGrayImg[i] = (unsigned char)((temp >= 255) ? 255 : temp);
		}

	}
	return 0;
}

//test illuminationCompensation
int main()
{
	Mat srcImg = imread("test.jpg", 0);
	namedWindow("in");
	imshow("in", srcImg);
	waitKey(0);
	vecDataBuffer srcData;
	Mat dstImg(srcImg.rows, srcImg.cols, CV_8UC1);
	int ret = illuminationCompensation(std::vector<unsigned char>(srcImg.data, srcImg.data + srcImg.cols * srcImg.rows),
		srcData, srcImg.cols, srcImg.rows, 119, 35, 35, 100);
	memcpy(dstImg.data, &srcData[0], srcImg.cols * srcImg.rows);
	namedWindow("out");
	imshow("out", dstImg);
	waitKey(0);
	destroyAllWindows();
	return 0;
}