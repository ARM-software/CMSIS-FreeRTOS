boot.S:
    Added CPU ID determination and stack initialization code after multicore startup.
xil-crt0.S:
    Initialize the C runtime environment only on the primary core
    Start the secondary cores 
cpu.c / cpu.h:
    APIs of secondary cores power up and reset

xscugic.c:
    Added CPU interface initialization of the GIC for secondary cores

scugic_v4_2_diff.png:
standalone_v7_2_diff.png:
    Xlinx bsp libaray modification details