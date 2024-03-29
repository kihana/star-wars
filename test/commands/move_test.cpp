#include <gtest/gtest.h>

#include "commands/move.h"
#include "commands/set_velocity.h"
#include "core/object.h"
#include "utils.h"

namespace server::commands {

TEST(MoveTest, Common) {
  auto movable_object = std::make_shared<core::Object>();
  MovableAdapter move_adapter(movable_object);
  move_adapter.SetPosition({12, 5});
  VelocitySetableAdapter velocity_setable_adapter(movable_object);
  velocity_setable_adapter.SetVelocity({-7, 3});
  Move move(movable_object);
  move.Execute();
  EXPECT_EQ(move_adapter.GetPosition(), core::Vector(5, 8));

  move.Execute();
  EXPECT_EQ(move_adapter.GetPosition(), core::Vector(-2, 11));
}

TEST(MoveTest, EmptyMovable) {
  std::unique_ptr<Movable> empty_movable;
  Move move(std::move(empty_movable));
  EXPECT_ERROR(move, sw::Exception, std::format("'{}' is unavailable.", kMovableName));
}

TEST(MoveTest, EmptyPropertyHolder) {
  std::shared_ptr<core::Object> empty_property_holder;
  std::unique_ptr<Movable> movable = std::make_unique<MovableAdapter>(empty_property_holder);
  Move move(std::move(movable));
  EXPECT_THROW(
      {
        try {
          move.Execute();
        } catch (const sw::Exception& e) {
          EXPECT_TRUE((e.what() == std::format("'{}' to get '{}' is unavailable in '{}'.", kMovableName, kPositionName,
                                               kMoveCommandName)) ||
                      (e.what() == std::format("'{}' to get '{}' is unavailable in '{}'.", kMovableName, kVelocityName,
                                               kMoveCommandName)));
          throw;
        }
      },
      sw::Exception);
}
TEST(MoveTest, NoPositionOrVelocityInMovable) {
  auto property_holder = std::make_shared<core::Object>();
  std::unique_ptr<Movable> movable = std::make_unique<MovableAdapter>(property_holder);
  Move move(std::move(movable));
  EXPECT_THROW(
      {
        try {
          move.Execute();
        } catch (const sw::Exception& e) {
          EXPECT_TRUE((e.what() == std::format("'{}' property is not specified for '{}' object in '{}'.", kPositionName,
                                               kMovableName, kMoveCommandName)) ||
                      (e.what() == std::format("'{}' property is not specified for '{}' object in '{}'.", kVelocityName,
                                               kMovableName, kMoveCommandName)));
          throw;
        }
      },
      sw::Exception);
}

TEST(MoveTest, EmptyAnyPositionOrVelocityValueInMovable) {
  auto property_holder = std::make_shared<core::Object>();
  property_holder->SetValue(kPositionName, std::any());
  property_holder->SetValue(kVelocityName, std::any());
  std::unique_ptr<Movable> movable = std::make_unique<MovableAdapter>(property_holder);
  Move move(std::move(movable));
  EXPECT_THROW(
      {
        try {
          move.Execute();
        } catch (const sw::Exception& e) {
          EXPECT_TRUE((e.what() == std::format("'{}' property value is not specified for '{}' object in '{}'.",
                                               kPositionName, kMovableName, kMoveCommandName)) ||
                      (e.what() == std::format("'{}' property value is not specified for '{}' object in '{}'.",
                                               kVelocityName, kMovableName, kMoveCommandName)));
          throw;
        }
      },
      sw::Exception);
}

TEST(MoveTest, WrongPositionOrVelocityTypeInMovable) {
  auto property_holder = std::make_shared<core::Object>();
  property_holder->SetValue(kPositionName, 5);
  property_holder->SetValue(kVelocityName, 6);
  std::unique_ptr<Movable> movable = std::make_unique<MovableAdapter>(property_holder);
  Move move(std::move(movable));
  EXPECT_THROW(
      {
        try {
          move.Execute();
        } catch (const sw::Exception& e) {
          EXPECT_TRUE((e.what() == std::format("Unexpected type of '{}' property in '{}' object in '{}'.",
                                               kPositionName, kMovableName, kMoveCommandName)) ||
                      (e.what() == std::format("Unexpected type of '{}' property in '{}' object in '{}'.",
                                               kVelocityName, kMovableName, kMoveCommandName)));
          throw;
        }
      },
      sw::Exception);
}

} // namespace server::commands
