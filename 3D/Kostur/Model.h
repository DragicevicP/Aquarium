#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Mesh.h"   // tvoj Mesh koji radi sa Vertex: pos+col+uv

class Model {
public:
    Model(const std::string& path) { loadModel(path); }
    void Draw() {
        for (auto& m : meshes) m.Draw();
    }

private:
    std::vector<Mesh> meshes;

    void loadModel(const std::string& path);

    struct IndexKey {
        int v = 0;   // 1-based from OBJ, 0 means missing
        int vt = 0;  // 1-based
        int vn = 0;  // 1-based (ignored for now)

        bool operator==(const IndexKey& o) const {
            return v == o.v && vt == o.vt && vn == o.vn;
        }
    };

    struct IndexKeyHash {
        size_t operator()(const IndexKey& k) const {
            // simple hash combine
            size_t h1 = std::hash<int>{}(k.v);
            size_t h2 = std::hash<int>{}(k.vt);
            size_t h3 = std::hash<int>{}(k.vn);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    static bool parseFaceToken(const std::string& tok, IndexKey& out);
    static int  fixIndex(int idx, int size); // handles negative indices in OBJ
};