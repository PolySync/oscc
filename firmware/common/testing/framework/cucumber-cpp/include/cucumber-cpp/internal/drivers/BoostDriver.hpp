#ifndef CUKE_BOOSTDRIVER_HPP_
#define CUKE_BOOSTDRIVER_HPP_

#include "../step/StepManager.hpp"

namespace cucumber {
namespace internal {

class CukeBoostLogInterceptor;

class BoostStep : public BasicStep {
protected:
    const InvokeResult invokeStepBody();

private:
    static void initBoostTest();
    void runWithMasterSuite();
};

#define STEP_INHERITANCE(step_name) ::cucumber::internal::BoostStep

}
}

#endif /* CUKE_BOOSTDRIVER_HPP_ */
