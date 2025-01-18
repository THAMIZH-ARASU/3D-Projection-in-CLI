#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

float A = 0, B = 0;

int width = 160, height = 44;
float R1 = 1;  // Radius of the torus tube
float R2 = 2;  // Radius from the center of the torus to the tube center
float K2 = 9;  // Distance of the viewer from the screen
float K1 = 45;  // Scale factor for projection

void calculateForSurface(float theta, float phi, float* zBuffer, char* buffer) {
    // Precompute sines and cosines for performance
    float cosTheta = cos(theta), sinTheta = sin(theta);
    float cosPhi = cos(phi), sinPhi = sin(phi);
    float cosA = cos(A), sinA = sin(A);
    float cosB = cos(B), sinB = sin(B);

    // Parametric equations for the torus
    float circleX = R2 + R1 * cosTheta;
    float circleY = R1 * sinTheta;

    // 3D rotation transformations
    float x = circleX * (cosB * cosPhi + sinA * sinB * sinPhi) - circleY * cosA * sinB;
    float y = circleX * (sinB * cosPhi - sinA * cosB * sinPhi) + circleY * cosA * cosB;
    float z = K2 + cosA * circleX * sinPhi + circleY * sinA;
    float ooz = 1 / z;  // "One over z" for perspective projection

    // Project 3D coordinates onto 2D screen
    int xp = (int)(width / 2 + K1 * ooz * x);
    int yp = (int)(height / 2 - K1 * ooz * y);

    // Calculate luminance
    float L = cosPhi * cosTheta * sinB - cosA * cosTheta * sinPhi -
              sinA * sinTheta + cosB * (cosA * sinTheta - cosTheta * sinA * sinPhi);

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

        for (float theta = 0; theta < 2 * M_PI; theta += 0.07) {
            for (float phi = 0; phi < 2 * M_PI; phi += 0.02) {
                calculateForSurface(theta, phi, zBuffer, buffer);
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

