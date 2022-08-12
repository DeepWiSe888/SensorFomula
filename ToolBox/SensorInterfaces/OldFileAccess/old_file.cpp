#include "old_file.h"
#include <stdio.h>

OldFileBase::OldFileBase()
{
    fp = 0;
    frame_size = 0;
}
OldFileBase::~OldFileBase()
{
    close();
}

int OldFileBase::open(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if(f==0)
        return -1;

    fp = (long)f;

    return fp;
}

void OldFileBase::close()
{
    if(fp)
        fclose((FILE*)fp);
    fp = 0;
}


ArtsEngineFileAccess::ArtsEngineFileAccess()
{

}
ArtsEngineFileAccess::~ArtsEngineFileAccess()
{

}

int ArtsEngineFileAccess::readFrame(x4_frame_arts* out)
{
    const long frame_head_size =  (const long)(  ((x4_frame_arts*)0)->i );
    int nRead = fread(out, frame_head_size, 1, (FILE*)fp);
    if(nRead)
    {
        frame_size = out->bin_cnt;
        out->bin_cnt /= 2;
        if( fread(out->i, sizeof (float), out->bin_cnt, (FILE*)fp) < out->bin_cnt)
            return -1;
        if( fread(out->q, sizeof (float), out->bin_cnt, (FILE*)fp) < out->bin_cnt)
            return -1;
    }
    return nRead;
}