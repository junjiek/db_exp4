#include "search.h"
#include "json/json.h"
#include "RTree.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <queue>

using namespace std;
struct Rect {
    Rect()  {}

    Rect(double a_minX, double a_minY, double a_maxX, double a_maxY) {
        min[0] = a_minX;
        min[1] = a_minY;

        max[0] = a_maxX;
        max[1] = a_maxY;
    }
    void print() {
        printf("xmin: %.4lf, ymin: %.4lf, xmax: %.4lf, ymax: %.4lf\n",
                min[0], min[1], max[0], max[1]);
    }
    bool operator == (const Rect& r) {
        if(r.min[0] == min[0] && r.min[1] == min[1] && r.max[0] == max[0]
            && r.max[1] == max[1]) {
            return true;
        }
        return false;
    }
    double min[2];
    double max[2];
};

vector<POI> poi_list;
unordered_map<string, int> wordSet;
vector<string> dict;
vector<vector<int> > wordLists;
RTree<int, double, 2> rtree;

bool validChar(char c) {
    if (c >= 'a' && c <= 'z')
        return true;
    if (c >= 'A' && c <= 'Z')
        return true;
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

vector<string> GetKeywords(const string& input) {
    vector<string> keywords;
    string keyword;
    for (int i = 0; i < (int)input.length(); i++) {
        if (!validChar(input[i]) && !keyword.empty()) {
            keywords.push_back(keyword);
            keyword.clear();
        } else {
            keyword.push_back(input[i]);
        }
    }
    if (!keyword.empty())
        keywords.push_back(keyword);
    return keywords;
}

bool ReadJson(char* file_name) {
    ifstream fin(file_name);
    string str;

    poi_list.clear();

    cout << "server: Reading json from " << file_name << " ..." << endl;
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
    cout << "server: Finish loading data." << endl;

    wordSet.clear();
    dict.clear();
    cout << "server: Building dict ..." << endl;
    for (unsigned poi_index = 0; poi_index < poi_list.size(); ++poi_index) {
        string& name = poi_list[poi_index].name_;
        vector<string> keywords = GetKeywords(name);
        vector<string>::iterator iter = keywords.begin();
        vector<int> wlist;
        for (; iter != keywords.end(); ++iter) {
            if (wordSet.find(*iter) == wordSet.end()) {
                wordSet[*iter] = dict.size();
                dict.push_back(*iter);
            }
            wlist.push_back(wordSet[*iter]);
        }
        wordLists.push_back(wlist);
    }
    cout << "server: Finish building dict." << endl;

    cout << "server: Building rtree ..." << endl;
    for (unsigned poi_index = 0; poi_index < poi_list.size(); ++poi_index) {
        Rect node(poi_list[poi_index].lat_, poi_list[poi_index].lng_,
                  poi_list[poi_index].lat_, poi_list[poi_index].lng_);
        rtree.Insert(node.min, node.max, poi_index);
    }
    cout << "server: Finish building rtree." << endl;

    return true;
}


Rect cached_bound;
unsigned cached_num;
// vector<POI> cached_pois;
vector<pair<string, int> > cached_tags;
vector<unsigned> hit_pois;

bool MySearchCallback(int id, void* arg) {
    hit_pois.push_back(id);
    return true; // keep going
}

class PairCmp {
public:
    bool operator() (pair<int, int> p1, pair<int, int> p2) {
        return p1.second < p2.second;
    }
};

vector<pair<string, int> > Search(unsigned num, double xmin, double ymin, double xmax, double ymax) {
    Rect bound(xmin, ymin, xmax, ymax);
    if (num == cached_num && bound == cached_bound)
        return cached_tags;
    hit_pois.clear();
    rtree.Search(bound.min, bound.max, MySearchCallback, NULL);
    cout << "Contained " << hit_pois.size() << " pois." << endl;

    cached_tags.clear();
    unordered_map<int, int> wordCnt; // wordNum, frequency
    for (unsigned index : hit_pois) {
        for (int wordNum : wordLists[index]) {
            wordCnt[wordNum] ++;
        }
    }
    priority_queue<pair<int, int>, vector<pair<int, int> >, PairCmp> pq;
    for (auto& entry: wordCnt) {
        pq.push(make_pair(entry.first, entry.second));
    }
    for (unsigned i = 0; i < num; i++) {
        string w = dict[pq.top().first];
        // cout << w << " " << pq.top().second << endl;
        cached_tags.push_back(make_pair(w, pq.top().second));
        pq.pop();
    }
    return cached_tags;
}
