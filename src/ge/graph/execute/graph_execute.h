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

#ifndef GE_GRAPH_EXECUTE_GRAPH_EXECUTE_H_
#define GE_GRAPH_EXECUTE_GRAPH_EXECUTE_H_

#include <cstdarg>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "common/debug/log.h"
#include "common/debug/memory_dumper.h"
#include "common/ge_types.h"
#include "common/properties_manager.h"
#include "common/string_util.h"
#include "common/types.h"
#include "common/util.h"
#include "ge/ge_api_types.h"
#include "graph/compute_graph.h"
#include "graph/manager/graph_context.h"
#include "graph/manager/graph_manager_utils.h"
#include "graph/model.h"
#include "graph/utils/graph_utils.h"
#include "graph/utils/tensor_utils.h"

namespace ge {
class GraphExecutor {
 public:
  GraphExecutor();

  virtual ~GraphExecutor();

  Status ExecuteGraph(GraphId graph_id, const GeModelPtr &ge_model, const std::vector<GeTensor> &input_tensor,
                      std::vector<GeTensor> &output_tensor);

  Status ExecuteGraphAsync(GraphId graph_id, const GeModelPtr &ge_model, const std::vector<TensorInfo> &input_tensor,
                           std::vector<TensorInfo> &output_tensor);

  Status SetCondition(std::mutex *mutex, std::condition_variable *cond, std::shared_ptr<GraphModelListener> listener);

  Status SetGraphContext(GraphContextPtr graph_context_ptr);

  void SetTrainFlag(bool is_train_graph);

  const std::vector<InputOutputDescInfo> &GetOutputsDesc() const { return outputs_desc_; }

  Status FreeExecuteMemory();

  static Status DataInput(const InputData &input_data, OutputData &output_data);

  static Status GetInputOutputDescInfo(const uint32_t model_id, vector<InputOutputDescInfo> &input_desc,
                                       vector<InputOutputDescInfo> &output_desc);

  static Status GetInputOutputDescInfo(const uint32_t model_id, vector<InputOutputDescInfo> &input_desc,
                                       vector<InputOutputDescInfo> &output_desc, std::vector<uint32_t> &input_formats,
                                       std::vector<uint32_t> &output_formats);

  static Status GetInputOutputDescInfoForZeroCopy(uint32_t model_id, vector<InputOutputDescInfo> &input_desc,
                                                  vector<InputOutputDescInfo> &output_desc,
                                                  std::vector<uint32_t> &input_formats,
                                                  std::vector<uint32_t> &output_formats);

 private:
  Status PrepareInputData(const std::vector<GeTensor> &input_tensor, InputData &graph_input_data,
                          OutputData &graph_output_data, std::vector<InputOutputDescInfo> &output_desc);

  Status SyncExecuteModel(uint32_t model_id, const std::vector<GeTensor> &input_tensor,
                          std::vector<GeTensor> &output_tensor);

  Status AsyncExecuteModel(uint32_t model_id, const std::vector<TensorInfo> &input_tensor,
                           std::vector<TensorInfo> &output_tensor);

  void InitModelIdInfo(std::vector<uint32_t> &out_model_id_info, std::vector<SubGraphInfoPtr> &sub_graph_vec,
                       uint32_t output_size);

  Status FreeInOutBuffer();

  Status MallocInOutBuffer(const std::vector<uint32_t> &buffer_size, std::vector<void *> &data_addr);

  bool init_flag_;

  bool train_graph_flag_;
  // For run graph synchronous return
  std::mutex *sync_run_mutex_;
  std::condition_variable *condition_;

  // Run graph asynchronous call back listener
  std::shared_ptr<GraphModelListener> graph_run_listener_;

  GraphContextPtr graph_context_;

  std::vector<InputOutputDescInfo> outputs_desc_;
  GraphId last_graph_id_;

  bool malloc_flag_;
  std::vector<void *> buffer_addr_;
  std::vector<uint32_t> buffer_size_;
};
}  // namespace ge

#endif  // GE_GRAPH_EXECUTE_GRAPH_EXECUTE_H_
