/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scpUtils.hpp"
#include "scpKeynodes.hpp"
#include "sc-core/sc_helper.h"
#include "sc-memory/sc_stream.hpp"
#include "scpOperand.hpp"

#include <regex>
#include <string>
#include <iostream>

using namespace std;

namespace scp
{
namespace Utils
{

bool addToSet(ScAgentContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr)
{
  if (ctx.CheckConnector(setAddr, elAddr, ScType::ConstPermPosArc))
    return false;

  ScAddr arcAddr = ctx.GenerateConnector(ScType::ConstPermPosArc, setAddr, elAddr);
  assert(arcAddr.IsValid());
  return true;
}

bool eraseFromSet(ScAgentContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr)
{
  ScIterator3Ptr it = ctx.CreateIterator3(setAddr, ScType::ConstPermPosArc, elAddr);
  bool result = false;
  while (it->Next())
    ctx.EraseElement(it->Get(1));

  return result;
}

bool resolveOrderRoleRelation(ScAgentContext & ctx, ScAddr const & arcAddr, ScAddr & relationAddr)
{
  ScIterator3Ptr it = ctx.CreateIterator3(ScType::ConstNode, ScType::MembershipArc, arcAddr);
  while (it->Next())
  {
    if (ctx.CheckConnector(Keynodes::order_role_relation, it->Get(0), ScType::ConstPermPosArc))
    {
      relationAddr = it->Get(0);
      return true;
    }
  }

  return false;
}

bool resolveOrderRoleRelation(ScAgentContext & ctx, uint8_t const order, ScAddr & relationAddr)
{
  switch (order)
  {
  case 1:
    relationAddr = Keynodes::rrel_1;
    break;
  case 2:
    relationAddr = Keynodes::rrel_2;
    break;
  case 3:
    relationAddr = Keynodes::rrel_3;
    break;
  case 4:
    relationAddr = Keynodes::rrel_4;
    break;
  case 5:
    relationAddr = Keynodes::rrel_5;
    break;
  case 6:
    relationAddr = Keynodes::rrel_6;
    break;
  case 7:
    relationAddr = Keynodes::rrel_7;
    break;
  case 8:
    relationAddr = Keynodes::rrel_8;
    break;
  case 9:
    relationAddr = Keynodes::rrel_9;
    break;
  case 10:
    relationAddr = Keynodes::rrel_10;
    break;
  default:
    relationAddr.Reset();
    break;
  }

  return relationAddr.IsValid();
}

bool resolveOperatorType(ScAgentContext & ctx, ScAddr const & operatorAddr, ScAddr & operatorType)
{
  ScIterator3Ptr it = ctx.CreateIterator3(ScType::ConstNode, ScType::MembershipArc, operatorAddr);
  while (it->Next())
  {
    if (ctx.CheckConnector(Keynodes::scp_operator_atomic_type, it->Get(0), ScType::ConstPermPosArc))
    {
      operatorType = it->Get(0);
      return true;
    }
  }

  return false;
}

void printSystemIdentifier(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  if (ctx.GetElementType(elemAddr).IsNode() || ctx.GetElementType(elemAddr).IsLink())
  {
    string s = ctx.GetElementSystemIdentifier(elemAddr);
    if (s.empty())
      cout << elemAddr.GetRealAddr().seg << "|" << elemAddr.GetRealAddr().offset;
    else
      cout << s;
  }
  else
  {
    cout << "(";
    printSystemIdentifier(ctx, ctx.GetArcSourceElement(elemAddr));
    cout << "->";
    printSystemIdentifier(ctx, ctx.GetArcTargetElement(elemAddr));
    cout << ")";
  }
}

void printInfo(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  int c_in = 0, c_out = 0;
  assert(ctx.IsElement(elemAddr));
  printSystemIdentifier(ctx, elemAddr);
  cout << endl;

  cout << "Input arcs:\n";
  ScIterator3Ptr it = ctx.CreateIterator3(ScType::Unknown, ScType::Unknown, elemAddr);
  while (it->Next())
  {
    c_in++;
    cout << "\t" << it->Get(1).GetRealAddr().seg << "|" << it->Get(1).GetRealAddr().offset;
    if (ctx.GetElementType(it->Get(1)).BitAnd(sc_type_membership_arc))
      cout << " <- ";
    else
      cout << " <= ";
    printSystemIdentifier(ctx, it->Get(0));
    cout << endl;
  }
  cout << "Total input arcs: " << c_in << endl;

  cout << "Output arcs:\n";
  it = ctx.CreateIterator3(elemAddr, ScType::Unknown, ScType::Unknown);
  while (it->Next())
  {
    c_out++;
    cout << "\t" << it->Get(1).GetRealAddr().seg << "|" << it->Get(1).GetRealAddr().offset;
    if (ctx.GetElementType(it->Get(1)).BitAnd(sc_type_membership_arc))
      cout << " -> ";
    else
      cout << " => ";
    printSystemIdentifier(ctx, it->Get(2));
    cout << endl;
  }
  cout << "Total output arcs: " << c_out << endl;
}

bool scLinkContentIsInt(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    if (ScStreamConverter::StreamToString(stream, string))
    {
      if (string.find("int:") != std::string::npos)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  return false;
}

int scLinkGetContentInt(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    std::string intInit = "int: ";
    if (ScStreamConverter::StreamToString(stream, string))
    {
      std::string::size_type i = string.find(intInit);

      if (i != std::string::npos)
        string.erase(i, intInit.length());
      int convertedInt = std::stoi(string);

      return convertedInt;
    }
  }

  return 0;
}

string scLinkGetString(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    if (ScStreamConverter::StreamToString(stream, string))
    {
      return string;
    }
  }

  return {};
}

bool scLinkContentIsUint(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    if (ScStreamConverter::StreamToString(stream, string))
    {
      if (string.find("uint:") != std::string::npos)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  return false;
}

int scLinkGetContentUint(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    std::string intInit = "uint: ";
    if (ScStreamConverter::StreamToString(stream, string))
    {
      std::string::size_type i = string.find(intInit);

      if (i != std::string::npos)
        string.erase(i, intInit.length());

      int convertedInt = std::stoi(string);

      return convertedInt;
    }
  }

  return 0;
}

bool scLinkContentIsFloat(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    if (ScStreamConverter::StreamToString(stream, string))
    {
      if (string.find("float:") != std::string::npos)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }

  return false;
}

float scLinkGetContentFloat(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    std::string intInit = "float:";
    if (ScStreamConverter::StreamToString(stream, string))
    {
      std::string::size_type i = string.find(intInit);

      if (i != std::string::npos)
        string.erase(i, intInit.length());
      float convertedFloat = std::stof(string);
      return convertedFloat;
    }
  }

  return 0;
}

bool scLinkContentIsDouble(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    if (ScStreamConverter::StreamToString(stream, string))
    {
      if (string.find("double:") != std::string::npos)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  return false;
}

double scLinkGetContentDouble(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  ScStreamPtr stream = ctx.GetLinkContent(elemAddr);
  if (stream)
  {
    std::string string;
    std::string intInit = "double: ";
    if (ScStreamConverter::StreamToString(stream, string))
    {
      std::string::size_type i = string.find(intInit);

      if (i != std::string::npos)
        string.erase(i, intInit.length());
      double convertedDouble = std::stod(string);
      return convertedDouble;
    }
  }

  return 0;
}

ScStreamMemory::ScStreamMemory(MemoryBufferPtr const & buff)
  : ScStream(static_cast<sc_char const *>(buff->Data()), sc_uint(buff->Size()), SC_STREAM_FLAG_READ)
  , m_buffer(buff)
{
}

ScStreamMemory::~ScStreamMemory() = default;

ScStreamPtr StreamFromString(std::string const & str)
{
  MemoryBufferPtr buff = MemoryBufferPtr(new MemoryBufferSafe(str.c_str(), (uint32_t)str.size()));
  return std::make_shared<ScStreamMemory>(buff);
}

string getIntegerString(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  string input = scLinkGetString(ctx, elemAddr);
  regex integer_expr("(\\+|-)?[[:digit:]]+");

  string result;

  if (input.find("int:") != std::string::npos && regex_match(input, integer_expr))
    result = input;
  return result;
}

void printOperatorAnswer(ScAgentContext & ctx, SCPOperand * nodeAddr, ScAddr const & linkAddr)
{
  ScAddr elem1, elem3, elem5, arc1;
  elem1.Reset();
  elem3.Reset();
  elem5.Reset();
  arc1.Reset();

  elem5 = Keynodes::nrel_scp_var_value;
  elem3 = linkAddr;
  elem1 = nodeAddr->CreateNodeOrLink();

  arc1 = ctx.GenerateConnector(ScType::ConstCommonArc, elem1, elem3);
  ctx.GenerateConnector(ScType::ConstPermPosArc, elem5, arc1);
  printInfo(ctx, elem5);
  printInfo(ctx, elem3);
  printInfo(ctx, elem1);
  nodeAddr->SetValue(elem1);
}

std::string scLinkPlainNumbers(ScAgentContext & ctx, ScAddr const & elemAddr)
{
  std::string str_link = Utils::scLinkGetString(ctx, elemAddr);

  std::regex integer_expr("(\\+|-)?[[:digit:]]+");
  std::regex double_expr("(\\+|-)?[[:digit:]]+\\.[[:digit:]]*");

  std::string answer_str;

  if (std::regex_match(str_link, integer_expr))
  {
    std::cout << "Input is an integer" << std::endl;
    // int value = std::atoi(str_link.c_str());
    // int answer = (int)sin((double)value);
    std::string int_str = "int: ";
    answer_str = int_str.append(str_link);
  }

  if (std::regex_match(str_link, double_expr))
  {
    std::cout << "Input is a double" << std::endl;
    // double convertedDouble = std::stod(str_link);
    // double answer = sin(convertedDouble);
    std::string double_str = "double: ";
    answer_str = double_str.append(str_link);
    std::cout << answer_str << std::endl;
  }

  return answer_str;
}

int64_t GetTimeFromStart(std::chrono::time_point<std::chrono::high_resolution_clock> const & startTime)
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime)
      .count();
}

#ifdef SCP_DEBUG
void logUnknownOperatorTypeError(ScAgentContext & ctx, ScAddr & addr)
{
  cout << "SCP Error: Unknown scp-operator type: ";
  printSystemIdentifier(ctx, addr);
  cout << endl;
}

void logSCPError(ScAgentContext & ctx, string text, ScAddr const & addr)
{
  cout << "SCP Error: " << text << ": ";
  printSystemIdentifier(ctx, addr);
  cout << endl;
}
#endif

}  // namespace Utils
}  // namespace scp
