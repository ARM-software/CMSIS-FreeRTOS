#include <stddef.h>
#include <stdio.h>
#include "../../../../../tasks.c"

#define DEFINE(sym,val) \
    printf("#define %s %d /* %s */\n", #sym, (val), #val)

int main(void)
{
    DEFINE(TCB_TOP_OF_STACK_OFF, offsetof(TCB_t, pxTopOfStack));
#if portUSING_MPU_WRAPPERS
    DEFINE(TCB_MPU_SETTINGS_OFF, offsetof(TCB_t, xMPUSettings.mpumap));
    DEFINE(MPU_ENTRY_SIZE, sizeof(xthal_MPU_entry));
    DEFINE(MPU_ENTRY_AS_OFF, offsetof(xthal_MPU_entry, as));
    DEFINE(MPU_ENTRY_AT_OFF, offsetof(xthal_MPU_entry, at));
    DEFINE(TCB_TASK_NAME_OFF, offsetof(TCB_t, pcTaskName));
#endif
#if ( ( portSTACK_GROWTH > 0 ) || ( configRECORD_STACK_HIGH_ADDRESS == 1 ) )
    DEFINE(TCB_END_OF_STACK_OFF, offsetof(TCB_t, pxEndOfStack));
#endif
#if ( configUSE_C_RUNTIME_TLS_SUPPORT == 1 )
    DEFINE(TCB_IMPURE_PTR_OFF, offsetof(TCB_t, xTLSBlock));
#endif
}
