#include "sml/sml_file.h"

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
    
    return 0;
}