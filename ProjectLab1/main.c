#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "lib/dplist.h"
#include "datamgr.h"
#include <time.h>

int main(){
    printf("Hello World\n");

    FILE * map = fopen("room_sensor.map", "r");
    FILE * data = fopen("sensor_data", "rb");
    //ERROR_HANDLER(map == NULL || data == NULL, "Failed to open input files");

    if(map == NULL){
    ERROR_HANDLER(map == NULL,"Failed to open room_sensor.map");
    return -1;}
    if(data == NULL){
    ERROR_HANDLER(data == NULL, "Failed to open sensor_data");
    return -1;}

    datamgr_parse_sensor_files(map, data);

    datamgr_free();
    fclose(map);
    fclose(data);
   
}
