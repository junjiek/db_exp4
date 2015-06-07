#include "kit.h"
#include "json/json.h"
#include "trie.h"
#include "knn.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

vector<POI> poi_list;
Trie trie;

vector<string> GetKeywords(const string& input) {
    vector<string> keywords;
    string keyword;
    for (int i = 0; i < (int)input.length(); i++) {
        int c = MapChar(input[i]);
        if (c == kInvalidChar) {
            if (!keyword.empty()) {
                keywords.push_back(keyword);
                keyword.clear();
            }
        } else {
            keyword.push_back(input[i]);
        }
    }
    if (!keyword.empty())
        keywords.push_back(keyword);
    return keywords;
}

int poi_compare(const POI& a, const POI& b) {
    return a.lat_ < b.lat_;
}

bool ReadJson(char* file_name) {
    ifstream fin(file_name);
    string str;

    poi_list.clear();
    trie.Clear();

    cout << "server: Reading json from " << file_name << "..." << endl;
    while (getline(fin, str)) {
        stringstream stream(str);
        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(stream, root, false))
            continue;

        string name = root["name"].asString();
        Json::Value pos = root["latlng"];
        double lat = pos[(Json::Value::UInt)0].asDouble();
        double lng = pos[(Json::Value::UInt)1].asDouble();
        string addr = root["addr"].asString();
        poi_list.push_back(POI(name, lat, lng, addr));
    }
    fin.close();
    cout << "server: Finish loading data" << endl;

    sort(poi_list.begin(), poi_list.end(), poi_compare);

    cout << "server: Building trie..." << endl;
    for (unsigned poi_index = 0; poi_index < poi_list.size(); ++poi_index) {
        string& name = poi_list[poi_index].name_;
        vector<string> keywords = GetKeywords(name);
        vector<string>::iterator iter = keywords.begin();
        for (; iter != keywords.end(); ++iter) {
            trie.Push(*iter, poi_index);
        }
    }
    cout << "server: Finish building trie-tree." << endl;
    return true;
}

// Intersection of 2 sorted list.
vector<unsigned> IntersectRefs(const vector<unsigned>& refs_a,
                               const vector<unsigned>& refs_b) {
    vector<unsigned> refs;
    unsigned index_a = 0;
    unsigned index_b = 0;
    while (index_a < refs_a.size() && index_b < refs_b.size()) {
        if (refs_a[index_a] == refs_b[index_b]) {
            refs.push_back(refs_a[index_a]);
            index_a++;
            index_b++;
        } else if (refs_a[index_a] < refs_b[index_b]) {
            index_a++;
        } else {
            index_b++;
        }
    }
    return refs;
}

double cached_lat;
double cached_lng;
unsigned cached_num;
string cached_input;
vector<POI> cached_pois;

vector<string> cached_keywords;
vector<vector<unsigned> > cached_refss;

vector<POI> Search(double lat, double lng, const string& input,
                   unsigned num) {
    if (lat == cached_lat && lng == cached_lng && input == cached_input &&
        num == cached_num) return cached_pois;
    cached_input = input;

    // Find the first different position.
    vector<string> keywords = GetKeywords(input);
    unsigned index = 0;
    for (; index < keywords.size() && index < cached_keywords.size(); ++index) {
        if (keywords[index] != cached_keywords[index])
            break;
    }

    if (keywords.size() == cached_keywords.size() && index == keywords.size()) {
        // Same keywords, no needs to update refs.
        if (lat == cached_lat && lng == cached_lng && num == cached_num)
            return cached_pois;

        cached_lat = lat;
        cached_lng = lng;
        cached_num = num;
    } else {
        cached_lat = lat;
        cached_lng = lng;
        cached_num = num;

        for (int i = (int)cached_keywords.size()-1; i >= (int)index; --i) {
            cached_keywords.pop_back();
            cached_refss.pop_back();
        }
        if (cached_refss.size() > 0 && cached_refss.back().size() == 0) {
            return cached_pois;
        }

        for (unsigned i = index; i < keywords.size(); ++i) {
            cached_keywords.push_back(keywords[i]);
            const vector<unsigned>* refs = trie.GetRefs(keywords[i]);
            if (refs == NULL) {
                cached_refss.push_back(vector<unsigned>());
                cached_pois = vector<POI>();
                return cached_pois;
            }
            if (cached_refss.size() == 0)
                cached_refss.push_back(*refs);
            else
                cached_refss.push_back(IntersectRefs(cached_refss.back(), *refs));
        }
    }

    const vector<unsigned>& u_refs = cached_refss.back();
    cout << "Matched " << u_refs.size() << " entries." << endl;

    vector<POI> matched_poi_list;
    for (unsigned index = 0; index < u_refs.size(); ++index)
        matched_poi_list.push_back(poi_list.at(u_refs[index]));
    cached_pois = Naive(lat, lng, matched_poi_list, num);
    return cached_pois;
}
