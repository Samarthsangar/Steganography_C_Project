/*---------------------------------------------------------STEGANOGHRAPHY---------------------------------------------------------------------------
Name : Samarth Rajendra Sangar.
Date : 18/09/2024
Project Name : Steganoghraphy.
               Steganoghraphy is a process of hidding a data,message,video or image with other data ,message,video or image. This provides the security
               to the confidential data.

---------------------------------------------------------STEGANOGHRAPHY---------------------------------------------------------------------------*/

/* Header file include */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "Colour.h"

void error();              //function prototype

int main(int argc, char *argv[])               //main function with parameter's
{
    if(argc==1)                                //some condition's to chcek CLA
    {
        printf(BRED"INFO: Less argument's in CLA !\n"reset);
        error();
        return e_failure;
    }
    else if(argc==2)
    {
        printf(BRED"INFO: Less argument's in CLA !\n"reset);
        error();
        return e_failure;
    }

    if(check_operation_type(argv)==e_encode)        //function call for check operation
    {
        if(argc>5 || argc==3)                       //checking some conditions before encoding
        {
            printf(BRED"INFO: Check argument's in CLA for encoding !\n"reset);
            error();
            return e_failure;
        }
        char *ch1=strchr(argv[2],'.');              //checking file ext & with some condition's
        if(ch1==NULL || strcmp(ch1,".bmp")!=0)
        {
            printf(BRED"INFO: Source file error !\n"reset);
            error();
            return e_failure;
        }
        if(strchr(argv[3],'.')==NULL)
        {
            printf(BRED"INFO: Secret file error !\n"reset);
            error();
            return e_failure;
        }
        EncodeInfo encInfo;                          //structure variable creation
        encInfo.src_image_fname=argv[2];             //storing CLA in struct member's
        encInfo.secret_fname=argv[3];
        if(argv[4]==NULL)                            //checking some condition's
            encInfo.stego_image_fname="Stego.bmp";
        else
        {
            ch1=strchr(argv[4],'.');
            if(ch1==NULL || strcmp(ch1,".bmp")!=0)
            {
                printf(BRED"INFO: Output file error during encoding !\n"reset);
                error();
                return e_failure;
            }
            char *ch1=strchr(argv[2],'.');
            char *ch2=strchr(argv[4],'.');
            if(strcmp(ch1,ch2)!=0)
            {
                printf(BRED"INFO: Output file extension Error during encoding "reset URED"(Extension missmatch src & Out file)\n"reset);
                error();
                return e_failure;
            }
            else
                encInfo.stego_image_fname=argv[4];
        }

        printf(MAG"INFO: File's Opening for encoding\n"reset);
        if(do_encoding(&encInfo)==e_success)                       //function call for encoding secret message
            printf(GRN"INFO: ## Encoding Done\n"reset);
        else
            printf(BRED"INFO: Failure\n"reset);
    }
    else if(check_operation_type(argv)==e_decode)                  //function call to check operation
    {
        if(argc>4)                                                 //checking some condition's
        {
            printf(BRED"INFO: To many argument's in CLA for decoading !\n"reset);
            error();
            return e_failure;
        }
        char *ch1=strchr(argv[2],'.');
        if(ch1==NULL || strcmp(ch1,".bmp")!=0)
        {
            printf(BRED"INFO: Source file error !\n"reset);
            error();
            return e_failure;
        }
        DecodeInfo decInfo;                                        //structure variable creation
        decInfo.src_image_fname=argv[2];                           //storing CLA in struct members
        if(argv[3]==NULL)
            strcpy(decInfo.secret_fname,"Output");
        else
        {
            if((strchr(argv[3],'.'))!=NULL)
            {
                printf(BRED"INFO: Output file extension Error during decoding "reset URED"(Error : Output file name with extension) !\n"reset);
                error();
                return e_failure;
            }
            else
                strcpy(decInfo.secret_fname,argv[3]);
        }
        printf(MAG"INFO: File's Opening for decoding\n"reset);
        if(do_decoding(&decInfo)==e_success)                        //function call for decoding
            printf(GRN"INFO: ## Decoding Done\n"reset);
        else
            printf(BRED"INFO: Failure\n"reset);
    }
    else                                                            //printing error if Invalid entry
    {
        printf(BRED"INFO: Invalid Entry (%s) !\n"reset,argv[1]);
        error();
        return e_failure;
    }
    
    return e_success;
}

OperationType check_operation_type(char *argv[])                 //called function to check operation type
{
    if(strcmp(argv[1],"-e")==0)                                 //checking some condition's & return Enum member
        return e_encode;
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}

void error()                                                   //called function to print help message if error occured
{
    printf(BWHT"\n----------------------------------------------------"reset URED"Help"reset BWHT"------------------------------------------------------\n"reset);
    printf(BRED"\n1. -e : Encoding like : ./a.out -e  < .bmp file >  < .txt/.c/.sh file >  < .bmp output file >(optional)\n"reset);
    printf(BRED"\n2. -d : Decoding like : ./a.out -d  < .bmp file >  < file without extension >(optional)\n"reset);
    printf(BWHT"\n---------------------------------------------------------------------------------------------------------------\n"reset);
}
