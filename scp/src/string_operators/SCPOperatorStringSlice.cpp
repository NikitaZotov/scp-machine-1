/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorStringSlice.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_stream.hpp"
#include <sc-memory/sc_link.hpp>
#include <iostream>
#include <cstring>
#include <regex>

using namespace std;

namespace scp
{

SCPOperatorStringSlice::SCPOperatorStringSlice(ScAgentContext & ctx, ScAddr addr)
  : SCPOperatorElStr5(ctx, addr)
{
}

string SCPOperatorStringSlice::GetTypeName()
{
  return "stringSlice";
}

sc_result SCPOperatorStringSlice::Parse()
{
  return SCPOperatorElStr5::Parse();
}

sc_result SCPOperatorStringSlice::Execute()
{
  if (SC_RESULT_OK != ResetValues())
    return SC_RESULT_ERROR;

  if (!(operands[1]->IsFixed() && operands[2]->IsFixed() && operands[3]->IsFixed()))
  {
#ifdef SCP_DEBUG
    Utils::logSCPError(m_memoryCtx, "All operands must have FIXED modifier", addr);
#endif
    FinishExecutionWithError();
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }
  if (!operands[1]->GetValue().IsValid())
  {
#ifdef SCP_DEBUG
    Utils::logSCPError(m_memoryCtx, "Operand 1 has modifier FIXED, but has no value", addr);
#endif
    FinishExecutionWithError();
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  if (!operands[2]->GetValue().IsValid())
  {
#ifdef SCP_DEBUG
    Utils::logSCPError(m_memoryCtx, "Operand 2 has modifier FIXED, but has no value", addr);
#endif
    FinishExecutionWithError();
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  if (!operands[3]->GetValue().IsValid())
  {
#ifdef SCP_DEBUG
    Utils::logSCPError(m_memoryCtx, "Operand 3 has modifier FIXED, but has no value", addr);
#endif
    FinishExecutionWithError();
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  string input = Utils::scLinkGetString(m_memoryCtx, operands[1]->GetValue());
  string startIndexStr = Utils::scLinkGetString(m_memoryCtx, operands[2]->GetValue());
  string endIndexStr = Utils::scLinkGetString(m_memoryCtx, operands[3]->GetValue());

  size_t startIndex;
  size_t endIndex;

  string intInit = "int: ";

  try
  {
    size_t pos = startIndexStr.find(intInit);
    if (pos != string::npos)
    {
      startIndexStr.erase(pos, intInit.length());
    }
    startIndex = stoi(startIndexStr);
  }
  catch (...)
  {
    cout << "StartIndex is not int!" << endl;
    FinishExecutionUnsuccessfully();
    return SC_RESULT_OK;
  }

  try
  {
    size_t pos = endIndexStr.find(intInit);
    if (pos != string::npos)
    {
      endIndexStr.erase(pos, intInit.length());
    }
    endIndex = stoi(endIndexStr);
  }
  catch (...)
  {
    cout << "EndIndex is not int!" << endl;
    FinishExecutionUnsuccessfully();
    return SC_RESULT_OK;
  }

  if (startIndex < 0 || startIndex > input.length())
  {
    cout << "StartIndex is out of range" << endl;
    FinishExecutionUnsuccessfully();
  }
  else if (endIndex < startIndex || endIndex > input.length())
  {
    cout << "EndIndex is out of range" << endl;
    FinishExecutionUnsuccessfully();
  }
  else
  {
    string res = input.substr(startIndex, endIndex - startIndex);
    ScStreamPtr streamPtr = Utils::StreamFromString(res);
    ScAddr answerLink = m_memoryCtx.GenerateLink();

    m_memoryCtx.SetLinkContent(answerLink, streamPtr);

    Utils::printOperatorAnswer(m_memoryCtx, operands[0], answerLink);

    FinishExecutionSuccessfully();
  }

  return SC_RESULT_OK;
}

}  // namespace scp
