/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorWaitReturn.hpp"
#include "sc-memory/sc_memory.hpp"
#include <iostream>

namespace scp
{

// waitReturn
SCPOperatorWaitReturn::SCPOperatorWaitReturn(ScAgentContext & ctx, ScAddr addr)
  : SCPOperatorElStr1(ctx, addr)
{
}

std::string SCPOperatorWaitReturn::GetTypeName()
{
  return "waitReturn";
}

sc_result SCPOperatorWaitReturn::Parse()
{
  return SCPOperatorElStr1::Parse();
}

sc_result SCPOperatorWaitReturn::Execute()
{
  if (SC_RESULT_OK != ResetValues())
    return SC_RESULT_ERROR;

  if (!operands[0]->IsFixed())
  {
#ifdef SCP_DEBUG
    Utils::logSCPError(m_memoryCtx, "Operand must have FIXED modifier", addr);
#endif
    FinishExecutionWithError();
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }
  if (!operands[0]->GetValue().IsValid())
  {
#ifdef SCP_DEBUG
    Utils::logSCPError(m_memoryCtx, "Operand has modifier FIXED, but has no value", addr);
#endif
    FinishExecutionWithError();
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  if (m_memoryCtx.CheckConnector(Keynodes::action_finished, operands[0]->GetValue(), ScType::ConstPermPosArc))
  {
    FinishExecutionSuccessfully();
  }

  return SC_RESULT_OK;
}
}  // namespace scp
