﻿/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorStringLen.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_stream.hpp"
#include <sc-memory/sc_link.hpp>
#include <iostream>
#include <cstring>

using namespace std;

namespace scp
{

SCPOperatorStringLen::SCPOperatorStringLen(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr2(ctx, addr)
{
}

string SCPOperatorStringLen::GetTypeName()
{
    return "stringLen";
}

sc_result SCPOperatorStringLen::Parse()
{
    return SCPOperatorElStr2::Parse();
}

sc_result SCPOperatorStringLen::Execute()
{
    if (SC_RESULT_OK != ResetValues())
        return SC_RESULT_ERROR;

    if (!operands[1]->IsFixed())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(m_memoryCtx, "Operand must have FIXED modifier", addr);
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


        string str1 = Utils::scLinkGetString(m_memoryCtx, operands[1]->GetValue());

            size_t len1 = str1.length();

            ScStreamPtr streamPtr = Utils::StreamFromString(to_string(len1));
            ScAddr answerLink =m_memoryCtx.CreateLink();

           m_memoryCtx.SetLinkContent(answerLink, streamPtr);

            Utils::printOperatorAnswer(m_memoryCtx, operands[0], answerLink);


            FinishExecutionSuccessfully();

    return SC_RESULT_OK;
}

}
