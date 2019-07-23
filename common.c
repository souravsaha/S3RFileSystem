#include "common.h"

// TODO take filesize as argument
void initFs()
{
    FILE *fp = fopen("/tmp/S3R.fs", "w");
    fseek(fp, SIZE-1 , SEEK_SET);
    fputc('\0', fp);
    fclose(fp);

}

int main()
{
    initFs();
    return 0;
}