/**
 * Copyright 2019-2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "graph/passes/constant_folding_pass.h"

#include <vector>

#include "common/debug/log.h"
#include "common/types.h"
#include "framework/common/debug/ge_log.h"
#include "graph/utils/attr_utils.h"
#include "graph/utils/node_utils.h"
#include "graph/utils/op_desc_utils.h"
#include "graph/utils/type_utils.h"
#include "inc/kernel.h"

namespace ge {
Status ConstantFoldingPass::Run(ge::NodePtr &node) {
  GE_CHECK_NOTNULL(node);
  GELOGD("Begin to run constant folding on node %s", node->GetName().c_str());

  OpDescPtr node_desc = node->GetOpDesc();
  if (node_desc == nullptr) {
    return SUCCESS;
  }
  DataType data_type = node_desc->GetOutputDesc(0).GetDataType();
  Format format = node_desc->GetOutputDesc(0).GetFormat();
  GELOGD("current [node:%s, type:%s] info: format: %s, datatype:%s", node->GetName().c_str(), node->GetType().c_str(),
         TypeUtils::FormatToSerialString(format).c_str(), TypeUtils::DataTypeToSerialString(data_type).c_str());
  auto input_nodes = OpDescUtils::GetConstInputNode(*node);
  if (input_nodes.empty() || input_nodes.size() != node_desc->GetInputsSize()) {
    GELOGI("Const input nodes size is %zu, and nodeDesc inputsSize is %zu.",
        input_nodes.size(), node_desc->GetInputsSize());
    return SUCCESS;
  }

  auto op_kernel = folding_pass::GetKernelByType(node);
  if (op_kernel == nullptr) {
    GELOGD("No op kernel for node %s type %s, skip the constant folding", node->GetName().c_str(),
           node->GetType().c_str());
    return SUCCESS;
  }
  auto inputs = OpDescUtils::GetInputData(input_nodes);
  vector<GeTensorPtr> outputs;
  auto ret = op_kernel->Compute(node_desc, inputs, outputs);
  if (ret != SUCCESS) {
    if (ret == NOT_CHANGED) {
      return SUCCESS;
    }
    GELOGE(INTERNAL_ERROR, "Calculate for node %s failed in constant folding", node->GetName().c_str());
    return ret;
  }
  if (outputs.empty()) {
    GELOGE(INTERNAL_ERROR,
           "Failed to constant folding on node %s,"
           " no output weight",
           node->GetName().c_str());
    return INTERNAL_ERROR;
  }

  return Folding(node, outputs);
}
}  // namespace ge
