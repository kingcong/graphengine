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

#ifndef GE_GRAPH_PREPROCESS_INSERT_OP_UTIL_INSERT_AIPP_OP_H_
#define GE_GRAPH_PREPROCESS_INSERT_OP_UTIL_INSERT_AIPP_OP_H_

#include <memory>
#include <string>
#include <vector>

#include "graph/compute_graph.h"
#include "graph/preprocess/insert_op/base_insert_op.h"
#include "proto/insert_op.pb.h"

namespace ge {
enum AippType { kOldType, kNewType };

class InsertNewOpUtil {
 public:
  static InsertNewOpUtil &Instance() {
    static InsertNewOpUtil instance;
    return instance;
  }

  Status Init();

  Status Parse(const char *conf_path);

  Status InsertNewOps(const ge::ComputeGraphPtr &graph);

  Status InsertAippOps(ge::ComputeGraphPtr &graph, std::string &aipp_config_path);

  void ClearNewOps();

 private:
  Status CheckPositionNotRepeat();

  Status AddMultiShapeInputData(const ge::ComputeGraphPtr &graph);

  Status GetAippParams(const std::unique_ptr<domi::AippOpParams> &aipp_params, const ge::NodePtr &aipp_node);

  Status CheckGraph(const ge::ComputeGraphPtr &graph);

  InsertNewOpUtil() {}

  Status AddAippInputData(const ge::NodePtr &aipp_node, const ge::ComputeGraphPtr &graph);

  ~InsertNewOpUtil() = default;

  std::vector<std::unique_ptr<InsertOpBase>> insert_ops_;

  std::unique_ptr<domi::InsertNewOps> insert_op_conf_;
};
}  // namespace ge

#endif  // GE_GRAPH_PREPROCESS_INSERT_OP_UTIL_INSERT_AIPP_OP_H_
