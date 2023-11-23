#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <variant>

using namespace std;
using namespace std::literals;

enum class Language {
    RU,
    EN,
    FR,
};

struct City {
    string name;
    string iso_code;
    string phone_code;
    string country_name;
    string country_iso_code;
    string country_time_zone;
    vector<Language> languages;
};

struct Country {
    string name;
    string iso_code;
    string phone_code;
    string time_zone;
    vector<Language> languages;
};

template<typename T>
T FromString(const string &str) {
    return T();
}

class Json {
public:
    using Object = std::map<std::string, Json>;
    using List = std::vector<Json>;

    [[nodiscard]] List &AsList() const { return std::get<List>(Json::value_); }

    [[nodiscard]] Object &AsObject() const { return std::get<Object>(value_); }

    std::string AsString() const { return std::get<std::string>(value_); }

    Json &operator[](const std::string &key) { return std::get<Object>(value_)[key]; }

    const Json &operator[](const std::string &key) const { return std::get<Object>(value_).at(key); }

    using const_iterator = List::const_iterator;

    const_iterator begin() const { return AsList().cbegin(); }

    const_iterator end() const { return AsList().cend(); }

private:
    mutable std::variant<std::nullptr_t, bool, int, double, std::string, List, Object> value_;
};

const std::string NAME = "name";
const std::string ISO_CODE = "iso_code";
const std::string PHONE_CODE = "phone_code";
const std::string TIME_ZONE = "time_zone";
const std::string LANGUAGES = "languages";
const std::string CITIES = "cities";

Country ParseCountry(const Json &country_json) {
    auto &country_obj = country_json.AsObject();
    Country country = {
            country_obj[NAME].AsString(),
            country_obj[ISO_CODE].AsString(),
            country_obj[PHONE_CODE].AsString(),
            country_obj[TIME_ZONE].AsString()
    };

    for (const auto &lang_obj: country_obj[LANGUAGES].AsList()) {
        country.languages.push_back(FromString<Language>(lang_obj.AsString()));
    }
    return country;
}

City ParseCity(const Json &city_json, const Country &country) {
    auto &city_obj = city_json.AsObject();
    return {
            city_obj[NAME].AsString(),
            city_obj[ISO_CODE].AsString(),
            country.phone_code + city_obj[PHONE_CODE].AsString(),
            country.name,
            country.iso_code,
            country.time_zone,
            country.languages
    };
}

void ParseCountryJson(const Json &json, vector<Country> &countries, vector<City> &cities) {
    for (auto &country_json: json.AsList()) {
        Country country = ParseCountry(country_json);
        countries.push_back(country);

        for (const auto &city_json: country_json[CITIES]) {
            cities.push_back(ParseCity(city_json, country));
        }
    }
}
