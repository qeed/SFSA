#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <time.h>
#include <map>
#include <algorithm>
#include "mols/mols.h"

void do_ls(char*);
void dostat(char*,char*);
void show_file_info(char*,struct stat*);
void mode_to_letters(int,char*);

std::map<int, double> file_list;

int main(int ac,char *argv[])
{
    do_ls((char*)"/home/qeed/Video");

    std::map<int, double>::iterator i;
    std::vector<float> xv;
    std::vector<float> yv;
    float sum = 0;
    for(i = file_list.begin(); i != file_list.end(); i++)
    {
        //std::cout<<"day: "<<i->first<<" | bytes: "<<i->second<<std::endl;
        xv.push_back((float)(i->first));
        yv.push_back(i->second);

        sum += i->second;
    }


    MOLS mols(xv, yv);
    mols.defK();
    mols.defB();
    float limit = 100 * 1024; /// 100mb limit
    mols.defX(limit); // defined day of free space end

    printf("LIMIT: %f\n", mols.getX());

    printf("SUM: %f\n", sum);


    return 0;
}


void do_ls(char *dirname)
{
    DIR *dir_ptr;
    dirent *direntp;
    if((dir_ptr=opendir(dirname))==NULL)
    {
        fprintf(stderr,"myls:cannot open %s\n",dirname);
    }else{
        while((direntp=readdir(dir_ptr))!=NULL){
            if(*(direntp->d_name) != '.' ){
                if(direntp->d_type == DT_DIR)
                {
                    char *newdir = new char[256];
                    memset(newdir, 0, sizeof(newdir));
                    strcpy(newdir, dirname);
                    if(newdir[strlen(dirname)-1] != '/') strcat(newdir, "/");
                    strcat(newdir, direntp->d_name);
                    do_ls(newdir);
                }else
                if(direntp->d_type != DT_LNK)
                    dostat(direntp->d_name,dirname);
                //printf("%8d ",(int)direntp->d_ino);
            }
        }
    closedir(dir_ptr);
    }
}

void dostat(char *filename,char *dirname)
{
    struct stat info;
    char* way_to_file=new char [strlen(filename)+strlen(dirname)+2];
    strcpy(way_to_file,dirname);
    strcpy(way_to_file+strlen(dirname),"/\0");
    strcpy(way_to_file+strlen(way_to_file),filename);

    if(stat(way_to_file,&info)==-1){
        printf("Error %s\n",way_to_file);
        perror(filename);
    }
    else
        show_file_info(filename,&info);
    delete way_to_file;
}

void show_file_info(char *filename,struct stat *info_p)
{
    //printf("%4d ", (int)info_p->st_nlink);
    //printf("%-8s ",uid_to_name(info_p->st_uid));
    //printf("%-8s",gid_to_name(info_p->st_gid));
    tm* gmtm = gmtime(&(info_p->st_mtime));
    file_list[gmtm->tm_yday] += (double)info_p->st_size/1024/1024;
    //printf("%d ", gmtm->tm_yday);
    //printf("%8ld ", info_p->st_mtime);
//printf("%8ld ",(long)info_p->st_size);
//printf("%.12s ",4+ctime(&(info_p->st_mtime)));
//std::cout<<4+ctime(&(info_p->st_mtime))<<std::endl;
 //printf("%s\n",filename);

//    tm* gmtm = gmtime(&(info_p->st_mtime));
    /*
    printf("sec: %d\nmin:%d\nhour+4: %d\nmday: %d\nmon: %d\nyear: %d\nwday: %d\nyday: %d\nisdst: %d\n",
            gmtm->tm_sec, gmtm->tm_min, gmtm->tm_hour+4, gmtm->tm_mday, gmtm->tm_mon, gmtm->tm_year, gmtm->tm_wday, gmtm->tm_yday, gmtm->tm_isdst);
    */
}
