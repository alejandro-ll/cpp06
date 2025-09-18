#include "ScalarConverter.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cctype>
#include <cstdlib>
#include <cmath>
#include <string>

namespace {
    bool isPseudoFloat(std::string s) {
        return (s == "nanf" || s == "+inff" || s == "-inff");
    }
    bool isPseudoDouble(std::string s) {
        return (s == "nan" || s == "+inf" || s == "-inf");
    }
    bool isCharLiteral(std::string const &s) {
        return s.size() == 3 && s[0]=='\'' && s[2]=='\'' && std::isprint(static_cast<unsigned char>(s[1]));
    }
    bool isIntLiteral(std::string const &s) {
        if (s.empty()) return false;
        size_t i = (s[0]=='+' || s[0]=='-') ? 1 : 0;
        if (i >= s.size()) return false;
        for (; i < s.size(); ++i) if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
        return true;
    }
    bool isFloatLiteral(std::string const &s) {
        if (isPseudoFloat(s)) return true;
        if (s.size()<2 || s[s.size()-1]!='f') return false;
        std::string core = s.substr(0, s.size()-1);
        bool dot=false;
        size_t i=(core[0]=='+'||core[0]=='-')?1:0;
        if (i>=core.size()) return false;
        for (; i<core.size(); ++i) {
            if (core[i]=='.') { if (dot) return false; dot=true; }
            else if (!std::isdigit(static_cast<unsigned char>(core[i]))) return false;
        }
        return dot; // pedimos decimal para float con 'f'
    }
    bool isDoubleLiteral(std::string const &s) {
        if (isPseudoDouble(s)) return true;
        bool dot=false;
        if (s.empty()) return false;
        size_t i=(s[0]=='+'||s[0]=='-')?1:0;
        if (i>=s.size()) return false;
        for (; i<s.size(); ++i) {
            if (s[i]=='.') { if (dot) return false; dot=true; }
            else if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
        }
        return dot;
    }
    bool inCharDisplayRange(int v) {
        return v >= 0 && v <= 127;
    }
}

ScalarConverter::ScalarConverter() {}
ScalarConverter::ScalarConverter(ScalarConverter const &) {}
ScalarConverter &ScalarConverter::operator=(ScalarConverter const &) { return *this; }
ScalarConverter::~ScalarConverter() {}

void ScalarConverter::convert(std::string const &s) {
    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(1);

    if (isCharLiteral(s)) {
        char c = s[1];
        int i = static_cast<int>(c);
        float f = static_cast<float>(c);
        double d = static_cast<double>(c);

        std::cout << "char: '" << c << "'\n";
        std::cout << "int: " << i << "\n";
        std::cout << "float: " << f << "f\n";
        std::cout << "double: " << d << "\n";
        return;
    }

    if (isPseudoFloat(s) || isPseudoDouble(s)) {
        float f = 0.f;
        double d = 0.0;
        if (s == "nanf" || s == "nan") { f = std::numeric_limits<float>::quiet_NaN(); d = std::numeric_limits<double>::quiet_NaN(); }
        else if (s == "+inff" || s == "+inf") { f = std::numeric_limits<float>::infinity(); d = std::numeric_limits<double>::infinity(); }
        else if (s == "-inff" || s == "-inf") { f = -std::numeric_limits<float>::infinity(); d = -std::numeric_limits<double>::infinity(); }
        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        std::cout << "float: " << (std::isnan(f) ? "nanf" : (std::isinf(f) ? (f>0?"+inff":"-inff") : "")) ;
        if (!std::isnan(f) && !std::isinf(f)) std::cout << f << "f";
        std::cout << "\n";
        std::cout << "double: " << (std::isnan(d) ? "nan" : (std::isinf(d) ? (d>0?"+inf":"-inf") : "")) ;
        if (!std::isnan(d) && !std::isinf(d)) std::cout << d;
        std::cout << "\n";
        return;
    }

    if (isIntLiteral(s)) {
        long long ll = 0;
        {
            std::istringstream iss(s);
            iss >> ll;
        }
        if (ll < std::numeric_limits<int>::min() || ll > std::numeric_limits<int>::max()) {
            std::cout << "char: impossible\nint: overflow\nfloat: impossible\ndouble: impossible\n";
            return;
        }
        int i = static_cast<int>(ll);
        char cOut;
        if (!inCharDisplayRange(i) || !std::isprint(static_cast<unsigned char>(i))) {
            std::cout << "char: Non displayable\n";
        } else {
            cOut = static_cast<char>(i);
            std::cout << "char: '" << cOut << "'\n";
        }
        std::cout << "int: " << i << "\n";
        std::cout << "float: " << static_cast<float>(i) << "f\n";
        std::cout << "double: " << static_cast<double>(i) << "\n";
        return;
    }

    if (isFloatLiteral(s)) {
        float f;
        if (isPseudoFloat(s)) {
            f = (s=="nanf") ? std::numeric_limits<float>::quiet_NaN()
                : (s=="+inff") ? std::numeric_limits<float>::infinity()
                : -std::numeric_limits<float>::infinity();
        } else {
            std::istringstream iss(s.substr(0, s.size()-1));
            iss >> f;
        }
            if (std::isnan(f) || std::isinf(f)) {
                std::cout << "char: impossible\nint: impossible\nfloat: " << (std::isnan(f)?"nanf":(f>0?"+inff":"-inff")) << "\n";
                std::cout << "double: " << (std::isnan(f)?"nan":(f>0?"+inf":"-inf")) << "\n";
                return;
            }
        int i = static_cast<int>(f);
        if (!inCharDisplayRange(i) || !std::isprint(static_cast<unsigned char>(i)))
            std::cout << "char: Non displayable\n";
        else
            std::cout << "char: '" << static_cast<char>(i) << "'\n";
        std::cout << "int: " << i << "\n";
        std::cout << "float: " << f << "f\n";
        std::cout << "double: " << static_cast<double>(f) << "\n";
        return;
    }

    if (isDoubleLiteral(s)) {
        double d;
        if (isPseudoDouble(s)) {
            d = (s=="nan") ? std::numeric_limits<double>::quiet_NaN()
                : (s=="+inf") ? std::numeric_limits<double>::infinity()
                : -std::numeric_limits<double>::infinity();
        } else {
            std::istringstream iss(s);
            iss >> d;
        }
        if (std::isnan(d) || std::isinf(d)) {
            std::cout << "char: impossible\nint: impossible\nfloat: " << (std::isnan(d)?"nanf":(d>0?"+inff":"-inff")) << "\n";
            std::cout << "double: " << (std::isnan(d)?"nan":(d>0?"+inf":"-inf")) << "\n";
            return;
        }
        int i;
        if (d < static_cast<double>(std::numeric_limits<int>::min()) || d > static_cast<double>(std::numeric_limits<int>::max())) {
            std::cout << "char: impossible\nint: overflow\n";
        } else {
            i = static_cast<int>(d);
            if (!inCharDisplayRange(i) || !std::isprint(static_cast<unsigned char>(i)))
                std::cout << "char: Non displayable\n";
            else
                std::cout << "char: '" << static_cast<char>(i) << "'\n";
            std::cout << "int: " << i << "\n";
        }
        std::cout << "float: " << static_cast<float>(d) << "f\n";
        std::cout << "double: " << d << "\n";
        return;
    }

    std::cout << "char: impossible\nint: impossible\nfloat: impossible\ndouble: impossible\n";
}
