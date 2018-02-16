#ifndef CUKE_WIREPROTOCOL_COMMANDS_HPP_
#define CUKE_WIREPROTOCOL_COMMANDS_HPP_

#include "WireProtocol.hpp"
#include <boost/shared_ptr.hpp>

namespace cucumber {
namespace internal {

class ScenarioCommand : public WireCommand {
protected:
    const CukeEngine::tags_type tags;

    ScenarioCommand(const CukeEngine::tags_type& tags);
};


class BeginScenarioCommand : public ScenarioCommand {
public:
    BeginScenarioCommand(const CukeEngine::tags_type& tags);

    boost::shared_ptr<WireResponse> run(CukeEngine& engine) const;
};


class EndScenarioCommand : public ScenarioCommand {
public:
    EndScenarioCommand(const CukeEngine::tags_type& tags);

    boost::shared_ptr<WireResponse> run(CukeEngine& engine) const;
};


class StepMatchesCommand : public WireCommand {
private:
    const std::string stepName;

public:
    StepMatchesCommand(const std::string & stepName);

    boost::shared_ptr<WireResponse> run(CukeEngine& engine) const;
};


class InvokeCommand : public WireCommand {
private:
    const std::string stepId;
    const CukeEngine::invoke_args_type args;
    const CukeEngine::invoke_table_type tableArg;

public:
    InvokeCommand(const std::string & stepId,
                  const CukeEngine::invoke_args_type& args,
                  const CukeEngine::invoke_table_type& tableArg);

    boost::shared_ptr<WireResponse> run(CukeEngine& engine) const;
};


class SnippetTextCommand : public WireCommand {
private:
    std::string keyword, name, multilineArgClass;

public:
    SnippetTextCommand(const std::string & keyword,
                       const std::string & name,
                       const std::string & multilineArgClass);

    boost::shared_ptr<WireResponse> run(CukeEngine& engine) const;
};


class FailingCommand : public WireCommand {
public:
    boost::shared_ptr<WireResponse> run(CukeEngine& engine) const;
};

}
}

#endif /* CUKE_WIREPROTOCOL_COMMANDS_HPP_ */
