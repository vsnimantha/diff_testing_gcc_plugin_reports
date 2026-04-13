// test4_templates.cpp
#include <string>
#include <map>

template <typename K, typename V>
class DataStore {
private:
    std::map<K, V> storage;
public:
    void add(K key, V value) {
        storage[key] = value;
    }
    V get(K key) {
        return storage[key];
    }
};

int main() {
    DataStore<std::string, int> store;
    store.add("Status", 200);
    store.add("Error", 404);
    
    return (store.get("Status") == 200) ? 0 : 1;
}