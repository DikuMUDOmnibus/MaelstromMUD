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

void curl_json_post args( ( const char* url, const char* payload ) );

/**
 * Write analytics data to Keen.io
 *
 * Required environment variables:
 *  KEENIO_PROJECT_ID
 *  KEENIO_API_KEY
 *
 * Example:
 *  json_t *obj = json_object();
 *  json_object_set_new(obj, "key", json_string( "value" ));
 *  
 * @param obj           JSON object
 * @param collection    Keen.io Collection
 */
void write_analytics( json_t* obj, const char* collection ) {
  char *payload;
  char url[MAX_STRING_LENGTH];

  if ( KEENIO_PROJECT_ID != NULL && KEENIO_API_KEY != NULL ) {
    sprintf(url, "https://api.keen.io/3.0/projects/%s/events/%s?api_key=%s", KEENIO_PROJECT_ID, collection, KEENIO_API_KEY);
    payload = json_dumps( obj, 0 );

    curl_json_post(url, payload);

    free(payload);
  }

  return;
}

/**
 * Trigger a PagerDuty incident
 *
 * Required environment variables:
 *  PAGERDUTY_API_KEY
 *  
 * @param description Description of the incident
 */
void trigger_incident( const char* description ) {
  char *payload;
  json_t *obj = json_object();

  if ( PAGERDUTY_API_KEY != NULL ) {
    json_object_set_new(obj, "service_key", json_string( PAGERDUTY_API_KEY ));
    json_object_set_new(obj, "event_type", json_string( "trigger" ));
    json_object_set_new(obj, "description", json_string( description ));

    payload = json_dumps(obj, 0);
    curl_json_post("https://events.pagerduty.com/generic/2010-04-15/create_event.json", payload);

    free(payload);
  }

  return;
}

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

    curl_json_post(url, payload);

    free(payload);
  }

  return;
}

/**
 * Helper method to make API JSON POST requests
 * a little easier to manage
 * 
 * @param url     The URL
 * @param payload The Payload
 */
void curl_json_post(const char* url, const char* payload) {
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
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);

      res = curl_easy_perform( curl );

      if(res != CURLE_OK) {
        sprintf(log_buf, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        bug(log_buf, 0);
      }
    }
    
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    _exit(0);
  } else if (pid > 0 ) {
    curl_global_cleanup();
  }

  return;
}