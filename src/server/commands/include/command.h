#pragma once

#include <any>
#include <format>
#include <list>
#include <memory>
#include <string_view>

#include "core/object.h"
#include "exception.h"

namespace server::commands {

static std::string_view kCommandQueueName = "CommandQueue";

struct Command {
  virtual ~Command() = default;

  virtual void Execute() = 0;
};

class Adapter {
public:
  explicit Adapter(std::shared_ptr<core::Object> property_holder);
  virtual ~Adapter() = default;

protected:
  std::any& GetAnyValue(std::string_view property_name);
  const std::any& GetAnyValue(std::string_view property_name) const;

  template <typename T> void SetValue(std::string_view property_name, T value) {
    if (!property_holder_)
      throw sw::Exception(std::format("'{}' to set '{}' is unavailable in '{}'.", GetAdapterName(), property_name,
                                      GetParentAdapterName()));

    property_holder_->SetValue(property_name, value);
  }

  template <typename T> T& CastAnyPointerToRef(std::string_view property_name, std::any& any_value) {
    try {
      return *std::any_cast<T*>(any_value);
    } catch (const std::bad_any_cast&) {
      throw sw::Exception(std::format("Unexpected type of '{}' property in '{}' object in '{}'.", property_name,
                                      GetAdapterName(), GetParentAdapterName()));
    }
  }

  template <typename T> T& CastAnyPointerToRef(std::string_view property_name, const std::any& any_value) const {
    try {
      return *std::any_cast<T*>(any_value);
    } catch (const std::bad_any_cast&) {
      throw sw::Exception(std::format("Unexpected type of '{}' property in '{}' object in '{}'.", property_name,
                                      GetAdapterName(), GetParentAdapterName()));
    }
  }

  template <typename T> T* CastAnyPointerToPointer(std::string_view property_name, std::any& any_value) {
    try {
      return std::any_cast<T*>(any_value);
    } catch (const std::bad_any_cast&) {
      throw sw::Exception(std::format("Unexpected type of '{}' property in '{}' object in '{}'.", property_name,
                                      GetAdapterName(), GetParentAdapterName()));
    }
  }

  template <typename T> T& CastAnyRefToRef(std::string_view property_name, std::any& any_value) {
    try {
      return std::any_cast<T&>(any_value);
    } catch (const std::bad_any_cast&) {
      throw sw::Exception(std::format("Unexpected type of '{}' property in '{}' object in '{}'.", property_name,
                                      GetAdapterName(), GetParentAdapterName()));
    }
  }

  template <typename T> const T& CastAnyRefToRef(std::string_view property_name, const std::any& any_value) const {
    try {
      return std::any_cast<const T&>(any_value);
    } catch (const std::bad_any_cast&) {
      throw sw::Exception(std::format("Unexpected type of '{}' property in '{}' object in '{}'.", property_name,
                                      GetAdapterName(), GetParentAdapterName()));
    }
  }

  template <typename T> T CastAnyRefToValue(std::string_view property_name, const std::any& any_value) const {
    try {
      return std::any_cast<T>(any_value);
    } catch (const std::bad_any_cast&) {
      throw sw::Exception(std::format("Unexpected type of '{}' property in '{}' object in '{}'.", property_name,
                                      GetAdapterName(), GetParentAdapterName()));
    }
  }

private:
  virtual std::string_view GetAdapterName() const = 0;
  virtual std::string_view GetParentAdapterName() const = 0;

  template <typename T> static auto& GetAnyValueImpl(T& self, std::string_view property_name) {
    if (!self.property_holder_)
      throw sw::Exception(std::format("'{}' to get '{}' is unavailable in '{}'.", self.GetAdapterName(), property_name,
                                      self.GetParentAdapterName()));
    try {
      auto& any_value = self.property_holder_->GetValue(property_name);
      if (!any_value.has_value())
        throw sw::Exception(std::format("'{}' property value is not specified for '{}' object in '{}'.", property_name,
                                        self.GetAdapterName(), self.GetParentAdapterName()));

      return any_value;
    } catch (const std::out_of_range&) {
      throw sw::Exception(std::format("'{}' property is not specified for '{}' object in '{}'.", property_name,
                                      self.GetAdapterName(), self.GetParentAdapterName()));
    }
  }

protected:
  std::shared_ptr<core::Object> property_holder_;
};

template <typename T> using Queue = std::list<T>;

class InjectableCommand : public Command {
public:
  explicit InjectableCommand(std::unique_ptr<Command> command);

  void Inject(std::unique_ptr<Command> command);
  void Execute() override;
  const std::unique_ptr<Command>& GetCommand() const noexcept;

private:
  std::unique_ptr<Command> command_;
};

class NopCommand : public Command {
  void Execute() override;
};

} // namespace server::commands
