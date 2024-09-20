//header file of decoding message
#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret message
 */
typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Secret File Info */
    char secret_fname[50];
    FILE *fptr_secret;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;

/* decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);


/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p files */
Status open_file(DecodeInfo *decInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(int len,DecodeInfo *decInfo);

/* decode function, which does the real decoding */
Status decode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* decode a lsb to byte of image data array */
Status decode_lsb_to_byte(char *data, char *image_buffer);

/* decode a LSB to int of length magic string array */
Status decode_lsb_to_int(char *buff);

#endif