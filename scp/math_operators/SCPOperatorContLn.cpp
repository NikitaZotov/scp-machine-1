/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/


#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorContLn.hpp"
#include "sc-core/sc_helper.h"
#include "sc-core/sc_memory_headers.h"
#include "sc-memory/sc_stream.hpp"
#include "sc-memory/sc_link.hpp"
#include <iostream>
#include <math.h>

namespace scp
{

SCPOperatorContLn::SCPOperatorContLn(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr2(ctx, addr)
{
}

std::string SCPOperatorContLn::GetTypeName()
{
    return "contLn";
}

sc_result SCPOperatorContLn::Parse()
{
    return SCPOperatorElStr2::Parse();
}

sc_result SCPOperatorContLn::Execute()
{
    std::cout << "SCPOperatorContLn execute(): start";
    if (SC_RESULT_OK != ResetValues())
        return SC_RESULT_ERROR;

    if (!(operands[1]->IsFixed()))
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(m_memoryCtx, "Second operand must have FIXED modifier", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }

    if (!operands[1]->GetValue().IsValid())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(m_memoryCtx, "Second operand is FIXED, but has no value", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }

    std::string answer_str;

    if (Utils::scLinkContentIsInt(m_memoryCtx, operands[1]->GetValue()) == true)
    {
        std::cout << "Link is int" << std::endl;
        int value = Utils::scLinkGetContentInt(m_memoryCtx, operands[1]->GetValue());
        double answer = log(value);
        answer_str = std::to_string(answer);
    }

    if (Utils::scLinkContentIsUint(m_memoryCtx, operands[1]->GetValue()) == true)
    {
        std::cout << "Link is uint" << std::endl;
        int value = Utils::scLinkGetContentUint(m_memoryCtx, operands[1]->GetValue());
        double answer = log(value);
        answer_str = std::to_string(answer);
    }

    if (Utils::scLinkContentIsFloat(m_memoryCtx, operands[1]->GetValue()) == true)
    {
        std::cout << "Link is float" << std::endl;
        float value = Utils::scLinkGetContentFloat(m_memoryCtx, operands[1]->GetValue());
        double answer = log(value);
        answer_str = std::to_string(answer);
    }

    if (Utils::scLinkContentIsDouble(m_memoryCtx, operands[1]->GetValue()) == true)
    {
        std::cout << "Link is double" << std::endl;
        double value = Utils::scLinkGetContentDouble(m_memoryCtx, operands[1]->GetValue());
        double answer = log(value);
        answer_str = std::to_string(answer);
    }

    if (answer_str.empty()){
        if(!Utils::scLinkPlainNumbers(m_memoryCtx, operands[1]->GetValue()).empty()){
            answer_str = Utils::scLinkPlainNumbers(m_memoryCtx, operands[1]->GetValue());
            std::string intInit = "int: ";
            std::string::size_type i = answer_str.find(intInit);

            if (i != std::string::npos)
            {
                std::cout << "Link is INT" << std::endl;
                answer_str.erase(i, intInit.length());
                int value = std::atoi(answer_str.c_str());
                double answer = log(value);
                answer_str = std::to_string(answer);
            }

            std::string intDouble = "double: ";
            std::string::size_type d = answer_str.find(intDouble);

            if (d != std::string::npos)
            {
                std::cout << "Link is Double" << std::endl;
                answer_str.erase(d, intDouble.length());
                double value = std::stod(answer_str);
                double answer = log((double)value);
                answer_str = std::to_string(answer);
            }
        }
        else{
            FinishExecutionUnsuccessfully();
            #ifdef SCP_DEBUG
                Utils::logSCPError(m_memoryCtx, "Second operand is not numeric!", addr);
            #endif
            return SC_RESULT_OK;
        }
    }

    ScAddr answerLink =m_memoryCtx.CreateLink();
    ScStreamPtr streamPtr = Utils::StreamFromString(answer_str);
    m_memoryCtx.SetLinkContent(answerLink, streamPtr);

    operands[0]->ResetValue();
    operands[0]->SetValue(answerLink);

    std::cout << "SCPOperatorContLn execute(): end";
    FinishExecutionSuccessfully();
    return SC_RESULT_OK;
}

}
