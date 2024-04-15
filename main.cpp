#include <cstdint>
#include <cstring>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>

std::string urlEncode(const std::string& input) {
    std::string encodedString;
    const std::string safeChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";

    for (char c : input) {
        if (safeChars.find(c) != std::string::npos) {
            encodedString.push_back(c);
        } else {
            encodedString.push_back('%');
            encodedString.push_back("0123456789ABCDEF"[(c >> 4) & 0x0F]);
            encodedString.push_back("0123456789ABCDEF"[c & 0x0F]);
        }
    }

    return encodedString;
}

// Callback function to handle the response from Factordb
size_t writeCallback(char* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(contents, totalSize);
    return totalSize;
}

//uint64_t extrahiereID(const std::string& input) {
std::string extrahiereID(const std::string& input) {
    // Suchen nach dem Index des ersten Vorkommens von ":"
    size_t start_index = input.find(":");
    if (start_index == std::string::npos)
        return 0; // Rückgabe 0 im Fehlerfall

    // Suchen nach dem Index des ersten Vorkommens von ","
    size_t end_index = input.find(",", start_index);
    if (end_index == std::string::npos)
        return 0; // Rückgabe 0 im Fehlerfall

    // Extrahieren des Substrings, der die ID enthält
    std::string id_str = input.substr(start_index + 1, end_index - start_index - 1);

    // Konvertieren des Substrings in eine uint64_t-Zahl
    //uint64_t id = std::stoull(id_str);

    //return id;
    return id_str;
}

std::string extrahiereStatus(const std::string& input) {
    // Suchen nach dem Index des ersten Vorkommens von "\"status\":\""
    size_t start_index = input.find("\"status\":\"");
    if (start_index == std::string::npos)
        return ""; // Rückgabe eines leeren Strings im Fehlerfall
    //std::cerr << "\nstart_index: " << start_index;
    // Verschieben des Startindex auf das Ende des gefundenen Musters
    start_index += strlen("\"status\":\""); // Die Länge von "\"status\":\"" ist 9

    // Suchen nach dem Index des ersten Vorkommens von "\""
    size_t end_index = input.find("\",\"", start_index);
    if (end_index == std::string::npos)
        return ""; // Rückgabe eines leeren Strings im Fehlerfall
    //std::cerr << "\nend_index: " << end_index << std::endl;

    // Extrahieren des Substrings, der den Status enthält
    std::string status = input.substr(start_index, end_index - start_index);

    return status;
}

int main(int argc, char* argv[])
{
    uint64_t anzAufrufe = 0;
    std::string filename;
    std::ifstream inputFile;
    // Prüfen, ob das Flag --file übergeben wurde
    bool useFile = false;
    if (argc == 2 && std::string(argv[1]) == "--file")
    {
        useFile = true;

        // Zenity-Dialog anzeigen, um den Dateinamen zu erhalten
        FILE* pipe = popen("zenity --file-selection --title=\"Select a file\"", "r");
        if (!pipe)
        {
            std::cerr << "Zenity konnte nicht ausgeführt werden." << std::endl;
            return 1;
        }
        char buffer[512];
        while (!feof(pipe))
        {
            if (fgets(buffer, 512, pipe) != NULL)
                filename += buffer;
        }
        pclose(pipe);
        // Entferne Zeilenumbruch aus dem Dateinamen
        filename.erase(filename.find_last_not_of("\n") + 1);

        // Lese die Zahl aus der Datei "test.txt"
        inputFile.open(filename);
        if (inputFile.is_open())
        {
            //
        } else {
            // Prompt the user to enter a number
            std::cerr << "Datei konnte nicht geöffnet werden." << std::endl;
            return 1;
        }
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
while(true){
    // Create a CURL object for making HTTP requests
    CURL* curl = curl_easy_init();

        if (curl)
        {
            anzAufrufe++;

            std::string input;
            if (useFile)
            {
                std::getline(inputFile, input);
                if (input == "") goto ende;                
            } else {
                std::cout << "Enter a number: ";
                std::getline(std::cin, input);
            }
            auto URLizedInput = urlEncode(input);

            // Construct the URL for Factordb
            std::string url = "http://factordb.com/api?query=" + URLizedInput;
//            std::cout << url << '\n';

            // Set the URL for the HTTP request
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            // Set the callback function to handle the response
            std::string response;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Perform the HTTP request
            CURLcode result = curl_easy_perform(curl);

            // Check if the request was successful
            if (result == CURLE_OK)
            {
                // Print the response from Factordb
                if (useFile)
                {
                    std::cout << "FactorDB Nr.: " << anzAufrufe << " Status: " << extrahiereStatus(response) << "\tAnzahl der Stellen: " << input.size() << "   \tid: " << extrahiereID(response) << std::endl;
                } else {
                    std::cout << "FactorDB response:\n" << response << std::endl;
                }
                
            } else {
                std::cout << "Request failed: " << curl_easy_strerror(result) << std::endl;
            }

        // Cleanup the CURL object
        curl_easy_cleanup(curl);

        //if(useFile) goto ende;
    }}

    ende:

    inputFile.close();

    // Cleanup the libcurl library
    curl_global_cleanup();

    return anzAufrufe;
}
