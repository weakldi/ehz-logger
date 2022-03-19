#include "sml/sml_file.h"
#include "sml/sml_transport.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"  /* String function definitions */
#include "unistd.h"  /* UNIX standard function definitions */
#include "fcntl.h"   /* File control definitions */
#include "errno.h"   /* Error number definitions */
#include "termios.h" /* POSIX terminal control definitions */
#include <time.h>
unsigned char obis_str[6] = {0x01,0x00,0x01,0x08,0x01,0xFF};
FILE* out_fd;
int open_serial(char* device){

}

int is_obis(octet_string* str, octet_string* obis){
   bool is_eq = str->len == obis->len;
  
   for(int i = 0; i < str->len && is_eq; i++){
       is_eq = is_eq & (str->str[i] == obis->str[i]);
   }
   return is_eq;
}

void print_name(FILE* f,octet_string* str){
    for(int i = 0; i < str->len; i++){
        fprintf(f,"%02x", str->str[i]);
    }
}

void sml_parse_obis(sml_get_list_response* response, octet_string* obis){
    //serverid;time;value
    
    sml_list* list = response->val_list;
    while(list->next != NULL){
       if(is_obis(list->obj_name, obis)){
            double value = sml_value_to_double(list->value);
            
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            print_name(out_fd,response->server_id);
            fprintf(out_fd,";%f;%s",value,asctime(tm));
        }
        
        list = list->next;
    }
    
}

void sml_handler(unsigned char *buffer, size_t len){
    sml_file* file = sml_file_parse(buffer+8,len-8);
    for(int i = 0; i < file->messages_len; i++){
        u32 tag = *file->messages[i]->message_body->tag;
        if(tag == SML_MESSAGE_GET_LIST_RESPONSE){
            sml_get_list_response* response = file->messages[i]->message_body->data;
            octet_string obis;
           
            obis.str = obis_str;
            obis.len = 6;
            sml_parse_obis(response, &obis);
        }
    }
}
void obis_str_to_bin(char* obis, char* out){
    //unsafe
    //1-0:1.8.0
    out[0] = obis[0] - '0';
    out[1] = obis[2] - '0';
    out[2] = obis[4] - '0';
    out[3] = obis[6] - '0';
    out[4] = obis[8] - '0';
    out[5] = 0xFF;
    octet_string str;
           
    str.str = out;
    str.len = 6;

    print_name(stdout, &str);
}

int main(int argc, char**argv){
    if(argc <= 3) {
        printf("usage: %s <inputfile> <outputfile> <obis>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    int fd = open(argv[1], O_RDONLY | O_NOCTTY | O_NDELAY);
    out_fd = fopen(argv[2], "a");
    obis_str_to_bin(argv[3], obis_str);

    
    perror("File open");
    sml_transport_listen(fd, sml_handler);

    return EXIT_SUCCESS;
}