#
#   Run cppcheck, filering out the noise.
#   
cppcheck \
    -I ./include -I ../../../FreeRTOS/Source/include/ \
    --enable=warning,information,style,performance,portability,missingInclude \
    --std=c++03 \
    --force \
    *.cpp \
    | grep -v "^Checking "

