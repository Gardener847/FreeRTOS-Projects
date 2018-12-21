#include "LPC17xx.h"
#include <stddef.h>
enum InterruptCondition 
{
    kRisingEdge,
    kFallingEdge,
    kBothEdges,
};

typedef void (*IsrPointer)(void);

class INTDrv
{
 public:

    static INTDrv* instance();
 
    void Initialize();

    bool AttachInterruptHandler(uint8_t port, uint32_t pin, IsrPointer pin_isr, InterruptCondition condition);
    
    void HandleInterrupt();

 private:
    IsrPointer pin_isr_map[4];
    static INTDrv * m_instance;
    INTDrv();
};