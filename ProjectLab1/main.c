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
    
    //test datamgr_get¨room_id
    
    sensor_id_t test_id = 15; 
    uint16_t room = datamgr_get_room_id(test_id);
    printf("Sensor %u belongs to room %u\n", test_id, room);

    sensor_value_t avg = datamgr_get_avg(test_id);
    printf("Average temperature for sensor %u: %.2f°C\n", test_id, avg);
    printf("Checking total number of sensors:\n");
    int total = datamgr_get_total_sensors();
    printf("Total sensors recorded: %d\n", total);

    printf("Checking last modified timestamp:\n");
    time_t last = datamgr_get_last_modified(test_id);
    printf("Sensor %u last updated at %ld (Unix time)\n", test_id, (long)last);

    datamgr_free();
    fclose(map);
    fclose(data);
   
}
