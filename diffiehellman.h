#ifndef DH_H
#define DH_H

#include <stdio.h>
#include <stdint.h>


// Define the maximum length for the shared key
#define DH_SHARED_KEY_LEN 256

// Structure to hold the DH parameters
typedef struct {
    uint64_t p; // Prime number
    uint64_t g; // Generator
} DH_Params;

// Function to generate DH parameters (p, g, and private key)
DH_Params generate_dh_params();

// Function to generate the public key based on DH parameters
uint64_t generate_public_key(DH_Params dh_params ,uint64_t priv_key);

// Function to compute the shared secret key using DH parameters and public key
uint64_t compute_shared_secret(DH_Params dh_params, uint64_t other_pub_key, uint64_t priv_key);

uint64_t generate_safe_prime(uint64_t min_p);

uint64_t generate_private_key(uint64_t modulus);

void init_random_generator();

#endif
