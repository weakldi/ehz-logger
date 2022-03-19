#include "sml/sml_file.h"
#include "sml/sml_transport.h"
#include "stdio.h"
#include "stdlib.h"

void print_name(octet_string* str){
    for(int i = 0; i < str->len; i++){
        printf("%02x", str->str[i]);
    }
}
void print_sml_get_list_response(sml_get_list_response* response){
    printf("\t\tclientid %x\n", response->client_id);
    printf("\t\tserverid "); print_name(response->server_id);
    printf("\n\t\ttime %u\n", response->act_sensor_time);
    sml_list* list = response->val_list;
    while(list->next != NULL){
        printf("\t\t\tName: ");
        print_name(list->obj_name);
        printf("\n\t\t\tValue %x (%i) (%u)\n", list->value->data.int64 ,list->value->data.int64 ,list->value->data.int64);
        
        list = list->next;
    }
    
}

void print_sml_msg(sml_message* msg){
    printf("{\n\tgroupID: %i\n\ttransactionId: %i\n\t", msg->group_id ,msg->transaction_id);
    u32 tag = *(msg->message_body->tag);
    switch (tag) {
	case SML_MESSAGE_OPEN_REQUEST:
		
		break;
	case SML_MESSAGE_OPEN_RESPONSE:
		printf("SML_MESSAGE_OPEN_RESPONSE\n");
		break;
	case SML_MESSAGE_CLOSE_REQUEST:
		
		break;
	case SML_MESSAGE_CLOSE_RESPONSE:
		printf("SML_MESSAGE_CLOSE_RESPONSE\n");
		break;
	case SML_MESSAGE_GET_PROFILE_PACK_REQUEST:
		
		break;
	case SML_MESSAGE_GET_PROFILE_PACK_RESPONSE:
					
		break;
	case SML_MESSAGE_GET_PROFILE_LIST_REQUEST:
		break;
	case SML_MESSAGE_GET_PROFILE_LIST_RESPONSE:
		break;
	case SML_MESSAGE_GET_PROC_PARAMETER_REQUEST:
		break;
	case SML_MESSAGE_GET_PROC_PARAMETER_RESPONSE:
		break;
	case SML_MESSAGE_SET_PROC_PARAMETER_REQUEST:
		break;
	case SML_MESSAGE_GET_LIST_REQUEST:
		
		break;
	case SML_MESSAGE_GET_LIST_RESPONSE:
		printf("SML_MESSAGE_GET_LIST_RESPONSE\n");
        print_sml_get_list_response(msg->message_body->data);
		break;
	case SML_MESSAGE_ATTENTION_RESPONSE:
		
		break;
	default:
		fprintf(stderr, "libsml: error: message type %04X not yet implemented\n",
				tag);
		break;
	}
    printf("}\n");
}

void print_sml_file(sml_file* file){
    sml_file_print(file);
    for(int i = 0; i < file->messages_len; i++){
        sml_message* msg = file->messages[i];
       
        print_sml_msg(msg);
    }
}
void sml_handler(unsigned char *buffer, size_t len){
    sml_file* file = sml_file_parse(buffer+8,len-8);
    print_sml_file(file);
}

int main(int argc, char** argv){
    octet_string* clientID = sml_octet_string_generate_uuid();
    sml_file* f;
    sml_message* m1;
    sml_message* m2;
    sml_message_body* b1;
    sml_message_body* b2;
    sml_open_request* open;
    sml_get_profile_list_request* request;
    open = sml_open_request_init();
    open->client_id = clientID;
    open->req_file_id = sml_octet_string_generate_uuid();
    request = sml_get_profile_list_request_init();
    request->parameter_tree_path = sml_tree_path_init();
    sml_tree_path_add_path_entry(request->parameter_tree_path, sml_octet_string_init_from_hex("0100010800"));
    f = sml_file_init();

    b1 = sml_message_body_init(SML_MESSAGE_OPEN_REQUEST, open);
    b2 = sml_message_body_init(SML_MESSAGE_GET_PROFILE_LIST_REQUEST, request);
    m1 = sml_message_init();
    m2 = sml_message_init();
    m1->transaction_id = sml_octet_string_generate_uuid();
    m1->group_id = sml_u8_init(0);
    m1->abort_on_error = 0;
    m1->message_body = b1;

    

    m2->transaction_id = sml_octet_string_generate_uuid();
    m2->group_id = sml_u8_init(0);
    m2->abort_on_error = 0;
    m2->message_body = b2;

    sml_file_add_message(f, m1);
    sml_file_add_message(f, m2);
   
    sml_file_write(f);

    for(int i = 0; i < f->buf->cursor; i++){
        printf("%02x\n", f->buf->buffer[i]);
    }

    FILE* example = fopen("sml_example_01.bin", "r");
    unsigned char buffer[2048*4];
    sml_transport_listen(fileno(example), sml_handler);
    return 0;
}