/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#define unix 1

#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include "merc.h"

void curl_json_push args( ( const char* url, const char* payload, const char* method ) );

/**
 * Report an issue to the GitHub repository
 *
 * @param title       Issue title
 * @param description Issue description
 * @param label       Issue label
 */
void report_issue( const char* title, const char* description, const char* label ) {
  char *payload;
  char url[MAX_STRING_LENGTH];

  json_t *obj = json_object();
  json_t *labels = json_array();

  if ( GITHUB_ACCESS_TOKEN != NULL && GITHUB_REPO_NAME != NULL ) {
    json_object_set_new(obj, "title", json_string( title ));

    if ( description != NULL ) {
      json_object_set_new(obj, "body", json_string( description ));
    }

    if ( label != NULL ) {
      json_array_append_new(labels, json_string( label ));
      json_object_set_new(obj, "labels", labels);
    }

    sprintf(url, "https://api.github.com/repos/%s/issues?access_token=%s", GITHUB_REPO_NAME, GITHUB_ACCESS_TOKEN);
    payload = json_dumps(obj, 0);

    curl_json_push(url, payload, "POST");

    free(payload);
  }

  return;
}

/**
 * Close a GitHub Issue
 *
 * @param number Issue number
 */
void close_issue( int number ) {
  char *payload;
  char url[MAX_STRING_LENGTH];
  json_t *obj = json_object();

  if ( GITHUB_ACCESS_TOKEN != NULL && GITHUB_REPO_NAME != NULL ) {
    json_object_set_new(obj, "state", json_string( "closed" ));

    payload = json_dumps(obj, 0);
    sprintf(url, "https://api.github.com/repos/%s/issues/%d?access_token=%s", GITHUB_REPO_NAME, number, GITHUB_ACCESS_TOKEN);
    curl_json_push(url, payload, "PATCH");

    free(payload);
  }

  return;
}

/**
 * Get GitHub issues
 *
 * @param  label Issue label
 * @return       JSON object
 */
json_t * get_issues( const char* label ) {
  char url[MAX_STRING_LENGTH];
  const char * response;
  json_error_t error;

  json_t * issues = json_array();

  if ( GITHUB_ACCESS_TOKEN != NULL && GITHUB_REPO_NAME != NULL ) {
    sprintf(url, "https://api.github.com/repos/%s/issues?access_token=%s&labels=%s", GITHUB_REPO_NAME, GITHUB_ACCESS_TOKEN, label);
    response = curl_get(url);
    issues = json_loads(response, 0, &error);

  	if(!issues) {
  		sprintf(log_buf, "get_issues(): JSON error on line %d: %s\n\r", error.line, error.text);
  		bug(log_buf, 0);
  	}
  }

  return issues;
}

/**
 * Helper method to make API JSON POST requests
 * a little easier to manage
 *
 * @param url     The URL
 * @param payload The Payload
 */
void curl_json_push(const char* url, const char* payload, const char* method) {
  pid_t pid;

  curl_global_init(CURL_GLOBAL_ALL);

  pid = fork();

  if ( pid == 0 ) {
    CURL *curl;
    struct curl_slist *headers = NULL;

    if ( (curl = curl_easy_init()) ) {
      CURLcode res;

      headers = curl_slist_append(headers, "Content-Type: application/json");
      headers = curl_slist_append(headers, "User-Agent: OASIS MUD");

      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);

      res = curl_easy_perform( curl );

      if(res != CURLE_OK) {
        sprintf(log_buf, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        bug(log_buf, 0);
      }

      curl_easy_cleanup(curl);
      curl_slist_free_all(headers);
    }

    _exit(0);
  } else if (pid > 0 ) {
    curl_global_cleanup();
  }

  return;
}

/**
 * Helper method to make API JSON GET requests
 * a little easier to manage
 *
 * @param url     The URL
 */
char * curl_get(const char* url) {
  curl_global_init(CURL_GLOBAL_ALL);

  CURL *curl;
  struct curl_slist *headers = NULL;
  struct curl_data data;

  data.size = 0;

   /* reasonable size initial buffer */
  data.data = malloc(4096);

  if(NULL == data.data) {
      sprintf(log_buf, "Failed to allocate memory.\n");
      bug(log_buf, 0);
      return NULL;
  }

  data.data[0] = '\0';

  if ( (curl = curl_easy_init()) ) {
    CURLcode res;

    headers = curl_slist_append(headers, "User-Agent: OASIS MUD");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);

    res = curl_easy_perform( curl );

    if(res != CURLE_OK) {
      sprintf(log_buf, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      bug(log_buf, 0);
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }

  curl_global_cleanup();

  return data.data;
}

size_t curl_callback(void *ptr, size_t size, size_t nmemb, struct curl_data *data) {
  size_t index = data->size;
  size_t n = (size * nmemb);
  char* tmp;

  data->size += (size * nmemb);
  tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

  if(tmp) {
      data->data = tmp;
  } else {
      if(data->data) {
          free(data->data);
      }

      sprintf(log_buf, "Failed to allocate memory.\n");
      bug(log_buf, 0);
      return 0;
  }

  memcpy((data->data + index), ptr, n);
  data->data[data->size] = '\0';

  return size * nmemb;
}
