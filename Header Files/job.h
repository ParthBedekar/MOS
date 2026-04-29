#ifndef JOB_H
#define JOB_H

typedef struct{
    int jobId;
    int time_limit;
    int line_limit;
} Job;

extern Job job;

void extract_params(char* line,int sect);
int get_line_limit();
int extract_address(char c1,char c2);
void reset_job();
#endif
