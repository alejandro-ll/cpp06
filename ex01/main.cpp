#include <iostream>
#include "Serializer.hpp"

int main() {
    Data d;
    d.id = 42;
    d.name = "forty-two";
    d.score = 3.14;

    Data *p = &d;
    uintptr_t raw = Serializer::serialize(p);
    Data *q = Serializer::deserialize(raw);

    std::cout << "p: " << p << "\n";
    std::cout << "q: " << q << "\n";
    std::cout << "same pointer? " << (p == q ? "yes" : "no") << "\n";
    std::cout << "q->id=" << q->id << ", name=" << q->name << ", score=" << q->score << "\n";
    return 0;
}
