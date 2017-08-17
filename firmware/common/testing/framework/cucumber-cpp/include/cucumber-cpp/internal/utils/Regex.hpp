#ifndef CUKE_REGEX_HPP_
#define CUKE_REGEX_HPP_

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>

namespace cucumber {
namespace internal {

struct RegexSubmatch {
    std::string value;
    int position;
};


class RegexMatch {
public:
    typedef std::vector<RegexSubmatch> submatches_type;

    virtual ~RegexMatch() {};

    bool matches();
    const submatches_type & getSubmatches();

protected:
    bool regexMatched;
    submatches_type submatches;
};

class FindRegexMatch : public RegexMatch {
public:
    FindRegexMatch(const boost::regex &regexImpl, const std::string &expression);
};

class FindAllRegexMatch : public RegexMatch {
public:
    FindAllRegexMatch(const boost::regex &regexImpl, const std::string &expression);
};


class Regex {
private:
    boost::regex regexImpl;

public:
    Regex(std::string expr);

    boost::shared_ptr<RegexMatch> find(const std::string &expression) const;
    boost::shared_ptr<RegexMatch> findAll(const std::string &expression) const;

    std::string str() const;
};

}
}

#endif /* CUKE_REGEX_HPP_ */

