#ifndef button_h
#define button_h

class Button {
public:

  enum class Event {
    SHORT_PRESS,
    LONG_PRESS
  };

  /*using EventCb = void (*)(button::Event event);*/

  Button(int buttonPin/*, Button::EventCb eventCb*/);
  void Tick(void);

private:
  int pin;
  int lastState;
  int buttonState;
  unsigned long pressedAt;
  unsigned long lastEdgeAt;
};

#endif // button_h
