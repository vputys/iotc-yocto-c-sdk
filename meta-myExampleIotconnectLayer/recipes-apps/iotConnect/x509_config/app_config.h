//
// Copyright: Avnet 2020
// Created by Nik Markovic <nikola.markovic@avnet.com> on 6/28/21.
//
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "iotconnect.h"

#define IOTCONNECT_CPID "avtds"

#define IOTCONNECT_ENV "avnetpoc"

// Device Unique ID
// If using TPM, and this value is a blank string, Registration ID will be used from output of tpm_device_provision. Otherwise, the provide Device Uinque ID will be used.
#define IOTCONNECT_DUID "CSDKYoctoVladSelfSigned"
//linuxDemoVladx509
//#define IOTCONNECT_DUID "justatoken"

// from iotconnect.h IotConnectAuthType IOTC_AT_SYMMETRIC_KEY
//#define IOTCONNECT_AUTH_TYPE IOTC_AT_X509
#define IOTCONNECT_AUTH_TYPE IOTC_AT_X509 

// if using Symmetric Key based authentication, provide the primary or secondary key here:
//#define IOTCONNECT_SYMMETRIC_KEY "MDEyMzQ1Njc4OWFiY2RlZg=="
#define IOTCONNECT_SYMMETRIC_KEY ""

// If using TPM, provide the Scope ID here:
#define IOTCONNECT_SCOPE_ID ""// AKA ID Scope.

#define IOTCONNECT_CERT_PATH "/etc/ssl/certs"
#define IOTC_TEST_CERT_PATH "/home/vlad"

// This is the CA Certificate used to validate the IoTHub TLS Connection and it is required for all authentication types.
// Alternatively, you can point this file to /etc/ssl/certs/Baltimore_CyberTrust_Root.pem on some Linux systems
#define IOTCONNECT_SERVER_CERT ("/etc/ssl/certs/server.pem")

// if IOTC_X509 is used: "/client-crt.pem"
#define IOTCONNECT_IDENTITY_CERT (IOTC_TEST_CERT_PATH "/client1.pem")
#define IOTCONNECT_IDENTITY_KEY (IOTC_TEST_CERT_PATH "/client1-key.pem")

#endif //APP_CONFIG_H
