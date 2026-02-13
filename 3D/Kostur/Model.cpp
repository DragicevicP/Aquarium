#include "Model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>


bool Model::parseFaceToken(const std::string& tok, IndexKey& out) {
    out = {};

    size_t s1 = tok.find('/');
    if (s1 == std::string::npos) {
        try { out.v = std::stoi(tok); }
        catch (...) { return false; }
        return true;
    }

    size_t s2 = tok.find('/', s1 + 1);

    std::string a = tok.substr(0, s1);
    std::string b;
    std::string c;

    if (s2 == std::string::npos) {
        b = tok.substr(s1 + 1);
    }
    else {
        b = tok.substr(s1 + 1, s2 - (s1 + 1));
        c = tok.substr(s2 + 1);
    }

    try {
        if (!a.empty()) out.v = std::stoi(a);
        if (!b.empty()) out.vt = std::stoi(b);
        if (!c.empty()) out.vn = std::stoi(c);
    }
    catch (...) {
        return false;
    }
    return true;
}

int Model::fixIndex(int idx, int size) {
    if (idx > 0) return idx;            
    if (idx < 0) return size + 1 + idx; 
    return 0;                           
}

void Model::loadModel(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::vector<glm::vec3> positions; 
    std::vector<glm::vec2> texcoords; 
    

    std::vector<Vertex> outVertices;
    std::vector<unsigned int> outIndices;

    std::unordered_map<IndexKey, unsigned int, IndexKeyHash> dedup;

    std::string line;
    while (std::getline(file, line)) {
    
        if (line.empty()) continue;
        if (line.size() >= 1 && line[0] == '#') continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (type == "vt") {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        }
        else if (type == "f") {

            std::vector<IndexKey> faceKeys;
            std::string tok;

            while (ss >> tok) {
                IndexKey k;
                if (!parseFaceToken(tok, k)) continue;

                k.v = fixIndex(k.v, (int)positions.size());
                k.vt = fixIndex(k.vt, (int)texcoords.size());

                if (k.v == 0) continue; 
                faceKeys.push_back(k);
            }

            if (faceKeys.size() < 3) continue;

            for (size_t i = 1; i + 1 < faceKeys.size(); i++) {
                IndexKey tri[3] = { faceKeys[0], faceKeys[i], faceKeys[i + 1] };

                for (int t = 0; t < 3; t++) {
                    const IndexKey& k = tri[t];

                    auto it = dedup.find(k);
                    if (it != dedup.end()) {
                        outIndices.push_back(it->second);
                        continue;
                    }

                    Vertex vtx{};

                    const glm::vec3& p = positions[(size_t)k.v - 1];
                    vtx.x = p.x;
                    vtx.y = p.y;
                    vtx.z = p.z;

                    vtx.r = 1.0f;
                    vtx.g = 1.0f;
                    vtx.b = 1.0f;
                    vtx.a = 1.0f;

                    if (k.vt != 0) {
                        const glm::vec2& uv = texcoords[(size_t)k.vt - 1];
                        vtx.u = uv.x;
                        vtx.v = uv.y;
                    }
                    else {
                        vtx.u = 0.0f;
                        vtx.v = 0.0f;
                    }

                    outVertices.push_back(vtx);
                    unsigned int newIndex = (unsigned int)(outVertices.size() - 1);
                    dedup[k] = newIndex;
                    outIndices.push_back(newIndex);
                }
            }
        }
    }

    if (outVertices.empty() || outIndices.empty()) {
        std::cout << "MODEL: Prazan ili neuspesno ucitan model: " << path << "\n";
        return;
    }

    meshes.clear();
    meshes.emplace_back(outVertices, outIndices);
}