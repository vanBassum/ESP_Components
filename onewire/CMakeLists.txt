#set(COMPONENT_ADD_INCLUDEDIRS ".")
#set(COMPONENT_SRCS "bus.cpp" )
#set(EXTRA_COMPONENT_DIRS "owb")
#set(PRIV_REQUIRES owb)


set(COMPONENT_ADD_INCLUDEDIRS "." "owb/include")
set(COMPONENT_SRCS "bus.cpp" "owb/owb.c" "owb/owb_gpio.c" "owb/owb_rmt.c")
set(PRIV_REQUIRES freertos esp_common)
register_component()
