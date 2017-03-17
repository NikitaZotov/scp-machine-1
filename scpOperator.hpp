/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"
#include "scpOperand.hpp"

#include <vector>

namespace scp
{

class SCPOperator
{
protected:
    ScAddr addr;
    ScAddr type;
    std::vector<SCPOperand*> operands = std::vector<SCPOperand*>(0);

    ScMemoryContext &ms_context;

public:
    SCPOperator(ScMemoryContext &ctx, ScAddr addr);
    ~SCPOperator();
    ScAddr GetAddr();
    virtual sc_result Parse();
    virtual sc_result Execute();
};

}

