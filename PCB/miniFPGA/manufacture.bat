@REM KiKit 1.5 or newer plugin required.https://yaqwsx.github.io/KiKit/v1.5/


rmdir C:\projects\miniPFGA_school\PCB\miniFPGA\gerber /s /q

kikit fab jlcpcb C:\projects\miniPFGA_school\PCB\miniFPGA\miniFPGA.kicad_pcb C:\projects\miniPFGA_school\PCB\miniFPGA\
del C:\projects\miniPFGA_school\PCB\miniFPGA\gerbers_single.zip
rename C:\projects\miniPFGA_school\PCB\miniFPGA\gerbers.zip gerbers_single.zip

kikit panelize -p C:\projects\miniPFGA_school\PCB\miniFPGA\KiKit_panelize.json C:\projects\miniPFGA_school\PCB\miniFPGA\miniFPGA.kicad_pcb C:\projects\miniPFGA_school\PCB\miniFPGA\miniFPGA_panel.kicad_pcb 
kikit fab jlcpcb C:\projects\miniPFGA_school\PCB\miniFPGA\miniFPGA_panel.kicad_pcb C:\projects\miniPFGA_school\PCB\miniFPGA\ 
del C:\projects\miniPFGA_school\PCB\miniFPGA\gerbers_panel.zip
rename C:\projects\miniPFGA_school\PCB\miniFPGA\gerbers.zip gerbers_panel.zip
