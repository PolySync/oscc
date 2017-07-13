#ifndef CUKE_HOOKREGISTRAR_HPP_
#define CUKE_HOOKREGISTRAR_HPP_

#include "Tag.hpp"
#include "../Scenario.hpp"
#include "../step/StepManager.hpp"

#include <boost/make_shared.hpp>
#include <boost/smart_ptr.hpp>
using boost::shared_ptr;

#include <list>

namespace cucumber {
namespace internal {

class CallableStep {
public:
    virtual void call() = 0;
};

class Hook {
public:
    void setTags(const std::string &csvTagNotation);
    virtual void invokeHook(Scenario *scenario, CallableStep *step);
    virtual void skipHook();
    virtual void body() = 0;
protected:
    bool tagsMatch(Scenario *scenario);
private:
    shared_ptr<TagExpression> tagExpression;
};

class BeforeHook : public Hook {
};

class AroundStepHook : public Hook {
public:
    virtual void invokeHook(Scenario *scenario, CallableStep *step);
    virtual void skipHook();
protected:
    CallableStep *step;
};

class AfterStepHook : public Hook {
};

class AfterHook : public Hook {
};

class UnconditionalHook : public Hook {
public:
    virtual void invokeHook(Scenario *scenario, CallableStep *step);
};

class BeforeAllHook : public UnconditionalHook {
};

class AfterAllHook : public UnconditionalHook {
};

class HookRegistrar {
public:
    typedef std::list< boost::shared_ptr<Hook> > hook_list_type;
    typedef std::list< boost::shared_ptr<AroundStepHook> > aroundhook_list_type;

    virtual ~HookRegistrar();

    void addBeforeHook(const boost::shared_ptr<BeforeHook>& afterHook);
    void execBeforeHooks(Scenario *scenario);

    void addAroundStepHook(const boost::shared_ptr<AroundStepHook>& aroundStepHook);
    InvokeResult execStepChain(Scenario *scenario, const boost::shared_ptr<const StepInfo>& stepInfo, const InvokeArgs *pArgs);

    void addAfterStepHook(const boost::shared_ptr<AfterStepHook>& afterStepHook);
    void execAfterStepHooks(Scenario *scenario);

    void addAfterHook(const boost::shared_ptr<AfterHook>& afterHook);
    void execAfterHooks(Scenario *scenario);

    void addBeforeAllHook(const boost::shared_ptr<BeforeAllHook>& beforeAllHook);
    void execBeforeAllHooks();

    void addAfterAllHook(const boost::shared_ptr<AfterAllHook>& afterAllHook);
    void execAfterAllHooks();

private:
    void execHooks(HookRegistrar::hook_list_type &hookList, Scenario *scenario);

protected:
    hook_list_type& beforeAllHooks();
    hook_list_type& beforeHooks();
    aroundhook_list_type& aroundStepHooks();
    hook_list_type& afterStepHooks();
    hook_list_type& afterHooks();
    hook_list_type& afterAllHooks();
};


class StepCallChain {
public:
    StepCallChain(Scenario *scenario, const boost::shared_ptr<const StepInfo>& stepInfo, const InvokeArgs *pStepArgs, HookRegistrar::aroundhook_list_type &aroundHooks);
    InvokeResult exec();
    void execNext();
private:
    void execStep();

    Scenario *scenario;
    boost::shared_ptr<const StepInfo> stepInfo;
    const InvokeArgs *pStepArgs;

    HookRegistrar::aroundhook_list_type::iterator nextHook;
    HookRegistrar::aroundhook_list_type::iterator hookEnd;
    InvokeResult result;
};

class CallableStepChain : public CallableStep {
public:
    CallableStepChain(StepCallChain *scc);
    void call();
private:
    StepCallChain *scc;
};


template<class T>
static int registerBeforeHook(const std::string &csvTagNotation) {
   HookRegistrar reg;
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   hook->setTags(csvTagNotation);
   reg.addBeforeHook(hook);
   return 0; // We are not interested in the ID at this time
}

template<class T>
static int registerAroundStepHook(const std::string &csvTagNotation) {
   HookRegistrar reg;
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   hook->setTags(csvTagNotation);
   reg.addAroundStepHook(hook);
   return 0;
}

template<class T>
static int registerAfterStepHook(const std::string &csvTagNotation) {
   HookRegistrar reg;
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   hook->setTags(csvTagNotation);
   reg.addAfterStepHook(hook);
   return 0;
}

template<class T>
static int registerAfterHook(const std::string &csvTagNotation) {
   HookRegistrar reg;
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   hook->setTags(csvTagNotation);
   reg.addAfterHook(hook);
   return 0;
}

template<class T>
static int registerBeforeAllHook() {
   HookRegistrar reg;
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   reg.addBeforeAllHook(hook);
   return 0;
}

template<class T>
static int registerAfterAllHook() {
   HookRegistrar reg;
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   reg.addAfterAllHook(hook);
   return 0;
}

}
}

#endif /* CUKE_HOOKREGISTRAR_HPP_ */
