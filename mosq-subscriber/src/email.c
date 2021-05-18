#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "email.h"

char payload_text[512];

struct upload_status {
  int lines_read;
};

static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;

  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }

  if (upload_ctx->lines_read == 0) {

    data = payload_text;

    size_t len = strlen(data);
    memcpy(ptr, data, len);
    upload_ctx->lines_read++;

    return len;
  }
  return 0;
}


int send_email(const char* message, char* receiver, struct Email *email)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx;

  upload_ctx.lines_read = 0;


  curl = curl_easy_init();
  if(curl) {
    /* Change payload text */
    snprintf(payload_text, 512, "To: A receiver <%s> From: Router <%s>\r\nSubject: Event received\r\n\r\n%s\r\n", receiver, email->sender_email, message);

    curl_easy_setopt(curl, CURLOPT_USERNAME, email->username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, email->password);

    /* This is the URL for your mailserver */
    char mailserver_url[512];
    if (email->secure_conn == 1) {
      snprintf(mailserver_url, 512, "smtps://%s:%d", email->smtp_ip, email->smtp_port);
      curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    } else {
      snprintf(mailserver_url, 512, "smtp://%s:%d", email->smtp_ip, email->smtp_port);
    }

    curl_easy_setopt(curl, CURLOPT_URL, mailserver_url);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, email->sender_email);

    /* Add recipient */
    recipients = curl_slist_append(recipients, receiver);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    /* Specify payload */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    /* Send the message */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    /* Free the list of recipients */
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    free(payload_text);
  }

  return (int)res;
}
