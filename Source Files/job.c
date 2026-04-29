#include "../Header Files/job.h"

Job job;

void extract_params(char* line,int sect){
    int c1=(int)(line[0]-'0');
    int c2=(int)(line[1]-'0');
    int c3=(int)(line[2]-'0');
    int c4=(int)(line[3]-'0');

    int num=c1*1000+c2*100+c3*10+c4;

    if(sect==0){
        job.jobId=num;
    }else if(sect==1){
        job.time_limit=num;
    }else if(sect==2){
        job.line_limit=num;
    }
}

int get_line_limit(){
    return job.line_limit;
}

int extract_address(char c1,char c2){
    int n1=(int)c1-'0';
    int n2=(int)(c2-'0');
    return n1*10+n2;
}

void reset_job(){
    job.line_limit=0;
    job.time_limit=0;
}
