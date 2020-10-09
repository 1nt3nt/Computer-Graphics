#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

typedef unsigned char pixel;
using namespace std;

void turn_image(pixel **image, int n) {
    // image: a 2D array of bytes.
    // To access the pixel at row y, column x, use image[y][x].

    pixel** temp;
    temp = new pixel*[n];
    int shifter = n - 1;
    for (int y = 0; y < n; y++)
    {
        temp[y] = new pixel[n];
        for (int x = 0; x < n; x++)
        {
            temp[y][x] = image[x][shifter - y];
        }
    }

    for (int y = 0; y < n; y++)
    {
        for (int x = 0; x < n; x++)
        {
            image[y][x] = temp[y][x];
        }
    }
}

void print_image(pixel **image, int n) {
    for (int y=0; y < n; y++) {
        for (int x=0; x < n; x++)
            cout << image[y][x] << " ";
        cout << "\n";
    }
}

int main(int argc, char *argv[]) {
    // MAIN PROGRAM: Do not change.
    pixel **image;
    int n = 7;
    image = new pixel*[n];
    for (int y=0; y < n; y++) {
        image[y] = new pixel[n];
        for (int x=0; x < n; x++)
            image[y][x] = '.';
    }
    image[0][0] = image[0][6] = '*';
    image[1][1] = image[1][6] = '*';
    image[2][1] = image[2][6] = '*';
    image[3][1] = image[3][6] = '*';
    image[4][1] = image[4][5] = '*';
    image[5][2] = image[5][4] = '*';
    image[6][3] = '*';

    cout << "BEFORE ROTATION:\n";
    print_image(image, n);

    turn_image(image, n);

    cout << "AFTER ROTATION:\n";
    print_image(image, n);
}


