class Light
{
public:
    Light();
    void Setup();
    void Tick();
    bool Changed();
    bool Door();
    bool On();

private:
    int lastDoorState;
    int doorState;
    int lightState;
    bool changed;
};
