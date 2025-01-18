#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

float A = 0, B = 0;

int width = 160, height = 44;
float cylinderHeight = 2;  // Height of the cylinder
float cylinderRadius = 1;  // Radius of the cylinder
float K2 = 7;  // Distance of the viewer from the screen
float K1 = 100;  // Scale factor for projection

void calculateForSurface(float theta, float heightFraction, float* zBuffer, char* buffer) {
    // Precompute sines and cosines for performance
    float cosTheta = cos(theta), sinTheta = sin(theta);
    float cosA = cos(A), sinA = sin(A);
    float cosB = cos(B), sinB = sin(B);

    // Parametric equations for the cylinder
    float x = cylinderRadius * cosTheta;
    float y = cylinderHeight * (heightFraction - 0.5);  // Shift center vertically
    float z = cylinderRadius * sinTheta;

    // 3D rotation transformations
    float xRot = cosB * x + sinB * (sinA * y + cosA * z);
    float yRot = cosA * y - sinA * z;
    float zRot = K2 + cosB * (sinA * y + cosA * z) - sinB * x;
    float ooz = 1 / zRot;  // "One over z" for perspective projection

    // Project 3D coordinates onto 2D screen
    int xp = (int)(width / 1.5 + K1 * ooz * xRot);
    int yp = (int)(height / 2 - K1 * ooz * yRot);

    // Calculate luminance
    float L = cosTheta * sinB - sinA * sinTheta + cosB * (cosA * sinTheta - cosTheta * sinA);

    // Map luminance to ASCII characters
    int luminanceIndex = (int)(8 * L);
    char luminanceChars[] = ".,-~:;=!*#$@";

    // Update the z-buffer and character buffer
    int idx = xp + yp * width;
    if (idx >= 0 && idx < width * height && ooz > zBuffer[idx]) {
        zBuffer[idx] = ooz;
        buffer[idx] = luminanceChars[luminanceIndex > 0 ? luminanceIndex : 0];
    }
}

int main() {
    printf("\x1b[2J");  // Clear the screen

    float zBuffer[160 * 44];
    char buffer[160 * 44];
    int backgroundASCIICode = ' ';

    while (1) {
        memset(buffer, backgroundASCIICode, width * height);
        memset(zBuffer, 0, width * height * sizeof(float));

        for (float theta = 0; theta < 2 * M_PI; theta += 0.05) {
            for (float heightFraction = 0; heightFraction <= 1; heightFraction += 0.02) {
                calculateForSurface(theta, heightFraction, zBuffer, buffer);
            }
        }

        printf("\x1b[H");  // Reset cursor to the top-left
        for (int k = 0; k < width * height; k++) {
            putchar(k % width ? buffer[k] : '\n');
        }

        A += 0.04;
        B += 0.02;
        usleep(30000);
    }

    return 0;
}

