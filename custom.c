#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h> // For sleep()

#define WIDTH 80
#define HEIGHT 24
#define K2 5
#define K1 20

// Rotation angles
float A = 0, B = 0;

// Define the custom shape points (e.g., a star)
float points[][3] = {
    {0, 1, 0}, {0.5, 0.5, 0}, {1, 0, 0}, {0.5, -0.5, 0}, {0, -1, 0},
    {-0.5, -0.5, 0}, {-1, 0, 0}, {-0.5, 0.5, 0}, {0, 1, 0}  // Closed loop
};
int num_points = 9;

// Function to rotate and project the shape
void render_frame() {
    char screen[HEIGHT][WIDTH];
    float zbuffer[HEIGHT][WIDTH];

    // Clear screen and z-buffer
    memset(screen, ' ', sizeof(screen));
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            zbuffer[i][j] = 0;
        }
    }

    for (int i = 0; i < num_points; i++) {
        float px = points[i][0];
        float py = points[i][1];
        float pz = points[i][2];

        // Rotate around X-axis
        float x1 = px;
        float y1 = py * cos(A) - pz * sin(A);
        float z1 = py * sin(A) + pz * cos(A);

        // Rotate around Z-axis
        float x2 = x1 * cos(B) - y1 * sin(B);
        float y2 = x1 * sin(B) + y1 * cos(B);
        float z2 = z1;

        // Project the 3D points to 2D
        z2 += K2;
        float ooz = 1 / z2;
        int xp = (int)(WIDTH / 2 + K1 * ooz * x2);
        int yp = (int)(HEIGHT / 2 - K1 * ooz * y2);

        // Update the screen and z-buffer
        if (xp >= 0 && xp < WIDTH && yp >= 0 && yp < HEIGHT) {
            if (ooz > zbuffer[yp][xp]) {
                zbuffer[yp][xp] = ooz;
                screen[yp][xp] = '*';
            }
        }
    }

    // Clear the terminal
    printf("\033[H");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            putchar(screen[i][j]);
        }
        putchar('\n');
    }
}

// Main function to animate the shape
int main() {
    while (1) {
        render_frame();
        A += 0.04; // Rotate around X-axis
        B += 0.03; // Rotate around Z-axis
        usleep(30000); // Delay for animation (30ms)
    }
    return 0;
}

