/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy n infile outfile\n");
        return 1;
    }

    // remember filenames and enlarge value
    int n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
    
    // checking if n is correct
    if (n <= 0 || n > 100)
    {
        printf("n must be from 1 to 100\n");
        return 1;
    }

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // saving input file height and width
    int inputHeight = abs(bi.biHeight);
    int inputWidth = bi.biWidth;

    // changing BITMABINFOHEADER width and height info
    bi.biWidth *= n;
    bi.biHeight *= n;

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // changing BITMABINFOHEADER image size info
    bi.biSizeImage = bi.biWidth * sizeof(RGBTRIPLE) * abs(bi.biHeight) + abs(bi.biHeight) * padding;

    // changing BITMAPFILEHEADER file size info
    bf.bfSize = bi.biSizeImage + 54;

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    // array for storing pixels line
    RGBTRIPLE lineArray[bi.biWidth];
    
    // iterate over infile's scanlines
    for (int i = 0; i < inputHeight; i++)
    {
        // lineArray index
        int lineArrayIndex = 0;

        // iterate over pixels in scanline
        for (int j = 0; j < inputWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple into storing array n times to enlarge width
            for (int k = 0; k < n; k++)
            {
                lineArray[lineArrayIndex] = triple;
                lineArrayIndex++;
            }
        }

        // placing stored line to a file n times to enlarge height
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < bi.biWidth; k++)
            {
                fwrite(&lineArray[k], sizeof(RGBTRIPLE), 1, outptr);    
            }
            
            // then add it back (to demonstrate how)
            for (int k = 0; k < padding; k++)
            {
                fputc(0x00, outptr);
            }
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
