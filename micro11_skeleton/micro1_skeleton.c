#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


#include <math.h>
#include <stdio.h>
#include <float.h>


void mirror_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void grayScale_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void sobelFiltering_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);

int main()
{

	int height;
	int width;
	int channel;

	char command;

	printf("Take a picture? (y/n)\n");
	scanf("%c", &command);

	if (command == 'n')
		exit(1);


	printf("Cheeze !\r\n");
	system("raspistill -w 640 -h 480 -t 10 -o image.bmp");


	unsigned char* imgIn = stbi_load("image.bmp", &width, &height, &channel, 3);


	unsigned char* imgOut_mirror = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);
	unsigned char* imgOut_grayScale = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);
	unsigned char* imgOut_sobelFiltering = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);

	mirror_transform(imgIn, height, width, channel, imgOut_mirror);
	grayScale_transform(imgIn, height, width, channel, imgOut_grayScale);
	sobelFiltering_transform(imgOut_grayScale, height, width, channel, imgOut_sobelFiltering);


	stbi_write_bmp("image_mirror.bmp", width, height, channel, imgOut_mirror);
	stbi_write_bmp("image_grayScale.bmp", width, height, channel, imgOut_grayScale);
	stbi_write_bmp("image_sobelFiltering.bmp", width, height, channel, imgOut_sobelFiltering);

	stbi_image_free(imgIn);
	free(imgOut_mirror);
	free(imgOut_grayScale);
	free(imgOut_sobelFiltering);


	return 0;
}

void mirror_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	for (int n = 0; n < 3; n++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				//�ȼ� �� �ݴ�� ����
				out[i * width * channel + (width - 1 - j) * channel + n] = in[i * width * channel + j * channel + n];
			}
		}
	}
}

void grayScale_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	for (int n = 0; n < 3; n++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				// RGB ��� �� ���
				unsigned char avg = (in[i * width * channel + j * channel + 0] + in[i * width * channel + j * channel + 1] + in[i * width * channel + j * channel + 2]) / 3;
				// out�� ��� �ȼ� �� ����
				out[i * width * channel + j * channel + n] = avg;
			}
		}
	}
}

void sobelFiltering_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {

	//x, y ���� Sobel Filter ���� ����
	int x_filter[3][3] = { {-1, 0 ,1},{-2, 0,2},{-1,0,1} };
	int y_filter[3][3] = { {1, 2 ,1},{0, 0,0},{-1,-2,-1} };

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			//x, y ���� convolution ��
			int x_conv = 0, y_conv = 0;
			// ������ ��
			int conv_sum = 0;

			//convolution ����
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					x_conv += (in[(i + y - 1) * width * channel + (j + x - 1) * channel + 0]) * x_filter[x][y];
					y_conv += (in[(i + y - 1) * width * channel + (j + x - 1) * channel + 0]) * y_filter[x][y];
				}
				//convolution sum ���
				conv_sum = abs(x_conv) + abs(y_conv);

				//��� �̹��� �迭�� ����
				for (int k = 0; k < 3; k++)
				{
					out[i * width * channel + j * channel + k] = conv_sum;
				}
			}
		}
	}
}