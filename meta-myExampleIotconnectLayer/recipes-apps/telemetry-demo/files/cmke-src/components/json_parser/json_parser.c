#include "json_parser.h"

#define STRINGS_ARE_EQUAL 0

static int parse_x509_certs(const cJSON* json_parser, char** id_key, char** id_cert);
static int parse_telemetry_settings(cJSON *json_parser, sensors_data_t *local_sensors);
static int parse_commands(cJSON *json_parser, commands_data_t *commands);
static int parse_base_params(char** dest, char* json_src, cJSON* json_parser);
static int parse_sensors(const cJSON* json_parser, sensors_data_t *sensors);
static int parse_auth(cJSON *parser, IotConnectClientConfig* iotc_config);
static int parse_auth_params(cJSON *parser, IotConnectClientConfig* iotc_config);

static int parse_telemetry_settings(cJSON *json_parser, sensors_data_t *local_sensors){

    cJSON *telemetry_parser = NULL;
    
    telemetry_parser = cJSON_GetObjectItem(json_parser, "telemetry");

    if (!telemetry_parser) {
        printf("Failed to get telemetry obj.\r\n");
        return 1;
    }


    //TODO; maybe rethink this
    if (cJSON_HasObjectItem(telemetry_parser, "sensors") == true){
        if (parse_sensors(telemetry_parser, local_sensors) != 0){
            printf("failed to parse sensor. Aborting\r\n");
            cJSON_Delete(telemetry_parser);
        }
        //printf("sensor data: name - %s; path - %s\r\n", sensor->s_name, sensor->s_path);
    }

    //cJSON_Delete(telemetry_parser);
    return 0;

}

static int parse_commands(cJSON *json_parser, commands_data_t *commands){

    int ret = 0;

    cJSON *commands_obj = NULL;
    cJSON *json_array_item = NULL;

    commands_obj = cJSON_GetObjectItem(json_parser, "commands");

    if (!commands_obj){
        printf("Failed to get sensor object. Aborting\n");
        return 1;
    }

    commands->counter = cJSON_GetArraySize(commands_obj);

    if (commands->counter <= 0) {
        printf("Failed to get array size\r\n");
        return 1;
    }

    commands->commands = calloc(commands->counter, sizeof(commands_t));

    if (!commands->commands){
        printf("failed to allocate\r\n");
        return 1;
    }

    for (int i = 0; i < commands->counter; i++){

        json_array_item = cJSON_GetArrayItem(commands_obj, i);

        if (!json_array_item){
            printf("Failed to access element %d of json sensor array\r\n", i);
            return 1;
        }

        if (parse_base_params(&commands->commands[i].name, "name", json_array_item) != 0){
            printf("Failed to get command name n%d from json file. Aborting.\r\n", i);
            return 1;
        }

#ifdef COMMAND_FUNCTION_PTR_PARSING
        if (parse_base_params(&commands->commands[i].f_ptr , "function", json_array_item) != 0){
            printf("Failed to get command function n%d from json file. Aborting.\r\n", i);
            return 1;
        }
#endif

        if (parse_base_params(&commands->commands[i].private_data, "private_data", json_array_item) != 0){
            printf("Failed to get command private data n%d from json file. Aborting.\r\n", i);
            return 1;
        }

        if (strcmp(commands->commands[i].name, "led") == STRINGS_ARE_EQUAL){
    
            FILE *fd = NULL;

            fd = fopen(commands->commands[i].private_data, "r");

            if (!fd){
                printf("failed to access led file - %s ; Aborting\n", commands->commands[i].private_data);
                return 1;
            }

            fclose(fd);


        } else if (strcmp(commands->commands[i].name, "echo") == STRINGS_ARE_EQUAL) {
            //TODO: placeholder
        } else {
            //TODO: placeholder for other types
        }
        
    }

    return 0;
}

static int parse_base_params(char** dest, char* json_src, cJSON* json_parser){
    if (!json_parser){
        printf("NULL PTR. Aborting\r\n");
        return 1;
    }

    cJSON* req_json_str = cJSON_GetObjectItemCaseSensitive(json_parser, json_src);
    if (!req_json_str) {
        printf("Failed to get %s from json. Aborting\n\r", json_src);
        return 1;
    }

    *dest = strdup(req_json_str->valuestring);
    if (!*dest){
        printf("failed to allocate memory for string %s\r\n", __func__);
        *dest = NULL;
        return 1;
    }

    if (strcmp(*dest, req_json_str->valuestring) != 0)
    {
        printf("copied string mismatch %s\r\n", __func__);
        *dest = NULL;
        return 1;
    }


    return 0;
}

//TODO: add proper error checking
static int parse_sensors(const cJSON* json_parser, sensors_data_t *sensors){

    if (!json_parser || !sensors){
        printf("NULL PTR. Aborting\r\n");
        return 1;
    }

    cJSON *sensor_obj = NULL;

    cJSON *device_name = NULL;
    cJSON *device_path = NULL;

    cJSON *json_array_item = NULL;

    sensor_obj = cJSON_GetObjectItem(json_parser, "sensors");

    if (!sensor_obj){
        printf("Failed to get sensor object. Aborting\n");
        cJSON_Delete(sensor_obj);
        return 1;
    }

    sensors->size = cJSON_GetArraySize(sensor_obj);

    if (sensors->size <= 0) {
        printf("Failed to get array size\r\n");
        return 1;
    }

    sensors->sensor = calloc(sensors->size, sizeof(sensor_info_t));

    if (!sensors->sensor){
        printf("failed to allocate\r\n");
        return 1;
    }

    for (int i = 0; i < sensors->size; i++){

        json_array_item = cJSON_GetArrayItem(sensor_obj, i);

        if (!json_array_item){
            printf("Failed to access element %d of json sensor array\r\n", i);
            return 1;
        }

        if (parse_base_params(&sensors->sensor[i].s_name, "name", json_array_item) != 0){
            printf("Failed to get sensor name n%d from json file. Aborting.\r\n", i);
            return 1;
        }

        if (parse_base_params(&sensors->sensor[i].s_path, "path", json_array_item) != 0){
            printf("Failed to get sensor path n%d from json file. Aborting.\r\n", i);
            return 1;
        }


    }

    return 0;
}


static int parse_x509_certs(const cJSON* json_parser, char** id_key, char** id_cert){

    if (!json_parser){
        printf("NULL PTR.\r\n");
        return 1;
    }

    cJSON *x509_obj = NULL;
    cJSON *x509_id_cert = NULL;
    cJSON *x509_id_key = NULL; 
    // ignoring auth type for now

    x509_obj = cJSON_GetObjectItem(json_parser, "x509_certs");

    if (!x509_obj){
        printf("Failed to get x509 object. Aborting\n");
        return 1;
    }

    printf("auth type: %s\n", x509_obj->valuestring);

    

    // TODO: add error checking
    x509_id_cert = cJSON_GetObjectItemCaseSensitive(x509_obj, "client_cert");
    x509_id_key = cJSON_GetObjectItemCaseSensitive(x509_obj, "client_key");

    printf("id cert path: {%s}\n", x509_id_cert->valuestring);
    printf("id key path: {%s}\n", x509_id_key->valuestring);


    int key_len = 0;
    key_len = strlen(x509_id_key->valuestring)*(sizeof(char));

    int cert_len = 0;
    cert_len = strlen(x509_id_cert->valuestring)*(sizeof(char));

    printf("cert len: %d, key len: %d:\r\n",cert_len, key_len);

    *id_cert = calloc(cert_len, sizeof(char));

    if (!*id_cert){
        printf("failed to malloc\r\n");
        *id_cert = NULL;
        return 1;
    }


    *id_key = calloc(key_len, sizeof(char));


    if (!*id_key){
        printf("failed to malloc\r\n");
        *id_key = NULL;
        return 1;
    }


    memcpy(*id_cert, x509_id_cert->valuestring, sizeof(char)*cert_len);

    memcpy(*id_key, x509_id_key->valuestring, sizeof(char)*key_len);


    id_cert[cert_len] = '\0';
    id_key[key_len] = '\0';

    return 0;
}

static int parse_auth_params(cJSON *parser, IotConnectClientConfig* iotc_config) {


    switch (iotc_config->auth_info.type){
    case IOTC_AT_X509:

        if (parse_base_params(&iotc_config->auth_info.data.cert_info.device_key, "client_key", parser) != 0){
            printf("Failed to get client key from json file. Aborting.\r\n");
            return 1;
        }

        if (parse_base_params(&iotc_config->auth_info.data.cert_info.device_cert, "client_cert", parser) != 0){
            printf("Failed to get client cert from json file. Aborting.\r\n");
            return 1;
        }
    

        break;
    case IOTC_AT_SYMMETRIC_KEY:
        
        if (parse_base_params(&iotc_config->auth_info.data.symmetric_key, "primary_key", parser) != 0){
            printf("Failed to get duid from json file. Aborting.\r\n");
            return 1;
        }

        printf("SYMMKEY: %s\r\n", iotc_config->auth_info.data.symmetric_key);
    
        break;
    case IOTC_AT_TOKEN:
        break;
    default:
        printf("placeholder auth type\r\n");
        break;
    }

}

static int parse_auth(cJSON *parser, IotConnectClientConfig* iotc_config){

    cJSON *auth_type;

    auth_type = cJSON_GetObjectItemCaseSensitive(parser, "auth_type");

    if (!auth_type) {
        printf("Failed to get auth_type. Aborting\n");
        return 1; 
    }

    printf("auth type: %s\n", auth_type->valuestring);

    if (strcmp(auth_type->valuestring, "IOTC_AT_X509") == STRINGS_ARE_EQUAL){
        iotc_config->auth_info.type = IOTC_AT_X509;
    } else if (strcmp(auth_type->valuestring, "IOTC_AT_SYMMETRIC_KEY") == STRINGS_ARE_EQUAL){
        iotc_config->auth_info.type = IOTC_AT_SYMMETRIC_KEY;
    } else if (strcmp(auth_type->valuestring, "IOTC_AT_TPM") == STRINGS_ARE_EQUAL) {
        iotc_config->auth_info.type = IOTC_AT_TPM;
    } else if (strcmp(auth_type->valuestring, "IOTC_AT_TOKEN") == STRINGS_ARE_EQUAL) {
        iotc_config->auth_info.type = IOTC_AT_TOKEN;
    } else {
        printf("unsupported auth type. Aborting\r\n");
        return 1;
    }

    if (cJSON_HasObjectItem(parser, "params") != true){
        //TODO: not sure what to do?
        return 1;
    }

    cJSON *auth_params_parser = NULL;

    auth_params_parser = cJSON_GetObjectItem(parser, "params");

    if (!auth_params_parser) {
        printf("Failed to get auth_type. Aborting\n");
        return 1; 
    }

    if (parse_auth_params(auth_params_parser, iotc_config) != 0){
        printf("failed to parser auth params\r\n");
        return 1;
    }
    

    

}

//TODO: add error checking
int parse_json_config(const char* json_str, IotConnectClientConfig* iotc_config, commands_data_t *local_commands, sensors_data_t *local_sensors, char** board){

    if (!json_str){
        printf("NULL PTR. Aborting\n");
        return 1;
    }

    int ret = 0;

    cJSON *json_parser = NULL;

    cJSON *auth_type = NULL;


    json_parser = cJSON_Parse(json_str);

    if (!json_parser){
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        ret = 1; 
        goto END;
    }

    if (parse_base_params(&iotc_config->duid, "duid", json_parser) != 0){
        printf("Failed to get duid from json file. Aborting.\r\n");
        ret = 1; 
        goto END;
    }

    if (parse_base_params(&iotc_config->cpid, "cpid", json_parser) != 0){
        printf("Failed to get duid from json file. Aborting.\r\n");
        ret = 1; 
        goto END;
    }

    if (parse_base_params(&iotc_config->env, "env", json_parser) != 0){
        printf("Failed to get duid from json file. Aborting.\r\n");
        ret = 1; 
        goto END;
    }

    if (parse_base_params(&iotc_config->auth_info.trust_store, "iotc_server_cert", json_parser) != 0){
        printf("Failed to get iotc_server_cert from json file. Aborting.\r\n");
        ret = 1; 
        goto END;
    }
    
    cJSON *auth_parser = NULL;

    if (cJSON_HasObjectItem(json_parser, "auth") == true){

        auth_parser = cJSON_GetObjectItem(json_parser, "auth");

        if (!auth_parser){
            ret = 1;
            goto END;
        }

        if (parse_auth(auth_parser, iotc_config) != 0) {
            printf("failed to parse auth object\r\n");
            ret = 1;
            goto END;
        }



    } else {
        printf("No auth obj.\r\n");
        // TODO: not sure what to do in this situation
        ret = 1;
        goto END;
    }

    

    cJSON *device_parser = NULL; 

    if (cJSON_HasObjectItem(json_parser, "device") == true){

        device_parser = cJSON_GetObjectItem(json_parser, "device");

        if (!device_parser){
            printf("Failed to get device object from json\r\n");
            ret = 1; 
            goto END;
        }



        
        if (parse_base_params(&board, "name", device_parser) != 0) {
            printf("failed to get board name\r\n");
            ret = 1; 
            goto END;
        }
        


        if (parse_telemetry_settings(device_parser, local_sensors) != 0){
            printf("Failed to parse telemetry settings\r\n");
            ret = 1; 
            goto END;
        }

        if (cJSON_HasObjectItem(device_parser, "commands") == true){

            if (parse_commands(device_parser, local_commands) != 0){
                printf("failed to parse commands. Aborting\r\n");
                ret = 1; 
                goto END;
            }

        //printf("sensor data: name - %s; path - %s\r\n", sensor->s_name, sensor->s_path);
        }


    }

END: 
    cJSON_Delete(json_parser);
    return ret;

}