#ifndef CUKE_SCENARIO_HPP_
#define CUKE_SCENARIO_HPP_

#include "hook/Tag.hpp"

namespace cucumber {
namespace internal {

class Scenario {
public:
    Scenario(const TagExpression::tag_list& tags = TagExpression::tag_list());

    const TagExpression::tag_list & getTags();
private:
    const TagExpression::tag_list tags;
};

}
}

#endif /* CUKE_SCENARIO_HPP_ */
