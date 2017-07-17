#ifndef CUKE_GENERICDRIVER_HPP_
#define CUKE_GENERICDRIVER_HPP_

#include "../step/StepManager.hpp"

namespace cucumber {
namespace internal {

class GenericStep : public BasicStep {
protected:
    virtual const InvokeResult invokeStepBody();
};

#define STEP_INHERITANCE(step_name) virtual ::cucumber::internal::GenericStep

}
}

#endif /* CUKE_GENERICDRIVER_HPP_ */
