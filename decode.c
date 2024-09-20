//header file include
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include "Colour.h"

/* Function Definitions */

Status open_file(DecodeInfo *decInfo)            //called function to open file
{   
    //opening src file in read mode
    decInfo->fptr_src_image=fopen(decInfo->src_image_fname,"r");
    if(decInfo->fptr_src_image == NULL)         //check condition
     {
	     printf(BRED"INFO: Source image file is not opened\n"reset);
	     return e_failure;
     }
     return e_success;
}

Status do_decoding(DecodeInfo *decInfo)          //called function for decoding the data
{   
    //function call to open file & check return value
    if(open_file(decInfo)==e_success)
          printf(MAG"INFO: File's opened successfully\n"reset);
    else
    {
        printf(BRED"INFO: Unable to open\n"reset);
        return e_failure;
    }
    printf(GRN"INFO: ## Decoding started\n"reset);

    //seek 54 byte of herder
    fseek(decInfo->fptr_src_image,54,SEEK_SET);

    //function call to decode magic string
    if(decode_magic_string(decInfo)==e_failure)
        return e_failure;
    printf(MAG"INFO: Passkey matched\n"reset);

    //function call to decode secret file ext
    decode_secret_file_extn(decInfo);
    printf(MAG"INFO: Secret File extension Decoded sucessfully\n"reset);

    //function call to decode size of secret data
    int len=decode_secret_file_size(decInfo);
    printf(MAG"INFO: secret File length Decoded sucessfully\n"reset);

    //function call to decode secret data message
    decode_secret_file_data(len,decInfo);
    printf(MAG"INFO: secret File data Decoded sucessfully\n"reset);

    //close file
    fclose(decInfo->fptr_src_image);
    printf(UMAG"INFO: Your decoded file name is %s\n"reset,decInfo->secret_fname);
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)           //called function to decode magic string
{
    printf(MAG"INFO: Decoding length of magic string\n"reset);
    //decoding magic string length from lsb bit of src file
    char *buff=malloc(8*sizeof(int));
    fread(buff,32,1,decInfo->fptr_src_image);
    int len=decode_lsb_to_int(buff);
    printf(MAG"INFO: Magic string length decoded sucessfully\n"reset);

    printf(MAG"INFO: Decoding magic string\n"reset);
    //realloc the size of buff
    buff=realloc(buff,1);
    char ch[len];
    //decoding magic string srom lsb bit by calling function
    for(int i=0; i<len; i++)
    {
        fread(buff,8,1,decInfo->fptr_src_image);
        decode_lsb_to_byte(ch+i,buff);
    }
    free(buff);
    printf(MAG"INFO: Magic string decoded sucessfully\n"reset);

    //taking input from user to match secret key
    char passkey[len];
    printf(BBLU"Enter Magic string (Passkey) to Decode secret message : "reset);
    scanf(" %[^\n]",passkey);
    //compare & return
    if(strcmp(ch,passkey)==0)
        return e_success;
    else
        return e_failure;
}

Status decode_lsb_to_int(char *buff)           //called function to decode lsb bit to int
{
    int len=0;
    //running loop & based on lsb bit,replace perticular bit of variable
    for(int i=0; i<32; i++)
    {
        if(buff[i]&1)
            len=((len<<1)|1);
        else
            len=(len<<1);
    }
    return len;         //return len variable
}

Status decode_lsb_to_byte(char *data, char *image_buffer)   //called function to decode lsb to byte
{
    //running loop & based on lsb bit,replace perticular bit 
    for(int i=0; i<8; i++)
    {
        if(image_buffer[i]&1)
            *data=((*data<<1)|1);
        else
            *data=(*data<<1);
    }
}

Status decode_secret_file_extn(DecodeInfo *decInfo)         //called function to decode magic string file ext
{
    printf(MAG"INFO: Decoding length of secret message extension\n"reset);
    //decoding the size of extension
    char *buff=malloc(8*sizeof(int));
    fread(buff,32,1,decInfo->fptr_src_image);
    int len=decode_lsb_to_int(buff);
    printf(MAG"INFO: Secret file extension length decoded sucessfully\n"reset);

    printf(MAG"INFO: Decoding secret message file extension\n"reset);
    //decoding ext of file byte by byte by running loop & calling function
    buff=realloc(buff,1);
    char ch[len];
    for(int i=0; i<len; i++)
    {
        fread(buff,8,1,decInfo->fptr_src_image);
        decode_lsb_to_byte(ch+i,buff);
    }
    free(buff);

    //concat the extension with name of file
    strcat(decInfo->secret_fname,ch);
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)         //called function to decode secret file size
{
    //decode the secret file size
    printf(MAG"INFO: Decoding data length of secret file\n"reset);
    char *buff=malloc(8*sizeof(int));
    fread(buff,32,1,decInfo->fptr_src_image);
    int len=decode_lsb_to_int(buff);
    free(buff);

    return len;      //return size of secret file 
}

Status decode_secret_file_data(int len,DecodeInfo *decInfo)     //called function to decode secret data
{
    printf(MAG"INFO: Decoding secret message file data\n"reset);
    //decoding secret data byte by byte by running loop & calling function
    char *buff=malloc(1*sizeof(char));
    char ch[len+1];
    for(int i=0; i<len; i++)
    {
        fread(buff,8,1,decInfo->fptr_src_image);
        decode_lsb_to_byte(ch+i,buff);
    }
    free(buff);
    //open output file in writing mode
    decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");
    int i=0;
    ch[len]='\0';
    //running loop & storing byte by byte data in file
    while(i<len)
        fputc(ch[i++],decInfo->fptr_secret);
    fclose(decInfo->fptr_secret);

    return e_success;
}