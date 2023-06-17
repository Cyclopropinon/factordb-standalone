#include <iostream>
#include <string>
#include <curl/curl.h>

// Callback function to handle the response from Factordb
size_t writeCallback(char* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(contents, totalSize);
    return totalSize;
}

int main() {
    // Initialize the libcurl library
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a CURL object for making HTTP requests
    CURL* curl = curl_easy_init();

    if (curl) {
        // Prompt the user to enter a number
        std::cout << "Enter a number: ";
        std::string input;
        std::cin >> input;

        // Construct the URL for Factordb
        std::string url = "http://factordb.com/api?query=" + input;

        // Set the URL for the HTTP request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to handle the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the HTTP request
        CURLcode result = curl_easy_perform(curl);

        // Check if the request was successful
        if (result == CURLE_OK) {
            // Print the response from Factordb
            std::cout << "Factordb response:\n" << response << std::endl;
        } else {
            std::cout << "Request failed: " << curl_easy_strerror(result) << std::endl;
        }

        // Cleanup the CURL object
        curl_easy_cleanup(curl);
    }

    // Cleanup the libcurl library
    curl_global_cleanup();

    return 0;
}
