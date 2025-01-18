#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

float A = 0, B = 0, C = 0;

float cubeWidth = 20;
int width = 160, height = 44;
float distanceFromCube = 80;
float K1 = 40;
float incrementSpeed = 1;

float calculateX(int i, int j, int k) {
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
           j * cos(A) * sin(C) + k * sin(A) * sin(C) +
           i * cos(B) * cos(C);
}

float calculateY(int i, int j, int k) {
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
           j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
           i * cos(B) * sin(C);
}

float calculateZ(int i, int j, int k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch, float* zBuffer, char* buffer, int width, int height) {
    float x = calculateX(cubeX, cubeY, cubeZ);
    float y = calculateY(cubeX, cubeY, cubeZ);
    float z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCube;

    float ooz = 1 / z;

    int xp = (int)(width / 1.5 + K1 * ooz * x * 2);
    int yp = (int)(height / 2 + K1 * ooz * y);

    int idx = xp + yp * width;
    if (idx >= 0 && idx < width * height) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main() {
    printf("\x1b[2J");

    float zBuffer[160 * 44];
    char buffer[160 * 44];
    int backgroundASCIICode = ' ';

    while (1) {
        memset(buffer, backgroundASCIICode, width * height);
        memset(zBuffer, 0, width * height * sizeof(float));

        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
                calculateForSurface(cubeX, cubeY, -cubeWidth, '.', zBuffer, buffer, width, height);
                calculateForSurface(cubeWidth, cubeY, cubeX, '$', zBuffer, buffer, width, height);
                calculateForSurface(-cubeWidth, cubeY, -cubeX, '~', zBuffer, buffer, width, height);
                calculateForSurface(-cubeX, cubeY, cubeWidth, '#', zBuffer, buffer, width, height);
                calculateForSurface(cubeX, -cubeWidth, -cubeY, ';', zBuffer, buffer, width, height);
                calculateForSurface(cubeX, cubeWidth, cubeY, '-', zBuffer, buffer, width, height);
            }
        }

        printf("\x1b[H");
        for (int k = 0; k < width * height; k++) {
            putchar(k % width ? buffer[k] : 10);
        }

        A += 0.005;
        B += 0.005;
        usleep(1000);
    }

    return 0;
}

