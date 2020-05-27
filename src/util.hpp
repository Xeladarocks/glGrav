

void push_vec3(std::vector<float>& array, glm::vec3 item, int num = 1) {
    for (int i = 0; i < num; i++) {
        array.push_back(item.x); array.push_back(item.y); array.push_back(item.z);
    }
}
void push_ivec3(std::vector<unsigned int>& array, glm::ivec3 item, int num = 1) {
    for (int i = 0; i < num; i++) {
        array.push_back(item.x); array.push_back(item.y); array.push_back(item.z);
    }
}
void removeFirstVec3(std::vector<float>& arr) {
    arr.erase(arr.begin()); arr.erase(arr.begin()); arr.erase(arr.begin());
}
void removeFirst2Vec3(std::vector<float>& arr) {
    removeFirstVec3(arr); removeFirstVec3(arr);
}
void removeFirstVec3(std::vector<glm::vec3>& arr) {
    arr.erase(arr.begin()); arr.erase(arr.begin()); arr.erase(arr.begin());
}
void removeFirst2Vec3(std::vector<glm::vec3>& arr) {
    removeFirstVec3(arr); removeFirstVec3(arr);
}

float randf() {
    return ((float)rand()) / (float)RAND_MAX;
}

glm::vec3 HSVtoRGB(float H, float S, float V) {
    float C = S * V;
    float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    float m = V - C;
    float Rs, Gs, Bs;
    if (H >= 0 && H < 60) {
        Rs = C;
        Gs = X;
        Bs = 0;
    } else if (H >= 60 && H < 120) {
        Rs = X;
        Gs = C;
        Bs = 0;
    } else if (H >= 120 && H < 180) {
        Rs = 0;
        Gs = C;
        Bs = X;
    } else if (H >= 180 && H < 240) {
        Rs = 0;
        Gs = X;
        Bs = C;
    } else if (H >= 240 && H < 300) {
        Rs = X;
        Gs = 0;
        Bs = C;
    } else {
        Rs = C;
        Gs = 0;
        Bs = X;
    }
    return glm::vec3(
        (Rs + m), 
        (Gs + m), 
        (Bs + m));
}