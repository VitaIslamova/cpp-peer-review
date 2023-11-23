#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>
#include <cassert>

using namespace std;

class Domain {
public:
    Domain(string line)
            : domain_(std::move(line)) {
        NormalizeDomain_();
    }

    bool operator==(const Domain &other) {
        return domain_ == other.domain_;
    }

    [[nodiscard]] bool IsSubdomain(const Domain &other) const {
        if (domain_.size() < other.domain_.size()) {
            return false;
        }
        return std::equal(other.domain_.begin(), other.domain_.end(), domain_.begin());
    }

    [[nodiscard]] const string &GetDomain() const {
        return domain_;
    }

private:
    void NormalizeDomain_() {
        std::reverse(domain_.begin(), domain_.end());
        domain_.push_back('.');
    }

    string domain_;
};

class DomainChecker {
public:
    template<typename Iterator>
    DomainChecker(Iterator beginIt, Iterator endIt)
            : forbidden_domains_(beginIt, endIt) {
        sort(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain &lhs, const Domain &rhs) {
            return lhs.GetDomain() < rhs.GetDomain();
        });
        auto it = unique(forbidden_domains_.begin(), forbidden_domains_.end(),
                       [](const Domain &lhs, const Domain &rhs) {
                           return rhs.IsSubdomain(lhs);
                       });
        forbidden_domains_.erase(it, forbidden_domains_.end());
    }

    bool IsForbidden(const Domain &domain) const {
        auto it = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domain,
                              [](const Domain &lhs, const Domain &rhs) {
                                  return lhs.GetDomain() < rhs.GetDomain();
                              });
        if (it == forbidden_domains_.begin()) {
            return false;
        }
        return domain.IsSubdomain(*prev(it));
    }

private:
    vector<Domain> forbidden_domains_;
};

template<typename Number>
vector<Domain> ReadDomains(istream &in, Number num) {
    vector<Domain> domains;
    domains.reserve(num);
    string domain;
    for (size_t i = 0; i < num; ++i) {
        getline(in, domain);
        domains.emplace_back(domain);
    }
    return domains;
}

void TestReadDomains() {
    istringstream input("ya.ru\n"
                        "maps.me\n"
                        "m.ya.ru\n");
    const vector<Domain> domains = ReadDomains(input, static_cast<size_t>(3));
    assert(domains.size() == 3);
    assert(domains[0].GetDomain() == "ur.ay.");
    assert(domains[1].GetDomain() == "em.spam.");
    assert(domains[2].GetDomain() == "ur.ay.m.");
}

template<typename Number>
Number ReadNumberOnLine(istream &input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

void TestReadNumberOnLine() {
    istringstream input("111");
    assert(ReadNumberOnLine<int>(input) == 111);
    input.clear();
    input.str("222");
    assert(ReadNumberOnLine<std::size_t>(input) == 222);
    input.clear();
    input.str("333");
    assert(ReadNumberOnLine<double>(input) == 333);
    input.clear();
    input.str("444");
    assert(ReadNumberOnLine<float>(input) == 444);
    input.clear();
    input.str("555");
    assert(ReadNumberOnLine<long>(input) == 555);
    input.clear();
    input.str("565");
    assert(ReadNumberOnLine<long long>(input) == 565);
    input.clear();
    input.str("567");
    assert(ReadNumberOnLine<unsigned long>(input) == 567);
    input.clear();
    input.str("987");
    assert(ReadNumberOnLine<unsigned long long>(input) == 987);
}

void TestForbiddenDomains() {
    const std::vector<Domain> forbidden_domains{
            "gdz.ru"s,
            "maps.me"s,
            "m.gdz.ru"s,
            "com"s,
    };
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    assert(checker.IsForbidden("gdz.ru"s)); // Bad
    assert(checker.IsForbidden("gdz.com"s)); // Bad
    assert(checker.IsForbidden("m.maps.me"s)); // Bad
    assert(checker.IsForbidden("alg.m.gdz.ru"s)); // Bad
    assert(checker.IsForbidden("maps.com"s)); // Bad
    assert(!checker.IsForbidden("maps.ru"s)); // Good
    assert(!checker.IsForbidden("gdz.ua"s)); // Good
}

void Tests() {
    TestReadDomains();
    TestReadNumberOnLine();
    TestForbiddenDomains();
}

int main() {
    Tests();

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain &domain: test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
