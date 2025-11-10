//
// Created by hajar on 11/10/25.
//
#include <stdint.h>
#include "datamgr.h"
#include "lib/dplist.h"
#include <string.h>

#include "config.h"


typedef uint16_t room_id_t;
dplist_t *sensor_data_list = NULL;


typedef struct {
    sensor_id_t id ;
    room_id_t room_id;
    double temperature_values[RUN_AVG_LENGTH];
    int num_readings;
    int next_index;
    sensor_ts_t last_modified;
} sensor_data_element_t;



void *element_copy(void *element) {
    sensor_data_element_t *copy = malloc(sizeof(sensor_data_element_t));
    ERROR_HANDLER(copy == NULL, "Memory allocation failed in element_copy");
    memcpy(copy, element, sizeof(sensor_data_element_t));  // Shallow copy is safe because there are no pointers in element structure
    return copy;

}


void element_free(void **element) {
    free( *element);  // free needs pointer in argument that points to the structure to be freed *element points to sensor_data_element_t
    *element = NULL;
}

int element_compare(void *x, void *y) {
  return( (((sensor_data_element_t *)x) -> id < ((sensor_data_element_t *)y)->id) ? -1 :
  (((sensor_data_element_t *)x) -> id >  ((sensor_data_element_t *)y)->id) ? 1:0);

}


void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {
    sensor_data_list = dpl_create(element_copy, element_free, element_compare);
    uint16_t room_id,  sensor_id ;
    while (fscanf(fp_sensor_map, "%hu %hu" , &room_id, &sensor_id) ==2)
    {
        sensor_data_element_t *new_sensor = malloc(sizeof(sensor_data_element_t));
        ERROR_HANDLER(new_sensor == NULL, "memory allocation failed");
        new_sensor -> id = sensor_id;
        new_sensor -> room_id = room_id;
        new_sensor -> num_readings = 0;
        new_sensor -> next_index = 0;
        new_sensor -> last_modified = 0;
        dpl_insert_at_index(sensor_data_list, new_sensor, 0, true);
        free(new_sensor);  // used true in dpl_insert_at_index() so copy is made of new_sensor

    }

    sensor_data_t data;
    while(fread (&data.id, sizeof(sensor_id_t),1, fp_sensor_data) ==1 && fread (&data.value, sizeof(sensor_value_t),1, fp_sensor_data) ==1 && fread (&data.ts, sizeof(sensor_ts_t),1, fp_sensor_data) ==1) {
        sensor_data_element_t temp;
        temp.id = data.id;
        int index = dpl_get_index_of_element(sensor_data_list, &temp);
        if(index == -1) {
            fprintf(stderr, "sensor ID %u not found in map. Ignoring.\n", data.id);
            continue;
        }

        sensor_data_element_t *sensor = dpl_get_element_at_index(sensor_data_list, index);
        sensor->temperature_values[sensor->next_index] = data.value;
        sensor->next_index = (sensor->next_index + 1) % RUN_AVG_LENGTH;
        if (sensor->num_readings < RUN_AVG_LENGTH) sensor->num_readings++;
        sensor->last_modified = data.ts;


        double sum = 0;
        for (int i = 0; i < sensor->num_readings; i++) {
            sum += sensor->temperature_values[i];
        }
        double avg = sum / sensor->num_readings;


        if (avg > SET_MAX_TEMP) {
            fprintf(stderr, "Room %u is too hot (avg = %.2f°C)\n", sensor->room_id, avg);
        } else if (avg < SET_MIN_TEMP) {
            fprintf(stderr, "Room %u is too cold (avg = %.2f°C)\n", sensor->room_id, avg);
        }
    }

    //free used because it's a binary file scanf only works on formatted files( elements seperated by spaces or newlines) reading whole structure at once would work if the struct layout exactly matches the binary file so reading field by field is safer
}

void datamgr_free() {
  dpl_free(&sensor_data_list, true);
  sensor_data_list = NULL;
  }
  
uint16_t datamgr_get_room_id(sensor_id_t sensor_id) {
  sensor_data_element_t temp;
  
  temp.id = sensor_id;
  int index = dpl_get_index_of_element( sensor_data_list, &temp);
  ERROR_HANDLER(index ==-1, "Invalid sensor ID");
  
  sensor_data_element_t *sensor = dpl_get_element_at_index(sensor_data_list, index);
  return sensor ->room_id;


}
