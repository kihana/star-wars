#include "end_rotate.h"

#include <format>

#include "exception.h"
#include "rotate.h"
#include "set_angle.h"

namespace server::commands {

EndRotateAdapter::EndRotateAdapter(const std::shared_ptr<core::Object>& order) : EndCommandAdapter(order) {
}

std::string_view EndRotateAdapter::GetAdapterName() const {
  return kEndRotateAdapterName;
}

std::string_view EndRotateAdapter::GetParentAdapterName() const {
  return kEndRotateCommandName;
}

InjectableCommand* EndRotateAdapter::GetRotateCommand() {
  auto& any_rotate_command = GetAnyValue(kRotateCommandName);

  return CastAnyPointerToPointer<InjectableCommand>(kRotateCommandName, any_rotate_command);
}

std::weak_ptr<core::Object>& EndRotateAdapter::GetRotatable() {
  auto& any_rotatable = GetAnyValue(kRotatableName);

  return CastAnyRefToRef<std::weak_ptr<core::Object>>(kRotatableName, any_rotatable);
}

Queue<std::unique_ptr<Command>>& EndRotateAdapter::GetCommandQueue() {
  auto& any_command_queue = GetAnyValue(kCommandQueueName);

  return CastAnyPointerToRef<Queue<std::unique_ptr<Command>>>(kCommandQueueName, any_command_queue);
}

void EndRotateAdapter::DoEndAction() {
  std::shared_ptr<core::Object> rotatable = GetRotatable().lock();
  if (!rotatable)
    throw sw::Exception(std::format("'{}' property value is empty for '{}' object in '{}'.", kRotatableName,
                                    kEndRotateAdapterName, kEndRotateCommandName));

  auto& command_queue = GetCommandQueue();
  const auto& command = GetRotateCommand();
  rotatable->RemoveKey(kAngleName);
  command->Inject(std::make_unique<NopCommand>());
}

} // namespace server::commands
