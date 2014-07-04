del /Q project\MDK-ARM(uV3)\Obj\*.*
del /Q project\MDK-ARM(uV3)\List\*.*
del /Q project\MDK-ARM(uV3)\JLinkLog.txt
del /Q project\MDK-ARM(uV3)\*.bak
del /Q project\MDK-ARM(uV3)\*.dep
rd /Q /S project\MDK-ARM(uV3)\Obj
rd /Q /S project\MDK-ARM(uV3)\List

del /Q project\MDK-ARM(uV4)\Obj\*.*
del /Q project\MDK-ARM(uV4)\List\*.*
del /Q project\MDK-ARM(uV4)\JLink*
del /Q project\MDK-ARM(uV4)\project.uvgui.*
del /Q project\MDK-ARM(uV4)\*.bak
del /Q project\MDK-ARM(uV4)\*.dep
rd /Q /S project\MDK-ARM(uV4)\Obj
rd /Q /S project\MDK-ARM(uV4)\List

del /Q project\EWARMv5\Project.dep
del /Q project\EWARMv5\Flash
del /Q project\EWARMv5\CpuRAM
del /Q project\EWARMv5\settings
rd  /Q /S project\EWARMv5\Flash
rd /Q /S project\EWARMv5\CpuRAM
rd /Q /S project\EWARMv5\ExtSRAM
rd /Q /S project\EWARMv5\settings

