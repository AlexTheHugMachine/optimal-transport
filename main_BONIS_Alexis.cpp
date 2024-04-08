#define _CRT_SECURE_NO_WARNINGS 1
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <random>
#include <algorithm> 

int main() {
	std::random_device rd{};
	std::mt19937 gen{ rd() };
	
	int W, H, C, W2, H2, C2, W3, H3, C3;
	
	unsigned char *image = stbi_load("imgA.jpg",
                                 &W,
                                 &H,
                                 &C,
                                 STBI_rgb);
								
	unsigned char *image2 = stbi_load("redim.jpg",
								 &W2,
								 &H2,
								 &C2,
								 STBI_rgb);

	unsigned char *image_result = stbi_load("imgA.jpg",
								 &W3,
								 &H3,
								 &C3,
								 STBI_rgb);
	
	std::vector<double> image_double(W*H*3);
	for (int i=0; i<W*H*3; i++)
		image_double[i] = image[i];

	std::vector<double> image2_double(W2*H2*3);
	for (int i=0; i<W2*H2*3; i++)
		image2_double[i] = image2[i];

	for (size_t i = 0; i < 100; i++)
	{
		// Random direction
		std::normal_distribution<double> distribution(0.0, 1.0);
		std::vector<double> direction(3);

		direction[0] = distribution(gen);
		direction[1] = distribution(gen);
		direction[2] = distribution(gen);

		// Normalize the direction
		double normale = sqrt(direction[0]*direction[0] + direction[1]*direction[1] + direction[2]*direction[2]);
		direction[0] /= normale;
		direction[1] /= normale;
		direction[2] /= normale;

		// Compute the projection of the image on the direction
		std::vector<double> projection(W*H);
		for (int i = 0; i < W*H; i++) {
			projection[i] = image_double[i*3 + 0] * direction[0] + image_double[i*3 + 1] * direction[1] + image_double[i*3 + 2] * direction[2];
		}

		std::vector<double> projection2(W2*H2);
		for (int i = 0; i < W2*H2; i++) {
			projection2[i] = image2_double[i*3 + 0] * direction[0] + image2_double[i*3 + 1] * direction[1] + image2_double[i*3 + 2] * direction[2];
		}

		// Sort the projection
		std::pair<double, int> *projection_sorted = new std::pair<double, int>[W*H];
		for (int i = 0; i < W*H; i++) {
			projection_sorted[i].first = projection[i];
			projection_sorted[i].second = i;
		}

		std::pair<double, int> *projection2_sorted = new std::pair<double, int>[W2*H2];
		for (int i = 0; i < W2*H2; i++) {
			projection2_sorted[i].first = projection2[i];
			projection2_sorted[i].second = i;
		}

		std::sort(projection_sorted, projection_sorted + W*H, [](const std::pair<double, int>& a, const std::pair<double, int>& b) { return a.first < b.first; });
		std::sort(projection2_sorted, projection2_sorted + W2*H2, [](const std::pair<double, int>& a, const std::pair<double, int>& b) { return a.first < b.first; });

		for(int i = 0; i < W*H; i++)
		{
			image_double[projection_sorted[i].second*3 + 0] += (projection2_sorted[i].first - projection_sorted[i].first) * direction[0];
			image_double[projection_sorted[i].second*3 + 1] += (projection2_sorted[i].first - projection_sorted[i].first) * direction[1];
			image_double[projection_sorted[i].second*3 + 2] += (projection2_sorted[i].first - projection_sorted[i].first) * direction[2];
		}
	}

	for(int i=0; i<W*H*3; i++)
	{
		if (image_double[i] > 255)
			image_double[i] = 255;
		if (image_double[i] < 0)
			image_double[i] = 0;
	}

	for (int i = 0; i < W*H*3; i++) {
		image_result[i] = (unsigned char)image_double[i];
	}

	stbi_write_png("image.png", W, H, 3, &image_result[0], 0);

	return 0;
}