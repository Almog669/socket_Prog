#include "diffiehellman.h"
#include <stdlib.h>
#include <time.h>
#include <openssl/rand.h>
#include <stdbool.h>
#include <netinet/in.h>

// Define a primitive root modulo p (g)
#define DH_GENERATOR 5

// Function to generate a safe prime number (p)
// A safe prime is of the form 2*q + 1, where q is also a prime number
// This function returns a safe prime greater than or equal to min_p
uint64_t generate_safe_prime(uint64_t min_p) {
    // Ensure min_p is odd
    uint64_t p = (min_p % 2 == 0) ? min_p + 1 : min_p;
    //p += rand() % 100;

    while (1) {
        // Check if p is prime
        int is_prime = 1;
        for (uint64_t i = 2; i * i <= p; ++i) {
            if (p % i == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) {
            // Check if (p - 1) / 2 is prime
            uint64_t q = (p - 1) / 2;
            is_prime = 1;
            for (uint64_t i = 2; i * i <= q; ++i) {
                if (q % i == 0) {
                    is_prime = 0;
                    break;
                }
            }
            if (is_prime) {
                return p;
            }
        }
        // Increment p to the next odd number
        p += 2;
    }
}

// Function to calculate (base ^ exponent) % modulus
uint64_t mod_pow(uint64_t base, uint64_t exponent, uint64_t modulus) {
    uint64_t result = 1;
    base = base % modulus;
    while (exponent > 0) {
        if (exponent & 1) {
            result = (result * base) % modulus;
        }
        exponent = exponent >> 1;
        base = (base * base) % modulus;
    }
    return result;
}

// Function to generate DH parameters (p, g, and private key)
DH_Params generate_dh_params() {
    init_random_generator();
    DH_Params dh_params;
    dh_params.p = generate_safe_prime(time(NULL)); // Minimum safe prime
    dh_params.g = DH_GENERATOR;
    return dh_params;
}

// Function to generate the public key based on DH parameters
uint64_t generate_public_key(DH_Params dh_params ,uint64_t priv_key) {
    uint64_t pub_key = mod_pow(dh_params.g, priv_key, dh_params.p);
    return pub_key;
}

// Function to compute the shared secret key using DH parameters and public key
uint64_t compute_shared_secret(DH_Params dh_params, uint64_t other_pub_key, uint64_t priv_key) {
    uint64_t shared_secret = mod_pow(other_pub_key, priv_key, dh_params.p);
    return shared_secret;
}

void init_random_generator(){
    srand(time(NULL));
}



// Function to generate a private key using a cryptographically secure random number generator
uint64_t generate_private_key(uint64_t modulus) {
    uint64_t private_key;
    do {
        if (RAND_bytes((unsigned char*)&private_key, sizeof(uint64_t)) != 1) {
            fprintf(stderr, "Error generating random bytes\n");
            exit(EXIT_FAILURE);
        }
        private_key %= modulus;
    } while (private_key == 0); // Ensure the private key is not zero
    return private_key;
}

