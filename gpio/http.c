#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <curl/curl.h>

#include "http.h"

CURL *curl;
CURLcode res;

void makehttprequest(char *string) {
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
       char *url; 
       int size = asprintf(&url, "http://127.0.0.1:8080/%s", string);
       if(size == -1) {
         fprintf(stderr, "asprintf() failed: cannot malloc memory.");
       } else {
           curl_easy_setopt(curl, CURLOPT_URL, url);
           curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dummy_write);
           printf( "request: %s\n", url);
           res = curl_easy_perform(curl);
           if(res != CURLE_OK) {
              fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
           }
       }
       curl_easy_cleanup(curl);
       free(url);
    }
    curl_global_cleanup();
}

size_t dummy_write(char *ptr, size_t size, size_t nmemb, void *userdata) {
   return size * nmemb;
}
