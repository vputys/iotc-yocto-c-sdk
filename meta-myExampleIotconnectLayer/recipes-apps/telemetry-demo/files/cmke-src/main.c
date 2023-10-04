//
// Copyright: Avnet 2020
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iotconnect_common.h"
#include "iotconnect.h"
#include "cJSON.h"

#include "json_parser.h"

#include "app_config.h"

// windows compatibility
#if defined(_WIN32) || defined(_WIN64)
#define F_OK 0
#include <Windows.h>
#include <io.h>
int usleep(unsigned long usec) {
    Sleep(usec / 1000);
    return 0;
}
#define access    _access_s
#else
#include <unistd.h>
#endif

#define APP_VERSION "00.01.00"
#define STRINGS_ARE_EQUAL 0
#define FREE(x) if ((x)) { free(x); (x) = NULL; }

#undef COMMAND_FUNCTION_PTR_PARSING

typedef struct cert_struct {

    char* x509_id_cert;
    char* x509_id_key;

} cert_struct_t;



typedef enum command_type
{
    ECHO = 1,
    LED = 2,
    COMMANDS_END
} command_type_t;

const char *command_strings[] = {
    [ECHO] = "echo ",
    [LED] = "led ",
};



typedef struct local_data {

    char* board_name;
    sensors_data_t sensors;
    commands_data_t commands;

} local_data_t;

static local_data_t local_data = {0};

#define DOES_COMMAND_MATCH(input_str, command_enum) (strncmp((input_str), command_strings[(command_enum)], strlen(command_strings[(command_enum)])) == STRINGS_ARE_EQUAL)

static void free_sensor_data(sensors_data_t *sensors);

// MOVE THIS LATER -afk
static void publish_message(const char* key_str,const char* value_str);

static void on_connection_status(IotConnectConnectionStatus status) {
    // Add your own status handling
    switch (status) {
        case IOTC_CS_MQTT_CONNECTED:
            printf("IoTConnect Client Connected\n");
            break;
        case IOTC_CS_MQTT_DISCONNECTED:
            printf("IoTConnect Client Disconnected\n");
            break;
        default:
            printf("IoTConnect Client ERROR\n");
            break;
    }
}


static void command_status(IotclEventData data, const char *command_name) {

    int command_type = 0;

    bool success = false;

    if (strcmp(command_name, "Internal error") == STRINGS_ARE_EQUAL){
        printf("Internal error (null ptr command)\r\n");
    }

    const char* message = success ? "OK" : "Failed_or_not_implemented";

    const char *ack = iotcl_create_ack_string_and_destroy_event(data, success, message);
    printf("command: %s status=%s: %s\n", command_name, success ? "OK" : "Failed", message);
    printf("Sent CMD ack: %s\n", ack);
    iotconnect_sdk_send_packet(ack);
    free((void *)ack);
}

static void on_command(IotclEventData data) {
    char *command = iotcl_clone_command(data);
    if (NULL != command) {
        command_status(data, command);
        free((void *) command);
    } else {
        command_status(data, "Internal error");
    }
}

static bool is_app_version_same_as_ota(const char *version) {
    return strcmp(APP_VERSION, version) == 0;
}

static bool app_needs_ota_update(const char *version) {
    return strcmp(APP_VERSION, version) < 0;
}

static void on_ota(IotclEventData data) {
    const char *message = NULL;
    char *url = iotcl_clone_download_url(data, 0);
    bool success = false;
    if (NULL != url) {
        printf("Download URL is: %s\n", url);
        const char *version = iotcl_clone_sw_version(data);
        if (is_app_version_same_as_ota(version)) {
            printf("OTA request for same version %s. Sending success\n", version);
            success = true;
            message = "Version is matching";
        } else if (app_needs_ota_update(version)) {
            printf("OTA update is required for version %s.\n", version);
            success = false;
            message = "Not implemented";
        } else {
            printf("Device firmware version %s is newer than OTA version %s. Sending failure\n", APP_VERSION,
                   version);
            // Not sure what to do here. The app version is better than OTA version.
            // Probably a development version, so return failure?
            // The user should decide here.
            success = false;
            message = "Device firmware version is newer";
        }

        free((void *) url);
        free((void *) version);
    } else {
        // compatibility with older events
        // This app does not support FOTA with older back ends, but the user can add the functionality
        const char *command = iotcl_clone_command(data);
        if (NULL != command) {
            // URL will be inside the command
            printf("Command is: %s\n", command);
            message = "Old back end URLS are not supported by the app";
            free((void *) command);
        }
    }
    const char *ack = iotcl_create_ack_string_and_destroy_event(data, success, message);
    if (NULL != ack) {
        printf("Sent OTA ack: %s\n", ack);
        iotconnect_sdk_send_packet(ack);
        free((void *) ack);
    }
}


static void publish_telemetry(sensors_data_t sensors) {
    IotclMessageHandle msg = iotcl_telemetry_create();

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);
    iotcl_telemetry_set_number(msg, "cpu", 3.123); // test floating point numbers
    
    for (int i = 0; i < sensors.size; i++){
        iotcl_telemetry_set_number(msg, sensors.sensor[i].s_name, sensors.sensor[i].reading);
    }

    const char *str = iotcl_create_serialized_string(msg, false);
    iotcl_telemetry_destroy(msg);
    printf("Sending: %s\n", str);
    iotconnect_sdk_send_packet(str); // underlying code will report an error
    iotcl_destroy_serialized(str);
}


static void publish_message(const char* key_str,const char* value_str) {
    IotclMessageHandle msg = iotcl_telemetry_create();

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, key_str, value_str);

    const char *str = iotcl_create_serialized_string(msg, false);
    iotcl_telemetry_destroy(msg);
    printf("Sending: %s\n", str);
    iotconnect_sdk_send_packet(str); // underlying code will report an error
    iotcl_destroy_serialized(str);
}






static void free_local_data() {

    printf("freeing local data\r\n");

    for (int i = 0; i < local_data.commands.counter; i++){
        
#ifdef COMMAND_FUNCTION_PTR_PARSING
        if (local_data.commands.commands[i].f_ptr){
            free(local_data.commands.commands[i].f_ptr);
            local_data.commands.commands[i].f_ptr = NULL;
        }
#endif

        if (local_data.commands.commands[i].name){
            free(local_data.commands.commands[i].name);
            local_data.commands.commands[i].name = NULL;
        }

        if (local_data.commands.commands[i].private_data){
            free(local_data.commands.commands[i].private_data);
            local_data.commands.commands[i].private_data = NULL;
        }

    }

    if (local_data.board_name){
        free(local_data.board_name);
        local_data.board_name = NULL;
    }

    free_sensor_data(&local_data.sensors);

}

static void free_sensor_data(sensors_data_t *sensors) {

    printf("freeing sensor data\r\n");

    for (int i = 0; i < sensors->size; i++){
        if (sensors->sensor[i].s_path){
            free(sensors->sensor[i].s_path);
            sensors->sensor[i].s_path = NULL;
        }

        if (sensors->sensor[i].s_path){
            free(sensors->sensor[i].s_path);
            sensors->sensor[i].s_path = NULL;
        }


    }

    if (sensors->sensor){
        free(sensors->sensor);
        sensors->sensor = NULL;
    }

}

static void free_iotc_config(IotConnectClientConfig* iotc_config) {
    
    printf("freeing iotconnect conf\r\n");

    if (iotc_config->cpid){
        free(iotc_config->cpid);
        iotc_config->cpid = NULL;
    }


    if (iotc_config->duid){
        free(iotc_config->duid);
        iotc_config->duid = NULL;
    }


    if (iotc_config->env){
        free(iotc_config->env);
        iotc_config->env = NULL;
    }

    if (iotc_config->auth_info.trust_store){
        free(iotc_config->auth_info.trust_store);
        iotc_config->auth_info.trust_store = NULL;
    }


    if (iotc_config->auth_info.data.cert_info.device_cert){
        free(iotc_config->auth_info.data.cert_info.device_cert);
        iotc_config->auth_info.data.cert_info.device_cert = NULL;
    }


    if (iotc_config->auth_info.data.cert_info.device_key){
        free(iotc_config->auth_info.data.cert_info.device_key);
        iotc_config->auth_info.data.cert_info.device_key = NULL;
    }


    if (iotc_config->auth_info.data.symmetric_key){
        free(iotc_config->auth_info.data.symmetric_key);
        iotc_config->auth_info.data.symmetric_key = NULL;
    }


}


// TODO: currently will only read first 5 characters from specified file
static int read_sensor(sensor_info_t sensor_data){

    char buff[6];

    if(access(sensor_data.s_path, F_OK) != 0){
        printf("failed to access sensor file - %s ; Aborting\n", sensor_data.s_path);
        return 1;
    }

    FILE* fd = NULL;
    int reading = 0;

        
    fd = fopen(sensor_data.s_path, "r");

    //TODO: magic number
    for (int i = 0; i < 5; i++){
        buff[i] = fgetc(fd);
    }

    buff[5] = '\0';

    fclose(fd);

    reading = (int)atof(buff);

    return reading;
}

static bool string_ends_with(const char * needle, const char* haystack)
{
    const char *str_end = haystack + strlen(haystack) -  strlen(needle);
    return (strncmp(str_end, needle, strlen(needle) ) == 0);
}

int main(int argc, char *argv[]) {
    if (access(IOTCONNECT_SERVER_CERT, F_OK) != 0) {
        fprintf(stderr, "Unable to access IOTCONNECT_SERVER_CERT. "
               "Please change directory so that %s can be accessed from the application or update IOTCONNECT_CERT_PATH\n",
               IOTCONNECT_SERVER_CERT);
    }

    char* input_json_file = NULL;

    sensors_data_t sensors;
    sensors.size = 0;

    IotConnectClientConfig *config = iotconnect_sdk_init_and_get_config();
    // leaving this non-modifiable or now
    // 

    if (argc == 2) {
        // assuming only 1 parameters for now
        local_data.commands.counter = 0;

        if (!string_ends_with(".json", argv[1]))
        {
            printf("File extension is not .json of filename %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    
        input_json_file = argv[1];

        printf("file: %s\n", input_json_file);

        if(access(input_json_file, F_OK) != 0){
            printf("failed to access input json file - %s ; Aborting\n", input_json_file);
            return 1;
        }

        FILE *fd = fopen(input_json_file, "r");
        if (!fd)
        {
            printf("File failed to open - %s", input_json_file);
            return EXIT_FAILURE;
        }
        fseek(fd, 0l, SEEK_END);
        long file_len = ftell(fd);

        if (file_len <= 0){
            printf("failed calculating file length: %ld. Aborting\n", file_len);
            return 1;
        }

        rewind(fd);


        char* json_str = (char*)calloc(file_len+1, sizeof(char));

        if (!json_str) {
            printf("failed to calloc. Aborting\n");
            json_str = NULL;
            return 1;
        }

        for (int i = 0; i < file_len; i++){
            json_str[i] = fgetc(fd);
        }
        //printf ("end str: \n%s\n", json_str);

        fclose(fd);

        if (parse_json_config(json_str, config, &local_data.commands, &local_data.sensors, &local_data.board_name) != 0) {
            printf("Failed to parse input JSON file. Aborting\n");
            if (json_str != NULL) {
                free(json_str);
                json_str = NULL;
            }
            free_local_data();
            free_iotc_config(config);


            return 1;
        }
        
        printf("DUID in main: %s\r\n", config->duid);

        

        if (config->auth_info.type == IOTC_AT_X509){
            printf("id cert path: {%s}\n", config->auth_info.data.cert_info.device_cert);
            printf("id key path: {%s}\n", config->auth_info.data.cert_info.device_key);
            if(access(config->auth_info.data.cert_info.device_cert, F_OK) != 0){
                printf("failed to access parameter 1 - %s ; Aborting\n", config->auth_info.data.cert_info.device_cert);
                return 1;
            }
            

            if(access(config->auth_info.data.cert_info.device_key, F_OK) != 0){
                printf("failed to access parameter 2 - %s ; Aborting\n", config->auth_info.data.cert_info.device_key);
                return 1;
            }
        }
        
        /*
        for (int i = 0; i < local_data.sensors.size; i++){
            printf("id: %d;\n name %s;\n path %s;\r\n_____________\r\n", i, local_data.sensors.sensor[i].s_name, local_data.sensors.sensor[i].s_path);
        }
        */

#ifdef COMMAND_FUNCTION_PTR_PARSING

/*
        for (int i = 0; i < local_data.commands.counter; i++) {
            printf("name: %s\r\nprivate_data: %s\r\n", local_data.commands.commands[i].name, (char*)local_data.commands.commands[i].f_ptr, local_data.commands.commands[i].private_data);
        }
*/
#endif


    } else {
        

        if (IOTCONNECT_AUTH_TYPE == IOTC_AT_X509) {
            if (access(IOTCONNECT_IDENTITY_CERT, F_OK) != 0 ||
                access(IOTCONNECT_IDENTITY_KEY, F_OK) != 0
                    ) {
                fprintf(stderr, "Unable to access device identity private key and certificate. "
                    "Please change directory so that %s can be accessed from the application or update IOTCONNECT_CERT_PATH\n",
                    IOTCONNECT_SERVER_CERT);
            }
        }

        
        config->cpid = IOTCONNECT_CPID;
        config->env = IOTCONNECT_ENV;
        config->duid = IOTCONNECT_DUID;
        config->auth_info.type = IOTCONNECT_AUTH_TYPE;
        config->auth_info.trust_store = IOTCONNECT_SERVER_CERT;

        if (config->auth_info.type == IOTC_AT_X509) {
            config->auth_info.data.cert_info.device_cert = IOTCONNECT_IDENTITY_CERT;
            config->auth_info.data.cert_info.device_key = IOTCONNECT_IDENTITY_KEY;
        } else if (config->auth_info.type == IOTC_AT_TPM) {
            config->auth_info.data.scope_id = IOTCONNECT_SCOPE_ID;
        } else if (config->auth_info.type == IOTC_AT_SYMMETRIC_KEY){
            config->auth_info.data.symmetric_key = IOTCONNECT_SYMMETRIC_KEY;
        } else if (config->auth_info.type != IOTC_AT_TOKEN) { // token type does not need any secret or info
            // none of the above
            fprintf(stderr, "IOTCONNECT_AUTH_TYPE is invalid\n");
            return -1;
        }
    }

    config->status_cb = on_connection_status;
    config->ota_cb = on_ota;
    config->cmd_cb = on_command;


    int reading = 0;

    // run a dozen connect/send/disconnect cycles with each cycle being about a minute
    for (int j = 0; j < 10; j++) {
        int ret = iotconnect_sdk_init();
        if (0 != ret) {
            fprintf(stderr, "IoTConnect exited with error code %d\n", ret);
            return ret;
        }

        // send 10 messages
        for (int i = 0; iotconnect_sdk_is_connected() && i < 10; i++) {
            for (int i = 0; i < local_data.sensors.size; i++){
                local_data.sensors.sensor[i].reading = read_sensor(local_data.sensors.sensor[i]);
            }
                
            publish_telemetry(local_data.sensors);
            // repeat approximately evey ~5 seconds
            for (int k = 0; k < 500; k++) {
                iotconnect_sdk_receive();
                usleep(10000); // 10ms
            }
        }
        iotconnect_sdk_disconnect();
    }

    free_iotc_config(config);

    free_local_data();

    return 0;
}

