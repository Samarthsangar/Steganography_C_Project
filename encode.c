/* Header files include */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "Colour.h"

/* Function Definitions */
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)                //called function to check height & width of src bmp file
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf(MAG"width = %u\n"reset, width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf(MAG"height = %u\n"reset, height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)                     //function call to open file
{
     int len1;
     //open the src file in read mode
     encInfo->fptr_src_image = fopen(encInfo->src_image_fname,"r");
     if(encInfo->fptr_src_image == NULL)                  //chcek condition & printing error
     {
	     printf(BRED"INFO : Source image file is not opened\n"reset);
	     return e_failure;
     }

     //open the secret file in read mode
     encInfo->fptr_secret = fopen(encInfo->secret_fname,"r");
     if(encInfo->fptr_secret == NULL)                     //chcek condition & printing error
     {
	     printf(BRED"INFO : Secret message file is not opened\n"reset);
	     return e_failure;
     }
     else
     {    //chceking secret file is empty or not
          fseek(encInfo->fptr_secret,0,SEEK_END);
          if((len1=ftell(encInfo->fptr_secret))==0)
          {
               printf(BRED"INFO: Secret file is empty !\n"reset);
               return e_failure;
          }          

          //check size of src file > secret file with some condition's
          uint len=get_image_size_for_bmp(encInfo->fptr_src_image);
          if(len<((4+strlen(MAGIC_STRING)+4+4+4+len1)*8))
          {
               printf(BRED"ERROR: Please keep Src_file size greater\n"reset);
               return e_failure;
          }
          //rewind both file's
          rewind(encInfo->fptr_secret);
          rewind(encInfo->fptr_src_image);

     }
     
     //open Output bmp file in write mode to store secret message
     encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname,"w");
     if(encInfo->fptr_stego_image == NULL)                             //chcek condition & printing error
     {
	     printf(BRED"INFO : Output file is not opened\n"reset);
	     return e_failure;
     }
     return e_success;
}

Status do_encoding(EncodeInfo *encInfo)                    //called function for encoding
{
     //function call to open file's
     if(open_files(encInfo)==e_success)
          printf(MAG"INFO: File's opened successfully\n"reset);
     else
     {
          printf(BRED"INFO: Unable to open\n"reset);
          return e_failure;
     }
     printf(GRN"INFO: ## Encoding started\n"reset);

     //function call to copy header of src file
     copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image);
     printf(MAG"INFO: Header copied sucessfully\n"reset);

     //function call to encode magic string
     encode_magic_string(MAGIC_STRING,encInfo);
     printf(MAG"INFO: magic_string Encoded sucessfully\n"reset);

     //function call to encode secret file extension
     encode_secret_file_extn(strchr(encInfo->secret_fname,'.'),encInfo);
     printf(MAG"INFO: Secret file extension Encoded sucessfully\n"reset);

     //seek length byte of secret file
     fseek(encInfo->fptr_secret,0,SEEK_END);

     //function call to store size of secret message
     encode_secret_file_size(ftell(encInfo->fptr_secret),encInfo);
     printf(MAG"INFO: Secret file size Encoded sucessfully\n"reset);

     //function call to encode secret message
     encode_secret_file_data(encInfo);
     printf(MAG"INFO: Secret file data Encoded sucessfully\n"reset);

     //function call to copy remaining data
     copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image);
     printf(MAG"INFO: Remaining all data encoded sucessfully\n"reset);

     //closing all files
     fclose(encInfo->fptr_src_image);
     fclose(encInfo->fptr_stego_image);
     fclose(encInfo->fptr_secret);
     return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)  //function call to copy header of src file
{
     printf(MAG"INFO: Header Copying\n"reset);
     char buff[54];
     //read 54 byte in 1 time from src file & write in output file
     fread(buff,54,1,fptr_src_image);
     fwrite(buff,54,1,fptr_dest_image);
     return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)    //called function to encode magic string
{
     printf(MAG"INFO: Encoding Magic string length\n"reset);
     int len=strlen(magic_string);
     char *buff=malloc(8*sizeof(int));
     //read 32 in 1 time from src file
     fread(buff,32,1,encInfo->fptr_src_image);
     //function call to encode length of magic string in lsb bit
     encode_int_to_lsb(len,buff);
     //write encode data in ouput src file
     fwrite(buff,32,1,encInfo->fptr_stego_image);
     printf(MAG"INFO: Magic string length Encoded sucessfully\n"reset);

     //encoding magic string
     printf(MAG"INFO: Encoding Magic string\n"reset);
     //realloc size of buff
     buff=realloc(buff,8);
     int i=0;
     //encoding & storing magic string in output file byte by byte
     while(magic_string[i]!=0)
     {
          fread(buff,8,1,encInfo->fptr_src_image);
          encode_byte_to_lsb(magic_string[i],buff);
          fwrite(buff,8,1,encInfo->fptr_stego_image);
          i++;
     }
     free(buff);
     return e_success;
}

Status encode_int_to_lsb(int len, char *buff)         //called function to encode int to lsb
{
     int index=0;
     //running loop and replace lsb
     for(int i=31; i>=0; i--)
     {
          if(len&(1<<i))
               buff[index]=(buff[index]|1);
          else
               buff[index]=(buff[index]&(~1));
          index++;
     }
     return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)     //called function to encode byte to lsb
{
     int index=0;
     //running loop and replace lsb
     for(int i=7; i>=0; i--)
     {
          if(data&(1<<i))
               image_buffer[index]=(image_buffer[index]|1);
          else
               image_buffer[index]=(image_buffer[index]&(~1));
          index++;
     }
     return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)   //called function to encode secret message ext
{
     int i=0,count=0;
     while(file_extn[i++]!=0)
          count++;
     
     //encding the extension length in lsb by calling function
     printf(MAG"INFO: Encoding Secret string extension length\n"reset);
     char *buff=malloc(8*sizeof(int));
     fread(buff,32,1,encInfo->fptr_src_image);
     encode_int_to_lsb(count,buff);
     fwrite(buff,32,1,encInfo->fptr_stego_image);
     printf(MAG"INFO: Secret string extension length Encoded sucessfully\n"reset);

     //encoding the extension of secret file
     printf(MAG"INFO: Encoding secret string extension\n"reset);
     buff=realloc(buff,8);
     i=0;
     //encoding & storing extesion in output file byte by byte
     while(file_extn[i]!=0)
     {
          fread(buff,8,1,encInfo->fptr_src_image);
          encode_byte_to_lsb(file_extn[i],buff);
          fwrite(buff,8,1,encInfo->fptr_stego_image);
          i++;
     }
     free(buff);
     return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)     //called function to store secret data length
{
     //encoding the secret file size
     printf(MAG"INFO: Encoding Secret file size\n"reset);
     char *buff=malloc(8*sizeof(int));
     fread(buff,32,1,encInfo->fptr_src_image);
     encode_int_to_lsb(file_size,buff);
     fwrite(buff,32,1,encInfo->fptr_stego_image);
     free(buff);
     return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)         //called function to store secret file data
{    
     //encoding & storing data in output file byte by byte
     printf(MAG"INFO: Encoding Secret file data\n"reset);
     char *buff=malloc(8*sizeof(char));
     char ch;
     rewind(encInfo->fptr_secret);
     while((ch=fgetc(encInfo->fptr_secret))!=EOF)
     {
          fread(buff,8,1,encInfo->fptr_src_image);
          encode_byte_to_lsb(ch,buff);
          fwrite(buff,8,1,encInfo->fptr_stego_image);
     }
     free(buff);
     return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)     //called function to store remaining data
{
     printf(MAG"INFO: Encoding Remaining data\n"reset);
     char *buff=malloc(1*sizeof(char));
     //read & write remaining data
     while(fread(buff,1,1,fptr_src)>0)
          fwrite(buff,1,1,fptr_dest);
     free(buff);
     return e_success;
}