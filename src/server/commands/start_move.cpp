#include "start_move.h"

#include <format>

#include "exception.h"
#include "move.h"
#include "set_velocity.h"

namespace server::commands {

StartMoveAdapter::StartMoveAdapter(const std::shared_ptr<core::Object>& order) : StartCommandAdapter(order) {
}

std::string_view StartMoveAdapter::GetAdapterName() const {
  return kStartMoveAdapterName;
}

std::string_view StartMoveAdapter::GetParentAdapterName() const {
  return kStartMoveCommandName;
}

std::weak_ptr<core::Object>& StartMoveAdapter::GetMovable() {
  auto& any_movable = GetAnyValue(kMovableName);

  return CastAnyRefToRef<std::weak_ptr<core::Object>>(kMovableName, any_movable);
}

core::Vector StartMoveAdapter::GetVelocity() const {
  const auto& any_velocity = GetAnyValue(kVelocityName);

  return CastAnyRefToValue<core::Vector>(kVelocityName, any_velocity);
}

Queue<std::unique_ptr<Command>>& StartMoveAdapter::GetCommandQueue() {
  auto& any_command_queue = GetAnyValue(kCommandQueueName);

  return CastAnyPointerToRef<Queue<std::unique_ptr<Command>>>(kCommandQueueName, any_command_queue);
}

void StartMoveAdapter::DoStartAction() {

  std::shared_ptr<core::Object> movable = GetMovable().lock();
  if (!movable)
    throw sw::Exception(std::format("'{}' property value is empty for '{}' object in '{}'.", kMovableName,
                                    kStartMoveAdapterName, kStartMoveCommandName));

  auto& command_queue = GetCommandQueue();
  const auto velocity = GetVelocity();

  auto set_velovity_adapter = std::make_shared<VelocitySetableAdapter>(movable);
  command_queue.emplace_back(std::make_unique<SetVelocity>(std::move(set_velovity_adapter), velocity));
  command_queue.emplace_back(std::make_unique<InjectableCommand>(std::make_unique<Move>(movable)));
}

} // namespace server::commands
