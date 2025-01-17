// email_sender.c
#include "email_sender.h"
#include <curl/curl.h>

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    FILE *file = (FILE *)stream;
    size_t retcode = fread(ptr, size, nmemb, file);
    return retcode;
}

int send_email_with_attachment(const char *to, const char *cc, const char *file_path) {
    CURL *curl;
    CURLcode res = CURLE_OK;

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", file_path);
        return 1;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist *recipients = NULL;

        // Specify email server details
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com");
        // here we can create pass code
        curl_easy_setopt(curl, CURLOPT_USERPWD, "naqvishehroze@gmail.com:tktm mppv fdwb vgfe");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // Add email recipients
        if (to)
            recipients = curl_slist_append(recipients, to);
        if (cc)
            recipients = curl_slist_append(recipients, cc);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Set the read callback function to read the file content
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, file);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Perform the email sending
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        // Clean up
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    // Close the file
    fclose(file);

    curl_global_cleanup();
    return (int)res;
}

