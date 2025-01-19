#pragma once

class Button {
public:

  enum class Event {
    SHORT_PRESS,
    LONG_PRESS,
    LONG_PRESS_END
  };

  enum class Id {
    LEFT,
    MID,
    RIGHT
  };

  using EventCb = void (*)(Id id, Event event);

  Button(int buttonPin, Id id, EventCb eventCb);
  void Tick(void);

private:
  Id  id;
  int pin;
  int lastState;
  int buttonState;
  unsigned long pressedAt;
  unsigned long lastEdgeAt;
  bool longPress;
  EventCb eventCb;

  void HandleEvent(Event event);
};
