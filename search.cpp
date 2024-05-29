#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <curl/curl.h>

using namespace std;

/* Timeout for the request in seconds */
const long TIMEOUT = 30;

class Site
{
public:
    string url;
    int wordNumOfOccurrences;
};

class SiteAnalyzer
{
public:
    SiteAnalyzer(const string &filename);

    void analyze(const string &keyword);

    void printResults();

private:
    vector<Site> sites;

    void fillSiteArray(const string &filename);

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *buffer);

    int countWordOccurrencesInSite(const string &buffer, const string &word);

    static bool compareByWordNumOfOccurrences(const Site &s1, const Site &s2);
};

SiteAnalyzer::SiteAnalyzer(const string &filename)
{
    fillSiteArray(filename);
}

void SiteAnalyzer::fillSiteArray(const string &filename)
{
    ifstream dataFile(filename);
    if (!dataFile.is_open())
    {
        cerr << "Failed opening file. Exiting!\n";
        exit(1);
    }

    Site site;
    while (dataFile >> site.url)
    {
        sites.push_back(site);
    }

    dataFile.close();
}

int SiteAnalyzer::countWordOccurrencesInSite(const string &buffer, const string &word)
{
    int wordCounter = 0;
    size_t pos = 0;

    while ((pos = buffer.find(word, pos)) != string::npos)
    {
        wordCounter++;
        pos += word.length();
    }
    return wordCounter;
}

void SiteAnalyzer::analyze(const string &keyword)
{
    curl_global_init(CURL_GLOBAL_ALL);

    // libcurl easy handle
    CURL *curl = curl_easy_init();

    if (curl)
    {
        // set options _
        // curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
        // Follow redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);

        for (auto &site : sites)
        {
            string url = site.url;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            string buffer;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

            CURLcode res = curl_easy_perform(curl);

            if (res == CURLE_OK) site.wordNumOfOccurrences = countWordOccurrencesInSite(buffer, keyword);
            else cerr << "Failed to fetch content for " << site.url << endl;
        }

        curl_easy_cleanup(curl);
    }
    //g cleanup
    curl_global_cleanup();
}

size_t SiteAnalyzer::WriteCallback(void *contents, size_t size, size_t nmemb, string *buffer)
{
    size_t total_size = size * nmemb;
    buffer->append((char *)contents, total_size);
    return total_size;
}

void SiteAnalyzer::printResults()
{
    sort(sites.begin(), sites.end(), compareByWordNumOfOccurrences);

    for (const auto &site : sites)
    {
        cout << site.url << " " << site.wordNumOfOccurrences << endl;
    }
}

// sort based on word occurrences
bool SiteAnalyzer::compareByWordNumOfOccurrences(const Site &s1, const Site &s2)
{
    return s1.wordNumOfOccurrences > s2.wordNumOfOccurrences;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <keyword> <datafile>\n";
        return 1;
    }

    SiteAnalyzer analyzer(argv[2]);
    analyzer.analyze(argv[1]);
    analyzer.printResults();

    return 0;
}
