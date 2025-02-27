/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scpFinishedInterpretationActionProcessor.hpp"

#include "scpKeynodes.hpp"
#include "scpOperator.hpp"

namespace scp
{

ScResult ASCPFinishedInterpretationActionProcessor::DoProgram(
    ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event,
    ScAction & action)
{
  ScAddr const & finishedAction = event.GetOtherElement();

  ScAddr waitOperatorAddr;

  // SCP_CONST case
  ScIterator5Ptr iter = m_context.CreateIterator5(
      ScType::ConstNode, ScType::ConstPermPosArc, finishedAction, ScType::ConstPermPosArc, Keynodes::rrel_scp_const);
  while (iter->Next())
  {
    waitOperatorAddr = iter->Get(0);
    if (!m_context.CheckConnector(Keynodes::op_waitReturn, waitOperatorAddr, ScType::ConstPermPosArc)
        || !m_context.CheckConnector(Keynodes::active_action, waitOperatorAddr, ScType::ConstPermPosArc))
      continue;

    SCPOperator::FinishExecutionSuccessfully(m_context, waitOperatorAddr);
    break;
  }

  // SCP_VAR case
  ScIterator3Ptr iter3 = m_context.CreateIterator3(ScType::ConstNode, ScType::ConstTempPosArc, finishedAction);
  while (iter3->Next())
  {
    ScIterator5Ptr iter5 = m_context.CreateIterator5(
        ScType::ConstNode, ScType::ConstPermPosArc, iter3->Get(0), ScType::ConstPermPosArc, Keynodes::rrel_scp_var);
    while (iter5->Next())
    {
      waitOperatorAddr = iter5->Get(0);
      if (!m_context.CheckConnector(Keynodes::op_waitReturn, waitOperatorAddr, ScType::ConstPermPosArc)
          || !m_context.CheckConnector(Keynodes::active_action, waitOperatorAddr, ScType::ConstPermPosArc))
        continue;

      SCPOperator::FinishExecutionSuccessfully(m_context, waitOperatorAddr);
      break;
    }
  }

  return action.FinishSuccessfully();
}

ScAddr ASCPFinishedInterpretationActionProcessor::GetActionClass() const
{
  return Keynodes::action_interpret_finished_action;
}

ScAddr ASCPFinishedInterpretationActionProcessor::GetEventSubscriptionElement() const
{
  return Keynodes::action_finished;
}

bool ASCPFinishedInterpretationActionProcessor::CheckInitiationCondition(
    ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event)
{
  ScAddr const & action = event.GetOtherElement();
  return m_context.CheckConnector(Keynodes::action_scp_interpretation_request, action, ScType::ConstPermPosArc);
}

}  // namespace scp
