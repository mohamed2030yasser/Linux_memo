#include <stdio.h>
#include <stdlib.h>
#define PATH "/dev/test_file"

void File_Write ()
{
    char str[100]={};
    FILE *fptr ;
    fptr = fopen(PATH ,"w");
    if (fptr == NULL)
    {
        return;
    }
    printf("please enter data :");
    scanf("%100[a-zA-Z ]",str);

    fprintf(fptr,"%s" ,str);
    fclose (fptr);
}

void File_Read ()
{
    char str[100]={};
    FILE *fptr ;
    fptr = fopen(PATH ,"r");
    if (fptr == NULL)
    {
        return;
    }

    fprintf(fptr,"%[^\n]" ,str);
    printf("Message is : %s\n",str);
    fclose (fptr);
}

void main ()
{
    File_Write();
    File_Read();
}